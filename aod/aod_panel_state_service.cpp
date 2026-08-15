/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "oplus-aod-panel-state"

#include "AodPanelStateService.h"

#include <AidlPanelFeatureTransport.h>
#include <PanelWriterClient.h>
#include <aidl/vendor/oplus/hardware/aodpanelstate/IAodPanelStateService.h>
#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <cstdlib>
#include <memory>
#include <string>
#include <utility>

namespace {

using aidl::vendor::oplus::hardware::aodpanelstate::IAodPanelStateService;
using oplus::aod::AodPanelPolicy;

constexpr char kDpfInstance[] =
        "vendor.oplus.hardware.displaypanelfeature.IDisplayPanelFeature/default";
const std::string kAodInstance = std::string(IAodPanelStateService::descriptor) + "/default";

AServiceManager_NotificationRegistration* gDpfRegistration = nullptr;

void OnDpfRegistered(const char*, AIBinder*, void* cookie) {
    auto* policy = static_cast<AodPanelPolicy*>(cookie);
    std::string error;
    if (!policy->Replay(&error)) {
        LOG(ERROR) << "cannot replay AOD panel state after DPF registration: " << error;
    }
}

}  // namespace

int main() {
    ndk::SpAIBinder dpf(AServiceManager_waitForService(kDpfInstance));
    if (!dpf.get()) {
        LOG(FATAL) << "declared DPF service did not become available";
        return EXIT_FAILURE;
    }

    std::string error;
    auto client = oplus::displaypanelfeature::CreateAidlDisplayPanelFeatureClient(&error);
    if (!client) {
        LOG(FATAL) << "cannot create typed DPF client: " << error;
        return EXIT_FAILURE;
    }
    auto writer =
            std::make_shared<oplus::displaypanelfeature::PanelWriterClient>(std::move(client));
    auto policy = std::make_shared<AodPanelPolicy>(std::move(writer));

    ABinderProcess_setThreadPoolMaxThreadCount(2);
    auto service = ndk::SharedRefBase::make<oplus::aod::AodPanelStateService>(policy);
    if (AServiceManager_addService(service->asBinder().get(), kAodInstance.c_str()) != STATUS_OK) {
        LOG(FATAL) << "cannot register " << kAodInstance;
        return EXIT_FAILURE;
    }

    gDpfRegistration = AServiceManager_registerForServiceNotifications(
            kDpfInstance, OnDpfRegistered, policy.get());
    if (gDpfRegistration == nullptr) {
        LOG(FATAL) << "cannot subscribe to DPF service registration";
        return EXIT_FAILURE;
    }

    LOG(INFO) << "serving " << kAodInstance;
    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;
}
