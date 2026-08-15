/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "FeatureRegistry.h"
#include "PanelBackend.h"

#include <cstdint>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {

// Why a request was refused. The interface itself reports only its int result,
// because every opaque stock client in the consumer census compares that result
// with zero and nothing else; this enum exists for the diagnostic log and for
// tests, and deliberately never reaches the wire.
enum class Refusal {
    kNone,
    kUnknownFeature,
    kReservedFeature,
    kUnknownDisplay,
    kDisplayNotAllowed,
    kWrongDirection,
    kBadPayloadCount,
    kValueOutOfRange,
    kBackendFailed,
};

const char* RefusalName(Refusal refusal);

struct Admission {
    const FeatureEntry* entry = nullptr;
    DisplayRole role = DisplayRole::kPrimary;
    int32_t displayId = 0;
};

// Deny by default. A request is admitted only when the GENERATED registry
// proves every one of feature, display, direction, payload shape and value
// range; anything else returns a reason and no admission. Callers must not
// construct backend work before this returns kNone - that ordering is what
// makes "a rejected request performs zero backend calls" observable.
Refusal Admit(const FeatureRegistry& registry, int32_t packedFeatureId, bool set,
              const std::vector<int32_t>& values, Admission* admission);

// The three dispatch entry points. Each admits first and touches the backend
// only afterwards, so a refused request leaves the backend untouched.
Refusal DispatchGet(const FeatureRegistry& registry, PanelBackend& backend, int32_t packedFeatureId,
                    std::vector<int32_t>* values, std::string* detail);
Refusal DispatchSet(const FeatureRegistry& registry, PanelBackend& backend, int32_t packedFeatureId,
                    const std::vector<int32_t>& values, std::string* detail);
Refusal DispatchInfo(const FeatureRegistry& registry, PanelBackend& backend,
                     int32_t packedFeatureId, std::vector<std::string>* info, std::string* detail);

}  // namespace oplus::displaypanelfeature
