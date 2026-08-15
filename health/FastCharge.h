/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <aidl/vendor/lineage/health/BnFastCharge.h>

#include <cstdint>

namespace aidl::vendor::lineage::health {

class FastCharge : public BnFastCharge {
  public:
    ndk::ScopedAStatus getSupportedFastChargeModes(int64_t* aidl_return) override;
    ndk::ScopedAStatus getFastChargeMode(FastChargeMode* aidl_return) override;
    ndk::ScopedAStatus setFastChargeMode(FastChargeMode mode, FastChargeMode* aidl_return) override;

    binder_status_t dump(int fd, const char** args, uint32_t num_args) override;
};

}  // namespace aidl::vendor::lineage::health
