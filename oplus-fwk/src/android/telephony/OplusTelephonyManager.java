/*
 * Copyright (C) 2024 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.telephony;

import android.content.Context;
import android.util.Log;

public class OplusTelephonyManager {
    private static final String TAG = "OplusTelephonyManager";
    private static OplusTelephonyManager sInstance = null;
    private static final Object sLock = new Object();

    private final OplusTelephonyAdapter mAdapter;

    public OplusTelephonyManager(Context context) {
        mAdapter = new OplusTelephonyAdapter(context);
    }

    public static OplusTelephonyManager getInstance(Context context) {
        synchronized (sLock) {
            if (sInstance == null) {
                sInstance = new OplusTelephonyManager(context);
            }
        }
        return sInstance;
    }

    public String getImsType(int slotId) {
        return mAdapter.resolveImsType(slotId);
    }

    public int setVsimId(int id) {
        return mAdapter.setSoftSimCardSlotId(id);
    }

    public static void invalidateOplusSoftSimCaches() {
        synchronized (sLock) {
            if (sInstance != null) {
                sInstance.mAdapter.invalidateSoftSimCache();
            }
        }
    }

    static void logUnsupported(String method, Exception e) {
        Log.w(TAG, method + " unsupported on this build", e);
    }
}
