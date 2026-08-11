/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "livedisplay-panelfeature"

#include <livedisplay/oplus/PanelFeature.h>

#include <AidlPanelFeatureTransport.h>
#include <FeatureRegistry.h>
#include <android-base/logging.h>

#include <memory>
#include <string>

namespace aidl::vendor::lineage::livedisplay::panel {
namespace {

constexpr const char* kRegistry = "/vendor/etc/display/displaypanelfeature_publisher.xml";
constexpr const char* kOemService =
        "/odm/bin/hw/vendor.oplus.hardware.displaypanelfeature-service";

std::shared_ptr<::oplus::displaypanelfeature::DisplayPanelFeatureClient> Client() {
    static const auto client = [] {
        std::string error;
        auto registry = ::oplus::displaypanelfeature::FeatureRegistry::Load(kRegistry, &error);
        if (!registry) {
            LOG(ERROR) << "DPF registry rejected: " << error;
            return std::shared_ptr<::oplus::displaypanelfeature::DisplayPanelFeatureClient>();
        }
        const auto serviceHash = ::oplus::displaypanelfeature::Sha256File(kOemService);
        if (!serviceHash || !registry->ValidateServiceHash(*serviceHash, &error)) {
            LOG(ERROR) << "DPF provenance rejected: " << error;
            return std::shared_ptr<::oplus::displaypanelfeature::DisplayPanelFeatureClient>();
        }
        return std::make_shared<::oplus::displaypanelfeature::DisplayPanelFeatureClient>(
                std::move(registry),
                ::oplus::displaypanelfeature::CreateAidlPanelFeatureTransport());
    }();
    return client;
}

}  // namespace

bool Get(FeatureId feature, int32_t* value) {
    const auto client = Client();
    if (!client) return false;
    std::string error;
    if (!client->GetScalar(::oplus::displaypanelfeature::DisplayRole::kPrimary, feature, value,
                           &error)) {
        LOG(ERROR) << "DPF get failed: " << error;
        return false;
    }
    return true;
}

bool Set(FeatureId feature, int32_t value) {
    const auto client = Client();
    if (!client) return false;
    std::string error;
    if (!client->SetScalar(::oplus::displaypanelfeature::DisplayRole::kPrimary, feature, value,
                           &error)) {
        LOG(ERROR) << "DPF set failed: " << error;
        return false;
    }
    return true;
}

}  // namespace aidl::vendor::lineage::livedisplay::panel
