/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * DC dimming owns the dim-layer depth, while AntiFlicker owns mechanism
 * selection. Both cross the typed DPF client: the proprietary ODM service is
 * the sole /dev/oplus_display ioctl and sysfs-fallback owner. In particular,
 * base 13 reads dim alpha and base 14 reads/writes the DC alpha override.
 */

#define LOG_TAG "DcDimmingController"

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <livedisplay/oplus/DcDimming.h>
#include <livedisplay/oplus/PanelFeature.h>
#include <sys/system_properties.h>

#include <string>
#include <thread>

namespace aidl::vendor::lineage::livedisplay {

bool DcDimming::getDimAlpha(unsigned int* alpha) {
    int32_t value = 0;
    if (!panel::Get(panel::FeatureId::kDimAlpha, &value) || value < 0) return false;
    *alpha = static_cast<unsigned int>(value);
    return true;
}

bool DcDimming::getDcAlpha(unsigned int* alpha) {
    int32_t value = 0;
    if (!panel::Get(panel::FeatureId::kDimDcAlpha, &value) || value < 0) return false;
    *alpha = static_cast<unsigned int>(value);
    return true;
}

bool DcDimming::setAlphaOverride(unsigned int alpha) {
    return panel::Set(panel::FeatureId::kDimDcAlpha, static_cast<int32_t>(alpha));
}

bool DcDimming::isSupported() {
    unsigned int value;
    return getDcAlpha(&value);
}

void DcDimming::apply() {
    const int alpha = ::android::base::GetIntProperty(kProperty, 0, 0, kMaxAlpha);
    if (!setAlphaOverride(static_cast<unsigned int>(alpha))) {
        LOG(ERROR) << "Failed to set DC dimming alpha override to " << alpha;
        return;
    }

    unsigned int dimAlpha = 0;
    unsigned int dcState = 0;
    getDimAlpha(&dimAlpha);
    getDcAlpha(&dcState);
    LOG(INFO) << "DC dimming override=" << alpha
              << (alpha == 0 ? " (kernel-automatic)" : " (pinned)")
              << " dim_alpha=" << dimAlpha << " dc_state=" << dcState;
}

void DcDimming::start() {
    if (!isSupported()) {
        LOG(INFO) << "DC dimming is not supported by the typed panel contract";
        return;
    }
    apply();

    std::thread([this] {
        std::string last = ::android::base::GetProperty(kProperty, "");
        const prop_info* pi = nullptr;
        uint32_t serial = 0;
        uint32_t globalSerial = 0;
        while (true) {
            if (pi == nullptr) pi = __system_property_find(kProperty);
            const bool ok = pi != nullptr
                                    ? __system_property_wait(pi, serial, &serial, nullptr)
                                    : __system_property_wait(nullptr, globalSerial,
                                                             &globalSerial, nullptr);
            if (!ok) {
                LOG(ERROR) << "__system_property_wait failed, stopping DC dimming watch";
                return;
            }
            const std::string current = ::android::base::GetProperty(kProperty, "");
            if (current == last) continue;
            last = current;
            apply();
        }
    }).detach();
}

}  // namespace aidl::vendor::lineage::livedisplay
