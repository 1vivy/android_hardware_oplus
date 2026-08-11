/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <DisplayPanelFeatureClient.h>

#include <cstdint>

namespace aidl::vendor::lineage::livedisplay::panel {

using ::oplus::displaypanelfeature::FeatureId;

bool Get(FeatureId feature, int32_t* value);
bool Set(FeatureId feature, int32_t value);

}  // namespace aidl::vendor::lineage::livedisplay::panel
