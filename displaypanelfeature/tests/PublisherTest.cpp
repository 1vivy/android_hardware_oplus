/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */
#include "Publisher.h"

#include <android-base/file.h>
#include <gtest/gtest.h>

namespace oplus::displaypanelfeature {
namespace {

class FakeTransport final : public PanelFeatureTransport {
  public:
    bool Get(int32_t, std::vector<int32_t>*, std::string*) override { return false; }
    bool Set(int32_t id, const std::vector<int32_t>& values, std::string*) override {
        ++calls;
        packed = id;
        payload = values;
        return true;
    }
    int calls = 0;
    int32_t packed = -1;
    std::vector<int32_t> payload;
};

TEST(PublisherTest, DispatchesOnlyExactEventsAndCoalescesDuplicates) {
    std::string error;
    const auto registry = FeatureRegistry::Load(
            android::base::GetExecutableDirectory() + "/tests/data/registry-publisher.xml", &error);
    ASSERT_NE(registry, nullptr) << error;
    auto transport = std::make_shared<FakeTransport>();
    DisplayPanelFeatureClient client(registry, transport);
    Publisher publisher(registry, client);

    EXPECT_TRUE(publisher.OnProperty("test.cabc", "1", &error)) << error;
    EXPECT_EQ(transport->packed, 184);
    EXPECT_EQ(transport->payload, std::vector<int32_t>({1}));
    EXPECT_TRUE(publisher.OnProperty("test.cabc", "1", &error));
    EXPECT_EQ(transport->calls, 1);
    EXPECT_FALSE(publisher.OnProperty("test.reserved", "1", &error));
    EXPECT_FALSE(publisher.OnProperty("test.unknown", "1", &error));
    EXPECT_EQ(transport->calls, 1);
}

}  // namespace
}  // namespace oplus::displaypanelfeature
