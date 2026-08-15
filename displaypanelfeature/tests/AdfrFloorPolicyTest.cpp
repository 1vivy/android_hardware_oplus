/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AdfrFloorPolicy.h"

#include <gtest/gtest.h>

namespace oplus::displaypanelfeature {
namespace {

// Mirrors the infiniti-adfr golden payload (tests/data/infiniti-adfr.payload) at the
// offsets AdfrConfig.cpp::kTables declares, zero elsewhere.
//
// The aod_* rows were ABSENT from this fixture until 2026-08-15, which made it a poor
// stand-in for the shipped payload: the AOD tier read as all-zero padding, so a test
// could not have told tier selection from tier blindness. They are 8 values where the
// interactive rows are 10 - that difference is the shipped data's own, and it is what
// lets a test prove which tier was read.
AdfrPayload InfinitiPayload() {
    AdfrPayload payload{};
    const int minfps120[] = {1, 1, 30, 1, 120, 1, 120, 10, 120, 10};
    const int minfps90[] = {1, 1, 30, 1, 90, 1, 90, 10, 90, 10};
    const int minfps60[] = {1, 1, 30, 1, 60, 1, 60, 10, 60, 10};
    const int aod120[] = {1, 1, 30, 1, 120, 1, 120, 10};
    const int aod90[] = {1, 1, 30, 1, 90, 1, 90, 10};
    const int aod60[] = {1, 1, 30, 1, 60, 1, 60, 10};
    for (size_t i = 0; i < 10; ++i) payload[48 + i] = minfps120[i];
    for (size_t i = 0; i < 10; ++i) payload[61 + i] = minfps90[i];
    for (size_t i = 0; i < 10; ++i) payload[74 + i] = minfps60[i];
    for (size_t i = 0; i < 8; ++i) payload[117 + i] = aod120[i];
    for (size_t i = 0; i < 8; ++i) payload[130 + i] = aod90[i];
    for (size_t i = 0; i < 8; ++i) payload[143 + i] = aod60[i];
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

// NextFloorWrite is the daemon's ENTIRE decision. The daemon around it only waits
// for a property edge and writes what this returns, so these cases are what stops a
// stale or malformed property from reaching the panel, and what stops a repeated
// property edge from rewriting a floor that has not changed.

TEST(AdfrFloorPolicyTest, UnknownObservedModeProducesNoWriteAtAll) {
    // Given: the property is unset (0) or carries a mode this panel never declares.
    const auto payload = InfinitiPayload();

    // When / Then: nothing is written. Returning a floor here would let an unset
    // property at early boot drive the panel, which is exactly the failure the
    // known-mode set exists to prevent.
    EXPECT_FALSE(NextFloorWrite(payload, 0, std::nullopt).has_value());
    EXPECT_FALSE(NextFloorWrite(payload, 77, std::nullopt).has_value());
    EXPECT_FALSE(NextFloorWrite(payload, -1, std::nullopt).has_value());
}

TEST(AdfrFloorPolicyTest, UnchangedFloorProducesNoRepeatWrite) {
    // Given: a mode whose floor has already been written.
    const auto payload = InfinitiPayload();
    const auto first = NextFloorWrite(payload, 120, std::nullopt);
    ASSERT_TRUE(first.has_value());

    // When / Then: the same observation again writes nothing. Property edges are
    // global, so this daemon wakes for changes it does not own; re-writing the
    // panel on every unrelated edge would make it a chattering writer.
    EXPECT_FALSE(NextFloorWrite(payload, 120, first).has_value());
}

TEST(AdfrFloorPolicyTest, ChangedModeProducesExactlyTheComputedFloor) {
    // Given: a floor already written for the gate mode.
    const auto payload = InfinitiPayload();
    const auto settled = NextFloorWrite(payload, kFloorGateModeHz, std::nullopt);
    ASSERT_TRUE(settled.has_value());
    EXPECT_EQ(*settled, ComputeAdfrFloor(payload, kFloorGateModeHz));

    // When: AOSP moves up to a mode outside the adaptive tier.
    const auto raised = NextFloorWrite(payload, 165, settled);

    // Then: the daemon writes that mode's own rate, releasing the descent.
    ASSERT_TRUE(raised.has_value());
    EXPECT_EQ(*raised, 165);
}

TEST(AdfrFloorPolicyTest, KnownPanelModeSetIsExactlyTheDeclaredModes) {
    for (const int mode : {60, 90, 120, 144, 165}) {
        EXPECT_TRUE(IsKnownPanelMode(mode)) << "declared mode " << mode << " rejected";
    }
    for (const int mode : {0, -1, 30, 61, 119, 166, 240}) {
        EXPECT_FALSE(IsKnownPanelMode(mode)) << "undeclared mode " << mode << " accepted";
    }
}

TEST(AdfrFloorPolicyTest, AodTierIsSeparateDataAndIsActuallyRead) {
    // Given: the shipped payload, whose AOD tier declares FEWER values than its
    // interactive counterpart (8 vs 10) - proof the two tiers are distinct data.
    const auto payload = InfinitiPayload();
    // aod_minfps60_level @143 and minfps60_level @74, per AdfrConfig.cpp::kTables.
    int interactive_filled = 0;
    int aod_filled = 0;
    for (size_t i = 0; i < 13; ++i) {
        if (payload[74 + i] > 0) ++interactive_filled;
        if (payload[143 + i] > 0) ++aod_filled;
    }
    ASSERT_GT(interactive_filled, aod_filled)
            << "tiers are indistinguishable; this test can no longer prove tier selection";

    // When / Then: asking for the AOD tier must read the AOD offsets. Both tiers
    // currently bottom out at the same legal value, so assert the READ, not the
    // number: mutate the AOD tier alone and only the AOD answer may move.
    AdfrPayload mutated = payload;
    for (size_t i = 0; i < 13; ++i) {
        if (mutated[143 + i] > 0) mutated[143 + i] = 45;
    }
    EXPECT_EQ(ComputeAdfrFloor(mutated, kFloorGateModeHz, PanelActivity::kAod), 45);
    EXPECT_EQ(ComputeAdfrFloor(mutated, kFloorGateModeHz, PanelActivity::kInteractive),
              ComputeAdfrFloor(payload, kFloorGateModeHz, PanelActivity::kInteractive));
}

TEST(AdfrFloorPolicyTest, InteractiveTierIsUnaffectedByTheAodTier) {
    // Given: a payload whose INTERACTIVE tier alone is mutated.
    AdfrPayload mutated = InfinitiPayload();
    for (size_t i = 0; i < 13; ++i) {
        if (mutated[74 + i] > 0) mutated[74 + i] = 33;
    }

    // When / Then: the AOD answer must not follow the interactive tier.
    EXPECT_EQ(ComputeAdfrFloor(mutated, kFloorGateModeHz, PanelActivity::kInteractive), 33);
    EXPECT_NE(ComputeAdfrFloor(mutated, kFloorGateModeHz, PanelActivity::kAod), 33);
}

TEST(AdfrFloorPolicyTest, TheDescentGateIsIdenticalForBothTiers) {
    // Given: the shipped payload at a mode ABOVE the gate.
    const auto payload = InfinitiPayload();

    // When / Then: neither tier may descend before AOSP settles at the gate mode.
    // Selecting a tier is derived; changing WHEN a floor may descend would not be.
    EXPECT_EQ(ComputeAdfrFloor(payload, 120, PanelActivity::kAod), 120);
    EXPECT_EQ(ComputeAdfrFloor(payload, 120, PanelActivity::kInteractive), 120);
}

}  // namespace
}  // namespace oplus::displaypanelfeature
