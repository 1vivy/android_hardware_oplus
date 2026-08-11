/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "FeatureRegistry.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {

enum class FeatureId : int32_t {
    kPanelPower = 1,
    kSeed = 4,
    kHbm = 12,
    kDimAlpha = 13,
    kDimDcAlpha = 14,
    kDimlayerBlEnable = 23,
    kCabc = 184,
    kDre = 185,
    kPwmTurbo = 199,
    kPwmPulse = 201,
    kAdfrControl = 232,
    kAdfrSupport = 233,
    kAdfrConfig = 234,
};

class PanelFeatureTransport {
  public:
    virtual ~PanelFeatureTransport() = default;
    virtual bool Get(int32_t packedId, std::vector<int32_t>* values, std::string* error) = 0;
    virtual bool Set(int32_t packedId, const std::vector<int32_t>& values, std::string* error) = 0;
};

class DisplayPanelFeatureClient {
  public:
    DisplayPanelFeatureClient(std::shared_ptr<const FeatureRegistry> registry,
                              std::shared_ptr<PanelFeatureTransport> transport);

    bool Get(DisplayRole display, FeatureId feature, std::vector<int32_t>* values,
             std::string* error) const;
    bool GetScalar(DisplayRole display, FeatureId feature, int32_t* value,
                   std::string* error) const;
    bool Set(DisplayRole display, FeatureId feature, const std::vector<int32_t>& values,
             std::string* error) const;
    bool SetScalar(DisplayRole display, FeatureId feature, int32_t value, std::string* error) const;

  private:
    bool Prepare(DisplayRole display, FeatureId feature, bool set,
                 const std::vector<int32_t>& values, int32_t* packedId, std::string* error) const;

    std::shared_ptr<const FeatureRegistry> registry_;
    std::shared_ptr<PanelFeatureTransport> transport_;
};

}  // namespace oplus::displaypanelfeature
