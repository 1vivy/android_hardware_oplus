/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "displaypanelfeature-publisher"

#include "AdfrConfig.h"
#include "AidlPanelFeatureTransport.h"
#include "DisplayPanelFeatureClient.h"
#include "FeatureRegistry.h"
#include "Publisher.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android/binder_process.h>
#include <sys/system_properties.h>

#include <cstdlib>
#include <memory>
#include <string>

namespace oplus::displaypanelfeature {
namespace {

constexpr auto kRegistryPath = "/vendor/etc/display/displaypanelfeature_publisher.xml";
constexpr auto kAdfrConfigPath = "/vendor/etc/display/multimedia_display_adfr2minfps_config.xml";
constexpr auto kOemServicePath =
        "/odm/bin/hw/vendor.oplus.hardware.displaypanelfeature-service";

bool PublishAdfr(const DisplayPanelFeatureClient& client) {
    std::string error;
    int32_t support = 0;
    if (!client.GetScalar(DisplayRole::kPrimary, FeatureId::kAdfrSupport, &support, &error) ||
        support == 0) {
        LOG(ERROR) << "ADFR support probe failed: " << error;
        return false;
    }
    const auto payload = LoadAdfrConfig(kAdfrConfigPath, &error);
    if (!payload) {
        LOG(ERROR) << "ADFR config rejected: " << error;
        return false;
    }
    if (!client.Set(DisplayRole::kPrimary, FeatureId::kAdfrConfig,
                    {payload->begin(), payload->end()}, &error) ||
        !client.Set(DisplayRole::kPrimary, FeatureId::kAdfrControl, {0, (*payload)[2]}, &error)) {
        LOG(ERROR) << "ADFR publish failed: " << error;
        return false;
    }
    return true;
}

void PublishCurrentEvents(const FeatureRegistry& registry, Publisher* publisher) {
    for (const auto& entry : registry.entries()) {
        std::string value;
        bool dispatched = false;
        std::string error;
        if (entry.source == ValueSource::kProperty) {
            value = android::base::GetProperty(entry.property, "");
            dispatched = !value.empty() && publisher->OnProperty(entry.property, value, &error);
        } else if (entry.source == ValueSource::kSysfsNode &&
                   android::base::ReadFileToString(entry.path, &value) && !value.empty()) {
            dispatched = publisher->OnSysfsEvent(entry.path, value, &error);
        } else {
            continue;
        }
        if (!dispatched && !error.empty()) {
            LOG(ERROR) << "producer event for feature " << entry.id << " rejected: " << error;
        }
    }
}

}  // namespace
}  // namespace oplus::displaypanelfeature

int main() {
    using namespace oplus::displaypanelfeature;

    ABinderProcess_setThreadPoolMaxThreadCount(1);
    ABinderProcess_startThreadPool();

    std::string error;
    const auto registry = FeatureRegistry::Load(kRegistryPath, &error);
    if (!registry) {
        LOG(ERROR) << "feature registry rejected: " << error;
        return EXIT_FAILURE;
    }
    const auto serviceHash = Sha256File(kOemServicePath);
    if (!serviceHash || !registry->ValidateServiceHash(*serviceHash, &error)) {
        LOG(ERROR) << "OEM service provenance rejected: " << error;
        return EXIT_FAILURE;
    }
    const DisplayPanelFeatureClient client(registry, CreateAidlPanelFeatureTransport());
    if (!PublishAdfr(client)) return EXIT_FAILURE;

    Publisher publisher(registry, client);
    PublishCurrentEvents(*registry, &publisher);
    uint32_t serial = 0;
    while (__system_property_wait(nullptr, serial, &serial, nullptr)) {
        PublishCurrentEvents(*registry, &publisher);
    }
    LOG(ERROR) << "property event subscription failed";
    return EXIT_FAILURE;
}
