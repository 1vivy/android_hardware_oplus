/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {

// The panel device grants only two concurrent O_RDWR opens. Every class in this
// HAL must share this one descriptor rather than opening its own, or the later
// openers silently receive -1 and skip every ioctl they issue.
int GetPanelFd();

}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
