/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.telephony;

import android.content.Context;
import android.provider.Settings;
import android.telecom.TelecomManager;
import android.telephony.ims.ImsMmTelManager;
import android.telephony.ims.feature.MmTelFeature;
import android.telephony.ims.stub.ImsRegistrationImplBase;
import android.util.Log;

import java.util.Collections;
import java.util.List;

final class OplusTelephonyAdapter {
    private static final String TAG = "OplusTelephonyAdapter";

    static final int INVALID_VSIM_ID = -1;
    static final String OPLUS_SIM_IMS_TYPE = "OPLUS_SIM_IMS_TYPE";
    static final String OPLUS_SIM_VSIM_ID = "OPLUS_SIM_VSIM_ID";
    static final String IMS_TYPE_NOT_REGISTERED = "not_register";
    static final String IMS_TYPE_WLAN = "wifi";
    static final String IMS_TYPE_WWAN = "volte";

    // Keep the binder closures here limited to public telephony/IMS features that map
    // cleanly onto Android or onto verified stock settings keys. AppRadio, DMTP, and
    // subsys_* vendor interfaces remain intentionally unbound until a real open client
    // exists; wiring those now would only recreate vendor-only control planes.
    private final Context mContext;
    private volatile Integer mCachedSoftSimSlotId;

    OplusTelephonyAdapter(Context context) {
        Context appContext = context.getApplicationContext();
        mContext = appContext != null ? appContext : context;
    }

    void invalidateSoftSimCache() {
        mCachedSoftSimSlotId = null;
    }

    TelephonyManager getTelephonyManager() {
        return mContext.getSystemService(TelephonyManager.class);
    }

    TelecomManager getTelecomManager() {
        return mContext.getSystemService(TelecomManager.class);
    }

    SubscriptionManager getSubscriptionManager() {
        return mContext.getSystemService(SubscriptionManager.class);
    }

    List<SubscriptionInfo> getActiveSubscriptions() {
        SubscriptionManager subscriptionManager = getSubscriptionManager();
        if (subscriptionManager == null) {
            return Collections.emptyList();
        }
        List<SubscriptionInfo> active = subscriptionManager.getActiveSubscriptionInfoList();
        return active != null ? active : Collections.emptyList();
    }

    List<SubscriptionInfo> getSelectableSubscriptions() {
        SubscriptionManager subscriptionManager = getSubscriptionManager();
        if (subscriptionManager == null) {
            return Collections.emptyList();
        }
        List<SubscriptionInfo> selectable = subscriptionManager.getSelectableSubscriptionInfoList();
        return selectable != null ? selectable : Collections.emptyList();
    }

    int getSubIdForSlotId(int slotId) {
        for (SubscriptionInfo info : getActiveSubscriptions()) {
            if (info.getSimSlotIndex() == slotId) {
                return info.getSubscriptionId();
            }
        }
        return SubscriptionManager.INVALID_SUBSCRIPTION_ID;
    }

    int getSlotIdForSubId(int subId) {
        for (SubscriptionInfo info : getActiveSubscriptions()) {
            if (info.getSubscriptionId() == subId) {
                return info.getSimSlotIndex();
            }
        }
        return SubscriptionManager.INVALID_SIM_SLOT_INDEX;
    }

    int getDefaultSubId() {
        return SubscriptionManager.getDefaultSubscriptionId();
    }

    int getDefaultDataSubId() {
        return SubscriptionManager.getDefaultDataSubscriptionId();
    }

    int getDefaultSmsSubId() {
        return SubscriptionManager.getDefaultSmsSubscriptionId();
    }

    int getDefaultVoiceSubId() {
        return SubscriptionManager.getDefaultVoiceSubscriptionId();
    }

    int getDefaultDataPhoneId() {
        return getSlotIdForSubId(getDefaultDataSubId());
    }

    boolean isValidSubId(int subId) {
        return SubscriptionManager.isValidSubscriptionId(subId);
    }

    boolean isValidSlotId(int slotId) {
        return slotId >= 0 && slotId < TelephonyManager.getDefault().getActiveModemCount();
    }

    TelephonyManager forSubId(int subId) {
        TelephonyManager telephonyManager = getTelephonyManager();
        if (telephonyManager == null || !isValidSubId(subId)) {
            return null;
        }
        return telephonyManager.createForSubscriptionId(subId);
    }

    TelephonyManager forSlotId(int slotId) {
        return forSubId(getSubIdForSlotId(slotId));
    }

    ImsMmTelManager getImsManager(int subId) {
        if (!isValidSubId(subId)) {
            return null;
        }
        try {
            ImsMmTelManager imsManager = mContext.getSystemService(ImsMmTelManager.class);
            return imsManager != null ? imsManager.createForSubscriptionId(subId) : null;
        } catch (RuntimeException e) {
            Log.w(TAG, "IMS manager unavailable for subId=" + subId, e);
            return null;
        }
    }

    String getCachedImsType(int slotId) {
        if (!isValidSlotId(slotId)) {
            return IMS_TYPE_NOT_REGISTERED;
        }
        String key = OPLUS_SIM_IMS_TYPE + slotId;
        String value = Settings.System.getString(mContext.getContentResolver(), key);
        return value != null ? value : IMS_TYPE_NOT_REGISTERED;
    }

    void setCachedImsType(int slotId, String imsType) {
        if (!isValidSlotId(slotId)) {
            return;
        }
        Settings.System.putString(mContext.getContentResolver(), OPLUS_SIM_IMS_TYPE + slotId, imsType);
    }

    String resolveImsType(int slotId) {
        int subId = getSubIdForSlotId(slotId);
        ImsMmTelManager imsManager = getImsManager(subId);
        if (imsManager == null) {
            return getCachedImsType(slotId);
        }
        try {
            if (imsManager.isAvailable(MmTelFeature.MmTelCapabilities.CAPABILITY_TYPE_VOICE,
                    ImsRegistrationImplBase.REGISTRATION_TECH_IWLAN)) {
                setCachedImsType(slotId, IMS_TYPE_WLAN);
                return IMS_TYPE_WLAN;
            }
            if (imsManager.isAvailable(MmTelFeature.MmTelCapabilities.CAPABILITY_TYPE_VOICE,
                    ImsRegistrationImplBase.REGISTRATION_TECH_LTE)
                    || imsManager.isAvailable(MmTelFeature.MmTelCapabilities.CAPABILITY_TYPE_VOICE,
                    ImsRegistrationImplBase.REGISTRATION_TECH_NR)) {
                setCachedImsType(slotId, IMS_TYPE_WWAN);
                return IMS_TYPE_WWAN;
            }
        } catch (RuntimeException e) {
            Log.w(TAG, "Failed to query IMS type for slotId=" + slotId, e);
        }
        return getCachedImsType(slotId);
    }

    int getSoftSimCardSlotId() {
        Integer cached = mCachedSoftSimSlotId;
        if (cached != null) {
            return cached;
        }
        int slotId = Settings.System.getInt(
                mContext.getContentResolver(), OPLUS_SIM_VSIM_ID, INVALID_VSIM_ID);
        mCachedSoftSimSlotId = slotId;
        return slotId;
    }

    int setSoftSimCardSlotId(int slotId) {
        Settings.System.putInt(mContext.getContentResolver(), OPLUS_SIM_VSIM_ID, slotId);
        invalidateSoftSimCache();
        return 0;
    }

    long rafToNetworkTypeBitmask(int raf) {
        long bitmask = 0L;
        if ((raf & (OplusOSTelephonyManager.RAF_GPRS | OplusOSTelephonyManager.RAF_EDGE
                | OplusOSTelephonyManager.RAF_GSM)) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_GSM;
        }
        if ((raf & OplusOSTelephonyManager.RAF_UMTS) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_UMTS;
        }
        if ((raf & OplusOSTelephonyManager.RAF_IS95A) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_CDMA;
        }
        if ((raf & OplusOSTelephonyManager.RAF_1xRTT) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_1xRTT;
        }
        if ((raf & OplusOSTelephonyManager.RAF_EVDO_0) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_EVDO_0;
        }
        if ((raf & OplusOSTelephonyManager.RAF_EVDO_A) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_EVDO_A;
        }
        if ((raf & OplusOSTelephonyManager.RAF_EVDO_B) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_EVDO_B;
        }
        if ((raf & OplusOSTelephonyManager.RAF_EHRPD) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_EHRPD;
        }
        if ((raf & OplusOSTelephonyManager.RAF_HSDPA) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_HSDPA;
        }
        if ((raf & OplusOSTelephonyManager.RAF_HSUPA) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_HSUPA;
        }
        if ((raf & OplusOSTelephonyManager.RAF_HSPA) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_HSPA;
        }
        if ((raf & OplusOSTelephonyManager.RAF_HSPAP) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_HSPAP;
        }
        if ((raf & OplusOSTelephonyManager.RAF_LTE) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_LTE;
        }
        if ((raf & OplusOSTelephonyManager.RAF_LTE_CA) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_LTE_CA;
        }
        if ((raf & OplusOSTelephonyManager.RAF_TD_SCDMA) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_TD_SCDMA;
        }
        if ((raf & OplusOSTelephonyManager.RAF_NR) != 0) {
            bitmask |= TelephonyManager.NETWORK_TYPE_BITMASK_NR;
        }
        return bitmask;
    }

    int networkTypeFromRaf(int raf) {
        if ((raf & OplusOSTelephonyManager.RAF_NR) != 0) {
            return TelephonyManager.NETWORK_TYPE_NR;
        }
        if ((raf & (OplusOSTelephonyManager.RAF_LTE | OplusOSTelephonyManager.RAF_LTE_CA)) != 0) {
            return TelephonyManager.NETWORK_TYPE_LTE;
        }
        if ((raf & (OplusOSTelephonyManager.RAF_HSPAP | OplusOSTelephonyManager.RAF_HSPA
                | OplusOSTelephonyManager.RAF_HSUPA | OplusOSTelephonyManager.RAF_HSDPA
                | OplusOSTelephonyManager.RAF_UMTS | OplusOSTelephonyManager.RAF_TD_SCDMA)) != 0) {
            return TelephonyManager.NETWORK_TYPE_UMTS;
        }
        if ((raf & (OplusOSTelephonyManager.RAF_EVDO_0 | OplusOSTelephonyManager.RAF_EVDO_A
                | OplusOSTelephonyManager.RAF_EVDO_B | OplusOSTelephonyManager.RAF_1xRTT
                | OplusOSTelephonyManager.RAF_IS95A)) != 0) {
            return TelephonyManager.NETWORK_TYPE_CDMA;
        }
        if ((raf & (OplusOSTelephonyManager.RAF_GSM | OplusOSTelephonyManager.RAF_GPRS
                | OplusOSTelephonyManager.RAF_EDGE)) != 0) {
            return TelephonyManager.NETWORK_TYPE_GSM;
        }
        return TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }
}
