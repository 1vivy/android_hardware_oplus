/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <aidl/vendor/lineage/health/BnChargingControl.h>

#include <cstdint>

namespace aidl::vendor::lineage::health {

class ChargingControl : public BnChargingControl {
  public:
    ndk::ScopedAStatus getChargingEnabled(bool* aidl_return) override;
    ndk::ScopedAStatus setChargingEnabled(bool enabled) override;
    ndk::ScopedAStatus setChargingDeadline(int64_t deadline) override;
    ndk::ScopedAStatus getSupportedMode(int32_t* aidl_return) override;
    ndk::ScopedAStatus getChargingDeadline(int64_t* aidl_return) override;
    ndk::ScopedAStatus getChargingLimit(ChargingLimitInfo* aidl_return) override;
    ndk::ScopedAStatus setChargingLimit(const ChargingLimitInfo& limit) override;

    binder_status_t dump(int fd, const char** args, uint32_t num_args) override;
};

}  // namespace aidl::vendor::lineage::health
