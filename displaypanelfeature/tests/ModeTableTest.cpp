/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AdfrConfig.h"
#include "AdfrFloorPolicy.h"
#include "ModeTable.h"

#include <android-base/file.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {
namespace {

std::string TestData(const char* name) {
    return android::base::GetExecutableDirectory() + "/tests/data/" + name;
}

// The contract this device declares, stated once. The rates are the panel's
// declared set: 60/90/120 carry minfps tables in the ADFR payload (the adaptive
// tier AdfrFloorPolicy descends within) and 144/165 are the reserve tier, with
// 165 the peak the framework overlay advertises as config_defaultPeakRefreshRate.
const std::vector<int> kRefreshRates = {60, 90, 120, 144, 165};
const std::vector<int> kAdaptiveRates = {60, 90, 120};
constexpr int kDeclaredPeakRefreshHz = 165;
constexpr size_t kExpectedResolutions = 2;

ModeTable BuiltFromDeviceConfig() {
    std::string error;
    auto resolutions = ModeTable::ParseDensityMapping(TestData("displayconfig.xml"), &error);
    EXPECT_TRUE(resolutions) << error;
    auto table = ModeTable::Build(*resolutions, kRefreshRates, kAdaptiveRates, &error);
    EXPECT_TRUE(table) << error;
    return *table;
}

TEST(ModeTableTest, StaticContractIsTheCompleteTwoByFiveCrossProduct) {
    const auto table = BuiltFromDeviceConfig();
    ASSERT_EQ(table.resolutions().size(), kExpectedResolutions);
    ASSERT_EQ(table.refresh_rates().size(), kRefreshRates.size());
    EXPECT_EQ(table.modes().size(), kExpectedResolutions * kRefreshRates.size());

    // Exhaustive, not sampled: every declared resolution must carry every declared
    // rate exactly once.
    for (const auto& resolution : table.resolutions()) {
        for (const int rate : kRefreshRates) {
            const auto found = std::count_if(
                    table.modes().begin(), table.modes().end(), [&](const DisplayMode& mode) {
                        return mode.resolution == resolution && mode.refresh_hz == rate;
                    });
            EXPECT_EQ(found, 1) << "resolution " << resolution.width << "x" << resolution.height
                                << " rate " << rate;
        }
    }
}

TEST(ModeTableTest, DensityComesFromTheDeviceConfigAndSurvivesEveryRate) {
    const auto table = BuiltFromDeviceConfig();
    // Geometry is a property of the resolution, never of the refresh rate: a mode
    // switch that changed density would move touch/cutout/UDFPS geometry with it.
    for (const auto& mode : table.modes()) {
        const auto density = table.DensityFor(mode.resolution.width, mode.resolution.height);
        ASSERT_TRUE(density);
        EXPECT_EQ(*density, mode.resolution.density);
    }
}

TEST(ModeTableTest, PeakMatchesTheAdvertisedPeakRefreshRate) {
    const auto table = BuiltFromDeviceConfig();
    EXPECT_EQ(table.PeakRefreshHz(), kDeclaredPeakRefreshHz);
}

TEST(ModeTableTest, AdaptiveTierIsExactlyTheTierWithARealDescentRange) {
    const auto table = BuiltFromDeviceConfig();
    std::string error;
    const auto payload = LoadAdfrConfig(TestData("infiniti-adfr.xml"), &error);
    ASSERT_TRUE(payload) << error;

    for (const int rate : table.refresh_rates()) {
        const int floor = ComputeAdfrFloor(*payload, rate);
        if (table.IsAdaptive(rate) && rate == kFloorGateModeHz) {
            // The gate mode is the only one that may actually descend, and against
            // the real payload it MUST: an adaptive tier whose table gives it
            // nowhere to go is a mislabelled tier, and `<=` would not notice.
            EXPECT_LT(floor, rate) << "rate " << rate;
        } else {
            // Every other mode holds its own rate - adaptive modes above the gate
            // because AOSP still wants that cadence, reserve modes because they are
            // explicit fixed rate. Neither may silently throttle.
            EXPECT_EQ(floor, rate) << "rate " << rate;
        }
    }
}

TEST(ModeTableTest, BuildRefusesContradictoryOrIncompleteDeclarations) {
    std::string error;
    const std::vector<Resolution> resolutions = {{1080, 2354, 450}, {1272, 2772, 560}};

    EXPECT_FALSE(ModeTable::Build({}, kRefreshRates, kAdaptiveRates, &error));
    EXPECT_FALSE(ModeTable::Build(resolutions, {}, {}, &error));
    // Same geometry twice is a contradiction, not a harmless repeat.
    EXPECT_FALSE(ModeTable::Build({{1080, 2354, 450}, {1080, 2354, 560}}, kRefreshRates,
                                  kAdaptiveRates, &error));
    EXPECT_FALSE(ModeTable::Build(resolutions, {60, 60, 90}, {60}, &error));
    EXPECT_FALSE(ModeTable::Build(resolutions, {60, 90}, {0}, &error));
    // An adaptive rate that is not a declared mode cannot be descended into.
    EXPECT_FALSE(ModeTable::Build(resolutions, {60, 90}, {120}, &error));
    EXPECT_FALSE(error.empty());
}

TEST(ModeTableTest, DensityMappingParserRejectsMalformedDeviceConfig) {
    std::string error;
    EXPECT_FALSE(ModeTable::ParseDensityMapping(TestData("displayconfig-nomapping.xml"), &error));
    EXPECT_FALSE(error.empty());
    EXPECT_FALSE(ModeTable::ParseDensityMapping(TestData("displayconfig-badrow.xml"), &error));
    EXPECT_FALSE(error.empty());
    EXPECT_FALSE(ModeTable::ParseDensityMapping(TestData("does-not-exist.xml"), &error));
    EXPECT_FALSE(error.empty());
}

}  // namespace
}  // namespace oplus::displaypanelfeature
