/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <PanelWriterClient.h>

#include <cstdint>
#include <memory>
#include <string>

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

    // The open backend exposes low-PWM AOD as a write-only operation even
    // though the retained stock front-door map proves a bidirectional route.
    bool setLowPwmAod(bool enabled);

  private:
    void logThrottled(const std::string& error);

    std::unique_ptr<::oplus::displaypanelfeature::PanelWriterClient> mPanelWriter;
    int64_t mLastLogNs = 0;
};

}  // namespace aod
}  // namespace oplus
