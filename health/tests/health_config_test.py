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
            "battery_state_of_health_path": (
                "OPLUS_HEALTH_BATTERY_STATE_OF_HEALTH_PATH",
                "batteryStateOfHealthPath",
            ),
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
            "battery_first_usage_date_path": (
                "OPLUS_HEALTH_BATTERY_FIRST_USAGE_DATE_PATH",
                "batteryFirstUsageDatePath",
            ),
        }

        for variable, (define, config_field) in expected_paths.items():
            with self.subTest(variable=variable):
                self.assertIn(f'"{variable}"', android_bp)
                self.assertIn(f"-D{define}=", android_bp)
                self.assertIn(f"config->{config_field}", health_cpp)
                self.assertIn(define, health_cpp)

    def test_placeholder_maximum_capacity_is_reported_as_unsupported(self):
        health_cpp = (HEALTH_DIR / "Health.cpp").read_text()

        android_bp = (HEALTH_DIR / "Android.bp").read_text()

        self.assertIn('"battery_full_charge_requires_independent_value"', android_bp)
        self.assertIn("OPLUS_HEALTH_FULL_CHARGE_REQUIRES_INDEPENDENT_VALUE", android_bp)
        self.assertIn("full_charge <= 0", health_cpp)
        self.assertIn("full_charge == design_capacity", health_cpp)
        self.assertIn("health_info->batteryFullChargeUah = 0", health_cpp)

    def test_oplus_charging_control_reports_charge_hold_without_bypass(self):
        android_bp = (HEALTH_DIR / "Android.bp").read_text()
        charging_control = (HEALTH_DIR / "ChargingControl.cpp").read_text()

        self.assertIn('name: "vendor.lineage.health-service.oplus"', android_bp)
        self.assertIn('"vendor.lineage.health-service.default"', android_bp)
        self.assertIn("ChargingControlSupportedMode::TOGGLE", charging_control)
        self.assertNotIn("ChargingControlSupportedMode::BYPASS", charging_control)
        self.assertIn(
            '"/sys/class/oplus_chg/battery/mmi_charging_enable"',
            charging_control,
        )

    def test_oplus_service_uses_unique_binary_and_overrides_default(self):
        android_bp = (HEALTH_DIR / "Android.bp").read_text()
        service_rc = (
            HEALTH_DIR / "vendor.lineage.health-service.oplus.rc"
        ).read_text()

        self.assertIn(
            'overrides: ["vendor.lineage.health-service.default"]',
            android_bp,
        )
        self.assertNotIn(
            'stem: "vendor.lineage.health-service.default"',
            android_bp,
        )
        self.assertIn(
            "/vendor/bin/hw/vendor.lineage.health-service.oplus",
            service_rc,
        )
        self.assertNotIn(
            "/vendor/bin/hw/vendor.lineage.health-service.default",
            service_rc,
        )

    def test_lineage_health_deployment_keeps_both_standard_instances(self):
        service = (HEALTH_DIR / "LineageHealthService.cpp").read_text()
        vintf = (HEALTH_DIR / "vendor.lineage.health-service.oplus.xml").read_text()

        self.assertIn("SharedRefBase::make<ChargingControl>", service)
        self.assertIn("SharedRefBase::make<FastCharge>", service)
        self.assertIn('Service::descriptor) + "/default"', service)
        self.assertIn("IChargingControl/default", vintf)
        self.assertIn("IFastCharge/default", vintf)


if __name__ == "__main__":
    _ = unittest.main()
