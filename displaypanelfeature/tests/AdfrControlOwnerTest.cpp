/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AdfrControlOwner.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace oplus::displaypanelfeature {
namespace {

// Stands in for the AIDL transport. A dead handle refuses the write and drops
// the connection, which is what makes the FOLLOWING call rebind; it never
// re-sends the value by itself.
class FakePanel {
  public:
    bool Write(const std::vector<int32_t>& values, std::string* error) {
        if (dead_) {
            *error = "set rejected: binder=DEAD_OBJECT";
            ++rebinds_;
            return false;
        }
        writes_.push_back(values);
        return true;
    }

    AdfrControlOwner::Writer writer() {
        return [this](const std::vector<int32_t>& values, std::string* error) {
            return Write(values, error);
        };
    }

    void Kill() { dead_ = true; }
    void Revive() { dead_ = false; }
    size_t writes() const { return writes_.size(); }
    int rebinds() const { return rebinds_; }
    const std::vector<int32_t>& last() const { return writes_.back(); }

  private:
    std::vector<std::vector<int32_t>> writes_;
    bool dead_ = false;
    int rebinds_ = 0;
};

constexpr int32_t kAdfrOn = 0;

TEST(AdfrControlOwnerTest, AppliedValueReachesThePanelAndConverges) {
    FakePanel panel;
    AdfrControlOwner owner(panel.writer());
    std::string error;

    ASSERT_TRUE(owner.Apply({kAdfrOn, 120}, &error)) << error;

    EXPECT_EQ(panel.last(), (std::vector<int32_t>{kAdfrOn, 120}));
    EXPECT_TRUE(owner.converged());
}

TEST(AdfrControlOwnerTest, DesiredIsRetainedAcrossAServiceDeath) {
    FakePanel panel;
    AdfrControlOwner owner(panel.writer());
    std::string error;
    ASSERT_TRUE(owner.Apply({kAdfrOn, 60}, &error)) << error;

    panel.Kill();
    const size_t before = panel.writes();

    EXPECT_FALSE(owner.Apply({kAdfrOn, 120}, &error));
    EXPECT_EQ(panel.writes(), before) << "a dead service must not record a write";
    EXPECT_EQ(panel.rebinds(), 1);
    ASSERT_TRUE(owner.desired().has_value());
    EXPECT_EQ(*owner.desired(), (std::vector<int32_t>{kAdfrOn, 120}));
    EXPECT_FALSE(owner.converged());
}

TEST(AdfrControlOwnerTest, DesiredIsReappliedOnTheNextCallAfterDeath) {
    FakePanel panel;
    AdfrControlOwner owner(panel.writer());
    std::string error;
    ASSERT_TRUE(owner.Apply({kAdfrOn, 60}, &error)) << error;
    panel.Kill();
    ASSERT_FALSE(owner.Apply({kAdfrOn, 120}, &error));

    panel.Revive();
    ASSERT_TRUE(owner.Reapply(&error)) << error;

    EXPECT_EQ(panel.last(), (std::vector<int32_t>{kAdfrOn, 120}))
            << "the rebind restores the channel; the owner must restore the value";
    EXPECT_TRUE(owner.converged());
}

TEST(AdfrControlOwnerTest, ReapplyIsANoOpOnceTheDesiredValueIsOnThePanel) {
    // Without this the owner would rewrite id 232 on every unrelated event, which
    // is panel traffic the rebind contract never asked for.
    FakePanel panel;
    AdfrControlOwner owner(panel.writer());
    std::string error;
    ASSERT_TRUE(owner.Apply({kAdfrOn, 120}, &error)) << error;
    const size_t settled = panel.writes();

    EXPECT_TRUE(owner.Reapply(&error));

    EXPECT_EQ(panel.writes(), settled);
}

TEST(AdfrControlOwnerTest, ReapplyWithNothingDesiredWritesNothing) {
    FakePanel panel;
    AdfrControlOwner owner(panel.writer());
    std::string error;

    EXPECT_TRUE(owner.Reapply(&error));

    EXPECT_EQ(panel.writes(), 0u);
    EXPECT_FALSE(owner.converged()) << "no desired value is not the same as converged";
}

}  // namespace
}  // namespace oplus::displaypanelfeature
