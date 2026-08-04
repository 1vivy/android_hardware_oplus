/*
 * Copyright (C) 2026 crDroid Android Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

package com.crdroid.ltpo.oplus;

final class LTPOConfig {
    private LTPOConfig() {}

    static int merge(int current, int enabledValue, int managedMask, boolean enabled) {
        int managedValue = enabled ? enabledValue & managedMask : 0;
        return (current & ~managedMask) | managedValue;
    }

    static boolean isEnabled(int current, int enabledValue, int managedMask) {
        return managedMask != 0 && (current & managedMask) == (enabledValue & managedMask);
    }
}
