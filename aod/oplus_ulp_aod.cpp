/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "oplus_ulp_aod"

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <oplus/aod/UltraLowPowerAod.h>

// One-shot applier: init runs it when the gate property changes. It always
// succeeds from init's point of view, because the only failure mode it can have is
// "this panel does not do ultra-low-power AOD", which must leave plain AOD running.
int main(int argc, char** argv) {
    const bool enable =
            argc > 1 ? argv[1][0] == '1'
                     : ::android::base::GetBoolProperty("persist.sys.oplus.ulp_aod.enabled",
                                                        false);

    oplus::aod::UltraLowPowerAod ulpAod;
    if (ulpAod.setEnabled(enable)) {
        LOG(INFO) << "ultra-low-power AOD " << (enable ? "enabled" : "disabled");
    }
    return 0;
}
