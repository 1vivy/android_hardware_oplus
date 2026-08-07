/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AdfrConfig.h"

#include <android-base/file.h>
#include <tinyxml2.h>

#include <array>
#include <charconv>
#include <string>

namespace oplus::displaypanelfeature {
namespace {

constexpr int32_t kExpectedVersion = 20250925;

struct ScalarField {
    const char* name;
    size_t index;
};

struct TableField {
    const char* name;
    size_t offset;
    size_t capacity;
};

constexpr std::array kScalars = {
        ScalarField{"enable", 2},          ScalarField{"debug_enable", 3},
        ScalarField{"sensor_enable", 4},   ScalarField{"panelnit_enable", 5},
        ScalarField{"gray_enable", 6},     ScalarField{"tracking_switch", 7},
        ScalarField{"gray_cal", 8},        ScalarField{"sampleInterval", 9},
        ScalarField{"sensor_inlux", 10},   ScalarField{"sensor_outlux", 11},
        ScalarField{"sensor_gaptime", 12}, ScalarField{"fullscreen_aod", 14},
        ScalarField{"reserve_mode", 15},
};

constexpr std::array kTables = {
        TableField{"panelnit_level", 18, 6},
        TableField{"Y_l_level", 24, 6},
        TableField{"Y_h_level", 30, 6},
        TableField{"sum_level", 36, 6},
        TableField{"max_level", 42, 6},
        TableField{"minfps120_level", 48, 13},
        TableField{"minfps90_level", 61, 13},
        TableField{"minfps60_level", 74, 13},
        TableField{"aod_panelnit_level", 87, 6},
        TableField{"aod_Y_l_level", 93, 6},
        TableField{"aod_Y_h_level", 99, 6},
        TableField{"aod_sum_level", 105, 6},
        TableField{"aod_max_level", 111, 6},
        TableField{"aod_minfps120_level", 117, 13},
        TableField{"aod_minfps90_level", 130, 13},
        TableField{"aod_minfps60_level", 143, 13},
        TableField{"reserve_panelnit_level", 156, 6},
        TableField{"reserve_Y_l_level", 162, 6},
        TableField{"reserve_Y_h_level", 168, 6},
        TableField{"reserve_sum_level", 174, 6},
        TableField{"reserve_max_level", 180, 6},
        TableField{"reserve_minfps_level1", 186, 13},
        TableField{"reserve_minfps_level2", 199, 13},
        TableField{"reserve_minfps_level3", 212, 13},
};

bool ParseInteger(std::string_view text, int32_t* value) {
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), *value);
    return error == std::errc{} && end == text.data() + text.size();
}

bool ReadScalar(const tinyxml2::XMLElement& parent, const char* name, int32_t* value,
                std::string* error) {
    const auto* element = parent.FirstChildElement(name);
    const std::string_view text =
            element != nullptr && element->GetText() != nullptr ? element->GetText() : "";
    if (!ParseInteger(text, value)) {
        *error = std::string("invalid or missing integer field: ") + name;
        return false;
    }
    return true;
}

bool ReadTable(const tinyxml2::XMLElement& mode, const TableField& field, int32_t* destination,
               std::string* error) {
    const auto* element = mode.FirstChildElement(field.name);
    if (element == nullptr || element->GetText() == nullptr) {
        *error = std::string("missing table field: ") + field.name;
        return false;
    }

    std::string_view remaining(element->GetText());
    size_t count = 0;
    while (!remaining.empty()) {
        const auto first = remaining.find_first_not_of(" \t\r\n");
        if (first == std::string_view::npos) break;
        remaining.remove_prefix(first);
        const auto separator = remaining.find_first_of(" \t\r\n");
        const auto token = remaining.substr(0, separator);
        if (count == field.capacity || !ParseInteger(token, &destination[count])) {
            *error = std::string("invalid or oversized table field: ") + field.name;
            return false;
        }
        ++count;
        if (separator == std::string_view::npos) break;
        remaining.remove_prefix(separator);
    }
    if (count == 0) {
        *error = std::string("empty table field: ") + field.name;
        return false;
    }
    return true;
}

}  // namespace

std::optional<AdfrPayload> ParseAdfrConfig(std::string_view xml, std::string* error) {
    tinyxml2::XMLDocument document;
    if (document.Parse(xml.data(), xml.size()) != tinyxml2::XML_SUCCESS) {
        *error = document.ErrorStr();
        return std::nullopt;
    }
    const auto* root = document.FirstChildElement("root");
    const auto* config = root == nullptr ? nullptr : root->FirstChildElement("oplsadfrCfg");
    const auto* mode = config == nullptr ? nullptr : config->FirstChildElement("mode");
    if (config == nullptr || mode == nullptr) {
        *error = "expected /root/oplsadfrCfg/mode";
        return std::nullopt;
    }

    AdfrPayload payload{};
    payload[0] = 1;
    if (!ReadScalar(*config, "version", &payload[1], error) || payload[1] != kExpectedVersion) {
        if (error->empty()) *error = "unsupported ADFR config version";
        return std::nullopt;
    }
    mode->QueryIntAttribute("panelid", &payload[13]);
    for (const auto& field : kScalars) {
        if (!ReadScalar(*mode, field.name, &payload[field.index], error)) return std::nullopt;
    }
    for (const auto& field : kTables) {
        if (!ReadTable(*mode, field, payload.data() + field.offset, error)) return std::nullopt;
    }
    return payload;
}

std::optional<AdfrPayload> LoadAdfrConfig(const std::string& path, std::string* error) {
    std::string xml;
    if (!android::base::ReadFileToString(path, &xml)) {
        *error = "failed to read " + path;
        return std::nullopt;
    }
    return ParseAdfrConfig(xml, error);
}

}  // namespace oplus::displaypanelfeature
