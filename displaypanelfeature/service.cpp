/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "oplus-dpf-service"

#include "PanelBackend.h"
#include "Server.h"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <cstdlib>

namespace {

constexpr auto kRegistryPath = "/vendor/etc/display/displaypanelfeature_publisher.xml";
constexpr auto kPanelDevice = "/dev/oplus_display";
constexpr auto kInstance = "vendor.oplus.hardware.displaypanelfeature.IDisplayPanelFeature/default";

}  // namespace

int main() {
    using namespace oplus::displaypanelfeature;

    std::string error;
    const auto registry = FeatureRegistry::Load(kRegistryPath, &error);
    if (!registry) {
        // Refuse rather than serve an empty map: registering the default
        // instance with nothing behind it would answer every client with a
        // silent failure that looks exactly like a working service.
        LOG(FATAL) << "feature registry rejected: " << error;
        return EXIT_FAILURE;
    }

    ABinderProcess_setThreadPoolMaxThreadCount(4);
    const auto service = ndk::SharedRefBase::make<DisplayPanelFeatureServer>(
            registry, CreateIoctlPanelBackend(kPanelDevice));
    if (AServiceManager_addService(service->asBinder().get(), kInstance) != STATUS_OK) {
        LOG(FATAL) << "cannot register " << kInstance;
        return EXIT_FAILURE;
    }

    LOG(INFO) << "serving " << kInstance << " with " << registry->entries().size()
              << " registry rows";
    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;  // joinThreadPool only returns on teardown.
}
