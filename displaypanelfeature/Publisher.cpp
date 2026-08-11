/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */
#include "Publisher.h"

namespace oplus::displaypanelfeature {

Publisher::Publisher(std::shared_ptr<const FeatureRegistry> registry,
                     const DisplayPanelFeatureClient& client)
    : registry_(std::move(registry)), client_(client) {}

bool Publisher::OnProperty(const std::string& name, const std::string& value, std::string* error) {
    return OnEvent(ValueSource::kProperty, name, value, error);
}

bool Publisher::OnSysfsEvent(const std::string& path, const std::string& value, std::string* error) {
    return OnEvent(ValueSource::kSysfsNode, path, value, error);
}

bool Publisher::OnEvent(ValueSource source, const std::string& key, const std::string& value,
                        std::string* error) {
    if (published_[key] == value) return true;
    for (const auto& entry : registry_->entries()) {
        const std::string& configured = source == ValueSource::kProperty ? entry.property : entry.path;
        if (entry.source != source || configured != key) continue;
        if (entry.status != RowStatus::kActive || entry.direction == Direction::kGet) {
            *error = "producer event targets a non-writable feature";
            return false;
        }
        const auto payload = source == ValueSource::kProperty ? ParsePropertyPayload(entry, value)
                                                               : ParseSysfsPayload(entry, value);
        if (!payload) {
            *error = "producer event has an invalid payload";
            return false;
        }
        if (!client_.Set(DisplayRole::kPrimary, static_cast<FeatureId>(entry.id), *payload, error)) {
            return false;
        }
        published_[key] = value;
        return true;
    }
    *error = "producer event is not declared";
    return false;
}

}  // namespace oplus::displaypanelfeature
