/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstdint>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {
namespace panel {

// Feature IDs read out of the vendor displaypanelfeature service's own dispatch
// switch at 0x00124024 (set) and 0x0012496c (get); see the canonical record
// rec:dpf-dispatch-full-enumeration. The service ioctls /dev/oplus_display on our
// behalf and falls back to the matching /sys/kernel/oplus_display node per
// feature, so a caller never opens the panel device itself.
enum FeatureId : int32_t {
    kSeed = 4,
    kHbm = 12,
    kDimAlpha = 13,
    kDimDcAlpha = 14,
    kDimlayerBlEn = 23,
    kCabc = 184,
    kPwmTurbo = 199,
    kPwmPulse = 201,
};

bool Get(int32_t featureId, int32_t* value);
bool Set(int32_t featureId, int32_t value);

}  // namespace panel
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
