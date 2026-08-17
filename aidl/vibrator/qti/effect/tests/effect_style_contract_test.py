# SPDX-License-Identifier: Apache-2.0
"""Contract for the haptic effect-style tier.

The shipped vibrator_effect.json declares more than one style tier and every
payload for both is installed (measured: def_style 88, soft_style 22, 110/110
files present). The loader used to read one tier and say so in a TODO, which
left 22 installed payloads permanently unopened.

What SHOULD fail this file: reverting the loader to a single hardcoded tier,
dropping the default-tier fallback, or removing any of the four SELinux rows
that let a system-side picker write the selection and the HAL read it.

What should NOT fail it: adding a third tier to the JSON, renaming a local
variable, or reflowing a comment. The assertions are on the selection
behaviour and the policy edges, not on incidental text.
"""

from __future__ import annotations

import os
import re
import unittest
from pathlib import Path

ROOT = Path(os.environ.get("EFFECT_STYLE_ROOT", Path(__file__).resolve().parents[5]))
EFFECT = ROOT / "aidl/vibrator/qti/effect"
LOADER_CPP = EFFECT / "VibrationEffectLoader.cpp"
LOADER_H = EFFECT / "VibrationEffectLoader.h"
SEPOLICY = ROOT / "sepolicy/qti"

PROPERTY = "persist.sys.oplus.vibrator.effect_style"
PROPERTY_TYPE = "system_oplus_vibrator_prop"
DEFAULT_STYLE = "def_style"


def read(path: Path) -> str:
    assert path.is_file(), f"missing contract file: {path}"
    return path.read_text(encoding="utf-8")


class EffectStyleContractTest(unittest.TestCase):
    def test_every_declared_tier_is_loaded_not_just_one(self) -> None:
        cpp = read(LOADER_CPP)
        # The tier set comes from the config's own member names, so a device
        # shipping a third tier needs no code change.
        self.assertIn("getMemberNames()", cpp)
        self.assertRegex(cpp, r"loadStyle\(\s*style\s*,")
        # The old shape selected one key and discarded the rest.
        self.assertNotIn("node = node[kKeyDefStyle]", cpp)
        self.assertNotIn("TODO: also load soft_style", cpp)

    def test_default_selection_preserves_previous_behaviour(self) -> None:
        header = read(LOADER_H)
        self.assertIn(f'kDefaultStyle = "{DEFAULT_STYLE}"', header)
        cpp = read(LOADER_CPP)
        # Active tier starts at the default before any property is consulted.
        self.assertRegex(cpp, r"VibrationEffectLoader\(\)\s*:\s*active_style_\(kDefaultStyle\)")
        # And an unset property resolves to the default.
        self.assertRegex(cpp, r"GetProperty\(\s*kStyleProperty\s*,\s*kDefaultStyle\s*\)")

    def test_absent_or_unknown_selection_cannot_silence_haptics(self) -> None:
        cpp = read(LOADER_CPP)
        # Unknown tier keeps the default rather than activating nothing.
        self.assertIn("styles_.count(requested)", cpp)
        self.assertIn("keeping ", cpp)
        # An id the active tier does not define resolves against the default
        # tier, so selecting the smaller tier loses no effect.
        fallback = cpp[cpp.index("effect_stream* VibrationEffectLoader::getEffectStream") :]
        self.assertIn("active_style_ != kDefaultStyle", fallback)
        self.assertIn("styles_.find(kDefaultStyle)", fallback)

    def test_selection_property_is_labelled_and_reaches_both_sides(self) -> None:
        # Declared once, as a system-writable/vendor-readable property.
        self.assertIn(
            f"system_public_prop({PROPERTY_TYPE})", read(SEPOLICY / "public/property.te")
        )
        # Labelled by prefix, so the exact key resolves to that type.
        contexts = read(SEPOLICY / "private/property_contexts")
        prefixes = [
            line.split()[0]
            for line in contexts.splitlines()
            if line.strip() and not line.startswith("#") and PROPERTY_TYPE in line
        ]
        self.assertTrue(
            any(PROPERTY.startswith(prefix) for prefix in prefixes),
            f"{PROPERTY} is not covered by any {PROPERTY_TYPE} prefix: {prefixes}",
        )
        # A system-side picker may write it; the HAL may read it.
        self.assertIn(
            f"set_prop(system_app, {PROPERTY_TYPE})", read(SEPOLICY / "vendor/system_app.te")
        )
        self.assertIn(
            f"get_prop(hal_vibrator_default, {PROPERTY_TYPE})",
            read(SEPOLICY / "vendor/hal_vibrator_default.te"),
        )

    def test_probe_would_have_matched_the_existing_display_precedent(self) -> None:
        # Positive control: the same parsing finds the display property that
        # already ships through this exact four-row shape. Without it, a typo
        # in the file layout would read as "rule absent" instead of failing.
        self.assertIn(
            "system_public_prop(system_displaypanel_publisher_prop)",
            read(SEPOLICY / "public/property.te"),
        )
        self.assertRegex(
            read(SEPOLICY / "private/property_contexts"),
            r"persist\.sys\.displaypanel\.\s+u:object_r:system_displaypanel_publisher_prop:s0",
        )


if __name__ == "__main__":
    unittest.main()
