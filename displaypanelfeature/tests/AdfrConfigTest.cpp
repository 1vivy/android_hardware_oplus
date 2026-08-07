/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AdfrConfig.h"

#include <android-base/file.h>
#include <gtest/gtest.h>

#include <sstream>

namespace oplus::displaypanelfeature {
namespace {

std::string TestData(const char* name) {
    return android::base::GetExecutableDirectory() + "/tests/data/" + name;
}

TEST(AdfrConfigTest, PacksDocumentedInfinitiPayloadWhenConfigIsValid) {
    // Given
    std::string xml;
    std::string golden;
    ASSERT_TRUE(android::base::ReadFileToString(TestData("infiniti-adfr.xml"), &xml));
    ASSERT_TRUE(android::base::ReadFileToString(TestData("infiniti-adfr.payload"), &golden));

    // When
    std::string error;
    const auto payload = ParseAdfrConfig(xml, &error);

    // Then
    ASSERT_TRUE(payload) << error;
    std::ostringstream packed;
    for (const auto value : *payload) packed << value << ' ';
    EXPECT_EQ(packed.str(), golden);
    EXPECT_EQ(payload->size(), 225U);
    EXPECT_EQ((*payload)[0], 1);
    EXPECT_EQ((*payload)[1], 20250925);
}

TEST(AdfrConfigTest, RejectsConfigWhenVersionDiffers) {
    // Given
    std::string xml;
    ASSERT_TRUE(android::base::ReadFileToString(TestData("infiniti-adfr.xml"), &xml));
    xml.replace(xml.find("20250925"), 8, "20250924");

    // When
    std::string error;
    const auto payload = ParseAdfrConfig(xml, &error);

    // Then
    EXPECT_FALSE(payload);
    EXPECT_FALSE(error.empty());
}

}  // namespace
}  // namespace oplus::displaypanelfeature
