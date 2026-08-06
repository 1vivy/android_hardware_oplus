/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>

namespace oplus::radio {

enum class Operation {
    kDsdaState,
    kSatelliteCalState,
};

enum class Outcome {
    kPending,
    kOk,
    kServiceAbsent,
    kTimeout,
    kVendorError,
    kMalformed,
};

struct DsdaState {
    int32_t msim_sub_mode = 0;
    int32_t dsda_tx_mode = 0;
};

struct SatelliteCalState {
    int32_t label = 0;
    int32_t backup_source = 0;
    int32_t restore_success = 0;
};

struct Result {
    Outcome outcome = Outcome::kPending;
    int32_t vendor_error = 0;
    std::optional<DsdaState> dsda;
    std::optional<SatelliteCalState> satellite;
};

inline const char* OperationName(Operation op) {
    return op == Operation::kDsdaState ? "dsda-state" : "satellite-cal-state";
}

inline const char* OutcomeName(Outcome outcome) {
    switch (outcome) {
        case Outcome::kPending: return "pending";
        case Outcome::kOk: return "ok";
        case Outcome::kServiceAbsent: return "service-absent";
        case Outcome::kTimeout: return "timeout";
        case Outcome::kVendorError: return "vendor-error";
        case Outcome::kMalformed: return "malformed";
    }
    return "malformed";
}

enum : int {
    kExitOk = 0,
    kExitServiceAbsent = 2,
    kExitTimeout = 3,
    kExitVendorError = 4,
    kExitMalformed = 5,
    kExitUsage = 64,
};

inline int ExitCodeFor(Outcome outcome) {
    switch (outcome) {
        case Outcome::kOk: return kExitOk;
        case Outcome::kServiceAbsent: return kExitServiceAbsent;
        case Outcome::kTimeout: return kExitTimeout;
        case Outcome::kVendorError: return kExitVendorError;
        case Outcome::kPending:
        case Outcome::kMalformed: return kExitMalformed;
    }
    return kExitMalformed;
}

/*
 * Renders one schema-1 record. Every emitted field is an opaque vendor status
 * integer; the probe never renders an IMEI, a subscriber or NV identifier, or
 * any calibration payload, so no redaction pass is needed downstream. The
 * SatelliteCalDataStatusType byte counts and backup timestamp are deliberately
 * not rendered - they describe stored calibration data rather than its state.
 */
inline std::string FormatJson(std::string_view slot, std::string_view service,
                              std::string_view operation, const Result& result) {
    std::string line = "{\"schema\":1";
    line += ",\"slot\":\"" + std::string(slot) + "\"";
    line += ",\"service\":\"" + std::string(service) + "\"";
    line += ",\"operation\":\"" + std::string(operation) + "\"";
    line += ",\"status\":\"" + std::string(OutcomeName(result.outcome)) + "\"";
    if (result.vendor_error != 0) {
        line += ",\"vendorError\":" + std::to_string(result.vendor_error);
    }
    if (result.dsda.has_value()) {
        line += ",\"msimSubMode\":" + std::to_string(result.dsda->msim_sub_mode);
        line += ",\"dsdaTxMode\":" + std::to_string(result.dsda->dsda_tx_mode);
    }
    if (result.satellite.has_value()) {
        line += ",\"calLabel\":" + std::to_string(result.satellite->label);
        line += ",\"calBackupSource\":" + std::to_string(result.satellite->backup_source);
        line += ",\"calRestoreSuccess\":" + std::to_string(result.satellite->restore_success);
    }
    line += "}";
    return line;
}

/*
 * Correlates asynchronous ISubsysRadio callbacks back to the oneway request
 * that caused them.
 *
 * The vendor contract gives no ordering guarantee and delivers responses and
 * indications on binder threads, so a reply is accepted only when both its
 * serial and its operation match a live entry. A reply that matches neither is
 * stale or malformed and is refused rather than absorbed - absorbing it would
 * let one slot's answer satisfy another slot's question. Deliberately holds no
 * binder types so the correlation rules are host-testable.
 */
class PendingRequests {
  public:
    using Clock = std::chrono::steady_clock;

    /* Records the request before it is issued, so a reply that races the
     * caller still finds a live entry. Serials are never zero and never reused
     * while in flight. */
    int32_t Open(Operation op, Clock::time_point deadline) {
        std::lock_guard<std::mutex> lock(mutex_);
        int32_t serial;
        do {
            serial = ++last_serial_;
            if (serial == 0) serial = ++last_serial_;
        } while (entries_.count(serial) != 0);
        entries_.emplace(serial, Entry{op, deadline, Result{}});
        return serial;
    }

    bool IsPending(int32_t serial) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = entries_.find(serial);
        return it != entries_.end() && it->second.result.outcome == Outcome::kPending;
    }

    void CompleteDsdaState(int32_t serial, int32_t error, int32_t msim_sub_mode,
                           int32_t dsda_tx_mode) {
        std::lock_guard<std::mutex> lock(mutex_);
        Entry* entry = Match(serial, Operation::kDsdaState);
        if (entry == nullptr) {
            ++refused_replies_;
            return;
        }
        if (error != 0) {
            entry->result.outcome = Outcome::kVendorError;
            entry->result.vendor_error = error;
        } else {
            entry->result.outcome = Outcome::kOk;
            entry->result.dsda = DsdaState{msim_sub_mode, dsda_tx_mode};
        }
        settled_.notify_all();
    }

    void CompleteSatelliteCalState(int32_t serial, int32_t error, int32_t label,
                                   int32_t backup_source, int32_t restore_success) {
        std::lock_guard<std::mutex> lock(mutex_);
        Entry* entry = Match(serial, Operation::kSatelliteCalState);
        if (entry == nullptr) {
            ++refused_replies_;
            return;
        }
        if (error != 0) {
            entry->result.outcome = Outcome::kVendorError;
            entry->result.vendor_error = error;
        } else {
            entry->result.outcome = Outcome::kOk;
            entry->result.satellite = SatelliteCalState{label, backup_source, restore_success};
        }
        settled_.notify_all();
    }

    /* An indication is unsolicited state, never a reply: it carries no serial
     * and must not settle a pending request. Repeats of the same value are
     * folded together so a chatty modem cannot inflate the record. */
    void RecordMsimIndication(int32_t msim_sub_mode, int32_t dsda_tx_mode) {
        std::lock_guard<std::mutex> lock(mutex_);
        DsdaState value{msim_sub_mode, dsda_tx_mode};
        if (last_indication_.has_value() &&
            last_indication_->msim_sub_mode == value.msim_sub_mode &&
            last_indication_->dsda_tx_mode == value.dsda_tx_mode) {
            return;
        }
        last_indication_ = value;
        ++distinct_indications_;
    }

    void FailPending(Outcome outcome) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& [serial, entry] : entries_) {
            if (entry.result.outcome == Outcome::kPending) entry.result.outcome = outcome;
        }
        settled_.notify_all();
    }

    /* Returns the settled result, or a timeout result once the deadline the
     * caller registered has passed. */
    Result Await(int32_t serial) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = entries_.find(serial);
        if (it == entries_.end()) return Result{Outcome::kMalformed, 0, std::nullopt, std::nullopt};

        const auto deadline = it->second.deadline;
        settled_.wait_until(lock, deadline, [&] {
            return entries_.at(serial).result.outcome != Outcome::kPending;
        });

        Entry& entry = entries_.at(serial);
        if (entry.result.outcome == Outcome::kPending) entry.result.outcome = Outcome::kTimeout;
        return entry.result;
    }

    int RefusedReplies() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return refused_replies_;
    }

    int DistinctIndications() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return distinct_indications_;
    }

    std::optional<DsdaState> LastIndication() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return last_indication_;
    }

  private:
    struct Entry {
        Operation op;
        Clock::time_point deadline;
        Result result;
    };

    Entry* Match(int32_t serial, Operation op) {
        auto it = entries_.find(serial);
        if (it == entries_.end()) return nullptr;
        if (it->second.op != op) return nullptr;
        if (it->second.result.outcome != Outcome::kPending) return nullptr;
        return &it->second;
    }

    mutable std::mutex mutex_;
    std::condition_variable settled_;
    std::map<int32_t, Entry> entries_;
    std::optional<DsdaState> last_indication_;
    int32_t last_serial_ = 0;
    int refused_replies_ = 0;
    int distinct_indications_ = 0;
};

}  // namespace oplus::radio
