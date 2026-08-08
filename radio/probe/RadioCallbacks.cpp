/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "RadioCallbacks.h"

namespace aidl::vendor::oplus::hardware::subsys_interface::subsys_radio {

RadioResponse::RadioResponse(::oplus::radio::PendingRequests& pending) : pending_(pending) {}

ndk::ScopedAStatus RadioResponse::backupNvBackupResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::configGsmTimingDataResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::configPaIcqDataResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::connectSarSensorResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::deleteEfsItemResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::deprioritizeNrResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::disconnectSarSensorResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::enableEndcResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::fetchOlogResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getAntForceStateByRatResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t enable) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getAntIdResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getAsdivFixPositionResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t position) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getAsdivStatesResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::AsdivState& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getAvailableBandModesResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<int32_t>& bandModes) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getBandResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int64_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getBandsTxNumResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getCABandComboResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& caInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getCallInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getCapabilityResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t value) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getDeviceLockStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getDeviceLockinfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getDiagPktVersionMismatchDbResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::DiagPacketVersionMismatchDb& mismatchDb) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getEchoLocateDlCarrierLogResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ElDlCellInfo& respInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getEchoLocateUlCarrierLogResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ElUlCellInfo& respInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getFiveGSaNsaModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getGpioStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t gpioInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getGsmPclPwrResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t pcl) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getImsPolMgrApnResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::string& apn_name) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getImsProfileApnResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::string& apn_name) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getImsProfileListResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& profileIndex) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getImsUssdEnabledResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t imsUssdEnabled) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getLtePowerClassResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, char16_t values) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getModemBasebandVersionResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::string& modemBaseband) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getModemHeapInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::HeapInfo& heapInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getModemHeapListResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::HeapList& heapList) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getMotionStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& motionStates) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNasSysInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::NasSysInfo& nasSysInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNr5gBlerResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t bler) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNr5gFullVoiceSupportResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t voiceSupport) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNrBandPreferResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& preferredBands) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNrServingCellInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::NrCellInfo& result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNrSupportResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t status) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNvBackupStatResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::NvBackupStatisticsType& stat) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getOperationModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t mode) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getPhySlotStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::PhySlotStatus& slotInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getRfBandInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t activeBand, int32_t active1xChannel, int32_t activeEhrpdChannel) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getRrcLogResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::RrcState& rrcState) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSarDsiStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t state) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSarRegionCodeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t code) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSarSnsDataResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getServingCellInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::CellInfo& result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimCardTypeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t type) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimPathResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t simPath) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimTrayStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<int32_t>& status) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockActivateTimeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int64_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockCategoryDataResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& categoryData) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockComboTypeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockCurrentRetryResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockDelayLockStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockFactoryResetTimeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int64_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockFeatureResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockFeeStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockFuseStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t secBootStatus, int8_t fuseStatus) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockIsRegionVietnamResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockLockStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::RsuSimlockLockStatus& lockStatus) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockLockTypeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockLockmarkResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockMaxRetryResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockOperatorIdResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockRsuModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t mode) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockSimStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockUnlockStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimlockVersionResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t minVersion, int8_t maxVersion) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSystemSelectionPreferenceResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::SystemSelectionPreference& preference) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getTestModeMaskResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int64_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getTxAdcResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getTxRxInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::TxRxInfo& txrxInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getVoiceConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::VoiceConfig& config) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::initRfPathResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::readEfsItemResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::readNvResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::refreshModemEfsResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::registerNr5gStatsEventResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::requireModemRebootResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::restoreNvBackupAllowedResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t allowed) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::restoreNvBackupResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::sendDciSyncReqAndRspResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& rspBuf) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::sendScreenStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAclStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAntForceStateByRatResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAntMaxPowerResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAolScenarioResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAsdivFixPositionResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setBandModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setBarCellResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCABandComboResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCalibrationStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCapabilityResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCarrierOmacpFqdnResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setComboRfTxInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCyberSenseConfigPolicyResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setDiagnoseConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setFiveGSaNsaModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setGameScenario2RfResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setGpioStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setGsmPclPwrResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setMotionConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setNr5gFullVoiceSupportResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setNrBandPreferResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setNvRestoreStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t state) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setOosLpmCfgResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setOperationModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setPdcActivateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setPdcDeactivateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRatAcqOrderResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRfDebugMaskResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRfTxInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRtSarModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRxChainsNumberResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRxDiversityResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSarControlStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSarDsiStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSarRegionCodeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSarSensorChannelResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSarStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSimPathResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSimlockAccumulatedTimeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSimlockActivateTimeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSimlockFactoryResetTimeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSimlockFeeStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setTestModeMaskResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setTimeZoneResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setUimPowerStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setUst5gUCConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setVoiceRoamingResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::testQlinkBlerResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::testQlinkPingResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::testQlinkSlaveIdResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::triggerModemCrashResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::unlockSimlockResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::updateKddiSimlockBlobResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::updateSimlockBlobResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::writeEfsItemResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::writeNvResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getRRCStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t rrcStatus) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setBandPreferResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getBandPreferResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& preferredBands) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::clearStoredFrequencyResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSidoIssueConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setLteAcqScanDuringLteResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNWLimitStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t limitState) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setDeviceIdleModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setDeviceDeepSleepResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setGameSpaceModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setIgnorePsPagingResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getEfsSpaceInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t availBlocks, int32_t blockSize, int32_t totalBlocks) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSimHotswapStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& hotswapStatus) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getRfBandwidthInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t lteBandwidth, int32_t nrBandwidth) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getRadioLinkInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& infoList) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setMdmFeatureResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::resetModemConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setHstConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::updateFoldScreenStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::readSingleSimArrayResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& singleSimData) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setMipiOscFreqHopStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setNecReportPeriodResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setNecConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNecDataResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSimOverdueResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setLpmScanConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getCalibrationStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& adjustStatus) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setMcfgRfsParamsResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t rfs_params_ver) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getMcfgRfsParamsResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::McfgRfsParams& rfsParams) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::trigerMcfgRemoteDiscoverResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::McfgRemoteDiscoverInfo& mcfgRemoteDiscoverInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRatScanResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setImeiSvnResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setUst5gUCBandConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setDssConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setDssAdditionalConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::readDssConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t config) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::readNRCAConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t config) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setWhiteSANRCAConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setBlackSANRCAConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setWhiteNSANRCAConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::resetSANSAWhiteListResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setTasForceIdxByRatResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<int32_t>& forceIdx) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setMaxTxPowerResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<int32_t>& txPowerConfig) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::lockCellAndBandResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::unlockCellAndBandResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setFastDormancyTimerResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::sendFastDormancyResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setPSConformanceModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::queryFastDormancyModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t mode, int32_t profile) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::queryLegacyFastDormancyModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t mode) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setServiceModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getServiceModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t mode) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCentricModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getCentricModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t mode) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCaStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getCaStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t status) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSyncWithApResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setBarCellOptResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getTxAntennaResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<int32_t>& antennas) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setNwRateLimitingDetectCfgResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNwRateLimitingInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::NwRateLimitingInfo& limitInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getHwidResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t hwid) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRfTxPowerResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::performPlmnUnblockResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::performPlmnBlockResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSaPriorityResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setPowerSavingStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::performLteAcqScanResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::plmnRatSelectionResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::mdmDispatchCaseResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setImsRegTimerResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setMicroMotionStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::performBgSearchResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::clearForbiddenTaiResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setDataStallParamResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setNfListResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNfListResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setL2nrSelectionResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setLteDubiousCellResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setNrDubiousCellResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setMsimSubModePrefResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getMsimSubModeStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t msimSubMode, int32_t dsdaTxMode) {
    pending_.CompleteDsdaState(info.serial, info.error, msimSubMode, dsdaTxMode);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getBwpConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t bwpState) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setBwpConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNrcaConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int8_t nrcaState) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setNrcaConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setBackgroundSearchResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::sendDeviceStateExtResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setOosCfgResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setQrxlvminCfgResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setArrearageCfgResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSimTagResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setQosReportPeriodResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setQosConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getQosDataResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setDsdaPreferCustThreshResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setIdcOffsetResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::writeNvItemResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getLteCellInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t pci, int32_t arfcn, bool sib24Available) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNrcaInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::NrcaInfo& nrcaInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::updateDcdcSleepStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::enableCellBarringResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::configCellBarringParamResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setFeatureStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getFeatureStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t featureState) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRfTxInfo2Response(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAtcTableInitResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getTxCarkitResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& antennas) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getServingCellularCellInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::CellInfos& respInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::startMetricsCollectResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::stopMetricsCollectResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getMetricsDataResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setHyperUplinkModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::sendSceneModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::sendQoeScoreResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setHyperUplinkParasResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSiteCellInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setModemIndicationPolicyResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getModemIndicationPolicyResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::PolicyConfig>& policy) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAolStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setImbStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setMtplLimitStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAntFixStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAntBlockStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setPowerBoostStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getModemProtocolInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getAtomDataResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::loadAtomApplicationResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::unloadAtomApplicationResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getAllCellularCellInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::CellInfos& servingCellInfo, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::NeighborCellInfos& neighborCellInfos) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setImsRtpInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setImsRtpThresholdResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRedirectOptSettingResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAttachPriorityResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setPingpongBarSettingResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::performRetHighRatResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getIndicationReportRecordsResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::IndReportRecord>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCiwlanUiSwitchResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getATCIStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, bool enable) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::enableATCIPortResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setVonrRollbackCfgResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::satelliteFirmwareDownLoadResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setEsimStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCarrierLockBlobResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCarrierLockStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getCarrierLockStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRegionLockBlobResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRegionLockStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getRegionLockStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::writeEncryptedSerialIdResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::readRegionLockDataResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::writeCarrierLockWhitelistResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::readCarrierLockWhitelistResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAtomStatusResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setImsAudioQualityThresholdResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setPagingErrorCfgResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAdaptiveHandoverThresholdResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setIndicationConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setEccListResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getQrxlvminCfgResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::QrxlvminRspInfo& qrxlvminCfg) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setSmartIdleCfgResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getPowerStatisticsResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::PowerStatisticsConfigs& powerStatistics) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::sendScoreInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::sendNetworkActionResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::rollbackNetworkActionResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getNetworkActionStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t clientId, int32_t actionId, int32_t actionState) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSceneModeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::SceneMode>& scenes) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::hplmnBgSearchResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getImsRtpRedunCapabilityResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t capability) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::syncImsRtpRedunDataPathConfigResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::exchangeImsRtpRedunPublicKeyResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ImsRtpRedunKeyRspInfo& keyRsp) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setImsRtpRedunControlInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ImsRtpRedunControlRspInfo& controlRsp) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::dumpDiagMdLogBufferResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setMdLogBufferSizeResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setTxPathFilterResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setAtcTableDeInitResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getModemFittingInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ModemFittingInfo& mdInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSatelliteImeiResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::string& imei3) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setRxBoostStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getRxBoostStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::RxBoostStateInfo& stateInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getSatelliteCalibrationDataStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::SatelliteCalDataStatusType& state) {
    pending_.CompleteSatelliteCalState(info.serial, info.error, state.label,
                                       state.backupSource, state.restoreSuccess);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setWifiQosInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getWifiQosInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ModemWifiQosInfo>& wifiQosRspInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::registerFenceClientResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t clientId) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setFenceRecognizeStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getFenceStateResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::FenceStateInfo>& fenceStateInfo, bool state) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::triggerFenceLearningResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, int32_t fenceId) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::sendWifiConnectInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::sendFenceInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::deregisterFenceClientResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::setCallScoreInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioResponse::getCallScoreInfoResponse(const ::aidl::vendor::oplus::hardware::subsys_interface::subsys::SubsysResponseInfo& info, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::CallCellScore& score) {
    return ndk::ScopedAStatus::ok();
}

RadioIndication::RadioIndication(::oplus::radio::PendingRequests& pending) : pending_(pending) {}

ndk::ScopedAStatus RadioIndication::radioCyberSenseCellInfoInd(int32_t type, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::CyberSenseHALCellInfo>& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioCyberSenseEventInd(int32_t type, int32_t value) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioDtmfStartInd(int32_t type, int8_t tone_type) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioDtmfStopInd(int32_t type, int8_t tone_type) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioImsMessageInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ImsMessage& imsMessage) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioImsRtpStateInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ImsRtpState& rtpState) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioLteCaInfoInd(int32_t type, const std::vector<int32_t>& ca_info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioLteRegDomainInd(int32_t type, int32_t value) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioNecInd(int32_t type, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioNonddsPagingInd() {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioNr5gDrxInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::Nr5gDrxType& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioNr5gFreqTypeIndication(int32_t type, int32_t nr5gFreqType) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioSimlockInfoChangedInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::SimlockInfoType& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioSimlockStateChangedInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::SimlockStateType& state) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioStateChanged(int32_t type, int32_t state) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioSidoIssueInd(int32_t type, const std::vector<int32_t>& params) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioNrCapInd(int32_t type, int8_t nr_cap) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioNonddsNullPagingInd(int32_t type, int8_t np_type, int32_t rat, int32_t errcode) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioCssnfNumInd(int32_t type, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioMipiOscFreqHopStateInd(int32_t type, int32_t value) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioSimOverdueInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::SimOverdueIndType& state) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioNwRateLimitingInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::NwRateLimitingInfo& limitInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioMdmTestInd(int32_t type, int32_t data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioMsimSubModeInd(int32_t type, int32_t msimSubMode, int32_t dsdaTxMode) {
    pending_.RecordMsimIndication(msimSubMode, dsdaTxMode);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioQosPeriodReportInd(int32_t type, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioQosEventReportInd(int32_t type, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioLteCellInfoInd(int32_t type, int32_t pci, int32_t arfcn, bool sib24_available) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioMccChangeInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::MccChangeIndInfo& roamInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioHstModeInd(int32_t type, int32_t hstMode) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioServingCellInfoInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::CellInfos& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioHyperUplinkStateInd(int32_t type, int32_t sceneState, int32_t hyperUplinkState, int32_t hyperUplinkType) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioAtomDataInd(int32_t type, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioLinkLatencyInfoInd(int32_t type, int32_t ulLatency) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioImsRtpControlInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ImsRtpControlInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioNasAccessBarringStatusInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::NasAccessBarringStatusInfo& nasAccessBarringStatusinfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioNrcaInfoChangeInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::NrcaInfo& nrcaInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioVonrBackoffInfoChangeInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::VonrBackoffInfo& backoffInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioVonrRollbackInfoChangeInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::VonrRollbackInfo& rollbackInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioSingleSimChangeInd(int32_t type, int8_t status) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioCarrierLockStatusChangeInd(int32_t type, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioRegionLockStatusChangeInd(int32_t type, const std::vector<uint8_t>& data) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioAlertEventInd(int32_t type, int32_t alertId, int32_t alertCause, const std::vector<int32_t>& alertParam) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioVodataEventInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::VodataEventInfo& vodataEventInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioAllCellInfoInd(int32_t type, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::CellInfos>& servingCellInfo, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::NeighborCellInfos>& neighborCellInfos) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioNetworkActionResultInd(int32_t type, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ActionResult>& result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioSceneModeInd(int32_t type, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::SceneMode>& scenes) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioSmartNetworkSelectInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::SmartNetworkSelectInfo& smartNetworkSelectInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioImsRtpRedunEventInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::ImsRtpRedunEventInfo& event) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::rxBoostStateInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::RxBoostStateInfo& state) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioRrcStatusChangeInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::RrcStatusIndInfo& rrcInfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioFenceStateChangeInd(int32_t type, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::FenceStateInfo>& fenceStateInfos) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioFenceRemovedInd(int32_t type, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::FenceRemovedInfo>& fenceRemovedInfos) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioFenceMergedInd(int32_t type, const std::vector<::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::FenceMergedInfo>& fenceMergedInfos) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus RadioIndication::radioFenceCreateInd(int32_t type, const ::aidl::vendor::oplus::hardware::subsys_interface::subsys_radio::FenceCreatedInfo& fenceCreatedInfo) {
    return ndk::ScopedAStatus::ok();
}

}  // namespace aidl::vendor::oplus::hardware::subsys_interface::subsys_radio
