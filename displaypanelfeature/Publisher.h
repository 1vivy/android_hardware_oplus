/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "DisplayPanelFeatureClient.h"

#include <map>
#include <string>

namespace oplus::displaypanelfeature {

class Publisher {
  public:
    Publisher(std::shared_ptr<const FeatureRegistry> registry,
              const DisplayPanelFeatureClient& client);

    // Called by property-wait/inotify subscribers. No timer or polling lives here.
    bool OnProperty(const std::string& name, const std::string& value, std::string* error);
    bool OnSysfsEvent(const std::string& path, const std::string& value, std::string* error);

  private:
    bool OnEvent(ValueSource source, const std::string& key, const std::string& value,
                 std::string* error);

    std::shared_ptr<const FeatureRegistry> registry_;
    const DisplayPanelFeatureClient& client_;
    std::map<std::string, std::string> published_;
};

}  // namespace oplus::displaypanelfeature
