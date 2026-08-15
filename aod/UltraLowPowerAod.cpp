/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "UltraLowPowerAod"

#include <oplus/aod/UltraLowPowerAod.h>

#include <AidlPanelFeatureTransport.h>
#include <android-base/logging.h>
#include <time.h>

#include <string>
#include <utility>

namespace oplus {
namespace aod {

namespace {

constexpr int64_t kLogIntervalNs = 60LL * 1000 * 1000 * 1000;

int64_t nowNs() {
    timespec ts = {};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<int64_t>(ts.tv_sec) * 1000 * 1000 * 1000 + ts.tv_nsec;
}

}  // namespace

UltraLowPowerAod::UltraLowPowerAod() {
    std::string error;
    auto panelClient = ::oplus::displaypanelfeature::CreateAidlDisplayPanelFeatureClient(&error);
    if (panelClient) {
        mPanelWriter = std::make_unique<::oplus::displaypanelfeature::PanelWriterClient>(
                std::move(panelClient));
    } else {
        logThrottled(error);
    }
}

// Rate limited so a panel that rejects the whole feature cannot turn every AOD
// transition into a log entry.
void UltraLowPowerAod::logThrottled(const std::string& error) {
    const int64_t now = nowNs();
    if (mLastLogNs != 0 && now - mLastLogNs < kLogIntervalNs) {
        return;
    }
    mLastLogNs = now;
    LOG(WARNING) << "ultra-low-power AOD unavailable (" << error
                 << "); leaving plain AOD in control";
}

bool UltraLowPowerAod::isSupported() {
    bool enabled = false;
    return getEnabled(&enabled);
}

bool UltraLowPowerAod::getEnabled(bool* enabled) {
    if (!mPanelWriter) return false;
    std::string error;
    if (!mPanelWriter->GetUltraLowPowerAod(enabled, &error)) {
        logThrottled(error);
        return false;
    }
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
    std::string error;
    if (!mPanelWriter->SetUltraLowPowerAod(enabled, &error)) {
        logThrottled(error);
        return false;
    }
    return true;
}

bool UltraLowPowerAod::setLowPwmAod(bool enabled) {
    if (!mPanelWriter) return false;
    std::string error;
    if (!mPanelWriter->SetLowPwmAod(enabled, &error)) {
        logThrottled(error);
        return false;
    }
    return true;
}

}  // namespace aod
}  // namespace oplus
