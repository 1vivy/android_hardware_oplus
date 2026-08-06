/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "UltraLowPowerAod"

#include <oplus/aod/UltraLowPowerAod.h>

#include <android-base/logging.h>
#include <fcntl.h>
#include <oplus/oplus_display_panel.h>
#include <sys/ioctl.h>
#include <time.h>

namespace oplus {
namespace aod {

namespace {

constexpr char kPanelDevice[] = "/dev/oplus_display";
constexpr int64_t kLogIntervalNs = 60LL * 1000 * 1000 * 1000;

int64_t nowNs() {
    timespec ts = {};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<int64_t>(ts.tv_sec) * 1000 * 1000 * 1000 + ts.tv_nsec;
}

}  // namespace

UltraLowPowerAod::UltraLowPowerAod()
    : mFd(::android::base::unique_fd(open(kPanelDevice, O_RDWR | O_CLOEXEC))) {
    if (mFd.get() < 0) {
        logThrottled("open", errno);
    }
}

// Rate limited so a panel that rejects the whole feature cannot turn every AOD
// transition into a log entry.
void UltraLowPowerAod::logThrottled(const char* what, int error) {
    const int64_t now = nowNs();
    if (mLastLogNs != 0 && now - mLastLogNs < kLogIntervalNs) {
        return;
    }
    mLastLogNs = now;
    LOG(WARNING) << "ultra-low-power AOD unavailable (" << what << ": " << strerror(error)
                 << "); leaving plain AOD in control";
}

bool UltraLowPowerAod::get(unsigned long request, unsigned int* value, const char* what) {
    if (mFd.get() < 0) {
        return false;
    }
    if (ioctl(mFd.get(), request, value) != 0) {
        logThrottled(what, errno);
        return false;
    }
    return true;
}

bool UltraLowPowerAod::set(unsigned long request, unsigned int value, const char* what) {
    if (mFd.get() < 0) {
        return false;
    }
    if (ioctl(mFd.get(), request, &value) != 0) {
        logThrottled(what, errno);
        return false;
    }
    return true;
}

bool UltraLowPowerAod::isSupported() {
    unsigned int value = 0;
    return get(PANEL_IOCTL_GET_ULTRA_LOW_POWER_AOD, &value, "probe");
}

bool UltraLowPowerAod::getEnabled(bool* enabled) {
    unsigned int value = 0;
    if (!get(PANEL_IOCTL_GET_ULTRA_LOW_POWER_AOD, &value, "get")) {
        return false;
    }
    *enabled = value > 0;
    return true;
}

bool UltraLowPowerAod::setEnabled(bool enabled) {
    // A panel that cannot report its current mode cannot be trusted to accept a new
    // one, so the probe gates the write rather than the write discovering it.
    bool current = false;
    if (!getEnabled(&current)) {
        return false;
    }
    if (current == enabled) {
        return true;
    }
    return set(PANEL_IOCTL_SET_ULTRA_LOW_POWER_AOD, enabled ? 1 : 0, "set");
}

bool UltraLowPowerAod::setLowPwmAod(bool enabled) {
    return set(PANEL_IOCTL_SET_LOW_PWM_AOD, enabled ? 1 : 0, "set-low-pwm");
}

}  // namespace aod
}  // namespace oplus
