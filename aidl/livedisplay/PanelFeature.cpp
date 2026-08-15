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

std::shared_ptr<::oplus::displaypanelfeature::DisplayPanelFeatureClient> Client() {
    static const auto client = [] {
        std::string error;
        auto registry = ::oplus::displaypanelfeature::FeatureRegistry::Load(kRegistry, &error);
        if (!registry) {
            LOG(ERROR) << "DPF registry rejected: " << error;
            return std::shared_ptr<::oplus::displaypanelfeature::DisplayPanelFeatureClient>();
        }
        // The registry's serviceSha256 records which stock binary the map was
        // DERIVED from; it is provenance for the derivation, not a runtime
        // dependency. The default instance is served from this tree now and the
        // ODM binary is no longer installed, so hashing it here would null this
        // client and take every LiveDisplay panel feature down with it.
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

bool SetVerified(FeatureId feature, int32_t value) {
    const auto client = Client();
    if (!client) return false;
    std::string error;
    if (!client->SetScalarVerified(::oplus::displaypanelfeature::DisplayRole::kPrimary, feature,
                                   value, &error)) {
        LOG(ERROR) << "DPF verified set failed: " << error;
        return false;
    }
    return true;
}

}  // namespace aidl::vendor::lineage::livedisplay::panel
