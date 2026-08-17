/*
 * Copyright (C) 2025 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "effect.h"

#include <json/json.h>
#include <string>
#include <unordered_map>

// The shipped vibrator_effect.json declares more than one style tier for the
// panel/model key (measured on infiniti and on stock OOS: def_style with 88
// effects and soft_style with 22). Loading only one leaves the other tier's
// payloads installed and never opened, which is how DEF-AOD-01 arose on the
// ADFR side. This loader therefore carries every declared tier and selects one.
class VibrationEffectLoader {
  public:
    // Style tier requested through kStyleProperty. kDefaultStyle is used when
    // the property is unset, empty, or names a tier this device does not ship,
    // so an unreadable selection can never silence haptics.
    static constexpr const char* kStyleProperty = "persist.sys.oplus.vibrator.effect_style";
    static constexpr const char* kDefaultStyle = "def_style";

    VibrationEffectLoader();
    ~VibrationEffectLoader();

    effect_stream* getEffectStream(uint32_t effect_id);

    // Exposed for the host contract test: which tiers were loaded, and which
    // one is active.
    const std::string& activeStyle() const { return active_style_; }
    bool hasStyle(const std::string& style) const { return styles_.count(style) != 0; }

  private:
    using EffectMap = std::unordered_map<uint32_t, effect_stream>;

    Json::Value parseEffectJson(std::ifstream& config_stream);
    void loadStyles(Json::Value&& model_node);
    void loadStyle(const std::string& style, const Json::Value& effect_nodes);
    static std::string requestedStyle();

    std::unordered_map<std::string, EffectMap> styles_;
    std::string active_style_;
};
