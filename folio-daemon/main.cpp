/*
 * SPDX-FileCopyrightText: 2017 The Android Open Source Project
 * SPDX-FileCopyrightText: 2023-2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "StateParser.h"

#include <cutils/log.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include <array>
#include <optional>
#include <string_view>

namespace {

constexpr char kMagneticCoverStatus[] = "/proc/magnetic_cover/magcvr_config_para";
constexpr useconds_t kPollPeriodUs = 250000;
constexpr time_t kWarnPeriodSeconds = 300;

int createUinputDevice() {
    const int fd = TEMP_FAILURE_RETRY(open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CLOEXEC));
    if (fd < 0) {
        ALOGE("Unable to open uinput node: %s", strerror(errno));
        return -1;
    }

    if (TEMP_FAILURE_RETRY(ioctl(fd, UI_SET_EVBIT, EV_SW)) != 0 ||
        TEMP_FAILURE_RETRY(ioctl(fd, UI_SET_EVBIT, EV_SYN)) != 0 ||
        TEMP_FAILURE_RETRY(ioctl(fd, UI_SET_SWBIT, SW_LID)) != 0) {
        ALOGE("Unable to enable SW_LID events: %s", strerror(errno));
        close(fd);
        return -1;
    }

    struct uinput_user_dev device{};
    snprintf(device.name, UINPUT_MAX_NAME_SIZE, "uinput-folio");
    device.id.bustype = BUS_VIRTUAL;

    if (TEMP_FAILURE_RETRY(write(fd, &device, sizeof(device))) !=
                static_cast<ssize_t>(sizeof(device)) ||
        TEMP_FAILURE_RETRY(ioctl(fd, UI_DEV_CREATE)) != 0) {
        ALOGE("Unable to create uinput device: %s", strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}

std::optional<bool> readClosedState() {
    const int fd = TEMP_FAILURE_RETRY(open(kMagneticCoverStatus, O_RDONLY | O_CLOEXEC));
    if (fd < 0) {
        return std::nullopt;
    }

    std::array<char, 256> status{};
    const ssize_t length = TEMP_FAILURE_RETRY(read(fd, status.data(), status.size() - 1));
    const int savedErrno = errno;
    close(fd);
    errno = savedErrno;
    if (length <= 0) {
        return std::nullopt;
    }

    const std::optional<bool> closed =
            folio::parseClosed(std::string_view(status.data(), static_cast<std::size_t>(length)));
    if (!closed.has_value()) {
        errno = EINVAL;
    }
    return closed;
}

bool writeInputEvent(int fd, __u16 type, __u16 code, __s32 value) {
    struct input_event event{};
    event.type = type;
    event.code = code;
    event.value = value;
    return TEMP_FAILURE_RETRY(write(fd, &event, sizeof(event))) ==
           static_cast<ssize_t>(sizeof(event));
}

bool sendLidState(int fd, bool closed) {
    return writeInputEvent(fd, EV_SW, SW_LID, closed ? 1 : 0) &&
           writeInputEvent(fd, EV_SYN, SYN_REPORT, 0);
}

}  // namespace

int main() {
    ALOGI("Started");

    const int uinputFd = createUinputDevice();
    if (uinputFd < 0) {
        return 1;
    }

    ALOGI("Registered uinput-folio for SW_LID events");
    std::optional<bool> lastState;
    time_t lastWarn = 0;

    for (;;) {
        const std::optional<bool> closed = readClosedState();
        if (!closed.has_value()) {
            const time_t now = time(nullptr);
            if (now > lastWarn + kWarnPeriodSeconds) {
                ALOGE("Unable to read magnetic-cover state: %s", strerror(errno));
                lastWarn = now;
            }
        } else if (closed != lastState) {
            if (!sendLidState(uinputFd, *closed)) {
                ALOGE("Unable to write SW_LID event: %s", strerror(errno));
                close(uinputFd);
                return 1;
            }
            lastState = closed;
            ALOGI("Sent lid %s event", *closed ? "closed" : "open");
        }

        usleep(kPollPeriodUs);
    }
}
