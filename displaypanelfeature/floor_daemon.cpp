/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "adfr-floor-daemon"

#include "AdfrConfig.h"
#include "AdfrFloorPolicy.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>

#include <sys/system_properties.h>

#include <optional>
#include <string>

namespace oplus::displaypanelfeature {
namespace {

constexpr auto kAdfrConfigPath = "/vendor/etc/display/multimedia_display_adfr2minfps_config.xml";
constexpr auto kMinFpsSysfsPath = "/sys/kernel/oplus_display/min_fps";
constexpr auto kAospModeProperty = "persist.sys.displaypanel.ltpo_aosp_mode";

bool WriteMinFps(int floor_hz) {
    const std::string value = std::to_string(floor_hz);
    if (!android::base::WriteStringToFile(value, kMinFpsSysfsPath)) {
        PLOG(WARNING) << "failed to write " << kMinFpsSysfsPath << " = " << value;
        return false;
    }
    return true;
}

}  // namespace
}  // namespace oplus::displaypanelfeature

int main() {
    using namespace oplus::displaypanelfeature;

    std::string error;
    const auto payload = LoadAdfrConfig(kAdfrConfigPath, &error);
    if (!payload) {
        LOG(ERROR) << "ADFR config rejected, floor daemon idle: " << error;
        return EXIT_FAILURE;
    }

    std::optional<int> last_written;
    const auto apply = [&payload, &last_written]() {
        const int mode_hz = android::base::GetIntProperty(kAospModeProperty, 0);
        const auto floor_hz = NextFloorWrite(*payload, mode_hz, last_written);
        if (floor_hz && WriteMinFps(*floor_hz)) {
            last_written = *floor_hz;
            LOG(INFO) << "mode=" << mode_hz << " -> min_fps floor=" << *floor_hz;
        }
    };

    // Subscribe to the mode property, then act on its edges. This daemon used to
    // re-read the property every 200ms forever, which is a wakeup five times a
    // second for the life of the device and, worse, makes a mode transition
    // observable only on a timer boundary rather than when it happens.
    //
    // The property may not exist yet this early in boot, so wait on the global
    // serial until it appears rather than spinning for it.
    const prop_info* info = __system_property_find(kAospModeProperty);
    for (uint32_t global = 0; info == nullptr;) {
        if (!__system_property_wait(nullptr, global, &global, nullptr)) {
            LOG(ERROR) << "property subscription failed before " << kAospModeProperty
                       << " appeared";
            return EXIT_FAILURE;
        }
        info = __system_property_find(kAospModeProperty);
    }

    apply();
    for (uint32_t serial = 0; __system_property_wait(info, serial, &serial, nullptr);) {
        apply();
    }
    LOG(ERROR) << "property subscription for " << kAospModeProperty << " failed";
    return EXIT_FAILURE;
}
