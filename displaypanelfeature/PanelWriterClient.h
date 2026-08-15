/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "DisplayPanelFeatureClient.h"

#include <memory>
#include <string>

namespace oplus::displaypanelfeature {

class PanelWriterClient {
  public:
    explicit PanelWriterClient(std::shared_ptr<DisplayPanelFeatureClient> client);

    bool SetDimlayerHbm(bool enabled, std::string* error) const;
    bool SetFpPress(bool pressed, std::string* error) const;
    bool GetUltraLowPowerAod(bool* enabled, std::string* error) const;
    bool SetUltraLowPowerAod(bool enabled, std::string* error) const;
    bool SetLowPwmAod(bool enabled, std::string* error) const;
    bool SetLongruiAodActive(bool active, std::string* error) const;

  private:
    bool GetBool(const std::string& feature, bool* value, std::string* error) const;
    bool SetBool(const std::string& feature, bool value, std::string* error) const;

    std::shared_ptr<DisplayPanelFeatureClient> client_;
};

}  // namespace oplus::displaypanelfeature
