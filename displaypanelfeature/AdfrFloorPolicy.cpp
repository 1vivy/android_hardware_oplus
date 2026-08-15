/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AdfrFloorPolicy.h"

#include <array>
#include <cstddef>

namespace oplus::displaypanelfeature {
namespace {

struct AdaptiveTable {
    int mode_hz;
    size_t offset;
    size_t capacity;
};

// Offsets/capacities mirror AdfrConfig.cpp's kTables entries for
// minfps120_level / minfps90_level / minfps60_level. Duplicated rather than shared
// because AdfrConfig.cpp's kTables is a translation-unit-local parser detail, while
// this table is policy: which modes have a legal descent range at all.
constexpr std::array<AdaptiveTable, 3> kAdaptiveTables = {{
        {120, 48, 13},
        {90, 61, 13},
        {60, 74, 13},
}};

const AdaptiveTable* FindTable(int active_mode_hz) {
    for (const auto& table : kAdaptiveTables) {
        if (table.mode_hz == active_mode_hz) return &table;
    }
    return nullptr;
}

}  // namespace

bool IsKnownPanelMode(int mode_hz) {
    switch (mode_hz) {
        case 60:
        case 90:
        case 120:
        case 144:
        case 165:
            return true;
        default:
            return false;
    }
}

std::optional<int> NextFloorWrite(const AdfrPayload& payload, int observed_mode_hz,
                                  std::optional<int> last_written) {
    if (!IsKnownPanelMode(observed_mode_hz)) return std::nullopt;
    const int floor_hz = ComputeAdfrFloor(payload, observed_mode_hz);
    if (last_written == floor_hz) return std::nullopt;
    return floor_hz;
}

int ComputeAdfrFloor(const AdfrPayload& payload, int active_mode_hz) {
    const AdaptiveTable* table = FindTable(active_mode_hz);
    if (table == nullptr) {
        // No adaptive table for this mode (144/165 reserve tier, or an unrecognized
        // mode): explicit fixed rate, never throttle below it.
        return active_mode_hz;
    }
    if (active_mode_hz != kFloorGateModeHz) {
        // AOSP has not settled at the lowest adaptive mode; treat content as active.
        return active_mode_hz;
    }

    int deepest = active_mode_hz;
    for (size_t i = 0; i < table->capacity; ++i) {
        const int value = payload[table->offset + i];
        // 0 is unfilled-table padding (see AdfrConfig.cpp::ReadTable), never a legal
        // fps value; skip it rather than letting it win the minimum.
        if (value > 0 && value < deepest) deepest = value;
    }
    return deepest;
}

}  // namespace oplus::displaypanelfeature
