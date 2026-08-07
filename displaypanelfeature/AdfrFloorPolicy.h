/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "AdfrConfig.h"

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

int ComputeAdfrFloor(const AdfrPayload& payload, int active_mode_hz);

}  // namespace oplus::displaypanelfeature
