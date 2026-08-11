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

#include <chrono>
#include <optional>
#include <string>
#include <thread>

using namespace std::chrono_literals;

namespace oplus::displaypanelfeature {
namespace {

constexpr auto kAdfrConfigPath = "/vendor/etc/display/multimedia_display_adfr2minfps_config.xml";
constexpr auto kMinFpsSysfsPath = "/sys/kernel/oplus_display/min_fps";
constexpr auto kAospModeProperty = "persist.sys.displaypanel.ltpo_aosp_mode";
constexpr auto kPollInterval = 200ms;

// The panel's declared mode set (device tree / dumpsys display). Anything else read
// from kAospModeProperty is treated as "not yet known" and skipped rather than acted
// on, so a stale, unset, or malformed property can never reach the sysfs write.
bool IsKnownMode(int mode_hz) {
    switch (mode_hz) {
        case 60:
        case 90:
        case 120:
        case 144:
        case 165:
            return true;
        default:
            return false;
    }
}

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
    while (true) {
        const int mode_hz = android::base::GetIntProperty(kAospModeProperty, 0);
        if (IsKnownMode(mode_hz)) {
            const int floor_hz = ComputeAdfrFloor(*payload, mode_hz);
            if (last_written != floor_hz) {
                if (WriteMinFps(floor_hz)) {
                    last_written = floor_hz;
                    LOG(INFO) << "mode=" << mode_hz << " -> min_fps floor=" << floor_hz;
                }
            }
        }
        std::this_thread::sleep_for(kPollInterval);
    }
}
