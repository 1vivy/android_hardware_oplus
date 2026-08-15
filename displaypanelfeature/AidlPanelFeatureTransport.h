/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "DisplayPanelFeatureClient.h"

#include <memory>

namespace oplus::displaypanelfeature {

// Non-blocking factory. Calls reconnect after service death or early boot absence.
std::shared_ptr<PanelFeatureTransport> CreateAidlPanelFeatureTransport();
std::shared_ptr<DisplayPanelFeatureClient> CreateAidlDisplayPanelFeatureClient(std::string* error);

}  // namespace oplus::displaypanelfeature
