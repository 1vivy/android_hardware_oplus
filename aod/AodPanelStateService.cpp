/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "oplus-aod-panel-state"

#include "AodPanelStateService.h"

#include <android-base/logging.h>

#include <string>
#include <utility>

namespace oplus::aod {

AodPanelStateService::AodPanelStateService(std::shared_ptr<AodPanelPolicy> policy)
    : policy_(std::move(policy)) {}

ndk::ScopedAStatus AodPanelStateService::send(
        int64_t generation, aidl::vendor::oplus::hardware::aodpanelstate::AodPanelEdge edge,
        bool* accepted) {
    AodPanelEdge policyEdge;
    switch (edge) {
        case aidl::vendor::oplus::hardware::aodpanelstate::AodPanelEdge::ENTER:
            policyEdge = AodPanelEdge::kEnter;
            break;
        case aidl::vendor::oplus::hardware::aodpanelstate::AodPanelEdge::EXIT:
            policyEdge = AodPanelEdge::kExit;
            break;
        default:
            return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
    }

    std::string error;
    *accepted = policy_->Send(generation, policyEdge, &error);
    if (!*accepted) {
        LOG(WARNING) << "AOD panel edge rejected: " << error;
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace oplus::aod
