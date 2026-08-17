# SPDX-FileCopyrightText: 2026 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
"""The picker and the vibrator service must agree, and the write must be legal.

A settings surface that writes a property nobody reads is indistinguishable, at
a glance, from a working feature - that is the failure this file exists to make
impossible. Every assertion here joins TWO files, so renaming either side of the
contract reddens the suite instead of silently producing a dead picker.
"""

from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
APP = ROOT / "packages/VibratorStyle"
LOADER_HEADER = ROOT / "aidl/vibrator/qti/effect/VibrationEffectLoader.h"
LOADER_SOURCE = ROOT / "aidl/vibrator/qti/effect/VibrationEffectLoader.cpp"
FRAGMENT = (
    APP / "src/org/lineageos/settings/vibratorstyle/VibratorStyleSettingsFragment.kt"
)
SEPOLICY = ROOT / "sepolicy/qti"
PROPERTY_TYPE = "system_oplus_vibrator_prop"


def read(path: Path) -> str:
    assert path.is_file(), f"missing contract file: {path}"
    return path.read_text(encoding="utf-8")


def cpp_constant(name: str) -> str:
    match = re.search(
        rf'{name}\s*=\s*"([^"]+)"', read(LOADER_HEADER)
    )
    assert match, f"{name} is not declared in {LOADER_HEADER.name}"
    return match.group(1)


def kotlin_constant(name: str) -> str:
    match = re.search(rf'{name}\s*=\s*"([^"]+)"', read(FRAGMENT))
    assert match, f"{name} is not declared in {FRAGMENT.name}"
    return match.group(1)


def offered_values() -> list[str]:
    arrays = read(APP / "res/values/arrays.xml")
    block = re.search(
        r'name="vibrator_style_values".*?</string-array>', arrays, re.S
    )
    assert block, "vibrator_style_values array is missing"
    return re.findall(r"<item>([^<]+)</item>", block.group(0))


class VibratorStyleContractTest(unittest.TestCase):
    def test_probe_resolves_both_sides_of_the_contract(self) -> None:
        # Without this, a typo in either regex would make every other
        # assertion below compare empty strings and pass vacuously.
        self.assertTrue(cpp_constant("kStyleProperty").startswith("persist."))
        self.assertTrue(kotlin_constant("STYLE_PROPERTY").startswith("persist."))
        self.assertTrue(offered_values())

    def test_picker_writes_exactly_the_property_the_service_reads(self) -> None:
        self.assertEqual(
            cpp_constant("kStyleProperty"), kotlin_constant("STYLE_PROPERTY")
        )

    def test_picker_default_matches_the_service_default_tier(self) -> None:
        default = cpp_constant("kDefaultStyle")
        self.assertEqual(default, kotlin_constant("DEFAULT_STYLE"))
        # A picker that cannot express the tier the service falls back to would
        # leave the user unable to return to standard haptics.
        self.assertIn(default, offered_values())
        self.assertIn(
            f'android:defaultValue="{default}"',
            read(APP / "res/xml/vibrator_style_settings.xml"),
        )

    def test_the_write_is_permitted_by_policy_end_to_end(self) -> None:
        # Three separate legs; dropping any one of them makes the write fail at
        # runtime while the UI still appears to work.
        self.assertIn(
            'android:sharedUserId="android.uid.system"',
            read(APP / "AndroidManifest.xml"),
            "the picker must run in the domain the property grant targets",
        )
        self.assertIn(
            f"system_public_prop({PROPERTY_TYPE})", read(SEPOLICY / "public/property.te")
        )
        self.assertIn(
            f"set_prop(system_app, {PROPERTY_TYPE})",
            read(SEPOLICY / "vendor/system_app.te"),
        )
        self.assertIn(
            PROPERTY_TYPE, read(SEPOLICY / "private/property_contexts")
        )

    def test_selection_applies_without_restarting_the_service(self) -> None:
        # All shipped tiers are resident, so honouring a new selection is a
        # re-point. Requiring a service restart would make the picker look
        # broken to the user on first use.
        source = read(LOADER_SOURCE)
        self.assertIn("__system_property_serial", source)
        lookup = source.index("VibrationEffectLoader::getEffectStream")
        body = source[lookup : lookup + 800]
        self.assertIn("refreshActiveStyle();", body)
        self.assertIn("std::lock_guard", body)

    def test_app_is_packaged_as_a_platform_signed_system_ext_surface(self) -> None:
        blueprint = read(APP / "Android.bp")
        for required in (
            'name: "OplusVibratorStyle"',
            'certificate: "platform"',
            "system_ext_specific: true",
            "privileged: true",
        ):
            self.assertIn(required, blueprint)


if __name__ == "__main__":
    _ = unittest.main()
