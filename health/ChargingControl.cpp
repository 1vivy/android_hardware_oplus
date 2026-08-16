/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.lineage.health-service.oplus"

#include "ChargingControl.h"

#include <aidl/vendor/lineage/health/ChargingControlSupportedMode.h>
#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>

#include <cstdio>
#include <string>

namespace aidl::vendor::lineage::health {
namespace {

// This measured node gates battery charging. It implements a charge hold; it
// does not prove that external power electrically bypasses the battery.
constexpr const char* kChargingEnabledPath = "/sys/class/oplus_chg/battery/mmi_charging_enable";
constexpr char kChargingEnabled[] = "1";
constexpr char kChargingDisabled[] = "0";

ndk::ScopedAStatus Unsupported() {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

}  // namespace

ndk::ScopedAStatus ChargingControl::getChargingEnabled(bool* aidl_return) {
    std::string value;
    if (!android::base::ReadFileToString(kChargingEnabledPath, &value, true)) {
        PLOG(ERROR) << "Failed to read charge-hold state";
        return Unsupported();
    }

    value = android::base::Trim(value);
    if (value == kChargingEnabled) {
        *aidl_return = true;
    } else if (value == kChargingDisabled) {
        *aidl_return = false;
    } else {
        LOG(ERROR) << "Unknown charge-hold state: " << value;
        return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus ChargingControl::setChargingEnabled(bool enabled) {
    bool current;
    auto status = getChargingEnabled(&current);
    if (!status.isOk()) {
        return status;
    }
    if (current == enabled) {
        return ndk::ScopedAStatus::ok();
    }

    const std::string value = enabled ? kChargingEnabled : kChargingDisabled;
    if (!android::base::WriteStringToFile(value, kChargingEnabledPath, true)) {
        PLOG(ERROR) << "Failed to write charge-hold state";
        return Unsupported();
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus ChargingControl::setChargingDeadline(int64_t) {
    return Unsupported();
}

ndk::ScopedAStatus ChargingControl::getSupportedMode(int32_t* aidl_return) {
    *aidl_return = static_cast<int32_t>(ChargingControlSupportedMode::TOGGLE);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus ChargingControl::getChargingDeadline(int64_t*) {
    return Unsupported();
}

ndk::ScopedAStatus ChargingControl::getChargingLimit(ChargingLimitInfo*) {
    return Unsupported();
}

ndk::ScopedAStatus ChargingControl::setChargingLimit(const ChargingLimitInfo&) {
    return Unsupported();
}

binder_status_t ChargingControl::dump(int fd, const char**, uint32_t) {
    bool enabled;
    const auto status = getChargingEnabled(&enabled);
    dprintf(fd, "Charging control node: %s\n", kChargingEnabledPath);
    dprintf(fd, "Charge hold supported: true\n");
    dprintf(fd, "Electrical bypass supported: false\n");
    dprintf(fd, "Charging enabled: %s\n",
            status.isOk() ? (enabled ? "true" : "false") : "unavailable");
    return STATUS_OK;
}

}  // namespace aidl::vendor::lineage::health
