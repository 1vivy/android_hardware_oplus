# SPDX-FileCopyrightText: 2026 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0

"""A public policy type we add must be mapped in the newest compat ignore CIL.

Generation 32 failed in NINJA at 408/908 with six treble_sepolicy_tests_*
targets refusing:

    SELinux: The following public types were found added to the policy without
    an entry into the compatibility mapping file(s) found in
    private/compat/V.v/V.v[.ignore].cil ...
    system_oplus_vibrator_prop

The type was declared correctly and labelled correctly; the compat mapping is a
separate obligation that nothing local checked, so a full governed build was
spent discovering it. This locks the coupling: declaring a public type without
mapping it fails here, in seconds, on the host.

It asserts the PROPERTY (every declared public type is mapped), not a count, so
adding a correctly-mapped type passes and only an unmapped one fails.
"""

import re
import unittest
from pathlib import Path

SEPOLICY = Path(__file__).resolve().parents[1] / "qti"
PUBLIC = SEPOLICY / "public" / "property.te"
COMPAT = SEPOLICY / "private" / "compat"

DECLARATION = re.compile(
    r"^\s*(?:system_public_prop|system_vendor_config_prop|system_restricted_prop)"
    r"\(\s*([A-Za-z0-9_]+)\s*\)",
    re.MULTILINE,
)


def latest_ignore_cil() -> Path:
    levels = sorted(p for p in COMPAT.iterdir() if p.is_dir())
    assert levels, f"no compat API levels under {COMPAT}"
    newest = levels[-1]
    ignore = newest / f"{newest.name}.ignore.cil"
    assert ignore.is_file(), f"missing compat ignore CIL: {ignore}"
    return ignore


class PublicTypeCompatTest(unittest.TestCase):
    def setUp(self) -> None:
        self.declared = set(DECLARATION.findall(PUBLIC.read_text(encoding="utf-8")))
        self.mapped = set(
            re.findall(r"[A-Za-z0-9_]+", latest_ignore_cil().read_text(encoding="utf-8"))
        )

    def test_probe_reads_real_declarations_and_a_real_mapping(self) -> None:
        # Positive control: without it, an empty parse would pass everything.
        self.assertIn("system_oplus_touch_prop", self.declared)
        self.assertIn("system_oplus_touch_prop", self.mapped)

    def test_every_public_type_we_declare_is_mapped_for_the_newest_api_level(self) -> None:
        unmapped = sorted(self.declared - self.mapped)
        self.assertEqual(
            unmapped,
            [],
            "public types declared without a compat mapping; treble_sepolicy_tests "
            f"will refuse the build: {unmapped}",
        )


if __name__ == "__main__":
    _ = unittest.main()
