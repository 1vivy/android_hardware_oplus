/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AdfrControlOwner.h"

#include <utility>

namespace oplus::displaypanelfeature {

AdfrControlOwner::AdfrControlOwner(Writer writer) : writer_(std::move(writer)) {}

bool AdfrControlOwner::Apply(std::vector<int32_t> values, std::string* error) {
    // Retain BEFORE writing. A write that never lands is precisely the case this
    // owner exists for, so the desired value must survive it.
    desired_ = std::move(values);
    outstanding_ = true;
    return Reapply(error);
}

bool AdfrControlOwner::Reapply(std::string* error) {
    if (!desired_.has_value() || !outstanding_) {
        return true;
    }
    if (!writer_(*desired_, error)) {
        return false;
    }
    outstanding_ = false;
    return true;
}

}  // namespace oplus::displaypanelfeature
