/*
 * Copyright (C) 2025 crDroid Android Project
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

import android.content.Context;
import android.content.SharedPreferences;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.UserHandle;
import android.os.Vibrator;
import android.provider.Settings;
import android.text.TextUtils;
import android.util.Log;
import android.view.MenuItem;
import androidx.preference.ListPreference;
import androidx.preference.PreferenceGroup;
import androidx.preference.PreferenceManager;
import androidx.preference.Preference;
import androidx.preference.SwitchPreferenceCompat;

import com.android.settingslib.widget.SettingsBasePreferenceFragment;

import java.util.Arrays;
import java.util.Locale;

public class LTPOSettings extends SettingsBasePreferenceFragment
        implements Preference.OnPreferenceChangeListener {
    private static final String TAG = LTPOSettings.class.getSimpleName();

    private static final String KEY_LTPO_SWITCH = "ltpo_enabled";

    private static final String FILE_LTPO = "/sys/kernel/oplus_display/adfr_config";

    private SwitchPreferenceCompat mLTPOSwitch;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.ltpo_settings);

        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(getContext());

        mLTPOSwitch = (SwitchPreferenceCompat) findPreference(KEY_LTPO_SWITCH);
        if (Utils.isLtpoSupported()) {
            mLTPOSwitch.setEnabled(true);
            Integer current = readLTPOConfig();
            boolean enabled = current != null && isLTPOEnabled(getContext(), current);
            mLTPOSwitch.setChecked(sharedPrefs.getBoolean(KEY_LTPO_SWITCH, enabled));
            mLTPOSwitch.setOnPreferenceChangeListener(this);
        } else {
            mLTPOSwitch.setEnabled(false);
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        if (preference == mLTPOSwitch) {
            boolean enabled = (Boolean) newValue;
            SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(getContext());
            if (!Utils.setLtpoEnabled(enabled)) {
                return false;
            }
            sharedPrefs.edit().putBoolean(KEY_LTPO_SWITCH, enabled).apply();
            writeLTPOSetting(getContext(), enabled);
            return true;
        }

        return false;
    }

    public static void restoreLTPOSetting(Context context) {
        if (Utils.isLtpoSupported()) {
            SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);
            Integer current = readLTPOConfig();
            boolean enabled = current != null && isLTPOEnabled(context, current);
            boolean value = sharedPrefs.getBoolean(KEY_LTPO_SWITCH, enabled);
            Utils.setLtpoEnabled(value);
            writeLTPOSetting(context, value);
        }
    }

    private static Integer readLTPOConfig() {
        String value = Utils.getFileValue(FILE_LTPO, null);
        if (value == null) {
            return null;
        }

        try {
            return Integer.decode(value.trim());
        } catch (NumberFormatException e) {
            Log.e(TAG, "Invalid LTPO config: " + value, e);
            return null;
        }
    }

    private static boolean isLTPOEnabled(Context context, int current) {
        Resources resources = context.getResources();
        int enabledValue = resources.getInteger(R.integer.config_ltpo_enabled_value);
        int managedMask = resources.getInteger(R.integer.config_ltpo_managed_mask);
        return LTPOConfig.isEnabled(current, enabledValue, managedMask);
    }

    private static void writeLTPOSetting(Context context, boolean enabled) {
        Integer current = readLTPOConfig();
        if (current == null) {
            return;
        }

        Resources resources = context.getResources();
        int enabledValue = resources.getInteger(R.integer.config_ltpo_enabled_value);
        int managedMask = resources.getInteger(R.integer.config_ltpo_managed_mask);
        int next = LTPOConfig.merge(current, enabledValue, managedMask, enabled);
        Utils.writeValue(FILE_LTPO, String.format(Locale.US, "0x%x", next));
    }
}
