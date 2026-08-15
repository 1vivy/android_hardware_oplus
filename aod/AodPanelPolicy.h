/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "PanelWriterClient.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

namespace oplus::aod {

enum class AodPanelEdge { kEnter, kExit };

class AodPanelPolicy {
  public:
    explicit AodPanelPolicy(std::shared_ptr<displaypanelfeature::PanelWriterClient> panelWriter);

    bool Send(int64_t generation, AodPanelEdge edge, std::string* error);
    bool Replay(std::string* error);

  private:
    std::shared_ptr<displaypanelfeature::PanelWriterClient> panelWriter_;
    std::mutex mutex_;
    int64_t generation_ = 0;
    bool active_ = false;
};

}  // namespace oplus::aod
