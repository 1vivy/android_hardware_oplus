/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ChargingControl.h"
#include "FastCharge.h"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <cstdlib>
#include <memory>
#include <string>

using aidl::vendor::lineage::health::ChargingControl;
using aidl::vendor::lineage::health::FastCharge;

namespace {

template <typename Service>
void Register(const std::shared_ptr<Service>& service) {
    const std::string instance = std::string(Service::descriptor) + "/default";
    CHECK_EQ(AServiceManager_addService(service->asBinder().get(), instance.c_str()), STATUS_OK);
}

}  // namespace

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(0);
    Register(ndk::SharedRefBase::make<ChargingControl>());
    Register(ndk::SharedRefBase::make<FastCharge>());
    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;
}
