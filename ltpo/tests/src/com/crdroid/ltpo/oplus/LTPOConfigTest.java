/*
 * Copyright (C) 2026 crDroid Android Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

package com.crdroid.ltpo.oplus;

public final class LTPOConfigTest {
    private LTPOConfigTest() {}

    public static void main(String[] args) {
        testLegacyEnablePreservesUnmanagedBits();
        testInfinitiEnablePreservesPanelSpecificBits();
        testInfinitiDisableClearsOnlyGlobalAdfr();
        testEnabledStateUsesOnlyManagedBits();
    }

    private static void testLegacyEnablePreservesUnmanagedBits() {
        assertEquals(
                "legacy enable must retain unknown panel bits",
                0x709f,
                LTPOConfig.merge(0x6000, 0x109f, 0x109f, true));
    }

    private static void testInfinitiEnablePreservesPanelSpecificBits() {
        assertEquals(
                "infiniti enable must restore global ADFR without replacing 0x6e40",
                0x6e41,
                LTPOConfig.merge(0x6e40, 0x6e41, 0x0001, true));
    }

    private static void testInfinitiDisableClearsOnlyGlobalAdfr() {
        assertEquals(
                "infiniti disable must preserve high-precision and OA configuration",
                0x6e40,
                LTPOConfig.merge(0x6e41, 0x6e41, 0x0001, false));
    }

    private static void testEnabledStateUsesOnlyManagedBits() {
        assertTrue(
                "unmanaged panel bits must not make LTPO appear disabled",
                LTPOConfig.isEnabled(0x6e41, 0x6e41, 0x0001));
        assertFalse(
                "cleared global ADFR bit must make LTPO appear disabled",
                LTPOConfig.isEnabled(0x6e40, 0x6e41, 0x0001));
    }

    private static void assertEquals(String message, int expected, int actual) {
        if (expected != actual) {
            throw new AssertionError(
                    message
                            + ": expected 0x"
                            + Integer.toHexString(expected)
                            + ", got 0x"
                            + Integer.toHexString(actual));
        }
    }

    private static void assertTrue(String message, boolean value) {
        if (!value) {
            throw new AssertionError(message);
        }
    }

    private static void assertFalse(String message, boolean value) {
        if (value) {
            throw new AssertionError(message);
        }
    }
}
