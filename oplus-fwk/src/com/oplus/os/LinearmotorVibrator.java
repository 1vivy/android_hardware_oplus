package com.oplus.os;

import android.content.Context;
import android.os.VibrationAttributes;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.provider.Settings;
import android.util.Slog;

/**
 * AOSP-backed implementation of the OOS linear-motor vibrator surface.
 *
 * OplusCamera and OppoGallery2 call this class for their haptics. The previous
 * implementation collapsed every request to a single predefined effect and reported
 * fabricated strength values, so both apps buzzed but every distinct haptic felt
 * identical and the strength sliders did nothing.
 *
 * This maps the OOS vocabulary onto the AOSP vibrator instead:
 *
 *   - callers that already hand us a real VibrationEffect get it played unchanged
 *     rather than discarded;
 *   - WaveformEffect ids are translated using the durations and amplitudes OOS itself
 *     declares in OplusVibratorConstant, with a per-family fallback so an unmapped id
 *     still feels like the right KIND of event;
 *   - strength getters report the user's real Settings intensity, scaled to the
 *     0..2400 range OOS uses (WaveformEffect.SETTINGS_VALUE_EFFECT_STRONG).
 *
 * Deliberately NOT emulated: the RAM/RTP waveform banks. Those live in the vendor
 * effect library and have no AOSP equivalent, so indexing them would report success
 * for something that never plays.
 */
public class LinearmotorVibrator {
    private static final String TAG = "LinearmotorVibrator";

    /** OOS reports vibration strength on a 0..2400 scale. */
    private static final int OOS_STRENGTH_MAX = 2400;
    /** AOSP Settings intensity is 0 (off) .. 3 (high). */
    private static final int AOSP_INTENSITY_MAX = 3;
    /** AOSP amplitude is 1..255. */
    private static final int AMPLITUDE_MAX = 255;

    private final Context mContext;
    private final Vibrator mVibrator;

    public LinearmotorVibrator(Context context, ILinearmotorVibratorService serviceIgnored) {
        mContext = context;
        mVibrator = context.getSystemService(Vibrator.class);
    }

    // ---------------------------------------------------------------- playback

    public void vibrate(WaveformEffect we) {
        if (mVibrator == null || we == null) return;
        VibrationEffect effect = toVibrationEffect(we.getEffectType(), we.getEffectStrength());
        if (effect != null) mVibrator.vibrate(effect);
    }

    public void cancelVibrate(WaveformEffect we) {
        if (mVibrator != null) mVibrator.cancel();
    }

    /** The caller already built a valid AOSP effect; play it rather than discarding it. */
    public void vibrate(VibrationEffect effect, VibrationAttributes attributes) {
        if (mVibrator == null || effect == null) return;
        if (attributes != null) {
            mVibrator.vibrate(effect, attributes);
        } else {
            mVibrator.vibrate(effect);
        }
    }

    public void vibrate(VibrationEffect effect, String reason, VibrationAttributes attributes) {
        vibrate(effect, attributes);
    }

    public void vibrate(
            int uid,
            String opPkg,
            VibrationEffect effect,
            String reason,
            VibrationAttributes attributes) {
        vibrate(effect, attributes);
    }

    // ---------------------------------------------------------------- creation

    public VibrationEffect createWaveformEffect(int effectId, int strength, int repeat) {
        return toVibrationEffect(effectId, strength);
    }

    public VibrationEffect createWaveformEffect(String ringtonePath, int strength, int repeat) {
        // A customised ringtone waveform is authored in the vendor RTP bank; there is no
        // AOSP equivalent to synthesise from a file path, so give the ringtone pattern.
        return waveform(
                OplusVibratorConstant.RAPID_STRONG_WAVEFORM_TIME,
                OplusVibratorConstant.RAPID_STRONG_WAVEFORM_AMPLITUDE,
                strength);
    }

    // ---------------------------------------------------------------- strength

    public int getVibratorStatus() {
        return (mVibrator != null && mVibrator.hasVibrator()) ? 0 : -1;
    }

    public void setVibratorStrength(int strength) {
        // Global vibration intensity belongs to the user, in Settings. A vendor app
        // silently overwriting it is the behaviour AOSP moved away from, so this stays
        // inert and the real value is reported back by the getters below.
        Slog.d(TAG, "setVibratorStrength(" + strength + ") ignored; intensity is a user setting");
    }

    public int getSettingsTouchEffectStrength() {
        return settingsStrength(Settings.System.HAPTIC_FEEDBACK_INTENSITY);
    }

    public int getSettingsRingtoneEffectStrength() {
        return settingsStrength(Settings.System.RING_VIBRATION_INTENSITY);
    }

    public int getSettingsNotificationEffectStrength() {
        return settingsStrength(Settings.System.NOTIFICATION_VIBRATION_INTENSITY);
    }

    public int getVibratorTouchStyle() {
        return 0;
    }

    public void setVibratorTouchStyle(int style) {
        Slog.d(TAG, "setVibratorTouchStyle(" + style + ") ignored; no AOSP equivalent");
    }

    public void updateVibrationAmplitude(float amplitudeRatio) {
        // Amplitude is carried per-effect below; there is no global AOSP scalar to set.
    }

    /** Read the user's intensity (0..3) and report it on the 0..2400 scale OOS expects. */
    private int settingsStrength(String key) {
        int intensity;
        try {
            intensity =
                    Settings.System.getInt(mContext.getContentResolver(), key, AOSP_INTENSITY_MAX);
        } catch (Exception e) {
            intensity = AOSP_INTENSITY_MAX;
        }
        if (intensity < 0) intensity = 0;
        if (intensity > AOSP_INTENSITY_MAX) intensity = AOSP_INTENSITY_MAX;
        return intensity * OOS_STRENGTH_MAX / AOSP_INTENSITY_MAX;
    }

    // ---------------------------------------------------------------- mapping

    /**
     * Translate an OOS effect id into the closest AOSP effect.
     *
     * The low ids are the generic families WaveformEffect documents, and they map onto
     * the durations and amplitudes in OplusVibratorConstant. Higher ids are named
     * ringtone/notification/game patterns from the vendor bank; those fall back by
     * CATEGORY so a notification still feels like a notification, instead of every id
     * producing one identical click.
     */
    private VibrationEffect toVibrationEffect(int effectType, int strength) {
        switch (effectType) {
            case 0: // EFFECT_WEAKEST_SHORT_VIBRATE_ONCE
                return oneShot(
                        OplusVibratorConstant.RAPID_WEAK_ONESHOT_TIME,
                        OplusVibratorConstant.WEAK_AMPLITUDE,
                        strength);
            case 1: // EFFECT_WEAK_SHORT_VIBRATE_ONCE
                return oneShot(
                        OplusVibratorConstant.RAPID_WEAK_ONESHOT_TIME,
                        OplusVibratorConstant.MIDDLE_AMPLITUDE,
                        strength);
            case 2: // EFFECT_MODERATE_SHORT_VIBRATE_ONCE
                return oneShot(
                        OplusVibratorConstant.RAPID_MIDDLE_ONESHOT_TIME,
                        OplusVibratorConstant.MIDDLE_AMPLITUDE,
                        strength);
            case 3: // EFFECT_MODERATE_SHORT_VIBRATE_TWICE (OOS gives TRIPLE the same id)
                return waveform(
                        OplusVibratorConstant.RAPID_MIDDLE_WAVEFORM_TIME,
                        OplusVibratorConstant.RAPID_MIDDLE_WAVEFORM_AMPLITUDE,
                        strength);
            case 9: // EFFECT_OTHER_BIG_SCALE
                return oneShot(
                        OplusVibratorConstant.LONG_STRONG_ONESHOT_TIME,
                        OplusVibratorConstant.STRONG_AMPLITUDE,
                        strength);
            case 10: // EFFECT_OTHER_SMALL_SCALE
                return oneShot(
                        OplusVibratorConstant.RAPID_WEAK_ONESHOT_TIME,
                        OplusVibratorConstant.WEAK_AMPLITUDE,
                        strength);
            case 156: // fingerprint accepted
            case 368: // virtual key feedback
            case 409: // screenshot feedback
                return VibrationEffect.createPredefined(VibrationEffect.EFFECT_CLICK);
            case 157: // keyboard weak
                return VibrationEffect.createPredefined(VibrationEffect.EFFECT_TICK);
            case 158: // keyboard medium
                return VibrationEffect.createPredefined(VibrationEffect.EFFECT_CLICK);
            case 159: // keyboard strong
                return VibrationEffect.createPredefined(VibrationEffect.EFFECT_HEAVY_CLICK);
            default:
                return fallbackForFamily(effectType, strength);
        }
    }

    /**
     * Named vendor patterns, grouped by the id ranges WaveformEffect uses. These are
     * RAM/RTP bank entries we cannot reproduce, so the goal is the right character.
     */
    private VibrationEffect fallbackForFamily(int effectType, int strength) {
        // 4..8 are the generic notification tunes.
        if (effectType >= 4 && effectType <= 8) {
            return VibrationEffect.createPredefined(VibrationEffect.EFFECT_DOUBLE_CLICK);
        }
        // 11..13 PUBG weapon feedback - short and firm.
        if (effectType >= 11 && effectType <= 13) {
            return VibrationEffect.createPredefined(VibrationEffect.EFFECT_HEAVY_CLICK);
        }
        // Ringtone patterns repeat; a single tap is the wrong character for them.
        if ((effectType >= 100 && effectType <= 213) || (effectType >= 300 && effectType <= 470)) {
            return waveform(
                    OplusVibratorConstant.RAPID_STRONG_WAVEFORM_TIME,
                    OplusVibratorConstant.RAPID_STRONG_WAVEFORM_AMPLITUDE,
                    strength);
        }
        // 214..299 are notification patterns.
        if (effectType >= 214 && effectType <= 299) {
            return VibrationEffect.createPredefined(VibrationEffect.EFFECT_DOUBLE_CLICK);
        }
        return VibrationEffect.createPredefined(VibrationEffect.EFFECT_CLICK);
    }

    private VibrationEffect oneShot(long durationMs, int amplitude, int strength) {
        return VibrationEffect.createOneShot(durationMs, scaleAmplitude(amplitude, strength));
    }

    private VibrationEffect waveform(long[] timings, int[] amplitudes, int strength) {
        int[] scaled = new int[amplitudes.length];
        for (int i = 0; i < amplitudes.length; i++) {
            scaled[i] = amplitudes[i] == 0 ? 0 : scaleAmplitude(amplitudes[i], strength);
        }
        return VibrationEffect.createWaveform(timings, scaled, -1);
    }

    /**
     * Apply a caller-supplied strength to an amplitude. OOS passes 0..2400, or -1 to
     * mean "use the user's setting", in which case the amplitude is left alone and the
     * framework's own intensity scaling applies.
     */
    private int scaleAmplitude(int amplitude, int strength) {
        if (strength < 0 || strength >= OOS_STRENGTH_MAX) {
            return clampAmplitude(amplitude);
        }
        long scaled = (long) amplitude * strength / OOS_STRENGTH_MAX;
        return clampAmplitude((int) scaled);
    }

    private int clampAmplitude(int amplitude) {
        if (amplitude < 1) return 1;
        if (amplitude > AMPLITUDE_MAX) return AMPLITUDE_MAX;
        return amplitude;
    }
}
