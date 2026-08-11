/*
 * SPDX-FileCopyrightText: 2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "AdaptiveBacklightService"

#include <android-base/logging.h>
#include <livedisplay/oplus/AdaptiveBacklight.h>
#include <livedisplay/oplus/PanelFeature.h>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {

ndk::ScopedAStatus AdaptiveBacklight::getEnabled(bool* _aidl_return) {
    int32_t value = 0;
    if (!panel::Get(panel::FeatureId::kCabc, &value)) {
        LOG(ERROR) << "Failed to read current AdaptiveBacklight state";
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }
    *_aidl_return = value > 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus AdaptiveBacklight::setEnabled(bool enabled) {
    bool isEnabled;
    if (auto status = getEnabled(&isEnabled); !status.isOk()) {
        return status;
    }
    if (isEnabled != enabled && !panel::Set(panel::FeatureId::kCabc, enabled)) {
        LOG(ERROR) << "Failed to set AdaptiveBacklight state";
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
