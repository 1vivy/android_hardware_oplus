# SPDX-FileCopyrightText: 2026 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#
# A vendor AIDL instance needs THREE things from this repository before it can
# exist under enforcing policy: a service type, a service_contexts label binding
# the instance name to that type, and an add_service grant for the domain that
# actually registers it. Declaring the instance in VINTF is not one of them, and
# a build can ship the provider, the data and the VINTF fragment and still be
# unable to register the service.
#
# That is not hypothetical. Measured 2026-08-16 on stock OOS (Macanc, SM8850,
# enforcing): the composer process has libPanelChaplin.so and the MixLut3D NDK
# library mapped, no other process holds them, and our own packaged
# odm/lib64/libPanelChaplin.so exports AServiceManager_addService together with
# the IMixLut3D descriptor. So libPanelChaplin IS the MixLut3D provider and it
# registers from inside the composer. Our image shipped the provider, the STC
# config that loads it, the LUT data and the VINTF fragment - and had no type,
# no label and no grant, so the instance could never appear.

import re
import unittest
from pathlib import Path

SEPOLICY = Path(__file__).resolve().parents[1] / "qti/vendor"

# Instances registered from the composer process. Each needs all three legs.
COMPOSER_REGISTERED = {
    "vendor.oplus.hardware.cwb.ICwbService/default": "hal_oplus_cwb_aidl_service",
    "vendor.oplus.hardware.display.MixLut3D.IMixLut3D/default": (
        "hal_oplus_displaymixlut3d_aidl_service"
    ),
    "vendor.oplus.hardware.displaycolorfeature.IDisplayColorFeature/default": (
        "hal_oplus_displaycolor_aidl_service"
    ),
    "vendor.pixelworks.hardware.display.IIris/default": "hal_display_iris_aidl_service",
}

# Registered by our own displaypanelfeature server, NOT by the composer. It is
# here so the composer grant list is asserted to be the set it should be rather
# than merely non-empty: granting the composer this one would be wrong.
SELF_REGISTERED = {
    "vendor.oplus.hardware.displaypanelfeature.IDisplayPanelFeature/default": (
        "hal_oplus_displaypanel_aidl_service"
    ),
}


def declared_types() -> set[str]:
    text = (SEPOLICY / "service.te").read_text(encoding="utf-8")
    return set(re.findall(r"^type\s+(\w+)\s*,", text, flags=re.MULTILINE))


def labelled_instances() -> dict[str, str]:
    text = (SEPOLICY / "service_contexts").read_text(encoding="utf-8")
    found: dict[str, str] = {}
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        instance, context = stripped.split()
        found[instance] = context.split(":")[2]
    return found


def composer_grants() -> set[str]:
    text = (SEPOLICY / "hal_graphics_composer_default.te").read_text(encoding="utf-8")
    return set(
        re.findall(
            r"^add_service\(hal_graphics_composer_default,\s*(\w+)\)",
            text,
            flags=re.MULTILINE,
        )
    )


class DisplayServiceLabelTest(unittest.TestCase):
    def test_every_composer_registered_instance_has_type_label_and_grant(self) -> None:
        types, labels, grants = declared_types(), labelled_instances(), composer_grants()

        for instance, expected_type in COMPOSER_REGISTERED.items():
            with self.subTest(instance=instance):
                self.assertIn(expected_type, types, "no type in service.te")
                self.assertEqual(
                    labels.get(instance), expected_type, "no service_contexts label"
                )
                self.assertIn(expected_type, grants, "composer cannot add_service")

    def test_self_registered_instance_is_labelled_but_not_granted_to_composer(
        self,
    ) -> None:
        # The case that must NOT fail the check above: displaypanelfeature is
        # labelled and typed, yet deliberately absent from the composer grants
        # because its own server registers it.
        types, labels, grants = declared_types(), labelled_instances(), composer_grants()

        for instance, expected_type in SELF_REGISTERED.items():
            with self.subTest(instance=instance):
                self.assertIn(expected_type, types)
                self.assertEqual(labels.get(instance), expected_type)
                self.assertNotIn(expected_type, grants)

    def test_every_label_references_a_declared_type(self) -> None:
        types = declared_types()
        # Labels may point at platform types this repository does not define
        # (hal_camera_service, hal_radio_service); only our own are checked.
        ours = {
            context
            for context in labelled_instances().values()
            if context.startswith(("hal_oplus_", "hal_display_", "hal_subsys"))
        }

        self.assertTrue(ours, "probe matched no labels; it would not detect a gap")
        self.assertEqual(ours - types, set(), "label references an undeclared type")


if __name__ == "__main__":
    _ = unittest.main()
