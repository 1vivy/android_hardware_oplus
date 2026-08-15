/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {

struct Resolution {
    int width = 0;
    int height = 0;
    int density = 0;

    bool operator==(const Resolution& other) const {
        return width == other.width && height == other.height && density == other.density;
    }
};

struct DisplayMode {
    Resolution resolution;
    int refresh_hz = 0;
    // True when the panel declares a legal descent range for this rate, i.e. the
    // ADFR payload carries a minfps table for it. Fixed-rate modes are not a
    // failure to configure - they are the panel's reserve tier.
    bool adaptive = false;
};

// The static mode contract: the complete cross product of the declared
// resolutions and refresh rates.
//
// It is BUILT from the tree's own declarations rather than authored: resolutions
// and densities come from the device `displayconfig.xml` density mapping, and the
// adaptive tier comes from the ADFR payload's minfps tables. A hand-written mode
// list is exactly what this replaces - `floor_daemon` previously carried one, and
// a hand list cannot notice when the device data moves underneath it.
class ModeTable {
  public:
    static std::optional<ModeTable> Build(std::vector<Resolution> resolutions,
                                          std::vector<int> refresh_rates,
                                          const std::vector<int>& adaptive_rates,
                                          std::string* error);

    // Parses ONLY the density mapping out of a device displayconfig.xml. The file
    // is the device tree's own, so nothing here re-declares resolutions.
    static std::optional<std::vector<Resolution>> ParseDensityMapping(const std::string& path,
                                                                      std::string* error);

    const std::vector<DisplayMode>& modes() const { return modes_; }
    const std::vector<Resolution>& resolutions() const { return resolutions_; }
    const std::vector<int>& refresh_rates() const { return refresh_rates_; }

    bool HasRefreshRate(int refresh_hz) const;
    bool IsAdaptive(int refresh_hz) const;
    std::optional<int> DensityFor(int width, int height) const;
    int PeakRefreshHz() const;

  private:
    std::vector<Resolution> resolutions_;
    std::vector<int> refresh_rates_;
    std::vector<int> adaptive_rates_;
    std::vector<DisplayMode> modes_;
};

}  // namespace oplus::displaypanelfeature
