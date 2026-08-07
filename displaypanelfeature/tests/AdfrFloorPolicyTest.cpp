/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AdfrFloorPolicy.h"

#include <gtest/gtest.h>

namespace oplus::displaypanelfeature {
namespace {

// Mirrors the infiniti-adfr golden payload's minfps120_level (offset 48),
// minfps90_level (offset 61), and minfps60_level (offset 74) table contents
// (see tests/data/infiniti-adfr.payload), zero elsewhere.
AdfrPayload InfinitiPayload() {
    AdfrPayload payload{};
    const int minfps120[] = {1, 1, 30, 1, 120, 1, 120, 10, 120, 10};
    const int minfps90[] = {1, 1, 30, 1, 90, 1, 90, 10, 90, 10};
    const int minfps60[] = {1, 1, 30, 1, 60, 1, 60, 10, 60, 10};
    for (size_t i = 0; i < 10; ++i) payload[48 + i] = minfps120[i];
    for (size_t i = 0; i < 10; ++i) payload[61 + i] = minfps90[i];
    for (size_t i = 0; i < 10; ++i) payload[74 + i] = minfps60[i];
    return payload;
}

TEST(AdfrFloorPolicyTest, KeepsModeRateAt120WhenNotAtTheFloorMode) {
    // Given
    const auto payload = InfinitiPayload();

    // When / Then
    EXPECT_EQ(ComputeAdfrFloor(payload, 120), 120);
}

TEST(AdfrFloorPolicyTest, KeepsModeRateAt90WhenNotAtTheFloorMode) {
    // Given
    const auto payload = InfinitiPayload();

    // When / Then
    EXPECT_EQ(ComputeAdfrFloor(payload, 90), 90);
}

TEST(AdfrFloorPolicyTest, DescendsToTheDeepestLegalValueAtTheFloorMode) {
    // Given
    const auto payload = InfinitiPayload();

    // When / Then
    EXPECT_EQ(ComputeAdfrFloor(payload, kFloorGateModeHz), 1);
    EXPECT_EQ(kFloorGateModeHz, 60);
}

TEST(AdfrFloorPolicyTest, TreatsUnshippedReserveTierModesAsExplicitFixed) {
    // Given
    const auto payload = InfinitiPayload();

    // When / Then: 144/165 have no minfpsNN_level table in the shipped payload, so
    // they must never receive an artificial floor below their own rate.
    EXPECT_EQ(ComputeAdfrFloor(payload, 144), 144);
    EXPECT_EQ(ComputeAdfrFloor(payload, 165), 165);
}

TEST(AdfrFloorPolicyTest, TreatsAnUnrecognizedModeAsExplicitFixed) {
    // Given
    const auto payload = InfinitiPayload();

    // When / Then: an active mode outside the panel's known set must never be
    // guessed at; keep it at its own rate.
    EXPECT_EQ(ComputeAdfrFloor(payload, 77), 77);
}

TEST(AdfrFloorPolicyTest, IgnoresZeroPaddingWhenSelectingTheDeepestValue) {
    // Given: a payload whose minfps60_level table is entirely unfilled zero padding
    // except for a single legal entry, matching how AdfrConfig::ReadTable leaves
    // capacity-count slots untouched.
    AdfrPayload payload{};
    payload[74] = 30;

    // When / Then: 0 must never win the minimum, and 30 is the only legal value.
    EXPECT_EQ(ComputeAdfrFloor(payload, kFloorGateModeHz), 30);
}

TEST(AdfrFloorPolicyTest, NeverDescendsBelowTheModeRateWhenTheTableIsEmpty) {
    // Given: a fully zeroed payload (e.g. before the first ADFR config publish).
    AdfrPayload payload{};

    // When / Then: with no legal value found, stay at the mode's own rate rather
    // than guessing a value the panel's table never sanctioned.
    EXPECT_EQ(ComputeAdfrFloor(payload, kFloorGateModeHz), kFloorGateModeHz);
}

}  // namespace
}  // namespace oplus::displaypanelfeature
