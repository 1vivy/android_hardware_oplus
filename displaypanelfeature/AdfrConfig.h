/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace oplus::displaypanelfeature {

constexpr size_t kAdfrPayloadSize = 225;
using AdfrPayload = std::array<int32_t, kAdfrPayloadSize>;

std::optional<AdfrPayload> ParseAdfrConfig(std::string_view xml, std::string* error);
std::optional<AdfrPayload> LoadAdfrConfig(const std::string& path, std::string* error);

}  // namespace oplus::displaypanelfeature
