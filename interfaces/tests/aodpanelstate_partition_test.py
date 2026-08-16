# SPDX-License-Identifier: Apache-2.0

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def module_block(path: Path, name: str) -> str:
    text = path.read_text(encoding="utf-8")
    marker = f'name: "{name}"'
    name_offset = text.index(marker)
    start = text.rfind("{", 0, name_offset)
    depth = 0
    for offset in range(start, len(text)):
        if text[offset] == "{":
            depth += 1
        elif text[offset] == "}":
            depth -= 1
            if depth == 0:
                return text[start : offset + 1]
    raise AssertionError(f"unterminated module {name}")


class AodPanelStatePartitionTest(unittest.TestCase):
    def test_interface_is_platform_generic_and_service_is_system_ext(self) -> None:
        interface = module_block(
            ROOT / "interfaces/Android.bp",
            "vendor.oplus.hardware.aodpanelstate",
        )
        service = module_block(
            ROOT / "aod/Android.bp",
            "oplus_aod_panel_state",
        )

        self.assertNotIn("system_ext_specific: true", interface)
        self.assertIn("platform_apis: true", interface)
        self.assertNotIn('sdk_version: "module_current"', interface)
        self.assertIn("system_ext_specific: true", service)


if __name__ == "__main__":
    _ = unittest.main()
