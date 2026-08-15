/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Dispatcher.h"
#include "FeatureRegistry.h"
#include "PanelBackend.h"

#include <android-base/file.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {
namespace {

// The SHIPPED generated map, not a hand-written fixture: these tests are only
// evidence about the server if they run against the same 101-row registry the
// device installs.
std::string ShippedRegistry() {
    return android::base::GetExecutableDirectory() + "/configs/displaypanelfeature_infiniti.xml";
}

// Counts every backend touch. "A rejected request performs zero backend calls"
// is asserted against `calls`, so a refusal that leaked through to hardware
// fails the test instead of passing quietly.
class CountingBackend final : public PanelBackend {
  public:
    bool Get(const FeatureEntry& entry, int32_t displayId, std::vector<int32_t>* values,
             std::string* error) override {
        ++calls;
        lastName = entry.name;
        lastDisplayId = displayId;
        if (!succeed) {
            *error = "fake backend refused";
            return false;
        }
        *values = getValues;
        return true;
    }

    bool Set(const FeatureEntry& entry, int32_t displayId, const std::vector<int32_t>& values,
             std::string* error) override {
        ++calls;
        lastName = entry.name;
        lastDisplayId = displayId;
        lastValues = values;
        if (!succeed) {
            *error = "fake backend refused";
            return false;
        }
        return true;
    }

    bool Info(const FeatureEntry& entry, int32_t displayId, std::vector<std::string>* info,
              std::string* error) override {
        ++calls;
        lastName = entry.name;
        lastDisplayId = displayId;
        if (!succeed) {
            *error = "fake backend refused";
            return false;
        }
        *info = {"panel"};
        return true;
    }

    int calls = 0;
    bool succeed = true;
    std::string lastName;
    int32_t lastDisplayId = -1;
    std::vector<int32_t> getValues{1};
    std::vector<int32_t> lastValues;
};

class DispatcherTest : public ::testing::Test {
  protected:
    void SetUp() override {
        std::string error;
        registry_ = FeatureRegistry::Load(ShippedRegistry(), &error);
        ASSERT_TRUE(registry_) << error;
        primary_ = registry_->DisplayId(DisplayRole::kPrimary);
        ASSERT_TRUE(primary_);
    }

    // Finds a shipped row by predicate so the tests name PROPERTIES of the map
    // rather than pinning feature ids, which belong to the derived dispatch.
    const FeatureEntry* FirstRow(bool (*predicate)(const FeatureEntry&)) const {
        for (const auto& entry : registry_->entries()) {
            if (predicate(entry)) return &entry;
        }
        return nullptr;
    }

    int32_t Pack(int32_t displayId, int32_t base) const { return (displayId << 12) | base; }

    std::shared_ptr<const FeatureRegistry> registry_;
    std::optional<int32_t> primary_;
};

TEST_F(DispatcherTest, ShippedRegistryStillDescribesTheDerivedMap) {
    // If the generated map is regenerated to a different shape these tests are
    // measuring something else, so say so here rather than everywhere below.
    EXPECT_FALSE(registry_->entries().empty());
    EXPECT_TRUE(registry_->DisplayId(DisplayRole::kSecondary).has_value());
}

TEST_F(DispatcherTest, UnknownFeatureIsRefusedWithoutTouchingTheBackend) {
    CountingBackend backend;
    std::string detail;
    std::vector<int32_t> values;
    // 0xfff cannot be a base id in the derived map: it is the mask itself.
    EXPECT_EQ(DispatchGet(*registry_, backend, Pack(*primary_, 0xfff), &values, &detail),
              Refusal::kUnknownFeature);
    EXPECT_EQ(backend.calls, 0);
}

TEST_F(DispatcherTest, ReservedRowIsRefusedWithoutTouchingTheBackend) {
    const auto* reserved =
            FirstRow([](const FeatureEntry& e) { return e.status == RowStatus::kReserved; });
    ASSERT_NE(reserved, nullptr);

    CountingBackend backend;
    std::string detail;
    std::vector<int32_t> values;
    EXPECT_EQ(DispatchGet(*registry_, backend, Pack(*primary_, reserved->id), &values, &detail),
              Refusal::kReservedFeature);
    EXPECT_EQ(backend.calls, 0);
}

TEST_F(DispatcherTest, UnknownDisplayIsRefusedWithoutTouchingTheBackend) {
    const auto* active =
            FirstRow([](const FeatureEntry& e) { return e.status == RowStatus::kActive; });
    ASSERT_NE(active, nullptr);

    CountingBackend backend;
    std::string detail;
    std::vector<int32_t> values;
    // 0xf is not a declared display id; the packed encoding must not be treated
    // as an offset into whatever the backend happens to accept.
    EXPECT_EQ(DispatchGet(*registry_, backend, Pack(0xf, active->id), &values, &detail),
              Refusal::kUnknownDisplay);
    EXPECT_EQ(backend.calls, 0);
}

TEST_F(DispatcherTest, WrongDirectionIsRefusedWithoutTouchingTheBackend) {
    const auto* getOnly = FirstRow([](const FeatureEntry& e) {
        return e.status == RowStatus::kActive && e.direction == Direction::kGet;
    });
    if (getOnly == nullptr) GTEST_SKIP() << "shipped map declares no active get-only row";

    CountingBackend backend;
    std::string detail;
    EXPECT_EQ(DispatchSet(*registry_, backend, Pack(*primary_, getOnly->id), {1}, &detail),
              Refusal::kWrongDirection);
    EXPECT_EQ(backend.calls, 0);
}

TEST_F(DispatcherTest, BadPayloadCountIsRefusedWithoutTouchingTheBackend) {
    const auto* scalar = FirstRow([](const FeatureEntry& e) {
        return e.status == RowStatus::kActive && e.shape == PayloadShape::kScalar &&
               (e.direction == Direction::kSet || e.direction == Direction::kBoth);
    });
    ASSERT_NE(scalar, nullptr);

    CountingBackend backend;
    std::string detail;
    const int32_t packed = Pack(*primary_, scalar->id);
    EXPECT_EQ(DispatchSet(*registry_, backend, packed, {1, 2}, &detail),
              Refusal::kBadPayloadCount);
    EXPECT_EQ(DispatchSet(*registry_, backend, packed, {}, &detail), Refusal::kBadPayloadCount);
    EXPECT_EQ(backend.calls, 0);
}

TEST_F(DispatcherTest, OutOfRangeValueIsRefusedWithoutTouchingTheBackend) {
    const auto* bounded = FirstRow([](const FeatureEntry& e) {
        return e.status == RowStatus::kActive && e.shape == PayloadShape::kScalar &&
               (e.direction == Direction::kSet || e.direction == Direction::kBoth) &&
               e.maximum < INT32_MAX;
    });
    ASSERT_NE(bounded, nullptr);

    CountingBackend backend;
    std::string detail;
    EXPECT_EQ(DispatchSet(*registry_, backend, Pack(*primary_, bounded->id),
                          {bounded->maximum + 1}, &detail),
              Refusal::kValueOutOfRange);
    EXPECT_EQ(backend.calls, 0);
}

TEST_F(DispatcherTest, AdmittedSetReachesTheBackendWithTheDecodedDisplay) {
    const auto* bounded = FirstRow([](const FeatureEntry& e) {
        return e.status == RowStatus::kActive && e.shape == PayloadShape::kScalar &&
               (e.direction == Direction::kSet || e.direction == Direction::kBoth) &&
               e.maximum < INT32_MAX;
    });
    ASSERT_NE(bounded, nullptr);

    CountingBackend backend;
    std::string detail;
    EXPECT_EQ(DispatchSet(*registry_, backend, Pack(*primary_, bounded->id), {bounded->maximum},
                          &detail),
              Refusal::kNone);
    EXPECT_EQ(backend.calls, 1);
    EXPECT_EQ(backend.lastName, bounded->name);
    EXPECT_EQ(backend.lastDisplayId, *primary_);
    EXPECT_EQ(backend.lastValues, std::vector<int32_t>{bounded->maximum});
}

TEST_F(DispatcherTest, SecondaryPanelIsAdmittedOnItsOwnDeclaredDisplayId) {
    const auto secondary = registry_->DisplayId(DisplayRole::kSecondary);
    ASSERT_TRUE(secondary);
    const auto* row = FirstRow([](const FeatureEntry& e) {
        return e.status == RowStatus::kActive && e.Allows(DisplayRole::kSecondary) &&
               (e.direction == Direction::kGet || e.direction == Direction::kBoth);
    });
    ASSERT_NE(row, nullptr);

    CountingBackend backend;
    std::string detail;
    std::vector<int32_t> values;
    EXPECT_EQ(DispatchGet(*registry_, backend, Pack(*secondary, row->id), &values, &detail),
              Refusal::kNone);
    EXPECT_EQ(backend.calls, 1);
    EXPECT_EQ(backend.lastDisplayId, *secondary);
}

TEST_F(DispatcherTest, BackendFailureIsDistinctFromRefusalAndIsReported) {
    const auto* row = FirstRow([](const FeatureEntry& e) {
        return e.status == RowStatus::kActive &&
               (e.direction == Direction::kGet || e.direction == Direction::kBoth);
    });
    ASSERT_NE(row, nullptr);

    CountingBackend backend;
    backend.succeed = false;
    std::string detail;
    std::vector<int32_t> values;
    // An admitted request that the hardware rejects must NOT be reported as a
    // refusal: conflating them would hide a dead panel behind a policy message.
    EXPECT_EQ(DispatchGet(*registry_, backend, Pack(*primary_, row->id), &values, &detail),
              Refusal::kBackendFailed);
    EXPECT_EQ(backend.calls, 1);
    EXPECT_FALSE(detail.empty());
}

// A config revision may move internal routing. What it may NOT do is quietly
// change what a capability MEANS: a moved route stays reachable under its own
// name, and a withdrawn one becomes explicitly unsupported rather than landing
// on whatever now occupies its old id.
class SchemaRevisionTest : public ::testing::Test {
  protected:
    void SetUp() override {
        std::string error;
        shipped_ = FeatureRegistry::Load(ShippedRegistry(), &error);
        ASSERT_TRUE(shipped_) << error;
        revision_ = FeatureRegistry::Load(
                android::base::GetExecutableDirectory() + "/tests/data/registry-revision-b.xml",
                &error);
        ASSERT_TRUE(revision_) << error;
    }

    static const FeatureEntry* ByName(const FeatureRegistry& registry, const std::string& name) {
        for (const auto& entry : registry.entries()) {
            if (entry.name == name) return &entry;
        }
        return nullptr;
    }

    int32_t Pack(int32_t displayId, int32_t base) const { return (displayId << 12) | base; }

    std::shared_ptr<const FeatureRegistry> shipped_;
    std::shared_ptr<const FeatureRegistry> revision_;
};

TEST_F(SchemaRevisionTest, RevisionMovesInternalRoutingNotSemanticIdentity) {
    const auto* before = ByName(*shipped_, "Seed");
    const auto* after = ByName(*revision_, "Seed");
    ASSERT_NE(before, nullptr);
    ASSERT_NE(after, nullptr);
    // The fixture exists to represent a MOVE; if the ids matched it would be
    // testing nothing.
    ASSERT_NE(before->id, after->id);
    EXPECT_EQ(before->shape, after->shape);
    EXPECT_EQ(before->minimum, after->minimum);
    EXPECT_EQ(before->maximum, after->maximum);

    CountingBackend backend;
    std::string detail;
    const auto primary = revision_->DisplayId(DisplayRole::kPrimary);
    ASSERT_TRUE(primary);
    EXPECT_EQ(DispatchSet(*revision_, backend, Pack(*primary, after->id), {after->maximum},
                          &detail),
              Refusal::kNone);
    EXPECT_EQ(backend.calls, 1);
    EXPECT_EQ(backend.lastName, "Seed");
}

TEST_F(SchemaRevisionTest, StaleRouteIdDoesNotSteerTheMovedCapability) {
    const auto* before = ByName(*shipped_, "Seed");
    ASSERT_NE(before, nullptr);

    CountingBackend backend;
    std::string detail;
    const auto primary = revision_->DisplayId(DisplayRole::kPrimary);
    ASSERT_TRUE(primary);
    // A caller holding the OLD id must not reach the moved capability. Under
    // this revision that id is a reserved slot, so it is refused outright.
    EXPECT_EQ(DispatchSet(*revision_, backend, Pack(*primary, before->id), {1}, &detail),
              Refusal::kReservedFeature);
    EXPECT_EQ(backend.calls, 0);
}

TEST_F(SchemaRevisionTest, WithdrawnCapabilityBecomesExplicitlyUnsupported) {
    const auto* withdrawn = ByName(*shipped_, "Cabc");
    ASSERT_NE(withdrawn, nullptr);
    ASSERT_EQ(ByName(*revision_, "Cabc"), nullptr);

    CountingBackend backend;
    std::string detail;
    std::vector<int32_t> values;
    const auto primary = revision_->DisplayId(DisplayRole::kPrimary);
    ASSERT_TRUE(primary);
    EXPECT_EQ(DispatchGet(*revision_, backend, Pack(*primary, withdrawn->id), &values, &detail),
              Refusal::kUnknownFeature);
    EXPECT_EQ(backend.calls, 0);
}

}  // namespace
}  // namespace oplus::displaypanelfeature
