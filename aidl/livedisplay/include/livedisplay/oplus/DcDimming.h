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
 * this controller applies to the panel. It lives in the LiveDisplay service
 * because that process is already the owner of /dev/oplus_display, which keeps
 * the panel character device behind a single domain and needs no new sepolicy
 * domain, exec type or init service.
 *
 * The ioctl subset, the AntiFlicker ownership boundary, and the alpha semantics
 * are documented at the top of DcDimming.cpp. Read that before touching this.
 */
class DcDimming {
  public:
    DcDimming();
    ~DcDimming();

    /* True when the panel answers the DC-dimming read ioctl. */
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

    int mOplusDisplayFd;
};

}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
