/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/logging.h>
#include <android/binder_auto_utils.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <string>
#include <string_view>
#include <vector>

#include <aidl/vendor/oplus/hardware/subsys_interface/subsys_radio/ISubsysRadio.h>

#include "PendingRequests.h"
#include "RadioCallbacks.h"

using ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ISubsysRadio;
using ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::RadioIndication;
using ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::RadioResponse;
using ::oplus::radio::ExitCodeFor;
using ::oplus::radio::FormatJson;
using ::oplus::radio::kExitMalformed;
using ::oplus::radio::kExitOk;
using ::oplus::radio::kExitServiceAbsent;
using ::oplus::radio::kExitUsage;
using ::oplus::radio::kExitVendorError;
using ::oplus::radio::Operation;
using ::oplus::radio::OperationName;
using ::oplus::radio::Outcome;
using ::oplus::radio::PendingRequests;
using ::oplus::radio::Result;

namespace {

constexpr auto kInstancePrefix =
        "vendor.oplus.hardware.subsys_interface.subsys_radio.ISubsysRadio/";
constexpr int kDefaultTimeoutMs = 3000;

constexpr auto kUsage =
        "usage:\n"
        "  oplus_radio_probe --dsda-state [--slot slot1|slot2|all] [--timeout-ms 3000]\n"
        "  oplus_radio_probe --satellite-cal-state [--slot slot1|slot2] [--timeout-ms 3000]\n"
        "  oplus_radio_probe --services\n";

void EmitJson(std::string_view slot, std::string_view service, std::string_view operation,
              const Result& result) {
    printf("%s\n", FormatJson(slot, service, operation, result).c_str());
}

/* AServiceManager_waitForService cannot be cancelled, so it is bounded by
 * running it on a detached future and abandoning it at the deadline. A probe
 * that blocked forever would hang the validation battery. */
ndk::SpAIBinder WaitForServiceBounded(const std::string& instance, std::chrono::milliseconds budget) {
    auto pending = std::async(std::launch::async, [instance] {
        return ndk::SpAIBinder(AServiceManager_waitForService(instance.c_str()));
    });
    if (pending.wait_for(budget) != std::future_status::ready) return ndk::SpAIBinder();
    return pending.get();
}

void OnBinderDied(void* cookie) {
    auto* pending = static_cast<PendingRequests*>(cookie);
    LOG(ERROR) << "vendor radio service died with a request in flight";
    pending->FailPending(Outcome::kMalformed);
}

int ProbeSlot(const std::string& slot, Operation op, std::chrono::milliseconds budget) {
    const std::string instance = kInstancePrefix + slot;

    if (!AServiceManager_isDeclared(instance.c_str())) {
        EmitJson(slot, "undeclared", OperationName(op), Result{Outcome::kServiceAbsent});
        return kExitServiceAbsent;
    }

    auto binder = WaitForServiceBounded(instance, budget);
    auto radio = ISubsysRadio::fromBinder(binder);
    if (radio == nullptr) {
        EmitJson(slot, "absent", OperationName(op), Result{Outcome::kServiceAbsent});
        return kExitServiceAbsent;
    }

    PendingRequests pending;
    ndk::ScopedAIBinder_DeathRecipient recipient(AIBinder_DeathRecipient_new(OnBinderDied));
    AIBinder_linkToDeath(binder.get(), recipient.get(), &pending);

    auto response = ndk::SharedRefBase::make<RadioResponse>(pending);
    auto indication = ndk::SharedRefBase::make<RadioIndication>(pending);
    if (auto status = radio->setCallback(response, indication); !status.isOk()) {
        LOG(ERROR) << "setCallback failed on " << slot << ": " << status.getDescription();
        AIBinder_unlinkToDeath(binder.get(), recipient.get(), &pending);
        EmitJson(slot, "ready", OperationName(op), Result{Outcome::kMalformed});
        return kExitMalformed;
    }

    /* Registered before the request is issued: the vendor may answer a oneway
     * call before the caller returns from it. */
    const int32_t serial = pending.Open(op, PendingRequests::Clock::now() + budget);
    ndk::ScopedAStatus issued = op == Operation::kDsdaState
                                        ? radio->getMsimSubModeState(serial)
                                        : radio->getSatelliteCalibrationDataState(serial);
    if (!issued.isOk()) {
        LOG(ERROR) << OperationName(op) << " rejected on " << slot << ": "
                   << issued.getDescription();
        AIBinder_unlinkToDeath(binder.get(), recipient.get(), &pending);
        EmitJson(slot, "ready", OperationName(op), Result{Outcome::kVendorError});
        return kExitVendorError;
    }

    Result result = pending.Await(serial);
    AIBinder_unlinkToDeath(binder.get(), recipient.get(), &pending);

    if (pending.RefusedReplies() > 0) {
        LOG(WARNING) << "refused " << pending.RefusedReplies() << " stale/mismatched reply(ies) on "
                     << slot;
    }
    EmitJson(slot, "ready", OperationName(op), result);
    return ExitCodeFor(result.outcome);
}

int ReportServices(const std::vector<std::string>& slots) {
    int rc = kExitOk;
    for (const auto& slot : slots) {
        const std::string instance = kInstancePrefix + slot;
        const bool declared = AServiceManager_isDeclared(instance.c_str());
        ndk::SpAIBinder binder;
        if (declared) binder = ndk::SpAIBinder(AServiceManager_checkService(instance.c_str()));

        const char* state = !declared ? "undeclared" : (binder.get() != nullptr ? "ready" : "absent");
        Result result;
        result.outcome = (binder.get() != nullptr) ? Outcome::kOk : Outcome::kServiceAbsent;
        EmitJson(slot, state, "services", result);
        if (result.outcome != Outcome::kOk) rc = kExitServiceAbsent;
    }
    return rc;
}

}  // namespace

int main(int argc, char** argv) {
    android::base::InitLogging(argv);
    android::base::SetDefaultTag("OplusRadioProbe");

    bool want_dsda = false;
    bool want_satellite = false;
    bool want_services = false;
    std::string slot_arg;
    int timeout_ms = kDefaultTimeoutMs;

    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];
        if (arg == "--dsda-state") {
            want_dsda = true;
        } else if (arg == "--satellite-cal-state") {
            want_satellite = true;
        } else if (arg == "--services") {
            want_services = true;
        } else if (arg == "--slot" && i + 1 < argc) {
            slot_arg = argv[++i];
        } else if (arg == "--timeout-ms" && i + 1 < argc) {
            timeout_ms = atoi(argv[++i]);
        } else {
            fprintf(stderr, "unknown argument: %s\n%s", argv[i], kUsage);
            return kExitUsage;
        }
    }

    if (want_dsda + want_satellite + want_services != 1 || timeout_ms <= 0) {
        fprintf(stderr, "%s", kUsage);
        return kExitUsage;
    }

    std::vector<std::string> slots;
    if (want_satellite) {
        /* The satellite read is per-slot by contract; 'all' is not offered so a
         * caller cannot fan a regional query out by accident. */
        if (slot_arg.empty()) slot_arg = "slot1";
        if (slot_arg != "slot1" && slot_arg != "slot2") {
            fprintf(stderr, "--satellite-cal-state accepts --slot slot1|slot2\n%s", kUsage);
            return kExitUsage;
        }
        slots = {slot_arg};
    } else {
        if (slot_arg.empty()) slot_arg = "all";
        if (slot_arg == "all") {
            slots = {"slot1", "slot2"};
        } else if (slot_arg == "slot1" || slot_arg == "slot2") {
            slots = {slot_arg};
        } else {
            fprintf(stderr, "unknown slot: %s\n%s", slot_arg.c_str(), kUsage);
            return kExitUsage;
        }
    }

    ABinderProcess_setThreadPoolMaxThreadCount(1);
    ABinderProcess_startThreadPool();

    if (want_services) return ReportServices(slots);

    const auto budget = std::chrono::milliseconds(timeout_ms);
    const Operation op = want_dsda ? Operation::kDsdaState : Operation::kSatelliteCalState;

    int rc = kExitOk;
    for (const auto& slot : slots) {
        const int slot_rc = ProbeSlot(slot, op, budget);
        if (slot_rc != kExitOk && rc == kExitOk) rc = slot_rc;
    }
    return rc;
}
