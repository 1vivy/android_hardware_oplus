/*
 * SPDX-FileCopyrightText: 2021-2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package org.lineageos.settings.device

import android.app.role.RoleManager
import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.preference.ListPreference
import com.android.settingslib.widget.SettingsBasePreferenceFragment

class ButtonSettingsFragment : SettingsBasePreferenceFragment() {
    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        setPreferencesFromResource(R.xml.button_panel, rootKey)

        if (!hasAssistant()) {
            dropAction(KeyHandler.PLUS_ACTION_ASSISTANT)
        }

        populateAppPicker()
    }

    private fun hasAssistant(): Boolean {
        val context = requireContext()

        try {
            val roleManager = context.getSystemService(RoleManager::class.java)
            if (roleManager?.getRoleHolders(RoleManager.ROLE_ASSISTANT)?.isNotEmpty() == true) {
                return true
            }
        } catch (e: SecurityException) {
            // An unreadable role is not an absent assistant: fall through to the probe.
            Log.w(TAG, "cannot read the assistant role holder", e)
        }

        return context.packageManager.resolveActivity(Intent(Intent.ACTION_ASSIST), 0) != null
    }

    private fun dropAction(action: Int) {
        val value = action.toString()

        for (key in ACTION_PREFERENCES) {
            val preference = findPreference<ListPreference>(key) ?: continue
            val index = preference.entryValues.indexOfFirst { it.toString() == value }
            if (index < 0) continue

            preference.entries =
                preference.entries.filterIndexed { i, _ -> i != index }.toTypedArray()
            preference.entryValues =
                preference.entryValues.filterIndexed { i, _ -> i != index }.toTypedArray()
            if (preference.value == value) {
                preference.value = KeyHandler.ACTION_NONE.toString()
            }
        }
    }

    private fun populateAppPicker() {
        val preference = findPreference<ListPreference>(KeyHandler.PLUS_KEY_APP) ?: return
        val packageManager = requireContext().packageManager
        val apps =
            packageManager
                .queryIntentActivities(
                    Intent(Intent.ACTION_MAIN).addCategory(Intent.CATEGORY_LAUNCHER),
                    0,
                )
                .map { it.loadLabel(packageManager).toString() to it.activityInfo.packageName }
                .distinctBy { it.second }
                .sortedBy { it.first.lowercase() }

        preference.entries = apps.map { it.first }.toTypedArray()
        preference.entryValues = apps.map { it.second }.toTypedArray()
    }

    companion object {
        private const val TAG = "ButtonSettings"

        private val ACTION_PREFERENCES =
            arrayOf("config_plus_key_action", "config_plus_key_long_press_action")
    }
}
