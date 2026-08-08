/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FeatureRegistry.h"

#include <android-base/file.h>
#include <gtest/gtest.h>

#include <algorithm>

namespace oplus::displaypanelfeature {
namespace {

std::string TestData(const char* name) {
    return android::base::GetExecutableDirectory() + "/tests/data/" + name;
}

const FeatureEntry* FindId(const std::vector<FeatureEntry>& entries, int32_t id) {
    const auto it = std::find_if(entries.begin(), entries.end(),
                                  [id](const FeatureEntry& entry) { return entry.id == id; });
    return it == entries.end() ? nullptr : &*it;
}

TEST(FeatureRegistryTest, ParsesLegacyThreeSourceRowsWithoutTheNewAttributes) {
    // Given: a row written before kSysfsNode/producer/status existed.
    // When
    std::string error;
    const auto entries = LoadFeatureRegistry(TestData("registry-legacy.xml"), &error);

    // Then
    ASSERT_TRUE(entries) << error;
    const auto* aod = FindId(*entries, 217);
    ASSERT_NE(aod, nullptr);
    EXPECT_EQ(aod->source, ValueSource::kProperty);
    EXPECT_EQ(aod->status, RowStatus::kActive);
    EXPECT_TRUE(aod->producer.empty());
    EXPECT_TRUE(aod->path.empty());
    EXPECT_DOUBLE_EQ(aod->scale, 1.0);
    EXPECT_DOUBLE_EQ(aod->offset, 0.0);
}

TEST(FeatureRegistryTest, RejectsSysfsNodeRowMissingPath) {
    // Given / When
    std::string error;
    const auto entries = LoadFeatureRegistry(TestData("registry-missing-path.xml"), &error);

    // Then
    EXPECT_FALSE(entries);
    EXPECT_EQ(error, "sysfs-node source is missing its path");
}

TEST(FeatureRegistryTest, RejectsUnknownStatusValue) {
    // Given / When
    std::string error;
    const auto entries = LoadFeatureRegistry(TestData("registry-bad-status.xml"), &error);

    // Then
    EXPECT_FALSE(entries);
    EXPECT_EQ(error, "invalid status attribute");
}

TEST(FeatureRegistryTest, RejectsUnknownSourceValue) {
    // Given / When
    std::string error;
    const auto entries = LoadFeatureRegistry(TestData("registry-unknown-source.xml"), &error);

    // Then
    EXPECT_FALSE(entries);
}

TEST(FeatureRegistryTest, ParsesSysfsNodeRowAndKeepsReservedOutOfTheActiveSet) {
    // Given: DPF id 29, a kSysfsNode row with no named consumer.
    // When
    std::string error;
    const auto entries = LoadFeatureRegistry(TestData("registry-sample.xml"), &error);

    // Then
    ASSERT_TRUE(entries) << error;
    const auto* brightness = FindId(*entries, 29);
    ASSERT_NE(brightness, nullptr);
    EXPECT_EQ(brightness->source, ValueSource::kSysfsNode);
    EXPECT_EQ(brightness->path, "/sys/kernel/oplus_display/oplus_brightness");
    EXPECT_EQ(brightness->status, RowStatus::kReserved);
    EXPECT_FALSE(brightness->producer.empty());
}

// This is the corrected row from wave7/DESIGN-registry-collapse.md: DPF id 4 was
// previously (wrongly) documented as inert/status="reserved". Measured this
// session, id 4 is the only seed write path that reaches the panel driver, so
// the registry must carry it active, with its real producer documented.
TEST(FeatureRegistryTest, IdFourIsCorrectedToActiveWithADocumentedProducer) {
    // Given
    std::string error;
    const auto entries = LoadFeatureRegistry(TestData("registry-sample.xml"), &error);

    // When
    ASSERT_TRUE(entries) << error;
    const auto* seed = FindId(*entries, 4);

    // Then
    ASSERT_NE(seed, nullptr);
    EXPECT_EQ(seed->status, RowStatus::kActive);
    EXPECT_FALSE(seed->producer.empty());
}

TEST(FeatureRegistryTest, ParseSysfsPayloadAppliesScaleAndOffsetAndRounds) {
    // Given
    FeatureEntry entry;
    entry.source = ValueSource::kSysfsNode;
    entry.shape = PayloadShape::kScalar;
    entry.scale = 0.5;
    entry.offset = 10.0;

    // When
    const auto payload = ParseSysfsPayload(entry, "7\n");

    // Then
    ASSERT_TRUE(payload);
    ASSERT_EQ(payload->size(), 1U);
    EXPECT_EQ((*payload)[0], 14);  // 7 * 0.5 + 10 = 13.5 -> rounds to 14
}

TEST(FeatureRegistryTest, ParseSysfsPayloadRejectsNonNumericContent) {
    // Given
    FeatureEntry entry;
    entry.source = ValueSource::kSysfsNode;
    entry.shape = PayloadShape::kScalar;

    // When
    const auto payload = ParseSysfsPayload(entry, "not-a-number\n");

    // Then
    EXPECT_FALSE(payload);
}

TEST(FeatureRegistryTest, ParseSysfsPayloadRejectsNonScalarShapes) {
    // Given: kSysfsNode is only defined for a single value (see DESIGN-registry-collapse.md).
    FeatureEntry entry;
    entry.source = ValueSource::kSysfsNode;
    entry.shape = PayloadShape::kPair;

    // When
    const auto payload = ParseSysfsPayload(entry, "1 2\n");

    // Then
    EXPECT_FALSE(payload);
}

}  // namespace
}  // namespace oplus::displaypanelfeature
