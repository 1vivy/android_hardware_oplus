/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "oplus-dpf-backend"

#include "PanelBackend.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include <android-base/unique_fd.h>
#include <oplus/oplus_display_panel.h>
#include <sys/ioctl.h>

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <string_view>
#include <utility>

namespace oplus::displaypanelfeature {
namespace {

// Kernel request bindings.
//
// The KEY is the generated registry's feature name, never a feature id: the id
// space belongs to the derived dispatch map, and duplicating it here would be
// exactly the hand-authored route the derivation exists to prevent. A row whose
// name is absent below, or whose direction has no request, is unsupported and
// performs no ioctl.
//
// Every request is a macro from kernel-headers/include/oplus/oplus_display_panel.h;
// none is spelled as a literal here, so a kernel header change moves this
// binding with it instead of silently disagreeing.
struct Binding {
    unsigned long set;
    unsigned long get;
};
constexpr unsigned long kNone = 0;

const std::map<std::string_view, Binding> kBindings = {
        {"Seed", {PANEL_IOCTL_SET_SEED, PANEL_IOCTL_GET_SEED}},
        {"Aod", {PANEL_IOCTL_SET_AOD, PANEL_IOCTL_GET_AOD}},
        {"Hbm", {PANEL_IOCTL_SET_HBM, PANEL_IOCTL_GET_HBM}},
        {"DimAlpha", {kNone, PANEL_IOCTL_GET_DIM_ALPHA}},
        {"DimDcAlpha", {PANEL_IOCTL_SET_DIM_DC_ALPHA, PANEL_IOCTL_GET_DIM_DC_ALPHA}},
        {"DimlayerHbm", {PANEL_IOCTL_SET_DIMLAYER_HBM, kNone}},
        {"DimlayerBlEnable", {PANEL_IOCTL_SET_DIMLAYER_BL_EN, PANEL_IOCTL_GET_DIMLAYER_BL_EN}},
        {"FpPress", {PANEL_IOCTL_SET_FP_PRESS, kNone}},
        {"UltraLowPowerAod",
         {PANEL_IOCTL_SET_ULTRA_LOW_POWER_AOD, PANEL_IOCTL_GET_ULTRA_LOW_POWER_AOD}},
        {"Cabc", {PANEL_IOCTL_SET_CABC_STATUS, PANEL_IOCTL_GET_CABC_STATUS}},
        {"PwmTurbo", {PANEL_IOCTL_SET_PWM_TURBO, PANEL_IOCTL_GET_PWM_TURBO}},
        {"PwmPulse", {PANEL_IOCTL_SET_PWM_PULSE, PANEL_IOCTL_GET_PWM_PULSE}},
        {"LowPwmAod", {PANEL_IOCTL_SET_LOW_PWM_AOD, kNone}},
};

unsigned long Request(const std::string& name, bool set) {
    const auto binding = kBindings.find(name);
    if (binding == kBindings.end()) return kNone;
    return set ? binding->second.set : binding->second.get;
}

class IoctlPanelBackend final : public PanelBackend {
  public:
    explicit IoctlPanelBackend(std::string devicePath) : devicePath_(std::move(devicePath)) {}

    bool Get(const FeatureEntry& entry, int32_t displayId, std::vector<int32_t>* values,
             std::string* error) override {
        const unsigned long request = Request(entry.name, false);
        if (request == kNone) {
            *error = "feature " + entry.name + " has no kernel get binding";
            return false;
        }
        unsigned int payload = static_cast<unsigned int>(displayId);
        if (!Call(request, &payload, error)) return false;
        *values = {static_cast<int32_t>(payload)};
        return true;
    }

    bool Set(const FeatureEntry& entry, int32_t displayId, const std::vector<int32_t>& values,
             std::string* error) override {
        const unsigned long request = Request(entry.name, true);
        if (request == kNone) {
            *error = "feature " + entry.name + " has no kernel set binding";
            return false;
        }
        // The admitted payload is validated by the server; the kernel contract
        // carries one unsigned word, so a wider row is refused here rather than
        // truncated into a different write than the caller asked for.
        if (values.size() != 1) {
            *error = "feature " + entry.name + " kernel binding takes exactly one value";
            return false;
        }
        (void)displayId;
        unsigned int payload = static_cast<unsigned int>(values.front());
        return Call(request, &payload, error);
    }

    bool Info(const FeatureEntry& entry, int32_t displayId, std::vector<std::string>* info,
              std::string* error) override {
        (void)displayId;
        if (entry.source != ValueSource::kSysfsNode || entry.path.empty()) {
            *error = "feature " + entry.name + " publishes no panel info node";
            return false;
        }
        std::string content;
        if (!android::base::ReadFileToString(entry.path, &content)) {
            *error = "cannot read " + entry.path + ": " + std::strerror(errno);
            return false;
        }
        *info = {android::base::Trim(content)};
        return true;
    }

  private:
    bool Call(unsigned long request, unsigned int* payload, std::string* error) {
        android::base::unique_fd device(TEMP_FAILURE_RETRY(open(devicePath_.c_str(), O_RDWR)));
        if (device.get() < 0) {
            *error = "cannot open " + devicePath_ + ": " + std::strerror(errno);
            return false;
        }
        if (TEMP_FAILURE_RETRY(ioctl(device.get(), request, payload)) != 0) {
            *error = "ioctl on " + devicePath_ + " failed: " + std::strerror(errno);
            return false;
        }
        return true;
    }

    const std::string devicePath_;
};

}  // namespace

bool HasKernelBinding(const std::string& name, bool set) {
    return Request(name, set) != kNone;
}

std::shared_ptr<PanelBackend> CreateIoctlPanelBackend(std::string devicePath) {
    return std::make_shared<IoctlPanelBackend>(std::move(devicePath));
}

}  // namespace oplus::displaypanelfeature
