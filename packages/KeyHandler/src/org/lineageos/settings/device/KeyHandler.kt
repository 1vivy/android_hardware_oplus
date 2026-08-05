/*
 * Copyright (C) 2021-2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package org.lineageos.settings.device

import android.app.NotificationManager
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.hardware.camera2.CameraCharacteristics
import android.hardware.camera2.CameraManager
import android.media.AudioManager
import android.media.AudioSystem
import android.os.VibrationAttributes
import android.os.VibrationEffect
import android.os.Vibrator
import android.provider.MediaStore
import android.provider.Settings
import android.view.KeyEvent
import android.view.ViewConfiguration
import com.android.internal.os.DeviceKeyHandler
import java.io.File
import java.util.concurrent.Executors
import java.util.concurrent.ScheduledFuture
import java.util.concurrent.TimeUnit

class KeyHandler(private val context: Context) : DeviceKeyHandler {
    private val audioManager = context.getSystemService(AudioManager::class.java)!!
    private val cameraManager = context.getSystemService(CameraManager::class.java)!!
    private val notificationManager = context.getSystemService(NotificationManager::class.java)!!
    private val vibrator = context.getSystemService(Vibrator::class.java)!!

    private val packageContext =
        context.createPackageContext(KeyHandler::class.java.getPackage()!!.name, 0)
    private val sharedPreferences
        get() =
            packageContext.getSharedPreferences(
                packageContext.packageName + "_preferences",
                Context.MODE_PRIVATE or Context.MODE_MULTI_PROCESS,
            )

    private val executorService = Executors.newSingleThreadExecutor()
    private val scheduler = Executors.newSingleThreadScheduledExecutor()

    // Plus Key press-length state. A single physical key serves two mappings, so the
    // short action can only be resolved once the key is released without the long
    // press having already fired.
    private var longPressFuture: ScheduledFuture<*>? = null
    private var longPressFired = false

    // Torch state has to be observed rather than assumed: the flashlight can also be
    // toggled from the tile or another app, and a toggle that tracks its own private
    // boolean gets out of step the first time that happens.
    private var torchEnabled = false
    private val torchCallback =
        object : CameraManager.TorchCallback() {
            override fun onTorchModeChanged(cameraId: String, enabled: Boolean) {
                if (cameraId == flashCameraId) torchEnabled = enabled
            }
        }
    private val flashCameraId: String? by lazy {
        cameraManager.cameraIdList.firstOrNull { id ->
            cameraManager.getCameraCharacteristics(id).get(CameraCharacteristics.FLASH_INFO_AVAILABLE) == true
        }
    }

    private var wasMuted = false
    private val broadcastReceiver =
        object : BroadcastReceiver() {
            override fun onReceive(context: Context, intent: Intent) {
                when (intent.action) {
                    AudioManager.STREAM_MUTE_CHANGED_ACTION -> {
                        val stream = intent.getIntExtra(AudioManager.EXTRA_VOLUME_STREAM_TYPE, -1)
                        val state =
                            intent.getBooleanExtra(AudioManager.EXTRA_STREAM_VOLUME_MUTED, false)
                        if (stream == AudioSystem.STREAM_MUSIC && !state) {
                            wasMuted = false
                        }
                    }

                    Intent.ACTION_BOOT_COMPLETED -> populateKeyState(true)
                }
            }
        }

    init {
        context.registerReceiver(
            broadcastReceiver,
            IntentFilter().apply {
                addAction(AudioManager.STREAM_MUTE_CHANGED_ACTION)
                addAction(Intent.ACTION_BOOT_COMPLETED)
            },
        )
        cameraManager.registerTorchCallback(executorService, torchCallback)
    }

    override fun handleKeyEvent(event: KeyEvent): KeyEvent? {
        // The Plus Key needs both the press and the release to tell a short press
        // from a long one, so it is routed before the ACTION_DOWN filter below.
        if (event.scanCode == PLUS_KEY_SCANCODE) {
            return handlePlusKey(event)
        }

        if (event.action != KeyEvent.ACTION_DOWN) {
            return event
        }

        val deviceName = event.device.name

        if (deviceName != "oplus,hall_tri_state_key" && deviceName != "oplus,tri-state-key") {
            return event
        }

        populateKeyState(false)

        return null
    }

    private fun populateKeyState(firstRun: Boolean) {
        val node = File("/proc/tristatekey/tri_state")
        if (!node.exists()) {
            // No three-position slider on this generation; nothing to restore.
            return
        }

        when (node.readText().trim()) {
            "1" -> handleMode(POSITION_TOP, firstRun)
            "2" -> handleMode(POSITION_MIDDLE, firstRun)
            "3" -> handleMode(POSITION_BOTTOM, firstRun)
        }
    }

    private fun vibrateIfNeeded(mode: Int) {
        when (mode) {
            AudioManager.RINGER_MODE_VIBRATE ->
                vibrator.vibrate(MODE_VIBRATION_EFFECT, HARDWARE_FEEDBACK_VIBRATION_ATTRIBUTES)
            AudioManager.RINGER_MODE_NORMAL ->
                vibrator.vibrate(MODE_NORMAL_EFFECT, HARDWARE_FEEDBACK_VIBRATION_ATTRIBUTES)
        }
    }

    private fun handleMode(position: Int, firstRun: Boolean) {
        val muteMedia = sharedPreferences.getBoolean(MUTE_MEDIA_WITH_SILENT, false)
        val showDialog = sharedPreferences.getBoolean(SHOW_DIALOG, true)

        val mode =
            when (position) {
                POSITION_TOP -> sharedPreferences.getString(ALERT_SLIDER_TOP_KEY, "0")!!.toInt()
                POSITION_MIDDLE ->
                    sharedPreferences.getString(ALERT_SLIDER_MIDDLE_KEY, "1")!!.toInt()
                POSITION_BOTTOM ->
                    sharedPreferences.getString(ALERT_SLIDER_BOTTOM_KEY, "2")!!.toInt()
                else -> return
            }

        executorService.submit {
            applyAction(mode, muteMedia)

            if (!firstRun) {
                if (showDialog) sendNotification(position, mode)
                vibrateIfNeeded(mode)
            }
        }
    }

    private fun handlePlusKey(event: KeyEvent): KeyEvent? {
        val shortAction = plusKeyAction(PLUS_KEY_ACTION)
        val longAction = plusKeyAction(PLUS_KEY_LONG_PRESS_ACTION)

        // Nothing mapped: hand the event back untouched so the keylayout assignment
        // still applies and the key keeps its stock behaviour.
        if (shortAction == ACTION_NONE && longAction == ACTION_NONE) {
            return event
        }

        when (event.action) {
            KeyEvent.ACTION_DOWN -> {
                if (event.repeatCount != 0) return null
                longPressFired = false
                longPressFuture?.cancel(false)
                longPressFuture =
                    if (longAction != ACTION_NONE) {
                        scheduler.schedule(
                            {
                                longPressFired = true
                                executorService.submit { applyPlusKeyAction(longAction) }
                            },
                            ViewConfiguration.getLongPressTimeout().toLong(),
                            TimeUnit.MILLISECONDS,
                        )
                    } else {
                        null
                    }
            }

            KeyEvent.ACTION_UP -> {
                longPressFuture?.cancel(false)
                longPressFuture = null
                if (!longPressFired && shortAction != ACTION_NONE) {
                    executorService.submit { applyPlusKeyAction(shortAction) }
                }
                longPressFired = false
            }
        }

        return null
    }

    private fun plusKeyAction(key: String) =
        sharedPreferences.getString(key, ACTION_NONE.toString())!!.toInt()

    /**
     * Plus Key actions. A programmable shortcut button wants button actions, which is a
     * different vocabulary from the alert slider's ringer positions - reusing those made
     * the key able to do nothing but set ringer modes.
     */
    private fun applyPlusKeyAction(action: Int) {
        when (action) {
            PLUS_ACTION_ASSISTANT -> startActivitySafely(Intent(Intent.ACTION_ASSIST))
            PLUS_ACTION_CAMERA ->
                startActivitySafely(Intent(MediaStore.INTENT_ACTION_STILL_IMAGE_CAMERA))
            PLUS_ACTION_SEARCH -> startActivitySafely(Intent(Intent.ACTION_WEB_SEARCH))

            PLUS_ACTION_PLAY_PAUSE -> {
                // Dispatch a full down/up pair; a lone down leaves the session waiting.
                val now = android.os.SystemClock.uptimeMillis()
                for (a in intArrayOf(KeyEvent.ACTION_DOWN, KeyEvent.ACTION_UP)) {
                    audioManager.dispatchMediaKeyEvent(
                        KeyEvent(now, now, a, KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE, 0)
                    )
                }
            }

            PLUS_ACTION_TORCH -> flashCameraId?.let {
                cameraManager.setTorchMode(it, !torchEnabled)
            }

            PLUS_ACTION_RINGER_CYCLE -> {
                setZenMode(Settings.Global.ZEN_MODE_OFF)
                val next =
                    when (audioManager.ringerModeInternal) {
                        AudioManager.RINGER_MODE_NORMAL -> AudioManager.RINGER_MODE_VIBRATE
                        AudioManager.RINGER_MODE_VIBRATE -> AudioManager.RINGER_MODE_SILENT
                        else -> AudioManager.RINGER_MODE_NORMAL
                    }
                audioManager.ringerModeInternal = next
                vibrateIfNeeded(next)
            }

            PLUS_ACTION_DND ->
                setZenMode(
                    if (notificationManager.zenMode == Settings.Global.ZEN_MODE_OFF)
                        Settings.Global.ZEN_MODE_IMPORTANT_INTERRUPTIONS
                    else Settings.Global.ZEN_MODE_OFF
                )

            PLUS_ACTION_MUTE -> {
                // Ask the system rather than tracking a private flag: media can be
                // muted from anywhere, and a toggle over a stale guess inverts.
                val muted = audioManager.isStreamMute(AudioManager.STREAM_MUSIC)
                audioManager.adjustVolume(
                    if (muted) AudioManager.ADJUST_UNMUTE else AudioManager.ADJUST_MUTE,
                    0,
                )
            }
        }
    }

    private fun startActivitySafely(intent: Intent) {
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TOP)
        try {
            context.startActivityAsUser(intent, android.os.UserHandle.CURRENT)
        } catch (e: android.content.ActivityNotFoundException) {
            android.util.Log.w(TAG, "no activity for ${intent.action}", e)
        }
    }

    private fun applyAction(mode: Int, muteMedia: Boolean) {
        when (mode) {
            AudioManager.RINGER_MODE_SILENT -> {
                setZenMode(Settings.Global.ZEN_MODE_OFF)
                audioManager.ringerModeInternal = mode
                if (muteMedia) {
                    audioManager.adjustVolume(AudioManager.ADJUST_MUTE, 0)
                    wasMuted = true
                }
            }
            AudioManager.RINGER_MODE_VIBRATE,
            AudioManager.RINGER_MODE_NORMAL -> {
                setZenMode(Settings.Global.ZEN_MODE_OFF)
                audioManager.ringerModeInternal = mode
                if (muteMedia && wasMuted) {
                    audioManager.adjustVolume(AudioManager.ADJUST_UNMUTE, 0)
                }
            }
            ZEN_PRIORITY_ONLY,
            ZEN_TOTAL_SILENCE,
            ZEN_ALARMS_ONLY -> {
                audioManager.ringerModeInternal = AudioManager.RINGER_MODE_NORMAL
                setZenMode(mode - ZEN_OFFSET)
                if (muteMedia && wasMuted) {
                    audioManager.adjustVolume(AudioManager.ADJUST_UNMUTE, 0)
                }
            }
            TORCH_ON,
            TORCH_OFF -> {
                val cameraId =
                    cameraManager.cameraIdList.firstOrNull { id ->
                        cameraManager
                            .getCameraCharacteristics(id)
                            .get(CameraCharacteristics.FLASH_INFO_AVAILABLE) == true
                    }
                if (cameraId != null) {
                    cameraManager.setTorchMode(cameraId, mode == TORCH_ON)
                }
            }
        }
    }

    private fun setZenMode(zenMode: Int) {
        // Set zen mode
        notificationManager.setZenMode(zenMode, null, TAG)

        // Wait until zen mode change is committed
        while (notificationManager.zenMode != zenMode) {
            Thread.sleep(10)
        }
    }

    private fun sendNotification(position: Int, mode: Int) {
        context.sendBroadcast(
            Intent(CHANGED_ACTION).apply {
                putExtra("position", position)
                putExtra("mode", mode)
            }
        )
    }

    companion object {
        private const val TAG = "KeyHandler"

        // Intent actions
        const val CHANGED_ACTION = "org.lineageos.settings.UPDATE_SETTINGS"

        // Slider key positions
        const val POSITION_TOP = 1
        const val POSITION_MIDDLE = 2
        const val POSITION_BOTTOM = 3

        // Plus Key. This generation replaced the three-position alert slider with a
        // single programmable key, which enumerates as BTN_TRIGGER_HAPPY32 on gpio-keys
        // and is assigned ASSIST by the device keylayout.
        private const val PLUS_KEY_SCANCODE = 735
        private const val PLUS_KEY_ACTION = "config_plus_key_action"
        private const val PLUS_KEY_LONG_PRESS_ACTION = "config_plus_key_long_press_action"
        const val ACTION_NONE = -1

        // Plus Key action vocabulary. Deliberately disjoint from the slider's ringer
        // values so the two mappings cannot be confused for one another.
        private const val PLUS_ACTION_ASSISTANT = 1
        private const val PLUS_ACTION_CAMERA = 2
        private const val PLUS_ACTION_SEARCH = 4
        private const val PLUS_ACTION_PLAY_PAUSE = 5
        private const val PLUS_ACTION_TORCH = 6
        private const val PLUS_ACTION_RINGER_CYCLE = 7
        private const val PLUS_ACTION_DND = 8
        private const val PLUS_ACTION_MUTE = 9

        // Preference keys
        private const val ALERT_SLIDER_TOP_KEY = "config_top_position"
        private const val ALERT_SLIDER_MIDDLE_KEY = "config_middle_position"
        private const val ALERT_SLIDER_BOTTOM_KEY = "config_bottom_position"
        private const val MUTE_MEDIA_WITH_SILENT = "config_mute_media"
        private const val SHOW_DIALOG = "config_show_dialog"

        // ZEN constants
        private const val ZEN_OFFSET = 2
        const val ZEN_PRIORITY_ONLY = 3
        const val ZEN_TOTAL_SILENCE = 4
        const val ZEN_ALARMS_ONLY = 5

        // Torch constants
        private const val TORCH_OFFSET = 8
        const val TORCH_ON = TORCH_OFFSET + 0
        const val TORCH_OFF = TORCH_OFFSET + 1

        // Vibration attributes
        private val HARDWARE_FEEDBACK_VIBRATION_ATTRIBUTES =
            VibrationAttributes.createForUsage(VibrationAttributes.USAGE_HARDWARE_FEEDBACK)

        // Vibration effects
        private val MODE_NORMAL_EFFECT = VibrationEffect.get(VibrationEffect.EFFECT_HEAVY_CLICK)
        private val MODE_VIBRATION_EFFECT = VibrationEffect.get(VibrationEffect.EFFECT_DOUBLE_CLICK)
    }
}
