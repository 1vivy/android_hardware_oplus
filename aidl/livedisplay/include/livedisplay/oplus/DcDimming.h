/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {

/*
 * Explicit control of the OPLUS dim-layer alpha ("DC dimming").
 *
 * This is not a binder interface: LiveDisplay has no DC-dimming contract to
 * implement, so the control surface is a system property (see kProperty) that
 * this controller applies through the typed DPF client. The proprietary ODM
 * service remains the sole owner of /dev/oplus_display and its sysfs fallback.
 * The AntiFlicker ownership boundary and alpha semantics are documented at the
 * top of DcDimming.cpp.
 */
class DcDimming {
  public:
    /* True when the typed DPF contract answers the DC-dimming read. */
    bool isSupported();

    /*
     * Applies the persisted alpha, then watches kProperty and re-applies on
     * every change. Spawns one detached thread; call at most once.
     */
    void start();

    /* Property carrying the alpha: 0 = kernel-automatic, 1..kMaxAlpha = pinned. */
    static constexpr const char* kProperty = "persist.sys.oplus.display.dc_alpha";
    static constexpr int kMaxAlpha = 255;

  private:
    bool getDimAlpha(unsigned int* alpha);
    bool getDcAlpha(unsigned int* alpha);
    bool setAlphaOverride(unsigned int alpha);
    void apply();

};

}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
