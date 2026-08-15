/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FeatureRegistry.h"

#include <android-base/file.h>
#include <openssl/sha.h>
#include <tinyxml2.h>

#include <algorithm>
#include <charconv>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <map>
#include <set>
#include <string_view>

namespace oplus::displaypanelfeature {
namespace {

template <typename Value>
std::optional<Value> ParseEnum(const char* text, const std::map<std::string_view, Value>& values) {
    if (text == nullptr) return std::nullopt;
    const auto entry = values.find(text);
    return entry == values.end() ? std::nullopt : std::optional(entry->second);
}

const std::map<std::string_view, Direction> kDirections = {
        {"set", Direction::kSet}, {"get", Direction::kGet}, {"both", Direction::kBoth}};
const std::map<std::string_view, PayloadShape> kShapes = {
        {"scalar", PayloadShape::kScalar}, {"pair", PayloadShape::kPair},
        {"triple", PayloadShape::kTriple}, {"adfr-control", PayloadShape::kAdfrControl},
        {"adfr-config", PayloadShape::kAdfrConfig}, {"probe", PayloadShape::kProbe}};
const std::map<std::string_view, ValueSource> kSources = {
        {"property", ValueSource::kProperty}, {"adfr-xml", ValueSource::kAdfrXml},
        {"support-probe", ValueSource::kSupportProbe}, {"sysfs-node", ValueSource::kSysfsNode}};
const std::map<std::string_view, RowStatus> kStatuses = {
        {"active", RowStatus::kActive}, {"reserved", RowStatus::kReserved}};
const std::map<std::string_view, DisplayRole> kRoles = {
        {"primary", DisplayRole::kPrimary}, {"secondary", DisplayRole::kSecondary}};

std::optional<std::vector<DisplayRole>> ParseRoles(const char* text) {
    if (text == nullptr) return std::vector<DisplayRole>{};
    std::vector<DisplayRole> roles;
    std::string_view remaining(text);
    while (!remaining.empty()) {
        const size_t separator = remaining.find(',');
        const auto token = remaining.substr(0, separator);
        const auto role = kRoles.find(token);
        if (role == kRoles.end() || std::find(roles.begin(), roles.end(), role->second) != roles.end()) {
            return std::nullopt;
        }
        roles.push_back(role->second);
        if (separator == std::string_view::npos) break;
        remaining.remove_prefix(separator + 1);
    }
    return roles;
}

std::optional<std::vector<int32_t>> ParseAllowedValues(const char* text) {
    std::vector<int32_t> values;
    if (text == nullptr) return values;
    std::string_view remaining(text);
    while (!remaining.empty()) {
        const size_t comma = remaining.find(',');
        const std::string_view token = remaining.substr(0, comma);
        int32_t value = 0;
        const auto [end, error] = std::from_chars(token.data(), token.data() + token.size(), value);
        if (token.empty() || error != std::errc() || end != token.data() + token.size() ||
            std::find(values.begin(), values.end(), value) != values.end()) {
            return std::nullopt;
        }
        values.push_back(value);
        if (comma == std::string_view::npos) break;
        remaining.remove_prefix(comma + 1);
    }
    return values;
}

bool IsSha256(const std::string& value) {
    return value.size() == 64 && std::all_of(value.begin(), value.end(), [](unsigned char c) {
               return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
           });
}

bool ParseDocument(const std::string& path, tinyxml2::XMLDocument* document, std::string* error) {
    std::string xml;
    if (!android::base::ReadFileToString(path, &xml)) {
        *error = "failed to read " + path;
        return false;
    }
    if (document->Parse(xml.c_str(), xml.size()) != tinyxml2::XML_SUCCESS) {
        *error = document->ErrorStr();
        return false;
    }
    return true;
}

}  // namespace

size_t FeatureEntry::PayloadCount() const {
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

bool FeatureEntry::Allows(DisplayRole role) const {
    return std::find(displays.begin(), displays.end(), role) != displays.end();
}

bool FeatureEntry::AllowsValue(int32_t value) const {
    return allowedValues.empty() ||
           std::find(allowedValues.begin(), allowedValues.end(), value) != allowedValues.end();
}

std::optional<std::vector<FeatureEntry>> LoadFeatureRegistry(const std::string& path,
                                                             std::string* error) {
    tinyxml2::XMLDocument document;
    if (!ParseDocument(path, &document, error)) return std::nullopt;
    const auto* root = document.FirstChildElement("displayPanelFeatures");
    if (root == nullptr) {
        *error = "missing displayPanelFeatures root";
        return std::nullopt;
    }

    std::vector<FeatureEntry> entries;
    for (auto* node = root->FirstChildElement("feature"); node != nullptr;
         node = node->NextSiblingElement("feature")) {
        FeatureEntry entry;
        const auto direction = ParseEnum(node->Attribute("direction"), kDirections);
        const auto shape = ParseEnum(node->Attribute("payload"), kShapes);
        const char* sourceText = node->Attribute("source");
        const auto source = sourceText == nullptr ? std::optional(ValueSource::kNone)
                                                  : ParseEnum(sourceText, kSources);
        if (node->QueryIntAttribute("id", &entry.id) != tinyxml2::XML_SUCCESS || entry.id <= 0 ||
            entry.id > 0xfff || !direction || !shape || !source) {
            *error = "invalid feature registry row";
            return std::nullopt;
        }
        entry.direction = *direction;
        entry.shape = *shape;
        entry.source = *source;
        const char* name = node->Attribute("name");
        const char* property = node->Attribute("property");
        const char* pathText = node->Attribute("path");
        const char* handler = node->Attribute("handler");
        const char* producer = node->Attribute("producer");
        entry.name = name == nullptr ? "" : name;
        entry.property = property == nullptr ? "" : property;
        entry.path = pathText == nullptr ? "" : pathText;
        entry.handler = handler == nullptr ? "" : handler;
        entry.producer = producer == nullptr ? "" : producer;
        if (entry.source == ValueSource::kProperty && entry.property.empty()) {
            *error = "property source is missing its property name";
            return std::nullopt;
        }
        if (entry.source == ValueSource::kSysfsNode && entry.path.empty()) {
            *error = "sysfs-node source is missing its path";
            return std::nullopt;
        }
        node->QueryDoubleAttribute("scale", &entry.scale);
        node->QueryDoubleAttribute("offset", &entry.offset);
        if (const char* status = node->Attribute("status"); status != nullptr) {
            const auto parsed = ParseEnum(status, kStatuses);
            if (!parsed) {
                *error = "invalid status attribute";
                return std::nullopt;
            }
            entry.status = *parsed;
        }
        const auto displays = ParseRoles(node->Attribute("displays"));
        if (!displays) {
            *error = "invalid displays attribute";
            return std::nullopt;
        }
        entry.displays = *displays;
        node->QueryIntAttribute("min", &entry.minimum);
        node->QueryIntAttribute("max", &entry.maximum);
        const auto allowedValues = ParseAllowedValues(node->Attribute("values"));
        if (!allowedValues || entry.minimum > entry.maximum ||
            std::any_of(allowedValues->begin(), allowedValues->end(),
                        [&entry](int32_t value) {
                            return value < entry.minimum || value > entry.maximum;
                        }) ||
            (!allowedValues->empty() && entry.shape != PayloadShape::kScalar)) {
            *error = "invalid feature value domain";
            return std::nullopt;
        }
        entry.allowedValues = *allowedValues;
        entries.push_back(std::move(entry));
    }
    if (entries.empty()) {
        *error = "feature registry is empty";
        return std::nullopt;
    }
    return entries;
}

std::shared_ptr<const FeatureRegistry> FeatureRegistry::Load(const std::string& path,
                                                             std::string* error) {
    tinyxml2::XMLDocument document;
    if (!ParseDocument(path, &document, error)) return nullptr;
    const auto* root = document.FirstChildElement("displayPanelFeatures");
    if (root == nullptr || root->IntAttribute("schemaVersion", 0) != 1) {
        *error = "unsupported feature registry schema";
        return nullptr;
    }
    const char* serviceHash = root->Attribute("serviceSha256");
    if (serviceHash == nullptr || !IsSha256(serviceHash)) {
        *error = "invalid or missing OEM service provenance";
        return nullptr;
    }

    auto registry = std::shared_ptr<FeatureRegistry>(new FeatureRegistry());
    registry->serviceHash_ = serviceHash;
    std::set<int32_t> numericDisplays;
    for (auto* node = root->FirstChildElement("display"); node != nullptr;
         node = node->NextSiblingElement("display")) {
        const auto role = ParseEnum(node->Attribute("role"), kRoles);
        int32_t id = -1;
        const char* aliases = node->Attribute("aliases");
        if (!role || node->QueryIntAttribute("id", &id) != tinyxml2::XML_SUCCESS || id < 0 ||
            id > 0x7ffff || aliases == nullptr || aliases[0] == '\0' ||
            registry->displays_.count(*role) != 0 || !numericDisplays.insert(id).second) {
            *error = "invalid or duplicate display registry row";
            return nullptr;
        }
        registry->displays_[*role] = id;
    }
    if (registry->displays_.empty()) {
        *error = "feature registry has no displays";
        return nullptr;
    }

    auto entries = LoadFeatureRegistry(path, error);
    if (!entries) return nullptr;
    std::set<int32_t> ids;
    size_t setCount = 0;
    size_t getCount = 0;
    for (const auto& entry : *entries) {
        if (!ids.insert(entry.id).second) {
            *error = "duplicate feature id " + std::to_string(entry.id);
            return nullptr;
        }
        if (entry.name.empty() || entry.handler.empty() || entry.producer.empty() ||
            entry.displays.empty()) {
            *error = "feature " + std::to_string(entry.id) + " is missing ownership metadata";
            return nullptr;
        }
        for (DisplayRole role : entry.displays) {
            if (registry->displays_.count(role) == 0) {
                *error = "feature refers to an undeclared display";
                return nullptr;
            }
        }
        setCount += entry.direction != Direction::kGet;
        getCount += entry.direction != Direction::kSet;
    }
    int expectedSet = -1;
    int expectedGet = -1;
    if (root->QueryIntAttribute("setCount", &expectedSet) != tinyxml2::XML_SUCCESS ||
        root->QueryIntAttribute("getCount", &expectedGet) != tinyxml2::XML_SUCCESS ||
        setCount != static_cast<size_t>(expectedSet) || getCount != static_cast<size_t>(expectedGet)) {
        *error = "feature direction counts do not match the pinned registry";
        return nullptr;
    }
    registry->entries_ = std::move(*entries);
    return registry;
}

const FeatureEntry* FeatureRegistry::Find(int32_t id) const {
    const auto entry = std::find_if(entries_.begin(), entries_.end(),
                                    [id](const FeatureEntry& row) { return row.id == id; });
    return entry == entries_.end() ? nullptr : &*entry;
}

const FeatureEntry* FeatureRegistry::Find(const std::string& name) const {
    const auto entry = std::find_if(entries_.begin(), entries_.end(),
                                    [&name](const FeatureEntry& row) { return row.name == name; });
    return entry == entries_.end() ? nullptr : &*entry;
}

std::optional<int32_t> FeatureRegistry::DisplayId(DisplayRole role) const {
    const auto display = displays_.find(role);
    return display == displays_.end() ? std::nullopt : std::optional(display->second);
}

bool FeatureRegistry::ValidateServiceHash(const std::string& actualHash, std::string* error) const {
    if (actualHash != serviceHash_) {
        *error = "registry provenance does not match the serving OEM binary";
        return false;
    }
    return true;
}

std::optional<std::string> Sha256File(const std::string& path) {
    std::string contents;
    if (!android::base::ReadFileToString(path, &contents)) return std::nullopt;
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(contents.data()), contents.size(), digest);
    constexpr char kHex[] = "0123456789abcdef";
    std::string result(SHA256_DIGEST_LENGTH * 2, '0');
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        result[i * 2] = kHex[digest[i] >> 4];
        result[i * 2 + 1] = kHex[digest[i] & 0xf];
    }
    return result;
}

std::optional<std::vector<int32_t>> ParsePropertyPayload(const FeatureEntry& entry,
                                                         const std::string& value) {
    const size_t expected = entry.PayloadCount();
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
        const auto [end, parseError] =
                std::from_chars(token.data(), token.data() + token.size(), parsed, 10);
        if (parseError != std::errc{} || end != token.data() + token.size()) return std::nullopt;
        payload.push_back(parsed);
        if (separator == std::string_view::npos) break;
        remaining.remove_prefix(separator);
    }
    return payload.size() == expected ? std::optional(std::move(payload)) : std::nullopt;
}

std::optional<std::vector<int32_t>> ParseSysfsPayload(const FeatureEntry& entry,
                                                      const std::string& value) {
    if (entry.source != ValueSource::kSysfsNode || entry.PayloadCount() != 1) return std::nullopt;
    std::string_view trimmed(value);
    const auto first = trimmed.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) return std::nullopt;
    trimmed.remove_prefix(first);
    trimmed = trimmed.substr(0, trimmed.find_last_not_of(" \t\r\n") + 1);
    const std::string token(trimmed);
    char* end = nullptr;
    errno = 0;
    const double parsed = std::strtod(token.c_str(), &end);
    if (end != token.c_str() + token.size() || errno == ERANGE) return std::nullopt;
    return std::vector<int32_t>{static_cast<int32_t>(std::lround(parsed * entry.scale + entry.offset))};
}

}  // namespace oplus::displaypanelfeature
