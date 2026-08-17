/*
 * Copyright (C) 2025 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "VibrationEffectLoader"

#include "VibrationEffectLoader.h"

#include <android-base/logging.h>
#include <android-base/properties.h>

#include <sys/system_properties.h>

#include <algorithm>
#include <fstream>

/*
 * Example of valid vibrator_effect.json:
 *
 * {
 *   "9999": {
 *     "def_style": [
 *       {
 *         "effect_file": "/odm/etc/vibrator/9999/def/effect_0.bin",
 *         "effect_id": 0,
 *         "play_rate_hz": 24000
 *       },
 *       ...
 *     ],
 *     "soft_style": [
 *       {
 *         "effect_file": "/odm/etc/vibrator/9999/soft/effect_0.bin",
 *         "effect_id": 0,
 *         "play_rate_hz": 24000
 *       },
 *       ...
 *     ]
 *   },
 *   ...
 * }
 */

namespace {
const auto kConfigPath = "/odm/etc/vibrator/vibrator_effect.json";

const auto kKeyEffectFile = "effect_file";
const auto kKeyEffectId = "effect_id";
const auto kKeyPlayRateHz = "play_rate_hz";

const int8_t* LoadEffectDataFromFile(const std::string& path, uint32_t& length) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        LOG(ERROR) << "Failed to open data file: " << path;
        return nullptr;
    }

    file.seekg(0, std::ios::end);
    length = file.tellg();
    file.seekg(0, std::ios::beg);

    int8_t* data = new int8_t[length];
    if (!file.read(reinterpret_cast<char*>(data), length)) {
        LOG(ERROR) << "Failed to read data file: " << path;
        delete[] data;
        return nullptr;
    }

    LOG(DEBUG) << "Successfully read data from " << path << ", length=" << length;

    return data;
}
};  // anonymous namespace

VibrationEffectLoader::VibrationEffectLoader() : active_style_(kDefaultStyle) {
    std::ifstream config_stream(kConfigPath);
    if (!config_stream) {
        LOG(INFO) << "Couldn't open " << kConfigPath
                  << " for parsing, falling back to built-in effects.";
        return;
    }
    auto node = parseEffectJson(config_stream);
    if (node.isNull()) {
        LOG(ERROR) << "Failed to parse " << kConfigPath << ", falling back to built-in effects.";
        return;
    }
    loadStyles(std::move(node));
    refreshActiveStyle();
    LOG(INFO) << "Effect styles loaded=" << styles_.size() << " active=" << active_style_;
}

void VibrationEffectLoader::refreshActiveStyle() {
    if (styles_.empty()) {
        return;
    }
    if (style_prop_ == nullptr) {
        // The property may legitimately not exist yet; retry on the next call
        // rather than caching "absent" forever.
        style_prop_ = __system_property_find(kStyleProperty);
        if (style_prop_ == nullptr) {
            return;
        }
    }
    const uint32_t serial = __system_property_serial(style_prop_);
    if (style_resolved_ && serial == style_serial_) {
        return;
    }
    style_serial_ = serial;
    style_resolved_ = true;

    // Resolve the selection only against tiers this device actually shipped. A
    // request for an absent tier keeps the default rather than leaving the
    // device with no effects at all.
    const auto requested = requestedStyle();
    const std::string next = styles_.count(requested) != 0 ? requested : kDefaultStyle;
    if (next == active_style_) {
        return;
    }
    if (requested != next) {
        LOG(WARNING) << "Requested effect style '" << requested
                     << "' is not shipped by this device; using " << next;
    }
    LOG(INFO) << "Effect style changed " << active_style_ << " -> " << next;
    active_style_ = next;
}

VibrationEffectLoader::~VibrationEffectLoader() {
    for (auto&& style : styles_) {
        std::for_each(style.second.begin(), style.second.end(),
                      [](auto&& v) { delete[] v.second.data; });
    }
}

std::string VibrationEffectLoader::requestedStyle() {
    return android::base::GetProperty(kStyleProperty, kDefaultStyle);
}

effect_stream* VibrationEffectLoader::getEffectStream(uint32_t effect_id) {
    // perform() dispatches from a worker thread, so selection state is shared.
    // The maps themselves are immutable after construction and their nodes are
    // stable, so the returned pointer stays valid once the lock is released.
    std::lock_guard<std::mutex> lock(mutex_);
    refreshActiveStyle();

    auto style = styles_.find(active_style_);
    if (style != styles_.end()) {
        auto entry = style->second.find(effect_id);
        if (entry != style->second.end()) {
            return &entry->second;
        }
    }
    // A non-default tier is allowed to carry fewer effects than the default one
    // (measured: 22 against 88). Anything it does not define resolves against
    // the default tier so selecting a style can never lose an effect.
    if (active_style_ != kDefaultStyle) {
        auto fallback = styles_.find(kDefaultStyle);
        if (fallback != styles_.end()) {
            auto entry = fallback->second.find(effect_id);
            if (entry != fallback->second.end()) {
                return &entry->second;
            }
        }
    }
    return nullptr;
}

Json::Value VibrationEffectLoader::parseEffectJson(std::ifstream& config_stream) {
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;

    if (!Json::parseFromStream(builder, config_stream, &root, &errs)) {
        LOG(ERROR) << "Failed to parse config JSON stream, error: " << errs;
        return Json::Value::null;
    }
    if (!root.isObject()) {
        LOG(ERROR) << "Root must be an object";
        return Json::Value::null;
    }

    // The panel/model node is the first object member; sibling scalars such as
    // "vibrator_arch" are metadata and carry no effect tiers.
    for (auto&& node : root) {
        if (node.isObject() && !node.empty()) {
            return node;
        }
    }
    LOG(ERROR) << "No effect node found in config";
    return Json::Value::null;
}

void VibrationEffectLoader::loadStyles(Json::Value&& model_node) {
    for (const auto& style : model_node.getMemberNames()) {
        const auto& nodes = model_node[style];
        if (!nodes.isArray() || nodes.empty()) {
            LOG(WARNING) << "Skipping empty or malformed effect style: " << style;
            continue;
        }
        loadStyle(style, nodes);
    }
}

void VibrationEffectLoader::loadStyle(const std::string& style, const Json::Value& effect_nodes) {
    EffectMap effects;
    for (auto&& node : effect_nodes) {
        auto attr = node[kKeyEffectId];
        if (!attr.isUInt()) {
            LOG(ERROR) << "Invalid effect_id";
            continue;
        }
        uint32_t effect_id = attr.asUInt();

        attr = node[kKeyPlayRateHz];
        if (!attr.isUInt()) {
            LOG(ERROR) << "Invalid play_rate_hz";
            continue;
        }
        uint32_t play_rate_hz = attr.asUInt();

        uint32_t length;
        auto data = LoadEffectDataFromFile(node[kKeyEffectFile].asString(), length);
        if (!data) {
            continue;
        }

        effects.emplace(effect_id, effect_stream{effect_id, length, play_rate_hz, data});
    }
    if (effects.empty()) {
        LOG(WARNING) << "Effect style '" << style << "' loaded no payload";
        return;
    }
    styles_.emplace(style, std::move(effects));
}
