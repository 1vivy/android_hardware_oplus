/*
 * SPDX-FileCopyrightText: 2021 The Android Open Source Project
 * SPDX-FileCopyrightText: 2022-2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/logging.h>
#include <android/binder_interface_utils.h>
#include <health-impl/Health.h>
#include <health/utils.h>
#include <utils/String8.h>

#ifndef CHARGER_FORCE_NO_UI
#define CHARGER_FORCE_NO_UI 0
#endif

#if !CHARGER_FORCE_NO_UI
#include <health-impl/ChargerUtils.h>
#endif

using aidl::android::hardware::health::HalHealthLoop;
using aidl::android::hardware::health::Health;

#if !CHARGER_FORCE_NO_UI
using aidl::android::hardware::health::charger::ChargerCallback;
using aidl::android::hardware::health::charger::ChargerModeMain;
#endif

static constexpr const char* gInstanceName = "default";
static constexpr std::string_view gChargerArg{"--charger"};

#if !CHARGER_FORCE_NO_UI
namespace aidl::android::hardware::health {
class ChargerCallbackImpl : public ChargerCallback {
  public:
    using ChargerCallback::ChargerCallback;
    bool ChargerEnableSuspend() override { return true; }
};
}  // namespace aidl::android::hardware::health
#endif

namespace {

// BatteryMonitor only autodetects the battery-health attributes under
// /sys/class/power_supply/<supply>/. The oplus_chg driver publishes them under
// its own class instead, so the paths are seeded here from the device tree.
// BatteryMonitor::init() then leaves them alone -- it only fills paths that are
// still empty. A seeded node that is absent at runtime reads back as 0, which is
// what the stock "unsupported" path already reports, so a wrong path degrades
// the reading rather than failing the HAL.
void SeedOplusChgHealthPaths(struct healthd_config* config) {
#ifdef OPLUS_HEALTH_BATTERY_STATE_OF_HEALTH_PATH
    config->batteryStateOfHealthPath =
            android::String8(OPLUS_HEALTH_BATTERY_STATE_OF_HEALTH_PATH);
#endif
#ifdef OPLUS_HEALTH_BATTERY_CYCLE_COUNT_PATH
    config->batteryCycleCountPath = android::String8(OPLUS_HEALTH_BATTERY_CYCLE_COUNT_PATH);
#endif
#ifdef OPLUS_HEALTH_BATTERY_FIRST_USAGE_DATE_PATH
    config->batteryFirstUsageDatePath =
            android::String8(OPLUS_HEALTH_BATTERY_FIRST_USAGE_DATE_PATH);
#endif
}

}  // namespace

int main(int argc, char** argv) {
#ifdef __ANDROID_RECOVERY__
    android::base::InitLogging(argv, android::base::KernelLogger);
#endif

    auto config = std::make_unique<healthd_config>();
    ::android::hardware::health::InitHealthdConfig(config.get());
    SeedOplusChgHealthPaths(config.get());
    auto binder = ndk::SharedRefBase::make<Health>(gInstanceName, std::move(config));

    if (argc >= 2 && argv[1] == gChargerArg) {
#if !CHARGER_FORCE_NO_UI
        return ChargerModeMain(
                binder,
                std::make_shared<aidl::android::hardware::health::ChargerCallbackImpl>(binder));
#endif

        LOG(INFO) << "Starting charger mode without UI.";
    } else {
        LOG(INFO) << "Starting health HAL.";
    }

    auto hal_health_loop = std::make_shared<HalHealthLoop>(binder, binder);
    return hal_health_loop->StartLoop();
}
