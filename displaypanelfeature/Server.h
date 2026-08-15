/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "Dispatcher.h"
#include "FeatureRegistry.h"
#include "PanelBackend.h"

#include <aidl/vendor/oplus/hardware/displaypanelfeature/BnDisplayPanelFeature.h>

#include <memory>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {

// Thin AIDL adapter. It owns no admission rules of its own: every decision
// comes from Dispatcher, which is where the deny-by-default contract is tested
// on host without binder.
class DisplayPanelFeatureServer
    : public aidl::vendor::oplus::hardware::displaypanelfeature::BnDisplayPanelFeature {
  public:
    DisplayPanelFeatureServer(std::shared_ptr<const FeatureRegistry> registry,
                              std::shared_ptr<PanelBackend> backend);

    ndk::ScopedAStatus getDisplayPanelFeatureValue(int32_t featureId,
                                                   std::vector<int32_t>* featureValues,
                                                   int32_t* result) override;
    ndk::ScopedAStatus setDisplayPanelFeatureValue(int32_t featureId,
                                                   const std::vector<int32_t>& featureValues,
                                                   int32_t* result) override;
    ndk::ScopedAStatus getDisplayPanelInfo(int32_t featureId, std::vector<std::string>* panelInfo,
                                           int32_t* result) override;

  private:
    std::shared_ptr<const FeatureRegistry> registry_;
    std::shared_ptr<PanelBackend> backend_;
};

}  // namespace oplus::displaypanelfeature
