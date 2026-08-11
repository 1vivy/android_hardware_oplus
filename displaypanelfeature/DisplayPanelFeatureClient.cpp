/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DisplayPanelFeatureClient.h"

#include <limits>

namespace oplus::displaypanelfeature {

DisplayPanelFeatureClient::DisplayPanelFeatureClient(
        std::shared_ptr<const FeatureRegistry> registry,
        std::shared_ptr<PanelFeatureTransport> transport)
    : registry_(std::move(registry)), transport_(std::move(transport)) {}

bool DisplayPanelFeatureClient::Prepare(DisplayRole display, FeatureId feature, bool set,
                                        const std::vector<int32_t>& values, int32_t* packedId,
                                        std::string* error) const {
    const int32_t id = static_cast<int32_t>(feature);
    if (id < 1 || id > 0xfff) {
        *error = "feature id is outside the base-id contract";
        return false;
    }
    const FeatureEntry* entry = registry_->Find(id);
    if (entry == nullptr) {
        *error = "feature " + std::to_string(id) + " is not declared";
        return false;
    }
    if (entry->status != RowStatus::kActive) {
        *error = "feature " + std::to_string(id) + " is reserved";
        return false;
    }
    if (set && entry->direction == Direction::kGet) {
        *error = "feature " + std::to_string(id) + " does not allow set";
        return false;
    }
    if (!set && entry->direction == Direction::kSet) {
        *error = "feature " + std::to_string(id) + " does not allow get";
        return false;
    }
    const size_t expected = entry->PayloadCount();
    if (set && expected != 0 && values.size() != expected) {
        *error = "feature " + std::to_string(id) + " payload has the wrong shape";
        return false;
    }
    if (set && entry->shape == PayloadShape::kScalar &&
        (values[0] < entry->minimum || values[0] > entry->maximum)) {
        *error = "feature " + std::to_string(id) + " payload is outside the declared range";
        return false;
    }
    const auto numericDisplay = registry_->DisplayId(display);
    if (!numericDisplay || !entry->Allows(display)) {
        *error = "feature " + std::to_string(id) + " is not allowed on the requested display";
        return false;
    }
    if (*numericDisplay < 0 || *numericDisplay > (std::numeric_limits<int32_t>::max() >> 12)) {
        *error = "display id is outside the packed-id contract";
        return false;
    }
    *packedId = (*numericDisplay << 12) | id;
    return true;
}

bool DisplayPanelFeatureClient::Get(DisplayRole display, FeatureId feature,
                                    std::vector<int32_t>* values, std::string* error) const {
    int32_t packedId;
    if (!Prepare(display, feature, false, {}, &packedId, error)) return false;
    return transport_->Get(packedId, values, error);
}

bool DisplayPanelFeatureClient::GetScalar(DisplayRole display, FeatureId feature, int32_t* value,
                                          std::string* error) const {
    std::vector<int32_t> values{0};
    if (!Get(display, feature, &values, error) || values.empty()) return false;
    *value = values.front();
    return true;
}

bool DisplayPanelFeatureClient::Set(DisplayRole display, FeatureId feature,
                                    const std::vector<int32_t>& values, std::string* error) const {
    int32_t packedId;
    if (!Prepare(display, feature, true, values, &packedId, error)) return false;
    return transport_->Set(packedId, values, error);
}

bool DisplayPanelFeatureClient::SetScalar(DisplayRole display, FeatureId feature, int32_t value,
                                          std::string* error) const {
    return Set(display, feature, {value}, error);
}

}  // namespace oplus::displaypanelfeature
