/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AodPanelPolicy.h"

#include <utility>

namespace oplus::aod {

AodPanelPolicy::AodPanelPolicy(std::shared_ptr<displaypanelfeature::PanelWriterClient> panelWriter)
    : panelWriter_(std::move(panelWriter)) {}

bool AodPanelPolicy::Send(int64_t generation, AodPanelEdge edge, std::string* error) {
    std::lock_guard lock(mutex_);
    if (generation <= 0) {
        *error = "AOD generation must be positive";
        return false;
    }

    if (edge == AodPanelEdge::kEnter) {
        if (active_) {
            if (generation == generation_) return true;
            *error = "AOD generation changed before its exit edge";
            return false;
        }
        if (generation <= generation_) {
            *error = "AOD enter edge is stale or already closed";
            return false;
        }
        if (!panelWriter_->SetLongruiAodActive(true, error)) return false;
        generation_ = generation;
        active_ = true;
        return true;
    }

    if (generation != generation_) {
        *error = "AOD exit edge does not match the active generation";
        return false;
    }
    if (!active_) return true;
    if (!panelWriter_->SetLongruiAodActive(false, error)) return false;
    active_ = false;
    return true;
}

bool AodPanelPolicy::Replay(std::string* error) {
    std::lock_guard lock(mutex_);
    if (generation_ == 0) return true;
    return panelWriter_->SetLongruiAodActive(active_, error);
}

}  // namespace oplus::aod
