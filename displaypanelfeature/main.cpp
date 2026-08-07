/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "displaypanelfeature-publisher"

#include "AdfrConfig.h"
#include "FeatureRegistry.h"

#include <aidl/vendor/oplus/hardware/displaypanelfeature/IDisplayPanelFeature.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <chrono>
#include <map>
#include <thread>

using aidl::vendor::oplus::hardware::displaypanelfeature::IDisplayPanelFeature;
using namespace std::chrono_literals;

namespace oplus::displaypanelfeature {
namespace {

constexpr auto kService = "vendor.oplus.hardware.displaypanelfeature.IDisplayPanelFeature/default";
constexpr auto kRegistryPath = "/vendor/etc/display/displaypanelfeature_publisher.xml";
constexpr auto kAdfrConfigPath = "/vendor/etc/display/multimedia_display_adfr2minfps_config.xml";

bool SetFeature(const std::shared_ptr<IDisplayPanelFeature>& service, int32_t id,
                const std::vector<int32_t>& payload) {
    int32_t result = -1;
    const auto status = service->setDisplayPanelFeatureValue(id, payload, &result);
    if (!status.isOk() || result != 0) {
        LOG(ERROR) << "set feature " << id << " failed: binder=" << status.getDescription()
                   << " result=" << result;
        return false;
    }
    return true;
}

void ProbeFeature(const std::shared_ptr<IDisplayPanelFeature>& service, int32_t id) {
    std::vector<int32_t> values{0};
    int32_t result = -1;
    const auto status = service->getDisplayPanelFeatureValue(id, &values, &result);
    if (!status.isOk() || result != 0) {
        LOG(WARNING) << "get feature " << id << " unavailable";
    }
}

bool PublishAdfr(const std::shared_ptr<IDisplayPanelFeature>& service) {
    std::vector<int32_t> support{0};
    int32_t result = -1;
    const auto status = service->getDisplayPanelFeatureValue(233, &support, &result);
    if (!status.isOk() || result != 0 || support.empty() || support[0] == 0) {
        LOG(ERROR) << "ADFR support probe failed";
        return false;
    }

    std::string error;
    const auto payload = LoadAdfrConfig(kAdfrConfigPath, &error);
    if (!payload) {
        LOG(ERROR) << "ADFR config rejected: " << error;
        return false;
    }
    if (!SetFeature(service, 234, {payload->begin(), payload->end()})) return false;
    return SetFeature(service, 232, {0, (*payload)[2]});
}

}  // namespace
}  // namespace oplus::displaypanelfeature

int main() {
    using namespace oplus::displaypanelfeature;

    ABinderProcess_setThreadPoolMaxThreadCount(1);
    ABinderProcess_startThreadPool();
    const auto service = IDisplayPanelFeature::fromBinder(
            ndk::SpAIBinder(AServiceManager_waitForService(kService)));
    if (!service) {
        LOG(ERROR) << "displaypanelfeature service unavailable";
        return EXIT_FAILURE;
    }

    std::string error;
    const auto registry = LoadFeatureRegistry(kRegistryPath, &error);
    if (!registry) {
        LOG(ERROR) << "feature registry rejected: " << error;
        return EXIT_FAILURE;
    }
    if (!PublishAdfr(service)) return EXIT_FAILURE;

    for (const auto& entry : *registry) {
        if (entry.direction != Direction::kSet) ProbeFeature(service, entry.id);
    }

    std::map<std::string, std::string> published;
    while (true) {
        for (const auto& entry : *registry) {
            if (entry.source != ValueSource::kProperty || entry.direction == Direction::kGet) {
                continue;
            }
            const auto value = android::base::GetProperty(entry.property, "");
            if (value.empty() || published[entry.property] == value) continue;
            const auto payload = ParsePropertyPayload(entry, value);
            if (!payload) {
                LOG(ERROR) << "invalid payload in " << entry.property;
                continue;
            }
            if (SetFeature(service, entry.id, *payload)) published[entry.property] = value;
        }
        std::this_thread::sleep_for(1s);
    }
}
