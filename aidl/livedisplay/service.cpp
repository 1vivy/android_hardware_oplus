/*
 * SPDX-FileCopyrightText: 2019-2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.lineage.livedisplay-service-oplus"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <binder/ProcessState.h>
#include <livedisplay/oplus/AdaptiveBacklight.h>
#include <livedisplay/oplus/AntiFlicker.h>
#include <livedisplay/oplus/DcDimming.h>
#include <livedisplay/oplus/DisplayModes.h>
#include <livedisplay/oplus/SunlightEnhancement.h>
#include <livedisplay/sdm/PictureAdjustment.h>
#include <livedisplay/sdm/Utils.h>

using ::aidl::vendor::lineage::livedisplay::AdaptiveBacklight;
using ::aidl::vendor::lineage::livedisplay::AntiFlicker;
using ::aidl::vendor::lineage::livedisplay::DcDimming;
using ::aidl::vendor::lineage::livedisplay::DisplayModes;
using ::aidl::vendor::lineage::livedisplay::SunlightEnhancement;
using ::aidl::vendor::lineage::livedisplay::sdm::HsicRanges;
using ::aidl::vendor::lineage::livedisplay::sdm::PictureAdjustment;
using ::aidl::vendor::lineage::livedisplay::sdm::SDMController;
namespace sdm_utils = ::aidl::vendor::lineage::livedisplay::sdm::utils;

namespace {

// The SDM-backed constructors abort when their feature is unavailable, taking
// unrelated components down. Probe their private readiness contracts first.

bool PictureAdjustmentReady(const std::shared_ptr<SDMController>& controller) {
    if (controller == nullptr) {
        return false;
    }
    if (sdm_utils::CheckFeatureVersion(controller, sdm_utils::FEATURE_VER_SW_PA_API) !=
        android::OK) {
        return false;
    }

    HsicRanges r{};
    if (controller->getGlobalPaRange(&r) != android::OK) {
        return false;
    }

    return r.hue.max != 0 && r.hue.min != 0 && r.saturation.max != 0.f && r.saturation.min != 0.f &&
           r.intensity.max != 0.f && r.intensity.min != 0.f && r.contrast.max != 0.f &&
           r.contrast.min != 0.f;
}

bool DisplayModesReady(const std::shared_ptr<SDMController>& controller) {
    if (controller == nullptr) {
        return false;
    }
    if (sdm_utils::CheckFeatureVersion(controller, sdm_utils::FEATURE_VER_SW_SAVEMODES_API) !=
        android::OK) {
        return false;
    }

    int32_t count = 0;
    if (controller->getNumDisplayModes(&count) != android::OK) {
        return false;
    }

    return count > 0;
}

}  // namespace

int main() {
    android::ProcessState::self()->setThreadPoolMaxThreadCount(1);
    android::ProcessState::self()->startThreadPool();

    LOG(INFO) << "LiveDisplay HAL service is starting.";

    std::shared_ptr<SDMController> controller =
            ENABLE_DM || ENABLE_PA ? std::make_shared<SDMController>() : nullptr;

    std::shared_ptr<AdaptiveBacklight> ab =
            ENABLE_AB ? ndk::SharedRefBase::make<AdaptiveBacklight>() : nullptr;
    std::shared_ptr<AntiFlicker> af = ENABLE_AF ? ndk::SharedRefBase::make<AntiFlicker>() : nullptr;
    /*
     * DC dimming is not a LiveDisplay interface - LiveDisplay has no
     * DC-dimming contract - so it is driven by a system property instead of a
     * binder object. It rides this process because /dev/oplus_display is
     * already open here under a single sepolicy domain. See DcDimming.cpp for
     * the ioctl subset it owns and the AntiFlicker ownership boundary.
     */
    std::shared_ptr<DcDimming> dc = ENABLE_DC ? std::make_shared<DcDimming>() : nullptr;
    bool dm_ready = ENABLE_DM && DisplayModesReady(controller);
    bool pa_ready = ENABLE_PA && PictureAdjustmentReady(controller);

    if (ENABLE_DM && !dm_ready) {
        LOG(WARNING) << "DisplayModes backend not ready; skipping it and serving the rest.";
    }
    if (ENABLE_PA && !pa_ready) {
        LOG(WARNING) << "PictureAdjustment backend not ready; skipping it and serving the rest.";
    }

    std::shared_ptr<DisplayModes> dm =
            dm_ready ? ndk::SharedRefBase::make<DisplayModes>(controller) : nullptr;
    std::shared_ptr<PictureAdjustment> pa =
            pa_ready ? ndk::SharedRefBase::make<PictureAdjustment>(controller) : nullptr;
    std::shared_ptr<SunlightEnhancement> se =
            ENABLE_SE ? ndk::SharedRefBase::make<SunlightEnhancement>() : nullptr;

    if (ab) {
        std::string instance = std::string() + AdaptiveBacklight::descriptor + "/default";
        binder_status_t status = AServiceManager_addService(ab->asBinder().get(), instance.c_str());
        CHECK_EQ(status, STATUS_OK);
    }

    if (af) {
        std::string instance = std::string() + AntiFlicker::descriptor + "/default";
        binder_status_t status = AServiceManager_addService(af->asBinder().get(), instance.c_str());
        CHECK_EQ(status, STATUS_OK);
    }

    if (dm) {
        std::string instance = std::string() + DisplayModes::descriptor + "/default";
        binder_status_t status = AServiceManager_addService(dm->asBinder().get(), instance.c_str());
        CHECK_EQ(status, STATUS_OK);
    }

    if (pa) {
        std::string instance = std::string() + PictureAdjustment::descriptor + "/default";
        binder_status_t status = AServiceManager_addService(pa->asBinder().get(), instance.c_str());
        CHECK_EQ(status, STATUS_OK);
    }

    if (se) {
        std::string instance = std::string() + SunlightEnhancement::descriptor + "/default";
        binder_status_t status = AServiceManager_addService(se->asBinder().get(), instance.c_str());
        CHECK_EQ(status, STATUS_OK);
    }

    if (dc) {
        dc->start();
    }

    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;  // should not reach
}
