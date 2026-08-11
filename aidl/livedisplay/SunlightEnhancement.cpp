/*
 * SPDX-FileCopyrightText: 2022-2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "SunlightEnhancementService"

#include <android-base/logging.h>
#include <livedisplay/oplus/SunlightEnhancement.h>
#include <livedisplay/oplus/PanelFeature.h>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {

ndk::ScopedAStatus SunlightEnhancement::getEnabled(bool* _aidl_return) {
    int32_t value = 0;
    if (!panel::Get(panel::FeatureId::kHbm, &value)) {
        LOG(ERROR) << "Failed to read current SunlightEnhancement state";
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }
    *_aidl_return = value > 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus SunlightEnhancement::setEnabled(bool enabled) {
    bool isEnabled;
    if (auto status = getEnabled(&isEnabled); !status.isOk()) {
        return status;
    }
    if (isEnabled != enabled && !panel::Set(panel::FeatureId::kHbm, enabled)) {
        LOG(ERROR) << "Failed to set SunlightEnhancement state";
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
