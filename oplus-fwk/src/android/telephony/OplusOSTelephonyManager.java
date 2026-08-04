/*
 * Copyright (C) 2024 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.telephony;

import android.content.Context;
import android.os.PersistableBundle;
import android.telecom.TelecomManager;
import android.telephony.CarrierConfigManager;
import android.telephony.ims.ImsMmTelManager;
import android.util.Log;

import java.util.List;

/**
 * Stub for the OEM android.telephony.OplusOSTelephonyManager.
 * Public surface ported faithfully from OOS16 oplus-framework.jar; all bodies
 * are safe non-crashing no-ops. getDefault() returns a real singleton so
 * consumers (OppoGallery2) never NPE on chained calls.
 */
public class OplusOSTelephonyManager {
    private static final String TAG = "OplusOSTelephonyManager";

    // Radio Access Family constants (values from OOS oplus-framework.jar).
    public static final int RAF_UNKNOWN = 0x0;
    public static final int RAF_GPRS = 0x1;
    public static final int RAF_EDGE = 0x2;
    public static final int RAF_UMTS = 0x4;
    public static final int RAF_IS95A = 0x8;
    public static final int RAF_IS95B = 0x8;
    public static final int RAF_EVDO_0 = 0x10;
    public static final int RAF_EVDO_A = 0x20;
    public static final int RAF_1xRTT = 0x40;
    public static final int RAF_HSDPA = 0x80;
    public static final int RAF_HSUPA = 0x100;
    public static final int RAF_HSPA = 0x200;
    public static final int RAF_EVDO_B = 0x800;
    public static final int RAF_LTE = 0x1000;
    public static final int RAF_EHRPD = 0x2000;
    public static final int RAF_HSPAP = 0x4000;
    public static final int RAF_GSM = 0x8000;
    public static final int RAF_TD_SCDMA = 0x10000;
    public static final int RAF_LTE_CA = 0x40000;
    public static final int RAF_NR = 0x80000;

    private static OplusOSTelephonyManager sInstance = null;

    private final Context mContext;
    private final OplusTelephonyAdapter mAdapter;

    public OplusOSTelephonyManager(Context context) {
        mContext = context.getApplicationContext() != null
                ? context.getApplicationContext() : context;
        mAdapter = new OplusTelephonyAdapter(mContext);
    }

    public static OplusOSTelephonyManager getDefault(Context context) {
        if (sInstance == null) {
            sInstance = new OplusOSTelephonyManager(context);
        }
        return sInstance;
    }

    // ---- static helpers ----
    public static boolean getBooleanCarrierConfig(Context context, String name, int subId) {
        CarrierConfigManager carrierConfigManager = context.getSystemService(
                CarrierConfigManager.class);
        PersistableBundle bundle = carrierConfigManager != null
                ? carrierConfigManager.getConfigForSubId(subId) : null;
        return bundle != null && bundle.getBoolean(name, false);
    }
    public static int getNetworkTypeFromRaf(int raf) {
        if ((raf & RAF_NR) != 0) {
            return TelephonyManager.NETWORK_TYPE_NR;
        }
        if ((raf & (RAF_LTE | RAF_LTE_CA)) != 0) {
            return TelephonyManager.NETWORK_TYPE_LTE;
        }
        if ((raf & (RAF_HSPAP | RAF_HSPA | RAF_HSUPA | RAF_HSDPA | RAF_UMTS | RAF_TD_SCDMA)) != 0) {
            return TelephonyManager.NETWORK_TYPE_UMTS;
        }
        if ((raf & (RAF_EVDO_0 | RAF_EVDO_A | RAF_EVDO_B | RAF_1xRTT | RAF_IS95A)) != 0) {
            return TelephonyManager.NETWORK_TYPE_CDMA;
        }
        if ((raf & (RAF_GSM | RAF_GPRS | RAF_EDGE)) != 0) {
            return TelephonyManager.NETWORK_TYPE_GSM;
        }
        return TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }
    public static int oplusgetActiveSubInfoCount(Context context) {
        return new OplusTelephonyAdapter(context).getActiveSubscriptions().size();
    }
    public static int oplusgetDefaultDataPhoneId(Context context) {
        return new OplusTelephonyAdapter(context).getDefaultDataPhoneId();
    }
    public static int oplusgetDefaultDataSubId(Context context) {
        return new OplusTelephonyAdapter(context).getDefaultDataSubId();
    }
    public static int oplusgetDefaultSmsPhoneId(Context context) {
        return new OplusTelephonyAdapter(context).getSlotIdForSubId(
                new OplusTelephonyAdapter(context).getDefaultSmsSubId());
    }
    public static int oplusgetDefaultSmsSubId(Context context) {
        return new OplusTelephonyAdapter(context).getDefaultSmsSubId();
    }
    public static int oplusgetDefaultSubId(Context context) {
        return new OplusTelephonyAdapter(context).getDefaultSubId();
    }
    public static int oplusgetOnDemandDataSubId(Context context) {
        return new OplusTelephonyAdapter(context).getDefaultDataSubId();
    }
    public static int oplusgetPhoneId(Context context, int subId) {
        return new OplusTelephonyAdapter(context).getSlotIdForSubId(subId);
    }
    public static int oplusgetSlotId(Context context, int subId) {
        return new OplusTelephonyAdapter(context).getSlotIdForSubId(subId);
    }
    public static int oplusgetSubId(Context context, int slotId) {
        return new OplusTelephonyAdapter(context).getSubIdForSlotId(slotId);
    }
    public static int oplusgetSubState(Context context, int subId) {
        return new OplusTelephonyAdapter(context).isValidSubId(subId) ? 1 : 0;
    }
    public static boolean oplusisValidPhoneId(Context context, int phoneId) {
        return new OplusTelephonyAdapter(context).isValidSlotId(phoneId);
    }
    public static boolean oplusisValidSlotId(Context context, int slotId) {
        return new OplusTelephonyAdapter(context).isValidSlotId(slotId);
    }
    public static boolean oplusisValidSubId(Context context, int subId) {
        return new OplusTelephonyAdapter(context).isValidSubId(subId);
    }
    public static void setDefaultApplication(String packageName, Context context) { }

    // ---- instance API ----
    public void activateSubId(int subId) { }
    public void answerRingingCallGemini(int subId) {
        TelecomManager telecomManager = mAdapter.getTelecomManager();
        if (telecomManager != null) {
            telecomManager.acceptRingingCall();
        }
    }
    public void deactivateSubId(int subId) { }
    public boolean endCallGemini(int subId) {
        TelecomManager telecomManager = mAdapter.getTelecomManager();
        return telecomManager != null && telecomManager.endCall();
    }
    public boolean getBooleanCarrierConfig(String name, int subId) {
        return getBooleanCarrierConfig(mContext, name, subId);
    }
    public int getCallStateGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getCallState() : TelephonyManager.CALL_STATE_IDLE;
    }
    public CellLocation getCellLocation(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getCellLocation() : null;
    }
    public int getCurrentPhoneTypeGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getPhoneType() : TelephonyManager.PHONE_TYPE_NONE;
    }
    public String getDeviceIdGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getDeviceId() : null;
    }
    public String getIccCardTypeGemini(int subId) { return oplusGetIccCardTypeGemini(subId); }
    public String getIccOperatorNumeric(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getSimOperator() : null;
    }
    public String getLine1NumberGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getLine1Number() : null;
    }
    public String getNetworkCountryIso(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getNetworkCountryIso() : null;
    }
    public String getNetworkOperatorGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getNetworkOperator() : null;
    }
    public int getNetworkTypeGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null
                ? telephonyManager.getDataNetworkType() : TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }
    public List getSelectableSubscriptionInfoList() {
        return mAdapter.getSelectableSubscriptions();
    }
    public String getSimOperatorGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getSimOperator() : null;
    }
    public String getSimSerialNumberGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getSimSerialNumber() : null;
    }
    public int getSimStateGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getSimState() : TelephonyManager.SIM_STATE_UNKNOWN;
    }
    public int getSubState(int subId) { return mAdapter.isValidSubId(subId) ? 1 : 0; }
    public String getSubscriberIdGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getSubscriberId() : null;
    }
    public String getVoiceMailNumberGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getVoiceMailNumber() : null;
    }
    public int getVoiceNetworkTypeGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null
                ? telephonyManager.getVoiceNetworkType() : TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }
    public boolean handlePinMmiForSubscriber(int subId, String dialString) { return false; }
    public boolean hasIccCardGemini(int slotId) {
        TelephonyManager telephonyManager = mAdapter.forSlotId(slotId);
        return telephonyManager != null && telephonyManager.hasIccCard();
    }
    public boolean isDualLteEnabled() { return false; }
    public boolean isDualLteSupportedByPlatform() {
        return TelephonyManager.getDefault().getActiveModemCount() > 1;
    }
    public boolean isIdleGemini(int subId) {
        return getCallStateGemini(subId) == TelephonyManager.CALL_STATE_IDLE;
    }
    public boolean isIdleGemini(int subId, String callingPackage) { return isIdleGemini(subId); }
    public boolean isNetworkRoamingGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null && telephonyManager.isNetworkRoaming();
    }
    public boolean isOffhookGemini(int subId, String callingPackage) {
        return getCallStateGemini(subId) == TelephonyManager.CALL_STATE_OFFHOOK;
    }
    public boolean isOplusHasSoftSimCard() { return oplusGetSoftSimCardSlotId() != OplusTelephonyAdapter.INVALID_VSIM_ID; }
    public boolean isOplusSingleSimCard() { return TelephonyManager.getDefault().getActiveModemCount() <= 1; }
    public boolean isRingingGemini(int subId) {
        return getCallStateGemini(subId) == TelephonyManager.CALL_STATE_RINGING;
    }
    public boolean isRingingGemini(int subId, String callingPackage) { return isRingingGemini(subId); }
    public boolean isSimInsert(int slotId) { return hasIccCardGemini(slotId); }
    public boolean isUriFileExist(String uri) { return false; }
    public void listenGemini(Context context, PhoneStateListener listener, int events, int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        if (telephonyManager != null) {
            telephonyManager.listen(listener, events);
        }
    }
    public void listenGemini(PhoneStateListener listener, int events, int subId) {
        listenGemini(mContext, listener, events, subId);
    }
    public int oplusGetActiveSubscriptionsCount(Context context) {
        return new OplusTelephonyAdapter(context).getActiveSubscriptions().size();
    }
    public int oplusGetDataSubscription() { return mAdapter.getDefaultDataSubId(); }
    public int oplusGetDefaultSubscription() { return mAdapter.getDefaultSubId(); }
    public String oplusGetIccCardTypeGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        if (telephonyManager == null) {
            return null;
        }
        return telephonyManager.getSimState() == TelephonyManager.SIM_STATE_ABSENT ? null
                : telephonyManager.getSimOperatorName();
    }
    public String oplusGetIccId(int subId) { return getSimSerialNumberGemini(subId); }
    public boolean oplusGetIccLockEnabled(int subId) { return false; }
    public String oplusGetMeid(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getMeid() : null;
    }
    public String oplusGetOemSpn(int subId) { return null; }
    public String oplusGetPlmnOverride(String numeric, ServiceState serviceState) { return null; }
    public int oplusGetQcomActiveSubscriptionsCount() { return mAdapter.getActiveSubscriptions().size(); }
    public String oplusGetQcomImeiGemini(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        return telephonyManager != null ? telephonyManager.getImei() : null;
    }
    public String[] oplusGetQcomLTECDMAImei(int subId) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        if (telephonyManager == null) {
            return new String[0];
        }
        return new String[] {telephonyManager.getImei(), telephonyManager.getMeid()};
    }
    public String oplusGetScAddressGemini(int subId, int type) { return null; }
    public int oplusGetSimIndicatorState(int subId) { return 0; }
    public int oplusGetSoftSimCardSlotId() { return mAdapter.getSoftSimCardSlotId(); }
    public boolean oplusIsImsRegistered(Context context, int subId) {
        return !OplusTelephonyAdapter.IMS_TYPE_NOT_REGISTERED.equals(
                new OplusTelephonyAdapter(context).resolveImsType(
                        new OplusTelephonyAdapter(context).getSlotIdForSubId(subId)));
    }
    public boolean oplusIsQcomSubActive(int subId) { return mAdapter.isValidSubId(subId); }
    public boolean oplusIsSubActive(int subId) { return mAdapter.isValidSubId(subId); }
    public boolean oplusIsVolteEnabledByPlatform(Context context, int subId) {
        ImsMmTelManager imsManager = new OplusTelephonyAdapter(context).getImsManager(subId);
        return imsManager != null && imsManager.isAvailable(
                MmTelFeature.MmTelCapabilities.CAPABILITY_TYPE_VOICE,
                ImsRegistrationImplBase.REGISTRATION_TECH_LTE);
    }
    public boolean oplusIsVtEnabledByPlatform(Context context, int subId) {
        ImsMmTelManager imsManager = new OplusTelephonyAdapter(context).getImsManager(subId);
        return imsManager != null && imsManager.isAvailable(
                MmTelFeature.MmTelCapabilities.CAPABILITY_TYPE_VIDEO,
                ImsRegistrationImplBase.REGISTRATION_TECH_LTE);
    }
    public boolean oplusIsWfcEnabledByPlatform(Context context, int subId) {
        ImsMmTelManager imsManager = new OplusTelephonyAdapter(context).getImsManager(subId);
        return imsManager != null && imsManager.isAvailable(
                MmTelFeature.MmTelCapabilities.CAPABILITY_TYPE_VOICE,
                ImsRegistrationImplBase.REGISTRATION_TECH_IWLAN);
    }
    public boolean oplusIsWhiteSIMCard(int subId) { return false; }
    public boolean oplusMvnoMatches(int subId, int mvnoType, String mvnoMatchData, String data) { return false; }
    public void oplusSetDataRoamingEnabled(int subId, boolean enabled) { }
    public void oplusSetDataSubscription(Context context, int subId) { }
    public boolean oplusSetLine1Number(int subId, String number) { return false; }
    public int oplusSetPreferredNetworkType(int subId, int networkType) {
        TelephonyManager telephonyManager = mAdapter.forSubId(subId);
        if (telephonyManager == null) {
            return -1;
        }
        long bitmask = mAdapter.rafToNetworkTypeBitmask(networkType);
        if (bitmask == 0L) {
            return -1;
        }
        try {
            telephonyManager.setAllowedNetworkTypesForReason(
                    TelephonyManager.ALLOWED_NETWORK_TYPES_REASON_USER, bitmask);
            return 0;
        } catch (RuntimeException e) {
            Log.w(TAG, "Failed to update preferred network type for subId=" + subId, e);
            return -1;
        }
    }
    public void oplusSetScAddressGemini(int subId, String address, int type) { }
    public void setDualLteEnabled(boolean enabled) { }
    public boolean showInCallScreenGemini(boolean showDialpad, String a, String b) {
        TelecomManager telecomManager = mAdapter.getTelecomManager();
        if (telecomManager == null) {
            return false;
        }
        telecomManager.showInCallScreen(showDialpad);
        return true;
    }
    public void silenceRingerGemini(int subId) {
        TelecomManager telecomManager = mAdapter.getTelecomManager();
        if (telecomManager != null) {
            telecomManager.silenceRinger();
        }
    }
    public void silenceRingerGemini(int subId, String callingPackage) { silenceRingerGemini(subId); }
    public boolean supplyPin(String pin, int subId) { return false; }
    public int[] supplyPinReportResult(String pin, int subId) { return new int[0]; }
    public boolean supplyPuk(String puk, String pin, int subId) { return false; }
    public int[] supplyPukReportResult(String puk, String pin, int subId) { return new int[0]; }
}
