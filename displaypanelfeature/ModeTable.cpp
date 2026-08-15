/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ModeTable.h"

#include <android-base/file.h>
#include <tinyxml2.h>

#include <algorithm>
#include <set>
#include <utility>

namespace oplus::displaypanelfeature {
namespace {

bool Duplicated(std::vector<int> values) {
    std::sort(values.begin(), values.end());
    return std::adjacent_find(values.begin(), values.end()) != values.end();
}

}  // namespace

std::optional<std::vector<Resolution>> ModeTable::ParseDensityMapping(const std::string& path,
                                                                      std::string* error) {
    std::string xml;
    if (!android::base::ReadFileToString(path, &xml)) {
        *error = "failed to read " + path;
        return std::nullopt;
    }
    tinyxml2::XMLDocument document;
    if (document.Parse(xml.c_str(), xml.size()) != tinyxml2::XML_SUCCESS) {
        *error = document.ErrorStr();
        return std::nullopt;
    }
    const auto* root = document.FirstChildElement("displayConfiguration");
    const auto* mapping = root == nullptr ? nullptr : root->FirstChildElement("densityMapping");
    if (mapping == nullptr) {
        *error = "displayconfig.xml declares no densityMapping";
        return std::nullopt;
    }

    std::vector<Resolution> resolutions;
    for (const auto* node = mapping->FirstChildElement("density"); node != nullptr;
         node = node->NextSiblingElement("density")) {
        Resolution resolution;
        const auto* width = node->FirstChildElement("width");
        const auto* height = node->FirstChildElement("height");
        const auto* density = node->FirstChildElement("density");
        if (width == nullptr || height == nullptr || density == nullptr ||
            width->QueryIntText(&resolution.width) != tinyxml2::XML_SUCCESS ||
            height->QueryIntText(&resolution.height) != tinyxml2::XML_SUCCESS ||
            density->QueryIntText(&resolution.density) != tinyxml2::XML_SUCCESS ||
            resolution.width <= 0 || resolution.height <= 0 || resolution.density <= 0) {
            *error = "incomplete or non-numeric density mapping row";
            return std::nullopt;
        }
        resolutions.push_back(resolution);
    }
    if (resolutions.empty()) {
        *error = "densityMapping declares no resolution";
        return std::nullopt;
    }
    return resolutions;
}

std::optional<ModeTable> ModeTable::Build(std::vector<Resolution> resolutions,
                                          std::vector<int> refresh_rates,
                                          const std::vector<int>& adaptive_rates,
                                          std::string* error) {
    if (resolutions.empty()) {
        *error = "mode table needs at least one resolution";
        return std::nullopt;
    }
    if (refresh_rates.empty()) {
        *error = "mode table needs at least one refresh rate";
        return std::nullopt;
    }
    if (std::any_of(refresh_rates.begin(), refresh_rates.end(), [](int hz) { return hz <= 0; })) {
        *error = "refresh rate must be positive";
        return std::nullopt;
    }
    if (Duplicated(refresh_rates)) {
        *error = "duplicate refresh rate";
        return std::nullopt;
    }

    // A resolution is identified by its geometry, so two rows describing the same
    // geometry with different densities is a contradiction, not a duplicate.
    std::set<std::pair<int, int>> geometry;
    for (const auto& resolution : resolutions) {
        if (resolution.width <= 0 || resolution.height <= 0 || resolution.density <= 0) {
            *error = "resolution fields must be positive";
            return std::nullopt;
        }
        if (!geometry.insert({resolution.width, resolution.height}).second) {
            *error = "duplicate resolution geometry";
            return std::nullopt;
        }
    }

    for (const int rate : adaptive_rates) {
        if (std::find(refresh_rates.begin(), refresh_rates.end(), rate) == refresh_rates.end()) {
            *error = "adaptive rate " + std::to_string(rate) + " is not a declared refresh rate";
            return std::nullopt;
        }
    }

    ModeTable table;
    table.resolutions_ = std::move(resolutions);
    table.refresh_rates_ = std::move(refresh_rates);
    table.adaptive_rates_ = adaptive_rates;
    std::sort(table.refresh_rates_.begin(), table.refresh_rates_.end());
    for (const auto& resolution : table.resolutions_) {
        for (const int rate : table.refresh_rates_) {
            table.modes_.push_back(DisplayMode{resolution, rate, table.IsAdaptive(rate)});
        }
    }
    return table;
}

bool ModeTable::HasRefreshRate(int refresh_hz) const {
    return std::find(refresh_rates_.begin(), refresh_rates_.end(), refresh_hz) !=
           refresh_rates_.end();
}

bool ModeTable::IsAdaptive(int refresh_hz) const {
    return std::find(adaptive_rates_.begin(), adaptive_rates_.end(), refresh_hz) !=
           adaptive_rates_.end();
}

std::optional<int> ModeTable::DensityFor(int width, int height) const {
    for (const auto& resolution : resolutions_) {
        if (resolution.width == width && resolution.height == height) return resolution.density;
    }
    return std::nullopt;
}

int ModeTable::PeakRefreshHz() const {
    return refresh_rates_.empty() ? 0 : refresh_rates_.back();
}

}  // namespace oplus::displaypanelfeature
