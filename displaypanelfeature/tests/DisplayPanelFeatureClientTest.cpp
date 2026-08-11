/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DisplayPanelFeatureClient.h"
#include "FeatureRegistry.h"

#include <android-base/file.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {
namespace {

std::string TestData(const char* name) {
    return android::base::GetExecutableDirectory() + "/tests/data/" + name;
}

class FakeTransport final : public PanelFeatureTransport {
  public:
    bool Get(int32_t packedId, std::vector<int32_t>* values, std::string*) override {
        lastPackedId = packedId;
        *values = getValues;
        return succeed;
    }

    bool Set(int32_t packedId, const std::vector<int32_t>& values, std::string*) override {
        lastPackedId = packedId;
        lastValues = values;
        return succeed;
    }

    bool succeed = true;
    int32_t lastPackedId = -1;
    std::vector<int32_t> getValues{1};
    std::vector<int32_t> lastValues;
};

TEST(DisplayPanelFeatureClientTest, PacksTheRegistryDisplayIdCentrally) {
    std::string error;
    auto registry = FeatureRegistry::Load(TestData("registry-infiniti-95.xml"), &error);
    ASSERT_NE(registry, nullptr) << error;
    auto transport = std::make_shared<FakeTransport>();
    DisplayPanelFeatureClient client(registry, transport);

    EXPECT_TRUE(client.SetScalar(DisplayRole::kPrimary, FeatureId::kDre, 1, &error)) << error;
    EXPECT_EQ(transport->lastPackedId, 185);
    EXPECT_TRUE(client.SetScalar(DisplayRole::kSecondary, FeatureId::kDre, 0, &error)) << error;
    EXPECT_EQ(transport->lastPackedId, 4281);
}

TEST(DisplayPanelFeatureClientTest, DeniesReservedUnknownWrongDirectionShapeAndRange) {
    std::string error;
    auto registry = FeatureRegistry::Load(TestData("registry-infiniti-95.xml"), &error);
    ASSERT_NE(registry, nullptr) << error;
    auto transport = std::make_shared<FakeTransport>();
    DisplayPanelFeatureClient client(registry, transport);

    EXPECT_FALSE(client.SetScalar(DisplayRole::kPrimary, FeatureId::kPanelPower, 1, &error));
    EXPECT_EQ(error, "feature 1 is reserved");
    EXPECT_FALSE(client.SetScalar(DisplayRole::kPrimary, static_cast<FeatureId>(4096), 1, &error));
    EXPECT_EQ(error, "feature id is outside the base-id contract");
    EXPECT_FALSE(client.SetScalar(DisplayRole::kPrimary, FeatureId::kAdfrSupport, 1, &error));
    EXPECT_EQ(error, "feature 233 does not allow set");
    EXPECT_FALSE(client.Set(DisplayRole::kPrimary, FeatureId::kAdfrControl, {1}, &error));
    EXPECT_EQ(error, "feature 232 payload has the wrong shape");
    EXPECT_FALSE(client.SetScalar(DisplayRole::kPrimary, FeatureId::kSeed, 5, &error));
    EXPECT_EQ(error, "feature 4 payload is outside the declared range");
    EXPECT_EQ(transport->lastPackedId, -1);
}

TEST(DisplayPanelFeatureClientTest, RefusesARegistryForAnotherOemService) {
    std::string error;
    auto registry = FeatureRegistry::Load(TestData("registry-infiniti-95.xml"), &error);
    ASSERT_NE(registry, nullptr) << error;
    EXPECT_FALSE(registry->ValidateServiceHash(std::string(64, '0'), &error));
    EXPECT_EQ(error, "registry provenance does not match the serving OEM binary");
}

}  // namespace
}  // namespace oplus::displaypanelfeature
