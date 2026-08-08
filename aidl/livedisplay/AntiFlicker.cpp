/*
 * SPDX-FileCopyrightText: 2022-2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "AntiFlickerService"

#include <android-base/logging.h>
#include <livedisplay/oplus/AntiFlicker.h>
#include <livedisplay/oplus/PanelFeature.h>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {

ndk::ScopedAStatus AntiFlicker::getEnabled(bool* _aidl_return) {
    int32_t value = 0;
    if (!panel::Get(panel::kPwmPulse, &value) && !panel::Get(panel::kPwmTurbo, &value) &&
        !panel::Get(panel::kDimlayerBlEn, &value)) {
        LOG(ERROR) << "Failed to read current AntiFlicker state";
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }
    *_aidl_return = value > 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus AntiFlicker::setEnabled(bool enabled) {
    bool isEnabled;
    if (auto status = getEnabled(&isEnabled); !status.isOk()) {
        return status;
    }
    if (isEnabled != enabled && !panel::Set(panel::kPwmPulse, enabled) &&
        !panel::Set(panel::kPwmTurbo, enabled) && !panel::Set(panel::kDimlayerBlEn, enabled)) {
        LOG(ERROR) << "Failed to set AntiFlicker state";
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
