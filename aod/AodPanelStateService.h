/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "AodPanelPolicy.h"

#include <aidl/vendor/oplus/hardware/aodpanelstate/BnAodPanelStateService.h>

#include <functional>
#include <memory>

namespace oplus::aod {

class AodPanelStateService final
    : public aidl::vendor::oplus::hardware::aodpanelstate::BnAodPanelStateService {
  public:
    using PermissionChecker = std::function<bool()>;

    explicit AodPanelStateService(std::shared_ptr<AodPanelPolicy> policy);
    AodPanelStateService(std::shared_ptr<AodPanelPolicy> policy,
                         PermissionChecker permissionChecker);

    ndk::ScopedAStatus send(int64_t generation,
                            aidl::vendor::oplus::hardware::aodpanelstate::AodPanelEdge edge,
                            bool* accepted) override;

  private:
    std::shared_ptr<AodPanelPolicy> policy_;
    PermissionChecker permissionChecker_;
};

}  // namespace oplus::aod
