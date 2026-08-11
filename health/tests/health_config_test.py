#!/usr/bin/env python3
#
# SPDX-FileCopyrightText: 2026 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

import pathlib
import unittest


HEALTH_DIR = pathlib.Path(__file__).resolve().parents[1]


class HealthConfigTest(unittest.TestCase):
    def test_all_battery_lifetime_paths_are_configurable_and_seeded(self):
        android_bp = (HEALTH_DIR / "Android.bp").read_text()
        health_cpp = (HEALTH_DIR / "Health.cpp").read_text()

        expected_paths = {
            "battery_cycle_count_path": (
                "OPLUS_HEALTH_BATTERY_CYCLE_COUNT_PATH",
                "batteryCycleCountPath",
            ),
            "battery_full_charge_path": (
                "OPLUS_HEALTH_BATTERY_FULL_CHARGE_PATH",
                "batteryFullChargePath",
            ),
            "battery_full_charge_design_capacity_uah_path": (
                "OPLUS_HEALTH_BATTERY_FULL_CHARGE_DESIGN_CAPACITY_UAH_PATH",
                "batteryFullChargeDesignCapacityUahPath",
            ),
        }

        for variable, (define, config_field) in expected_paths.items():
            with self.subTest(variable=variable):
                self.assertIn(f'"{variable}"', android_bp)
                self.assertIn(f"-D{define}=", android_bp)
                self.assertIn(f"config->{config_field}", health_cpp)
                self.assertIn(define, health_cpp)


if __name__ == "__main__":
    unittest.main()
