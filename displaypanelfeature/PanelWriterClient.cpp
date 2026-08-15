/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "PanelWriterClient.h"

#include <utility>

namespace oplus::displaypanelfeature {
namespace {

constexpr char kDimlayerHbm[] = "DimlayerHbm";
constexpr char kFpPress[] = "FpPress";
constexpr char kUltraLowPowerAod[] = "UltraLowPowerAod";
constexpr char kLowPwmAod[] = "LowPwmAod";
constexpr char kLongruiAodState[] = "LongruiAodState";

}  // namespace

PanelWriterClient::PanelWriterClient(std::shared_ptr<DisplayPanelFeatureClient> client)
    : client_(std::move(client)) {}

bool PanelWriterClient::GetBool(const std::string& feature, bool* value, std::string* error) const {
    int32_t result = 0;
    if (!client_->GetScalar(DisplayRole::kPrimary, feature, &result, error)) {
        return false;
    }
    *value = result > 0;
    return true;
}

bool PanelWriterClient::SetBool(const std::string& feature, bool value, std::string* error) const {
    return client_->SetScalar(DisplayRole::kPrimary, feature, value ? 1 : 0, error);
}

bool PanelWriterClient::SetDimlayerHbm(bool enabled, std::string* error) const {
    return SetBool(kDimlayerHbm, enabled, error);
}

bool PanelWriterClient::SetFpPress(bool pressed, std::string* error) const {
    return SetBool(kFpPress, pressed, error);
}

bool PanelWriterClient::GetUltraLowPowerAod(bool* enabled, std::string* error) const {
    return GetBool(kUltraLowPowerAod, enabled, error);
}

bool PanelWriterClient::SetUltraLowPowerAod(bool enabled, std::string* error) const {
    return SetBool(kUltraLowPowerAod, enabled, error);
}

bool PanelWriterClient::SetLowPwmAod(bool enabled, std::string* error) const {
    return SetBool(kLowPwmAod, enabled, error);
}

bool PanelWriterClient::SetLongruiAodActive(bool active, std::string* error) const {
    constexpr int32_t kOff = 0;
    constexpr int32_t kWorkshopAod = 2;
    return client_->SetScalar(DisplayRole::kPrimary, kLongruiAodState, active ? kWorkshopAod : kOff,
                              error);
}

}  // namespace oplus::displaypanelfeature
