/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "livedisplay-panelfeature"

#include <livedisplay/oplus/PanelFeature.h>

#include <aidl/vendor/oplus/hardware/displaypanelfeature/IDisplayPanelFeature.h>
#include <android-base/logging.h>
#include <android/binder_manager.h>

#include <memory>
#include <vector>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {
namespace panel {

namespace {

using ::aidl::vendor::oplus::hardware::displaypanelfeature::IDisplayPanelFeature;

constexpr const char* kService =
        "vendor.oplus.hardware.displaypanelfeature.IDisplayPanelFeature/default";

// Must stay non-blocking and must not cache a failure: these run from HAL
// constructors, before this process registers its own services.
std::shared_ptr<IDisplayPanelFeature> Service() {
    static std::shared_ptr<IDisplayPanelFeature> service;
    if (service == nullptr) {
        service = IDisplayPanelFeature::fromBinder(
                ndk::SpAIBinder(AServiceManager_checkService(kService)));
    }
    return service;
}

}  // namespace

bool Get(int32_t featureId, int32_t* value) {
    const auto service = Service();
    if (service == nullptr) {
        LOG(ERROR) << "displaypanelfeature unavailable, cannot read feature " << featureId;
        return false;
    }

    std::vector<int32_t> values{0};
    int32_t result = -1;
    const auto status = service->getDisplayPanelFeatureValue(featureId, &values, &result);
    if (!status.isOk() || result != 0 || values.empty()) {
        return false;
    }

    *value = values.front();
    return true;
}

bool Set(int32_t featureId, int32_t value) {
    const auto service = Service();
    if (service == nullptr) {
        LOG(ERROR) << "displaypanelfeature unavailable, feature " << featureId << " not applied";
        return false;
    }

    int32_t result = -1;
    const auto status = service->setDisplayPanelFeatureValue(featureId, {value}, &result);
    if (!status.isOk() || result != 0) {
        LOG(ERROR) << "feature " << featureId << " value " << value
                   << " rejected: binder=" << status.getDescription() << " result=" << result;
        return false;
    }
    return true;
}

}  // namespace panel
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
