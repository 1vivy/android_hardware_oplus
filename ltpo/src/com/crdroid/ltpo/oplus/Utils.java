/*
 * Copyright (C) 2018-2026 crDroid Android Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.crdroid.ltpo.oplus;

import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;

import vendor.oplus.hardware.displaypanelfeature.IDisplayPanelFeature;

public final class Utils {
    private static final String TAG = "OplusLtpo";
    private static final String SERVICE =
            "vendor.oplus.hardware.displaypanelfeature.IDisplayPanelFeature/default";
    private static final int FEATURE_ADFR_CONTROL = 232;
    private static final int FEATURE_ADFR_STATE = 233;

    private Utils() {}

    public static void writeValue(String filename, String value) {
        if (filename == null) {
            return;
        }
        try {
            FileOutputStream fos = new FileOutputStream(new File(filename));
            fos.write(value.getBytes());
            fos.flush();
            fos.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static String readLine(String filename) {
        if (filename == null) {
            return null;
        }
        BufferedReader br = null;
        String line = null;
        try {
            br = new BufferedReader(new FileReader(filename), 1024);
            line = br.readLine();
        } catch (IOException e) {
            return null;
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    // Ignore close failures after the value has been read.
                }
            }
        }
        return line;
    }

    public static String getFileValue(String filename, String defaultValue) {
        String fileValue = readLine(filename);
        return fileValue != null ? fileValue : defaultValue;
    }

    private static IDisplayPanelFeature getDisplayPanelFeature() {
        return IDisplayPanelFeature.Stub.asInterface(
                ServiceManager.waitForDeclaredService(SERVICE));
    }

    public static boolean isLtpoSupported() {
        IDisplayPanelFeature service = getDisplayPanelFeature();
        if (service == null) {
            return false;
        }
        int[] state = {0};
        try {
            return service.getDisplayPanelFeatureValue(FEATURE_ADFR_STATE, state) == 0
                    && state[0] != 0;
        } catch (RemoteException e) {
            Log.e(TAG, "Unable to query ADFR support", e);
            return false;
        }
    }

    public static boolean setLtpoEnabled(boolean enabled) {
        IDisplayPanelFeature service = getDisplayPanelFeature();
        if (service == null) {
            return false;
        }
        try {
            return service.setDisplayPanelFeatureValue(
                    FEATURE_ADFR_CONTROL, new int[] {0, enabled ? 1 : 0}) == 0;
        } catch (RemoteException e) {
            Log.e(TAG, "Unable to update ADFR state", e);
            return false;
        }
    }
}
