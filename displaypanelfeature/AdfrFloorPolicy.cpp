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
    size_t interactive_offset;
    size_t aod_offset;
    size_t capacity;
};

// Offsets/capacities mirror AdfrConfig.cpp's kTables entries for
// minfps{120,90,60}_level and aod_minfps{120,90,60}_level. Duplicated rather than
// shared because AdfrConfig.cpp's kTables is a translation-unit-local parser detail,
// while this table is policy: which modes have a legal descent range at all.
//
// The AOD tier is REAL, SEPARATE data, not a copy of the interactive tier - the
// shipped payload declares 8 values where the interactive rows declare 10. Reading
// only the interactive tier left the AOD rows dead, and that was masked by a
// coincidence rather than by design: today the deepest positive value happens to be
// 1 in both tiers, so the returned number was right for the wrong reason and would
// have silently diverged the moment either tier was re-derived from another panel
// revision.
constexpr std::array<AdaptiveTable, 3> kAdaptiveTables = {{
        {120, 48, 117, 13},
        {90, 61, 130, 13},
        {60, 74, 143, 13},
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
                                  std::optional<int> last_written, PanelActivity activity) {
    if (!IsKnownPanelMode(observed_mode_hz)) return std::nullopt;
    const int floor_hz = ComputeAdfrFloor(payload, observed_mode_hz, activity);
    if (last_written == floor_hz) return std::nullopt;
    return floor_hz;
}

int ComputeAdfrFloor(const AdfrPayload& payload, int active_mode_hz, PanelActivity activity) {
    const AdaptiveTable* table = FindTable(active_mode_hz);
    if (table == nullptr) {
        // No adaptive table for this mode (144/165 reserve tier, or an unrecognized
        // mode): explicit fixed rate, never throttle below it.
        return active_mode_hz;
    }
    if (active_mode_hz != kFloorGateModeHz) {
        // AOSP has not settled at the lowest adaptive mode; treat content as active.
        // Deliberately IDENTICAL for both tiers: selecting which table to read is
        // derived from the shipped data, but changing WHEN the floor may descend
        // would be a policy invention, and the bucket semantics that would justify
        // one are still underived (see i33-aod-floor-finding.md).
        return active_mode_hz;
    }

    const size_t offset =
            activity == PanelActivity::kAod ? table->aod_offset : table->interactive_offset;
    int deepest = active_mode_hz;
    for (size_t i = 0; i < table->capacity; ++i) {
        const int value = payload[offset + i];
        // 0 is unfilled-table padding (see AdfrConfig.cpp::ReadTable), never a legal
        // fps value; skip it rather than letting it win the minimum.
        if (value > 0 && value < deepest) deepest = value;
    }
    return deepest;
}

}  // namespace oplus::displaypanelfeature
