/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "oplus-dpf-server"

#include "Server.h"

#include <android-base/logging.h>

#include <utility>

namespace oplus::displaypanelfeature {
namespace {

// The stock interface reports outcome through its own int return value, and
// every opaque client in the consumer census compares that value with zero.
// Refusals therefore travel as this value rather than as a binder exception,
// which would change the ABI those clients depend on.
constexpr int32_t kOk = 0;
constexpr int32_t kFailed = -1;

int32_t Report(const char* operation, int32_t featureId, Refusal refusal,
               const std::string& detail) {
    if (refusal == Refusal::kNone) return kOk;
    LOG(WARNING) << operation << " feature=" << featureId << " refused=" << RefusalName(refusal)
                 << (detail.empty() ? "" : " detail=" + detail);
    return kFailed;
}

}  // namespace

DisplayPanelFeatureServer::DisplayPanelFeatureServer(
        std::shared_ptr<const FeatureRegistry> registry, std::shared_ptr<PanelBackend> backend)
    : registry_(std::move(registry)), backend_(std::move(backend)) {}

ndk::ScopedAStatus DisplayPanelFeatureServer::getDisplayPanelFeatureValue(
        int32_t featureId, std::vector<int32_t>* featureValues, int32_t* result) {
    std::string detail;
    const Refusal refusal =
            DispatchGet(*registry_, *backend_, featureId, featureValues, &detail);
    *result = Report("get", featureId, refusal, detail);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus DisplayPanelFeatureServer::setDisplayPanelFeatureValue(
        int32_t featureId, const std::vector<int32_t>& featureValues, int32_t* result) {
    std::string detail;
    const Refusal refusal = DispatchSet(*registry_, *backend_, featureId, featureValues, &detail);
    *result = Report("set", featureId, refusal, detail);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus DisplayPanelFeatureServer::getDisplayPanelInfo(
        int32_t featureId, std::vector<std::string>* panelInfo, int32_t* result) {
    std::string detail;
    const Refusal refusal = DispatchInfo(*registry_, *backend_, featureId, panelInfo, &detail);
    *result = Report("info", featureId, refusal, detail);
    return ndk::ScopedAStatus::ok();
}

}  // namespace oplus::displaypanelfeature
