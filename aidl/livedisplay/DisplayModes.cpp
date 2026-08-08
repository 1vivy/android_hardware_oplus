/*
 * SPDX-FileCopyrightText: 2019-2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "DisplayModesService"

#include <aidl/vendor/oplus/hardware/displaypanelfeature/IDisplayPanelFeature.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android/binder_manager.h>
#include <livedisplay/oplus/DisplayModes.h>
#include <fstream>
#include <vector>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {

using ::aidl::vendor::oplus::hardware::displaypanelfeature::IDisplayPanelFeature;

static const std::string kModeBasePath = "/sys/class/drm/card0-DSI-1/";
static const std::string kDefaultPath = "/data/vendor/display/default_display_mode";

namespace {

constexpr const char* kPanelFeatureService =
        "vendor.oplus.hardware.displaypanelfeature.IDisplayPanelFeature/default";

// Panel colour "seed" mode. Reaches the panel via panel_ioctl; the same ioctl
// issued directly on /dev/oplus_display never reaches the driver, which left
// every colour mode inert while still reporting the selection back as applied.
constexpr int32_t kFeatureSeed = 4;

// Must stay non-blocking and must not cache a failure: this runs from the
// constructor, before the HAL registers its own services.
std::shared_ptr<IDisplayPanelFeature> GetPanelFeature() {
    static std::shared_ptr<IDisplayPanelFeature> service;
    if (service == nullptr) {
        service = IDisplayPanelFeature::fromBinder(
                ndk::SpAIBinder(AServiceManager_checkService(kPanelFeatureService)));
    }
    return service;
}

bool ApplySeedMode(uint32_t seedMode) {
    const auto service = GetPanelFeature();
    if (service == nullptr) {
        LOG(ERROR) << "displaypanelfeature unavailable, seed mode " << seedMode << " not applied";
        return false;
    }

    int32_t result = -1;
    const auto status = service->setDisplayPanelFeatureValue(
            kFeatureSeed, std::vector<int32_t>{static_cast<int32_t>(seedMode)}, &result);
    if (!status.isOk() || result != 0) {
        LOG(ERROR) << "seed mode " << seedMode << " rejected: binder=" << status.getDescription()
                   << " result=" << result;
        return false;
    }
    return true;
}

}  // namespace

// Mode ids here must match qdcm display mode ids
const std::map<int32_t, DisplayModes::ModeInfo> DisplayModes::kModeMap = {
        {0, {"Vivid", 0, 0}},
        {1, {"Natural", 1, 1}},
        {2, {"Cinematic", 0, 1}},
        {3, {"Brilliant", 4, 0}},
};

DisplayModes::DisplayModes(std::shared_ptr<sdm::SDMController> controller)
    : mController(controller), mCurrentModeId(0), mDefaultModeId(0) {
    std::ifstream defaultFile(kDefaultPath);

    defaultFile >> mDefaultModeId;
    LOG(DEBUG) << "Default file read result " << mDefaultModeId << " fail " << defaultFile.fail();

    // kModeMap.at() throws, so a stored id this build no longer offers must not reach it.
    if (kModeMap.find(mDefaultModeId) == kModeMap.end()) {
        LOG(WARNING) << "stored default mode " << mDefaultModeId << " not offered, falling back to "
                     << kModeMap.begin()->first;
        mDefaultModeId = kModeMap.begin()->first;
    }

    setDisplayMode(mDefaultModeId, false);
}

// Methods from ::aidl::vendor::lineage::livedisplay::BnDisplayModes follow.
ndk::ScopedAStatus DisplayModes::getDisplayModes(std::vector<DisplayMode>* _aidl_return) {
    std::vector<DisplayMode> modes;

    for (const auto& entry : kModeMap) {
        modes.push_back({entry.first, entry.second.name});
    }
    *_aidl_return = modes;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus DisplayModes::getCurrentDisplayMode(DisplayMode* _aidl_return) {
    *_aidl_return = {mCurrentModeId, kModeMap.at(mCurrentModeId).name};
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus DisplayModes::getDefaultDisplayMode(DisplayMode* _aidl_return) {
    *_aidl_return = {mDefaultModeId, kModeMap.at(mDefaultModeId).name};
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus DisplayModes::setDisplayMode(int32_t modeID, bool makeDefault) {
    const auto iter = kModeMap.find(modeID);
    if (iter == kModeMap.end()) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }
    if (!ApplySeedMode(iter->second.seedMode)) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }
    mController->setActiveDisplayMode(iter->second.displayModeId);
    mCurrentModeId = iter->first;
    if (makeDefault) {
        std::ofstream defaultFile(kDefaultPath);
        defaultFile << iter->first;
        if (!defaultFile.fail()) {
            mController->setDefaultDisplayMode(iter->second.displayModeId);
            mDefaultModeId = iter->first;
        }
    }
    if (mOnDisplayModeSet) {
        mOnDisplayModeSet();
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
