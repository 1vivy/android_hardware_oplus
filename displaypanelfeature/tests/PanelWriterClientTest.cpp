/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

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

namespace oplus::displaypanelfeature {
namespace {

class RecordingTransport final : public PanelFeatureTransport {
  public:
    bool Get(int32_t packedId, std::vector<int32_t>* values, std::string*) override {
        calls.emplace_back("get", packedId, std::vector<int32_t>{});
        *values = getValues;
        return succeed;
    }

    bool Set(int32_t packedId, const std::vector<int32_t>& values, std::string*) override {
        calls.emplace_back("set", packedId, values);
        return succeed;
    }

    struct Call {
        Call(std::string operation, int32_t id, std::vector<int32_t> payload)
            : operation(std::move(operation)), id(id), payload(std::move(payload)) {}
        std::string operation;
        int32_t id;
        std::vector<int32_t> payload;
    };

    bool succeed = true;
    std::vector<int32_t> getValues{1};
    std::vector<Call> calls;
};

std::shared_ptr<const FeatureRegistry> Registry() {
    const std::string path = android::base::GetExecutableDirectory() +
            "/configs/displaypanelfeature_infiniti.xml";
    std::string error;
    return FeatureRegistry::Load(path, &error);
}

TEST(PanelWriterClientTest, ResolvesEveryDirectWriterThroughTheShippedRegistry) {
    auto transport = std::make_shared<RecordingTransport>();
    const auto registry = Registry();
    ASSERT_NE(registry, nullptr);
    auto client = std::make_shared<DisplayPanelFeatureClient>(registry, transport);
    const PanelWriterClient writer(std::move(client));
    std::string error;
    bool enabled = false;

    ASSERT_TRUE(writer.SetDimlayerHbm(true, &error)) << error;
    ASSERT_TRUE(writer.SetFpPress(false, &error)) << error;
    ASSERT_TRUE(writer.GetUltraLowPowerAod(&enabled, &error)) << error;
    ASSERT_TRUE(writer.SetUltraLowPowerAod(false, &error)) << error;
    ASSERT_TRUE(writer.SetLowPwmAod(true, &error)) << error;

    EXPECT_TRUE(enabled);
    ASSERT_EQ(transport->calls.size(), 5U);
    EXPECT_EQ(transport->calls[0].operation, "set");
    EXPECT_EQ(transport->calls[0].id, 22);
    EXPECT_EQ(transport->calls[0].payload, std::vector<int32_t>({1}));
    EXPECT_EQ(transport->calls[1].operation, "set");
    EXPECT_EQ(transport->calls[1].id, 28);
    EXPECT_EQ(transport->calls[1].payload, std::vector<int32_t>({0}));
    EXPECT_EQ(transport->calls[2].operation, "get");
    EXPECT_EQ(transport->calls[2].id, 195);
    EXPECT_EQ(transport->calls[3].operation, "set");
    EXPECT_EQ(transport->calls[3].id, 195);
    EXPECT_EQ(transport->calls[3].payload, std::vector<int32_t>({0}));
    EXPECT_EQ(transport->calls[4].operation, "set");
    EXPECT_EQ(transport->calls[4].id, 263);
    EXPECT_EQ(transport->calls[4].payload, std::vector<int32_t>({1}));
}

TEST(PanelWriterClientTest, TransportFailureIsReportedWithoutFallbackWriter) {
    auto transport = std::make_shared<RecordingTransport>();
    transport->succeed = false;
    const auto registry = Registry();
    ASSERT_NE(registry, nullptr);
    auto client = std::make_shared<DisplayPanelFeatureClient>(registry, transport);
    const PanelWriterClient writer(std::move(client));
    std::string error;

    EXPECT_FALSE(writer.SetFpPress(true, &error));
    ASSERT_EQ(transport->calls.size(), 1U);
    EXPECT_EQ(transport->calls[0].id, 28);
}

}  // namespace
}  // namespace oplus::displaypanelfeature
