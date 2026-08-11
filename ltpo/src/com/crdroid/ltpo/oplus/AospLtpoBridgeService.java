/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.crdroid.ltpo.oplus;

import android.app.Service;
import android.content.Intent;
import android.hardware.display.DisplayManager;
import android.os.IBinder;
import android.os.SystemProperties;
import android.util.Log;
import android.view.Display;

/**
 * Republishes AOSP's own refresh-rate mode decision (SurfaceFlinger's active
 * Display.Mode, driven by content, touch, idle, and per-app setFrameRate() votes
 * within the range configured by Settings.System.MIN_REFRESH_RATE /
 * PEAK_REFRESH_RATE) as {@link #PROP_AOSP_MODE}, so the vendor-side ADFR floor
 * daemon can translate it into a legal min-fps floor without itself depending on
 * any AOSP framework API.
 *
 * This service owns the policy input only; it never touches
 * /sys/kernel/oplus_display directly, and it never picks a floor value itself -
 * see hardware/oplus displaypanelfeature/AdfrFloorPolicy.{h,cpp} for that.
 */
public class AospLtpoBridgeService extends Service {
    private static final String TAG = "AospLtpoBridge";

    static final String PROP_AOSP_MODE = "persist.sys.displaypanel.ltpo_aosp_mode";

    private DisplayManager mDisplayManager;
    private final DisplayManager.DisplayListener mListener = new DisplayManager.DisplayListener() {
        @Override
        public void onDisplayAdded(int displayId) {}

        @Override
        public void onDisplayRemoved(int displayId) {}

        @Override
        public void onDisplayChanged(int displayId) {
            if (displayId == Display.DEFAULT_DISPLAY) publishActiveMode();
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();
        mDisplayManager = getSystemService(DisplayManager.class);
        if (mDisplayManager == null) {
            Log.e(TAG, "DisplayManager unavailable, AOSP cadence bridge is inert");
            return;
        }
        mDisplayManager.registerDisplayListener(mListener, null);
        publishActiveMode();
    }

    @Override
    public void onDestroy() {
        if (mDisplayManager != null) {
            mDisplayManager.unregisterDisplayListener(mListener);
        }
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void publishActiveMode() {
        Display display = mDisplayManager.getDisplay(Display.DEFAULT_DISPLAY);
        if (display == null) return;

        int modeHz = Math.round(display.getMode().getRefreshRate());
        SystemProperties.set(PROP_AOSP_MODE, Integer.toString(modeHz));
    }
}
