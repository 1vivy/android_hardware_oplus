/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "AodPanelPolicy.h"

#include <aidl/vendor/oplus/hardware/aodpanelstate/BnAodPanelStateService.h>

#include <memory>

namespace oplus::aod {

class AodPanelStateService final
    : public aidl::vendor::oplus::hardware::aodpanelstate::BnAodPanelStateService {
  public:
    explicit AodPanelStateService(std::shared_ptr<AodPanelPolicy> policy);

    ndk::ScopedAStatus send(int64_t generation,
                            aidl::vendor::oplus::hardware::aodpanelstate::AodPanelEdge edge,
                            bool* accepted) override;

  private:
    std::shared_ptr<AodPanelPolicy> policy_;
};

}  // namespace oplus::aod
