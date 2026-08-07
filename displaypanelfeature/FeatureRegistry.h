/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {

enum class Direction { kSet, kGet, kBoth };
enum class PayloadShape { kScalar, kPair, kTriple, kAdfrControl, kAdfrConfig, kProbe };
enum class ValueSource { kProperty, kAdfrXml, kSupportProbe };

struct FeatureEntry {
    int32_t id;
    Direction direction;
    PayloadShape shape;
    ValueSource source;
    std::string property;
};

std::optional<std::vector<FeatureEntry>> LoadFeatureRegistry(const std::string& path,
                                                             std::string* error);
std::optional<std::vector<int32_t>> ParsePropertyPayload(const FeatureEntry& entry,
                                                         const std::string& value);

}  // namespace oplus::displaypanelfeature
