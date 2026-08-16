/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "../AodPanelStateService.h"

#include <gtest/gtest.h>

#include <memory>

namespace oplus::aod {
namespace {

using aidl::vendor::oplus::hardware::aodpanelstate::AodPanelEdge;

TEST(AodPanelStateServiceTest, PermissionDenialPreventsPolicyMutation) {
    auto policy = std::make_shared<AodPanelPolicy>(nullptr);
    auto service =
            ndk::SharedRefBase::make<AodPanelStateService>(std::move(policy), [] { return false; });
    bool accepted = true;

    const auto status = service->send(1, AodPanelEdge::ENTER, &accepted);

    EXPECT_EQ(EX_SECURITY, status.getExceptionCode());
    EXPECT_FALSE(accepted);
}

}  // namespace
}  // namespace oplus::aod
