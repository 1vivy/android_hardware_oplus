/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {

// The single retaining owner of the ADFR control row (DPF base id 232,
// setAdfr2MinfpsParam).
//
// The AIDL transport already rebinds after STATUS_DEAD_OBJECT, so the next call
// reaches a live server. That is a CHANNEL guarantee, not a VALUE guarantee: the
// publisher wrote id 232 exactly once at startup, so a server restart left the
// panel holding whatever the fresh server came up with while every client still
// believed the desired cadence was in force. Rebinding converges the connection
// and diverges the state.
//
// This owner closes that gap by keeping the desired payload and rewriting it on
// the next call after a failed write. It holds no policy: what the desired value
// IS remains the caller's decision, and nothing here infers a value the caller
// never asked for.
class AdfrControlOwner {
  public:
    // Writes one id-232 payload. Returns false and fills error when the write did
    // not reach the panel; the transport underneath treats a dead handle as a
    // rebind for the following call.
    using Writer = std::function<bool(const std::vector<int32_t>&, std::string*)>;

    explicit AdfrControlOwner(Writer writer);

    // Records values as desired, then writes them. The value is retained whether
    // or not the write succeeds - a refused write is exactly when retention
    // matters.
    bool Apply(std::vector<int32_t> values, std::string* error);

    // Rewrites the retained desired payload if a write is still outstanding.
    // A no-op (reported as success) when nothing is desired yet or the desired
    // value is already on the panel, so callers may drive this from any event
    // without generating redundant panel traffic.
    bool Reapply(std::string* error);

    const std::optional<std::vector<int32_t>>& desired() const { return desired_; }

    // True once a desired payload exists and no write for it is outstanding.
    bool converged() const { return desired_.has_value() && !outstanding_; }

  private:
    Writer writer_;
    std::optional<std::vector<int32_t>> desired_;
    bool outstanding_ = false;
};

}  // namespace oplus::displaypanelfeature
