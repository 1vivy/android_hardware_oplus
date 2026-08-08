/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "livedisplay-panelfd"

#include "livedisplay/oplus/PanelFd.h"

#include <android-base/logging.h>
#include <fcntl.h>

namespace aidl {
namespace vendor {
namespace lineage {
namespace livedisplay {

int GetPanelFd() {
    static const int fd = [] {
        const int opened = open("/dev/oplus_display", O_RDWR);
        if (opened < 0) {
            PLOG(ERROR) << "failed to open /dev/oplus_display; panel controls are inert";
        }
        return opened;
    }();
    return fd;
}

}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
