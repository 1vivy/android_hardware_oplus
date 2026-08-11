/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {

enum class Direction { kSet, kGet, kBoth };
enum class PayloadShape { kScalar, kPair, kTriple, kAdfrControl, kAdfrConfig, kProbe };
enum class ValueSource { kNone, kProperty, kAdfrXml, kSupportProbe, kSysfsNode };
enum class RowStatus { kActive, kReserved };
enum class DisplayRole { kPrimary, kSecondary };

struct FeatureEntry {
    int32_t id = 0;
    std::string name;
    Direction direction = Direction::kGet;
    PayloadShape shape = PayloadShape::kProbe;
    ValueSource source = ValueSource::kNone;
    std::string property;
    std::string path;
    double scale = 1.0;
    double offset = 0.0;
    std::string handler;
    std::string producer;
    RowStatus status = RowStatus::kActive;
    std::vector<DisplayRole> displays;
    int32_t minimum = INT32_MIN;
    int32_t maximum = INT32_MAX;

    size_t PayloadCount() const;
    bool Allows(DisplayRole role) const;
};

class FeatureRegistry {
  public:
    static std::shared_ptr<const FeatureRegistry> Load(const std::string& path, std::string* error);

    const FeatureEntry* Find(int32_t id) const;
    std::optional<int32_t> DisplayId(DisplayRole role) const;
    bool ValidateServiceHash(const std::string& actualHash, std::string* error) const;

    const std::vector<FeatureEntry>& entries() const { return entries_; }
    const std::string& service_hash() const { return serviceHash_; }

  private:
    std::string serviceHash_;
    std::map<DisplayRole, int32_t> displays_;
    std::vector<FeatureEntry> entries_;
};

// Compatibility parsing helpers for property and sysfs producers. New callers
// use FeatureRegistry::Load, which applies the full provenance/display contract.
std::optional<std::vector<FeatureEntry>> LoadFeatureRegistry(const std::string& path,
                                                             std::string* error);
std::optional<std::vector<int32_t>> ParsePropertyPayload(const FeatureEntry& entry,
                                                         const std::string& value);
std::optional<std::vector<int32_t>> ParseSysfsPayload(const FeatureEntry& entry,
                                                      const std::string& value);
std::optional<std::string> Sha256File(const std::string& path);

}  // namespace oplus::displaypanelfeature
