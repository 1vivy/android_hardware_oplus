/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>

#include <thread>

#include "PendingRequests.h"

using namespace std::chrono_literals;
using ::oplus::radio::ExitCodeFor;
using ::oplus::radio::FormatJson;
using ::oplus::radio::Operation;
using ::oplus::radio::Outcome;
using ::oplus::radio::PendingRequests;
using ::oplus::radio::Result;
using ::oplus::radio::SatelliteCalState;

namespace {

PendingRequests::Clock::time_point Soon() {
    return PendingRequests::Clock::now() + 5s;
}

TEST(PendingRequests, SerialsAreNonZeroAndNotReusedWhileInFlight) {
    PendingRequests pending;
    const int32_t first = pending.Open(Operation::kDsdaState, Soon());
    const int32_t second = pending.Open(Operation::kDsdaState, Soon());

    EXPECT_NE(first, 0);
    EXPECT_NE(second, 0);
    EXPECT_NE(first, second);
    EXPECT_TRUE(pending.IsPending(first));
    EXPECT_TRUE(pending.IsPending(second));
}

TEST(PendingRequests, MatchingResponseSettlesTheRequest) {
    PendingRequests pending;
    const int32_t serial = pending.Open(Operation::kDsdaState, Soon());

    pending.CompleteDsdaState(serial, 0, 7, 3);

    const Result result = pending.Await(serial);
    EXPECT_EQ(result.outcome, Outcome::kOk);
    ASSERT_TRUE(result.dsda.has_value());
    EXPECT_EQ(result.dsda->msim_sub_mode, 7);
    EXPECT_EQ(result.dsda->dsda_tx_mode, 3);
    EXPECT_EQ(pending.RefusedReplies(), 0);
}

TEST(PendingRequests, ResponseForAnotherOperationIsRefused) {
    PendingRequests pending;
    const int32_t serial = pending.Open(Operation::kDsdaState, Soon());

    pending.CompleteSatelliteCalState(serial, 0, 1, 2, 3);

    EXPECT_TRUE(pending.IsPending(serial));
    EXPECT_EQ(pending.RefusedReplies(), 1);
}

TEST(PendingRequests, StaleSerialIsRefusedAndDoesNotSettleALiveRequest) {
    PendingRequests pending;
    const int32_t serial = pending.Open(Operation::kDsdaState, Soon());

    pending.CompleteDsdaState(serial + 4242, 0, 1, 1);

    EXPECT_TRUE(pending.IsPending(serial));
    EXPECT_EQ(pending.RefusedReplies(), 1);
}

TEST(PendingRequests, SecondResponseForASettledSerialIsRefused) {
    PendingRequests pending;
    const int32_t serial = pending.Open(Operation::kDsdaState, Soon());

    pending.CompleteDsdaState(serial, 0, 1, 1);
    pending.CompleteDsdaState(serial, 0, 9, 9);

    const Result result = pending.Await(serial);
    ASSERT_TRUE(result.dsda.has_value());
    EXPECT_EQ(result.dsda->msim_sub_mode, 1);
    EXPECT_EQ(pending.RefusedReplies(), 1);
}

TEST(PendingRequests, VendorErrorIsReportedRatherThanTreatedAsAValue) {
    PendingRequests pending;
    const int32_t serial = pending.Open(Operation::kDsdaState, Soon());

    pending.CompleteDsdaState(serial, 6, 0, 0);

    const Result result = pending.Await(serial);
    EXPECT_EQ(result.outcome, Outcome::kVendorError);
    EXPECT_EQ(result.vendor_error, 6);
    EXPECT_FALSE(result.dsda.has_value());
    EXPECT_EQ(ExitCodeFor(result.outcome), 4);
}

TEST(PendingRequests, DuplicateIndicationsAreCoalesced) {
    PendingRequests pending;

    pending.RecordMsimIndication(2, 1);
    pending.RecordMsimIndication(2, 1);
    pending.RecordMsimIndication(2, 1);

    EXPECT_EQ(pending.DistinctIndications(), 1);
    ASSERT_TRUE(pending.LastIndication().has_value());
    EXPECT_EQ(pending.LastIndication()->msim_sub_mode, 2);

    pending.RecordMsimIndication(3, 1);
    EXPECT_EQ(pending.DistinctIndications(), 2);
}

TEST(PendingRequests, IndicationNeverSettlesAPendingRequest) {
    PendingRequests pending;
    const int32_t serial = pending.Open(Operation::kDsdaState, Soon());

    pending.RecordMsimIndication(5, 5);

    EXPECT_TRUE(pending.IsPending(serial));
}

TEST(PendingRequests, DeathFailsTheRequestInFlight) {
    PendingRequests pending;
    const int32_t serial = pending.Open(Operation::kDsdaState, Soon());

    pending.FailPending(Outcome::kMalformed);

    const Result result = pending.Await(serial);
    EXPECT_EQ(result.outcome, Outcome::kMalformed);
    EXPECT_EQ(ExitCodeFor(result.outcome), 5);
}

TEST(PendingRequests, DeathAfterSettlingDoesNotOverwriteTheAnswer) {
    PendingRequests pending;
    const int32_t serial = pending.Open(Operation::kDsdaState, Soon());
    pending.CompleteDsdaState(serial, 0, 1, 2);

    pending.FailPending(Outcome::kMalformed);

    EXPECT_EQ(pending.Await(serial).outcome, Outcome::kOk);
}

TEST(PendingRequests, ElapsedDeadlineYieldsTimeout) {
    PendingRequests pending;
    const int32_t serial =
            pending.Open(Operation::kDsdaState, PendingRequests::Clock::now() - 1ms);

    const Result result = pending.Await(serial);
    EXPECT_EQ(result.outcome, Outcome::kTimeout);
    EXPECT_EQ(ExitCodeFor(result.outcome), 3);
}

TEST(PendingRequests, ResponseArrivingFromAnotherThreadIsAwaited) {
    PendingRequests pending;
    const int32_t serial = pending.Open(Operation::kDsdaState, Soon());

    std::thread responder([&] {
        std::this_thread::sleep_for(20ms);
        pending.CompleteDsdaState(serial, 0, 4, 2);
    });
    const Result result = pending.Await(serial);
    responder.join();

    EXPECT_EQ(result.outcome, Outcome::kOk);
    ASSERT_TRUE(result.dsda.has_value());
    EXPECT_EQ(result.dsda->msim_sub_mode, 4);
}

TEST(PendingRequests, AbsentSlotMapsToTheServiceAbsentExitCode) {
    Result absent;
    absent.outcome = Outcome::kServiceAbsent;

    EXPECT_EQ(ExitCodeFor(absent.outcome), 2);
    EXPECT_EQ(FormatJson("slot2", "undeclared", "dsda-state", absent),
              R"({"schema":1,"slot":"slot2","service":"undeclared",)"
              R"("operation":"dsda-state","status":"service-absent"})");
}

TEST(PendingRequests, DsdaRecordMatchesTheDocumentedSchema) {
    Result result;
    result.outcome = Outcome::kOk;
    result.dsda = ::oplus::radio::DsdaState{0, 0};

    EXPECT_EQ(FormatJson("slot1", "ready", "dsda-state", result),
              R"({"schema":1,"slot":"slot1","service":"ready","operation":"dsda-state",)"
              R"("status":"ok","msimSubMode":0,"dsdaTxMode":0})");
}

TEST(PendingRequests, SatelliteRecordCarriesStateButNoCalibrationPayload) {
    Result result;
    result.outcome = Outcome::kOk;
    result.satellite = SatelliteCalState{1, 2, 3};

    const std::string json = FormatJson("slot1", "ready", "satellite-cal-state", result);

    EXPECT_NE(json.find(R"("calLabel":1)"), std::string::npos);
    for (const char* forbidden : {"imei", "Imei", "backupTime", "backupSizeBytes", "restoreBytes"}) {
        EXPECT_EQ(json.find(forbidden), std::string::npos) << "leaked field: " << forbidden;
    }
}

}  // namespace
