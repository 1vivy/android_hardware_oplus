/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AodPanelPolicy.h"
#include "DisplayPanelFeatureClient.h"
#include "FeatureRegistry.h"
#include "PanelWriterClient.h"

#include <android-base/file.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace oplus::aod {
namespace {

class RecordingTransport final : public displaypanelfeature::PanelFeatureTransport {
  public:
    bool Get(int32_t, std::vector<int32_t>*, std::string*) override { return false; }

    bool Set(int32_t packedId, const std::vector<int32_t>& values, std::string*) override {
        calls.emplace_back(packedId, values);
        return succeed;
    }

    struct Call {
        Call(int32_t id, std::vector<int32_t> payload) : id(id), payload(std::move(payload)) {}
        int32_t id;
        std::vector<int32_t> payload;
    };

    bool succeed = true;
    std::vector<Call> calls;
};

struct PolicyFixture {
    PolicyFixture() {
        const std::string path = android::base::GetExecutableDirectory() +
                                 "/configs/displaypanelfeature_infiniti.xml";
        std::string error;
        auto registry = displaypanelfeature::FeatureRegistry::Load(path, &error);
        EXPECT_NE(registry, nullptr) << error;
        auto client = std::make_shared<displaypanelfeature::DisplayPanelFeatureClient>(
                std::move(registry), transport);
        writer = std::make_shared<displaypanelfeature::PanelWriterClient>(std::move(client));
        policy = std::make_unique<AodPanelPolicy>(writer);
    }

    std::shared_ptr<RecordingTransport> transport = std::make_shared<RecordingTransport>();
    std::shared_ptr<displaypanelfeature::PanelWriterClient> writer;
    std::unique_ptr<AodPanelPolicy> policy;
};

TEST(AodPanelPolicyTest, EmitsOneEnterAndExitPerGeneration) {
    PolicyFixture fixture;
    std::string error;

    ASSERT_TRUE(fixture.policy->Send(1, AodPanelEdge::kEnter, &error)) << error;
    ASSERT_TRUE(fixture.policy->Send(1, AodPanelEdge::kEnter, &error)) << error;
    ASSERT_TRUE(fixture.policy->Send(1, AodPanelEdge::kExit, &error)) << error;
    ASSERT_TRUE(fixture.policy->Send(1, AodPanelEdge::kExit, &error)) << error;

    ASSERT_EQ(fixture.transport->calls.size(), 2U);
    EXPECT_EQ(fixture.transport->calls[0].id, 217);
    EXPECT_EQ(fixture.transport->calls[0].payload, std::vector<int32_t>({2}));
    EXPECT_EQ(fixture.transport->calls[1].id, 217);
    EXPECT_EQ(fixture.transport->calls[1].payload, std::vector<int32_t>({0}));
}

TEST(AodPanelPolicyTest, RejectsStaleAndUnpairedEdgesBeforeTransport) {
    PolicyFixture fixture;
    std::string error;

    EXPECT_FALSE(fixture.policy->Send(1, AodPanelEdge::kExit, &error));
    ASSERT_TRUE(fixture.policy->Send(2, AodPanelEdge::kEnter, &error)) << error;
    EXPECT_FALSE(fixture.policy->Send(3, AodPanelEdge::kEnter, &error));
    EXPECT_FALSE(fixture.policy->Send(1, AodPanelEdge::kExit, &error));
    ASSERT_TRUE(fixture.policy->Send(2, AodPanelEdge::kExit, &error)) << error;

    ASSERT_EQ(fixture.transport->calls.size(), 2U);
}

TEST(AodPanelPolicyTest, FailedBackendWriteDoesNotAdvanceGeneration) {
    PolicyFixture fixture;
    std::string error;
    fixture.transport->succeed = false;

    EXPECT_FALSE(fixture.policy->Send(1, AodPanelEdge::kEnter, &error));
    fixture.transport->succeed = true;
    EXPECT_TRUE(fixture.policy->Send(1, AodPanelEdge::kEnter, &error)) << error;

    ASSERT_EQ(fixture.transport->calls.size(), 2U);
    EXPECT_EQ(fixture.transport->calls[0].payload, std::vector<int32_t>({2}));
    EXPECT_EQ(fixture.transport->calls[1].payload, std::vector<int32_t>({2}));
}

TEST(AodPanelPolicyTest, ReplayReappliesTheAcceptedStateAfterDpfRestart) {
    PolicyFixture fixture;
    std::string error;
    ASSERT_TRUE(fixture.policy->Send(1, AodPanelEdge::kEnter, &error)) << error;
    fixture.transport->calls.clear();

    ASSERT_TRUE(fixture.policy->Replay(&error)) << error;
    ASSERT_EQ(fixture.transport->calls.size(), 1U);
    EXPECT_EQ(fixture.transport->calls[0].payload, std::vector<int32_t>({2}));

    ASSERT_TRUE(fixture.policy->Send(1, AodPanelEdge::kExit, &error)) << error;
    fixture.transport->calls.clear();
    ASSERT_TRUE(fixture.policy->Replay(&error)) << error;
    ASSERT_EQ(fixture.transport->calls.size(), 1U);
    EXPECT_EQ(fixture.transport->calls[0].payload, std::vector<int32_t>({0}));
}

}  // namespace
}  // namespace oplus::aod
