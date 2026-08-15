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

// A transport that echoes writes back on read, i.e. a panel that HONOURS the
// write. `lie` makes it accept the write and keep reporting the old value -
// exactly the DEF-COL-01 signature of a successful call with no panel change.
class EchoTransport final : public PanelFeatureTransport {
  public:
    bool Get(int32_t packedId, std::vector<int32_t>* values, std::string*) override {
        ++gets;
        *values = {stored};
        return true;
    }

    bool Set(int32_t packedId, const std::vector<int32_t>& values, std::string*) override {
        ++sets;
        lastPackedId = packedId;
        if (!lie) stored = values.front();
        return true;
    }

    bool lie = false;
    int32_t stored = 0;
    int sets = 0;
    int gets = 0;
    int32_t lastPackedId = -1;
};

TEST(DisplayPanelFeatureClientTest, VerifiedSetPassesOnlyWhenThePanelReportsTheValue) {
    std::string error;
    auto registry = FeatureRegistry::Load(TestData("registry-infiniti-95.xml"), &error);
    ASSERT_NE(registry, nullptr) << error;
    auto transport = std::make_shared<EchoTransport>();
    DisplayPanelFeatureClient client(registry, transport);

    EXPECT_TRUE(client.SetScalarVerified(DisplayRole::kPrimary, FeatureId::kSeed, 3, &error))
            << error;
    EXPECT_EQ(transport->stored, 3);
    EXPECT_EQ(transport->sets, 1);
    EXPECT_EQ(transport->gets, 1);
}

TEST(DisplayPanelFeatureClientTest, VerifiedSetFailsWhenTheWriteIsAcceptedButIgnored) {
    std::string error;
    auto registry = FeatureRegistry::Load(TestData("registry-infiniti-95.xml"), &error);
    ASSERT_NE(registry, nullptr) << error;
    auto transport = std::make_shared<EchoTransport>();
    transport->lie = true;
    transport->stored = 1;
    DisplayPanelFeatureClient client(registry, transport);

    // This is DEF-COL-01: the transport refuses nothing, so a plain SetScalar
    // reports success while the panel still shows the previous profile.
    EXPECT_TRUE(client.SetScalar(DisplayRole::kPrimary, FeatureId::kSeed, 3, &error)) << error;

    EXPECT_FALSE(client.SetScalarVerified(DisplayRole::kPrimary, FeatureId::kSeed, 3, &error));
    EXPECT_EQ(error, "feature 4 did not take: wrote 3, panel reports 1");
}

TEST(DisplayPanelFeatureClientTest, VerifiedSetRefusesAnUnreadableRowWithoutWriting) {
    std::string error;
    auto registry = FeatureRegistry::Load(TestData("registry-infiniti-95.xml"), &error);
    ASSERT_NE(registry, nullptr) << error;
    auto transport = std::make_shared<EchoTransport>();
    DisplayPanelFeatureClient client(registry, transport);

    // kAdfrControl is set-only, so no readback can prove it took. Refuse BEFORE
    // writing rather than leaving the panel changed by a call reporting failure.
    EXPECT_FALSE(
            client.SetScalarVerified(DisplayRole::kPrimary, FeatureId::kAdfrControl, 1, &error));
    EXPECT_EQ(error, "feature 232 cannot be read back, so a write to it cannot be verified");
    EXPECT_EQ(transport->sets, 0);
    EXPECT_EQ(transport->gets, 0);
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
