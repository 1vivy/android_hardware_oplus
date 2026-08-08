/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FeatureRegistry.h"

#include <android-base/file.h>
#include <tinyxml2.h>

#include <charconv>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <map>
#include <string_view>

namespace oplus::displaypanelfeature {
namespace {

template <typename Value>
std::optional<Value> ParseEnum(const char* text, const std::map<std::string_view, Value>& values) {
    if (text == nullptr) return std::nullopt;
    const auto entry = values.find(text);
    return entry == values.end() ? std::nullopt : std::optional(entry->second);
}

size_t PayloadCount(PayloadShape shape) {
    switch (shape) {
        case PayloadShape::kScalar:
            return 1;
        case PayloadShape::kPair:
        case PayloadShape::kAdfrControl:
            return 2;
        case PayloadShape::kTriple:
            return 3;
        case PayloadShape::kAdfrConfig:
        case PayloadShape::kProbe:
            return 0;
    }
}

}  // namespace

std::optional<std::vector<FeatureEntry>> LoadFeatureRegistry(const std::string& path,
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
    const auto* root = document.FirstChildElement("displayPanelFeatures");
    if (root == nullptr) {
        *error = "missing displayPanelFeatures root";
        return std::nullopt;
    }

    const std::map<std::string_view, Direction> directions = {
            {"set", Direction::kSet}, {"get", Direction::kGet}, {"both", Direction::kBoth}};
    const std::map<std::string_view, PayloadShape> shapes = {
            {"scalar", PayloadShape::kScalar},
            {"pair", PayloadShape::kPair},
            {"triple", PayloadShape::kTriple},
            {"adfr-control", PayloadShape::kAdfrControl},
            {"adfr-config", PayloadShape::kAdfrConfig},
            {"probe", PayloadShape::kProbe}};
    const std::map<std::string_view, ValueSource> sources = {
            {"property", ValueSource::kProperty},
            {"adfr-xml", ValueSource::kAdfrXml},
            {"support-probe", ValueSource::kSupportProbe},
            {"sysfs-node", ValueSource::kSysfsNode}};
    const std::map<std::string_view, RowStatus> statuses = {
            {"active", RowStatus::kActive}, {"reserved", RowStatus::kReserved}};

    std::vector<FeatureEntry> entries;
    for (auto* node = root->FirstChildElement("feature"); node != nullptr;
         node = node->NextSiblingElement("feature")) {
        int32_t id = 0;
        const auto direction = ParseEnum(node->Attribute("direction"), directions);
        const auto shape = ParseEnum(node->Attribute("payload"), shapes);
        const auto source = ParseEnum(node->Attribute("source"), sources);
        if (node->QueryIntAttribute("id", &id) != tinyxml2::XML_SUCCESS || id <= 0 || !direction ||
            !shape || !source) {
            *error = "invalid feature registry row";
            return std::nullopt;
        }
        const char* property = node->Attribute("property");
        if (*source == ValueSource::kProperty && (property == nullptr || property[0] == '\0')) {
            *error = "property source is missing its property name";
            return std::nullopt;
        }
        const char* path = node->Attribute("path");
        if (*source == ValueSource::kSysfsNode && (path == nullptr || path[0] == '\0')) {
            *error = "sysfs-node source is missing its path";
            return std::nullopt;
        }
        double scale = 1.0;
        double offset = 0.0;
        node->QueryDoubleAttribute("scale", &scale);
        node->QueryDoubleAttribute("offset", &offset);
        const char* producer = node->Attribute("producer");
        RowStatus status = RowStatus::kActive;
        const char* statusText = node->Attribute("status");
        if (statusText != nullptr) {
            const auto parsedStatus = ParseEnum(statusText, statuses);
            if (!parsedStatus) {
                *error = "invalid status attribute";
                return std::nullopt;
            }
            status = *parsedStatus;
        }

        FeatureEntry entry;
        entry.id = id;
        entry.direction = *direction;
        entry.shape = *shape;
        entry.source = *source;
        entry.property = property == nullptr ? "" : property;
        entry.path = path == nullptr ? "" : path;
        entry.scale = scale;
        entry.offset = offset;
        entry.producer = producer == nullptr ? "" : producer;
        entry.status = status;
        entries.push_back(std::move(entry));
    }
    if (entries.empty()) {
        *error = "feature registry is empty";
        return std::nullopt;
    }
    return entries;
}

std::optional<std::vector<int32_t>> ParsePropertyPayload(const FeatureEntry& entry,
                                                         const std::string& value) {
    const size_t expected = PayloadCount(entry.shape);
    if (entry.source != ValueSource::kProperty || expected == 0) return std::nullopt;
    std::vector<int32_t> payload;
    std::string_view remaining(value);
    while (!remaining.empty()) {
        const auto first = remaining.find_first_not_of(" ,");
        if (first == std::string_view::npos) break;
        remaining.remove_prefix(first);
        const auto separator = remaining.find_first_of(" ,");
        const auto token = remaining.substr(0, separator);
        int32_t parsed = 0;
        const auto [end, error] =
                std::from_chars(token.data(), token.data() + token.size(), parsed, 10);
        if (error != std::errc{} || end != token.data() + token.size()) return std::nullopt;
        payload.push_back(parsed);
        if (separator == std::string_view::npos) break;
        remaining.remove_prefix(separator);
    }
    return payload.size() == expected ? std::optional(std::move(payload)) : std::nullopt;
}

std::optional<std::vector<int32_t>> ParseSysfsPayload(const FeatureEntry& entry,
                                                      const std::string& value) {
    if (entry.source != ValueSource::kSysfsNode || PayloadCount(entry.shape) != 1) {
        return std::nullopt;
    }
    std::string_view trimmed(value);
    const auto first = trimmed.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) return std::nullopt;
    trimmed.remove_prefix(first);
    const auto last = trimmed.find_last_not_of(" \t\r\n");
    trimmed = trimmed.substr(0, last + 1);

    const std::string token(trimmed);
    char* end = nullptr;
    errno = 0;
    const double parsed = std::strtod(token.c_str(), &end);
    if (end != token.c_str() + token.size() || errno == ERANGE) return std::nullopt;

    const double scaled = parsed * entry.scale + entry.offset;
    return std::vector<int32_t>{static_cast<int32_t>(std::lround(scaled))};
}

}  // namespace oplus::displaypanelfeature
