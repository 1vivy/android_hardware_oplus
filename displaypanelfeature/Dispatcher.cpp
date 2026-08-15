/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Dispatcher.h"

#include <cstdint>
#include <optional>

namespace oplus::displaypanelfeature {
namespace {

// The packing the stock dispatch computes: both stock entry points derive
// base_id = packed & 0xfff and display_id = packed >> 12, and switch only on
// the base id. Reproduced so opaque clients keep their existing encoding.
constexpr uint32_t kBaseMask = 0xfff;
constexpr int kDisplayShift = 12;

bool DirectionAllows(Direction direction, bool set) {
    if (direction == Direction::kBoth) return true;
    return set ? direction == Direction::kSet : direction == Direction::kGet;
}

}  // namespace

const char* RefusalName(Refusal refusal) {
    switch (refusal) {
        case Refusal::kNone:
            return "admitted";
        case Refusal::kUnknownFeature:
            return "unknown-feature";
        case Refusal::kReservedFeature:
            return "reserved-feature";
        case Refusal::kUnknownDisplay:
            return "unknown-display";
        case Refusal::kDisplayNotAllowed:
            return "display-not-allowed";
        case Refusal::kWrongDirection:
            return "wrong-direction";
        case Refusal::kBadPayloadCount:
            return "bad-payload-count";
        case Refusal::kValueOutOfRange:
            return "value-out-of-range";
        case Refusal::kBackendFailed:
            return "backend-failed";
    }
    return "unknown";
}

Refusal Admit(const FeatureRegistry& registry, int32_t packedFeatureId, bool set,
              const std::vector<int32_t>& values, Admission* admission) {
    const auto packed = static_cast<uint32_t>(packedFeatureId);
    const auto base = static_cast<int32_t>(packed & kBaseMask);
    const auto display = static_cast<int32_t>(packed >> kDisplayShift);

    const FeatureEntry* entry = registry.Find(base);
    if (entry == nullptr) return Refusal::kUnknownFeature;
    if (entry->status != RowStatus::kActive) return Refusal::kReservedFeature;

    std::optional<DisplayRole> role;
    for (const auto candidate : {DisplayRole::kPrimary, DisplayRole::kSecondary}) {
        const auto declared = registry.DisplayId(candidate);
        if (declared && *declared == display) {
            role = candidate;
            break;
        }
    }
    if (!role) return Refusal::kUnknownDisplay;
    if (!entry->Allows(*role)) return Refusal::kDisplayNotAllowed;
    if (!DirectionAllows(entry->direction, set)) return Refusal::kWrongDirection;

    if (set) {
        const size_t expected = entry->PayloadCount();
        // A shape with no fixed count is still not a licence to write nothing:
        // an empty payload names no value at all, so it can never be a legal set.
        if (expected == 0 ? values.empty() : values.size() != expected) {
            return Refusal::kBadPayloadCount;
        }
        for (const int32_t value : values) {
            if (value < entry->minimum || value > entry->maximum) {
                return Refusal::kValueOutOfRange;
            }
        }
    }

    *admission = Admission{entry, *role, display};
    return Refusal::kNone;
}

Refusal DispatchGet(const FeatureRegistry& registry, PanelBackend& backend, int32_t packedFeatureId,
                    std::vector<int32_t>* values, std::string* detail) {
    Admission admission;
    const Refusal refusal = Admit(registry, packedFeatureId, false, {}, &admission);
    if (refusal != Refusal::kNone) return refusal;
    if (!backend.Get(*admission.entry, admission.displayId, values, detail)) {
        return Refusal::kBackendFailed;
    }
    return Refusal::kNone;
}

Refusal DispatchSet(const FeatureRegistry& registry, PanelBackend& backend, int32_t packedFeatureId,
                    const std::vector<int32_t>& values, std::string* detail) {
    Admission admission;
    const Refusal refusal = Admit(registry, packedFeatureId, true, values, &admission);
    if (refusal != Refusal::kNone) return refusal;
    if (!backend.Set(*admission.entry, admission.displayId, values, detail)) {
        return Refusal::kBackendFailed;
    }
    return Refusal::kNone;
}

Refusal DispatchInfo(const FeatureRegistry& registry, PanelBackend& backend,
                     int32_t packedFeatureId, std::vector<std::string>* info, std::string* detail) {
    Admission admission;
    const Refusal refusal = Admit(registry, packedFeatureId, false, {}, &admission);
    if (refusal != Refusal::kNone) return refusal;
    if (!backend.Info(*admission.entry, admission.displayId, info, detail)) {
        return Refusal::kBackendFailed;
    }
    return Refusal::kNone;
}

}  // namespace oplus::displaypanelfeature
