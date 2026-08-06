/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * DC dimming - explicit control of the OPLUS dim-layer alpha.
 *
 * ---------------------------------------------------------------------------
 * OWNERSHIP BOUNDARY WITH AntiFlicker  (read before adding an ioctl here)
 * ---------------------------------------------------------------------------
 * AntiFlicker.cpp owns the flicker-POLICY chain and writes, in fallback order:
 *
 *     PANEL_IOCTL_SET/GET_PWM_PULSE        0x72 / 0x73
 *     PANEL_IOCTL_SET/GET_PWM_TURBO        0x66 / 0x67
 *     PANEL_IOCTL_SET/GET_DIMLAYER_BL_EN   0x21 / 0x22
 *
 * Those decide WHETHER the panel dims by high-frequency PWM or by a DC dim
 * layer. This file must never write any of them; two writers on one chain
 * would fight, and the last writer would win non-deterministically.
 *
 * DC dimming owns the dim-layer DEPTH, and only these:
 *
 *     PANEL_IOCTL_GET_DIM_ALPHA            0x12  _IOWR  read
 *     PANEL_IOCTL_SET_DIM_DC_ALPHA         0x13  _IOW   write (the override)
 *     PANEL_IOCTL_GET_DIM_DC_ALPHA         0x14  _IOWR  read  (composite state)
 *
 * The two roles compose rather than conflict: AntiFlicker picks the mechanism,
 * DC dimming tunes how deep the dim layer goes. 0x14 is the coordination point
 * - it folds oplus_dimlayer_bl_enable_v3_real into its answer, so we OBSERVE
 * AntiFlicker's contribution without ever writing its state.
 *
 * checks/01-ioctl-numbers.py enforces this boundary by parsing the ioctl call
 * sites out of both files, so the rule cannot silently drift from the code.
 *
 * ---------------------------------------------------------------------------
 * DELIBERATELY EXCLUDED from the DC family, with reasons
 * ---------------------------------------------------------------------------
 * PANEL_IOCTL_SET_DIM_ALPHA        0x11
 *     The kernel routes 0x11 and 0x13 to the same handler
 *     (oplus_display_device.c: PANEL_IOCTL_DEF -> oplus_display_panel_set_dim_alpha),
 *     so it is a pure alias. We issue the DC-named one to keep intent legible.
 *
 * PANEL_IOCTL_SET_DC_REAL_BACKLIGHT 0x37
 *     Defined in the kernel header but has NO PANEL_IOCTL_DEF entry in
 *     oplus_display_device.c, so the driver rejects it with -EINVAL. A #define
 *     is not evidence that an ioctl is dispatched; the check asserts dispatch.
 *
 * PANEL_IOCTL_SET_DC_COMPENSATE     0x74
 *     Not a dimming control. It latches dcc_flags and, on FILE_DESTROY, raises
 *     EXCEPTION_TRACKPOINT_REPORT("DCCompensate file destroied!!") - a
 *     DC-compensation-file health channel for OOS telemetry. Writing it here
 *     would forge an OEM fault report.
 *
 * ---------------------------------------------------------------------------
 * ALPHA SEMANTICS  (oplus_display_dc_diming.c)
 * ---------------------------------------------------------------------------
 * 0x13 stores oplus_panel_alpha, which OVERRIDES the kernel's computed
 * brightness->alpha curve: oplus_seed_bright_to_alpha() returns it verbatim
 * when nonzero and otherwise interpolates the panel's brightness_alpha_lut_dc.
 *
 *     0          release the override; kernel-automatic DC curve (stock, default)
 *     1..255     pin the dim-layer alpha; larger is darker, because the seed
 *                command path scales each byte by (255 - alpha) / 255
 *
 * 0x14 returns the first nonzero of oplus_dc2_alpha, oplus_underbrightness_alpha,
 * or 1 when oplus_dimlayer_bl_enable_v3_real - i.e. "is DC compensation doing
 * anything right now", from any source. That makes it the support probe too.
 *
 * 0x12 returns oplus_underbrightness_alpha and is forced to 0 whenever the panel
 * is not SDE_MODE_DPMS_ON, so a zero read on a blanked panel does NOT mean the
 * override was lost.
 */

#define LOG_TAG "DcDimmingController"

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <fcntl.h>
#include <livedisplay/oplus/DcDimming.h>
#include <oplus/oplus_display_panel.h>
#include <sys/system_properties.h>
#include <unistd.h>

#include <string>
#include <thread>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {

DcDimming::DcDimming() : mOplusDisplayFd(open("/dev/oplus_display", O_RDWR)) {}

DcDimming::~DcDimming() {
    if (mOplusDisplayFd >= 0) {
        close(mOplusDisplayFd);
    }
}

bool DcDimming::getDimAlpha(unsigned int* alpha) {
    return ioctl(mOplusDisplayFd, PANEL_IOCTL_GET_DIM_ALPHA, alpha) == 0;
}

bool DcDimming::getDcAlpha(unsigned int* alpha) {
    return ioctl(mOplusDisplayFd, PANEL_IOCTL_GET_DIM_DC_ALPHA, alpha) == 0;
}

bool DcDimming::setAlphaOverride(unsigned int alpha) {
    return ioctl(mOplusDisplayFd, PANEL_IOCTL_SET_DIM_DC_ALPHA, &alpha) == 0;
}

bool DcDimming::isSupported() {
    unsigned int value;
    return mOplusDisplayFd >= 0 && getDcAlpha(&value);
}

void DcDimming::apply() {
    int alpha = ::android::base::GetIntProperty(kProperty, 0, 0, kMaxAlpha);

    if (!setAlphaOverride(static_cast<unsigned int>(alpha))) {
        PLOG(ERROR) << "Failed to set DC dimming alpha override to " << alpha;
        return;
    }

    unsigned int dimAlpha = 0;
    unsigned int dcState = 0;
    getDimAlpha(&dimAlpha);
    getDcAlpha(&dcState);
    LOG(INFO) << "DC dimming override=" << alpha
              << (alpha == 0 ? " (kernel-automatic)" : " (pinned)")
              << " dim_alpha=" << dimAlpha << " dc_state=" << dcState;
}

void DcDimming::start() {
    if (!isSupported()) {
        LOG(INFO) << "DC dimming is not supported by this panel, leaving it alone";
        return;
    }

    apply();

    std::thread([this] {
        std::string last = ::android::base::GetProperty(kProperty, "");
        const prop_info* pi = nullptr;
        uint32_t serial = 0;
        uint32_t globalSerial = 0;

        while (true) {
            if (pi == nullptr) {
                pi = __system_property_find(kProperty);
            }
            /*
             * Until the property exists there is nothing to wait on, so fall
             * back to the global serial - a fresh flash reaches this before
             * the settings surface first writes the property.
             */
            const bool ok = pi != nullptr
                                    ? __system_property_wait(pi, serial, &serial, nullptr)
                                    : __system_property_wait(nullptr, globalSerial,
                                                             &globalSerial, nullptr);
            if (!ok) {
                LOG(ERROR) << "__system_property_wait failed, stopping DC dimming watch";
                return;
            }

            std::string current = ::android::base::GetProperty(kProperty, "");
            if (current == last) {
                continue;
            }
            last = current;
            apply();
        }
    }).detach();
}

}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
