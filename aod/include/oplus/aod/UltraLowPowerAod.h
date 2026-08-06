/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <android-base/unique_fd.h>

namespace oplus {
namespace aod {

// Every method reports failure by returning false and never propagates it further:
// an unsupported panel, a missing node or a rejected ioctl must leave plain AOD
// running untouched.
class UltraLowPowerAod {
  public:
    UltraLowPowerAod();

    bool isSupported();
    bool getEnabled(bool* enabled);
    bool setEnabled(bool enabled);

    // Write-only by kernel design: oplus_display_device.c dispatches
    // PANEL_IOCTL_SET_LOW_PWM_AOD but its GET counterpart is commented out.
    bool setLowPwmAod(bool enabled);

  private:
    bool get(unsigned long request, unsigned int* value, const char* what);
    bool set(unsigned long request, unsigned int value, const char* what);
    void logThrottled(const char* what, int error);

    ::android::base::unique_fd mFd;
    int64_t mLastLogNs = 0;
};

}  // namespace aod
}  // namespace oplus
