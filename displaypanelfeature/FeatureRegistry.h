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
enum class ValueSource { kProperty, kAdfrXml, kSupportProbe, kSysfsNode };
enum class RowStatus { kActive, kReserved };

struct FeatureEntry {
    int32_t id;
    Direction direction;
    PayloadShape shape;
    ValueSource source;
    std::string property;          // source == kProperty
    std::string path;              // source == kSysfsNode
    double scale = 1.0;            // source == kSysfsNode
    double offset = 0.0;           // source == kSysfsNode
    std::string producer;          // documentation only, any source
    RowStatus status = RowStatus::kActive;
};

std::optional<std::vector<FeatureEntry>> LoadFeatureRegistry(const std::string& path,
                                                             std::string* error);
std::optional<std::vector<int32_t>> ParsePropertyPayload(const FeatureEntry& entry,
                                                         const std::string& value);
std::optional<std::vector<int32_t>> ParseSysfsPayload(const FeatureEntry& entry,
                                                      const std::string& value);

}  // namespace oplus::displaypanelfeature
