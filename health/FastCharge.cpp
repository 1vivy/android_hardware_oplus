/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FastCharge.h"

#include <cstdio>

namespace aidl::vendor::lineage::health {

ndk::ScopedAStatus FastCharge::getSupportedFastChargeModes(int64_t* aidl_return) {
    *aidl_return = 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus FastCharge::getFastChargeMode(FastChargeMode*) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus FastCharge::setFastChargeMode(FastChargeMode, FastChargeMode*) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

binder_status_t FastCharge::dump(int fd, const char**, uint32_t) {
    dprintf(fd, "Fast charge selection supported: false\n");
    return STATUS_OK;
}

}  // namespace aidl::vendor::lineage::health
