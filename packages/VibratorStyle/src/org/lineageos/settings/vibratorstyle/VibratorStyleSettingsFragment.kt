/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package org.lineageos.settings.vibratorstyle

import android.os.Bundle
import android.os.SystemProperties
import androidx.preference.ListPreference
import androidx.preference.Preference
import com.android.settingslib.widget.SettingsBasePreferenceFragment

/**
 * Selects the vibrator effect-style tier.
 *
 * The tiers and their waveform payloads are shipped device data; this surface
 * only names which one the vibrator service should serve. The service resolves
 * the selection against the tiers it actually loaded and falls back to the
 * default tier for anything a smaller tier omits, so an unknown value here
 * degrades to standard haptics rather than to silence.
 */
class VibratorStyleSettingsFragment :
    SettingsBasePreferenceFragment(), Preference.OnPreferenceChangeListener {

    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        setPreferencesFromResource(R.xml.vibrator_style_settings, rootKey)

        val picker = findPreference<ListPreference>(STYLE_KEY)
        picker?.value = SystemProperties.get(STYLE_PROPERTY, DEFAULT_STYLE)
        picker?.onPreferenceChangeListener = this
    }

    override fun onPreferenceChange(preference: Preference, newValue: Any?): Boolean {
        val style = newValue as? String ?: return false
        // The service re-reads this property on its next effect lookup, so the
        // next haptic the user feels already uses the new tier.
        SystemProperties.set(STYLE_PROPERTY, style)
        return true
    }

    companion object {
        private const val STYLE_KEY = "vibrator_effect_style"
        const val STYLE_PROPERTY = "persist.sys.oplus.vibrator.effect_style"
        const val DEFAULT_STYLE = "def_style"
    }
}
