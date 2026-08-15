/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "FeatureRegistry.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace oplus::displaypanelfeature {

// The only surface that touches panel hardware.
//
// The server admits a request against the generated registry BEFORE it builds
// a backend call, so "rejected request performs zero backend work" is an
// observable property of this interface rather than a claim: a test backend
// that counts invocations must see zero for every rejected request.
class PanelBackend {
  public:
    virtual ~PanelBackend() = default;

    virtual bool Get(const FeatureEntry& entry, int32_t displayId, std::vector<int32_t>* values,
                     std::string* error) = 0;
    virtual bool Set(const FeatureEntry& entry, int32_t displayId,
                     const std::vector<int32_t>& values, std::string* error) = 0;
    virtual bool Info(const FeatureEntry& entry, int32_t displayId,
                      std::vector<std::string>* info, std::string* error) = 0;
};

// Real backend over the OPLUS panel character device. Feature rows are bound to
// kernel requests by the GENERATED registry's feature name, never by a feature
// id authored here; a name with no kernel binding is reported unsupported
// instead of being silently accepted.
std::shared_ptr<PanelBackend> CreateIoctlPanelBackend(std::string devicePath);

// True when the generated registry row names a binding this backend implements.
bool HasKernelBinding(const std::string& name, bool set);

}  // namespace oplus::displaypanelfeature
