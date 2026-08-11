/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "oplus-dpf-client"

#include "AidlPanelFeatureTransport.h"

#include <aidl/vendor/oplus/hardware/displaypanelfeature/IDisplayPanelFeature.h>
#include <android/binder_manager.h>

#include <mutex>

namespace oplus::displaypanelfeature {
namespace {

using aidl::vendor::oplus::hardware::displaypanelfeature::IDisplayPanelFeature;
constexpr const char* kService =
        "vendor.oplus.hardware.displaypanelfeature.IDisplayPanelFeature/default";

class AidlTransport final : public PanelFeatureTransport {
  public:
    bool Get(int32_t packedId, std::vector<int32_t>* values, std::string* error) override {
        const auto service = Service();
        if (!service) {
            *error = "displaypanelfeature service is unavailable";
            return false;
        }
        int32_t result = -1;
        const auto status = service->getDisplayPanelFeatureValue(packedId, values, &result);
        if (!status.isOk() || result != 0) {
            *error = "get rejected: binder=" + status.getDescription() +
                    " result=" + std::to_string(result);
            Invalidate(status);
            return false;
        }
        return true;
    }

    bool Set(int32_t packedId, const std::vector<int32_t>& values, std::string* error) override {
        const auto service = Service();
        if (!service) {
            *error = "displaypanelfeature service is unavailable";
            return false;
        }
        int32_t result = -1;
        const auto status = service->setDisplayPanelFeatureValue(packedId, values, &result);
        if (!status.isOk() || result != 0) {
            *error = "set rejected: binder=" + status.getDescription() +
                    " result=" + std::to_string(result);
            Invalidate(status);
            return false;
        }
        return true;
    }

  private:
    std::shared_ptr<IDisplayPanelFeature> Service() {
        std::lock_guard lock(mutex_);
        if (!service_) {
            service_ = IDisplayPanelFeature::fromBinder(
                    ndk::SpAIBinder(AServiceManager_checkService(kService)));
        }
        return service_;
    }

    void Invalidate(const ndk::ScopedAStatus& status) {
        if (status.getStatus() == STATUS_DEAD_OBJECT) {
            std::lock_guard lock(mutex_);
            service_.reset();
        }
    }

    std::mutex mutex_;
    std::shared_ptr<IDisplayPanelFeature> service_;
};

}  // namespace

std::shared_ptr<PanelFeatureTransport> CreateAidlPanelFeatureTransport() {
    return std::make_shared<AidlTransport>();
}

}  // namespace oplus::displaypanelfeature
