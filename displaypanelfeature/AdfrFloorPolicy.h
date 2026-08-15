/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "AdfrConfig.h"

#include <optional>

namespace oplus::displaypanelfeature {

// Derives the ADFR minimum-fps floor legal for the currently active display mode,
// purely from the already-published 225-int ADFR payload (see AdfrConfig.h). No
// content/histogram/sensor telemetry is read or required.
//
// - 60/90/120 are the panel's adaptive tier: the shipped minfpsNN_level tables (see
//   AdfrConfig.cpp's kTables) enumerate every legal descent value for that mode.
// - 144/165 have no adaptive table in this payload (the panel's reserve/high-rate
//   tier); ComputeAdfrFloor returns them unchanged, i.e. explicit fixed rate.
// - The floor only descends below the active mode's own rate once AOSP has itself
//   settled at kFloorGateModeHz (the lowest mode in the adaptive tier) - the signal
//   that AOSP judges content idle. Any other active mode keeps the floor at the
//   mode's own rate, so the panel cannot self-throttle while AOSP wants faster
//   cadence.
constexpr int kFloorGateModeHz = 60;

// Which descent tier of the shipped payload applies. The payload declares two
// genuinely different tiers - minfps{120,90,60}_level and aod_minfps{120,90,60}_level
// - and reading only the interactive one left the AOD rows dead data.
//
// Selecting the tier is DERIVED (the shipped tables are named for it). Everything
// else about AOD is not: the bucket structure pairing positions with brightness/gray
// thresholds is underived, and the shipped registry names no AOD state signal at all
// (its row 217 is `OemFeature217`, reserved/unowned - the "AOD" naming appears only
// in a host fixture that declares itself non-shipped). So the caller states the
// activity explicitly; nothing here infers it. See i33-aod-floor-finding.md.
enum class PanelActivity { kInteractive, kAod };

int ComputeAdfrFloor(const AdfrPayload& payload, int active_mode_hz,
                     PanelActivity activity = PanelActivity::kInteractive);

// The panel's declared mode set. Anything else - unset, stale, or malformed - is
// "not yet known" and must never reach a sysfs write.
bool IsKnownPanelMode(int mode_hz);

// The daemon's whole decision, as a pure function of the observed mode and what was
// last written: the floor to write now, or nothing.
//
// This exists as a seam so the decision is host-testable without a device. The
// daemon around it is then only "wait for a property edge, ask this, write" - it
// holds no policy of its own and, in particular, no timer: an unknown mode returns
// nothing rather than being retried on a clock.
std::optional<int> NextFloorWrite(const AdfrPayload& payload, int observed_mode_hz,
                                  std::optional<int> last_written,
                                  PanelActivity activity = PanelActivity::kInteractive);

}  // namespace oplus::displaypanelfeature
