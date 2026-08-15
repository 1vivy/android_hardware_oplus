from __future__ import annotations

import csv
import importlib.util
import sys
import xml.etree.ElementTree as ET
from functools import cache
from pathlib import Path
from typing import Protocol, cast

import pytest

ROOT = Path(__file__).resolve().parents[1]
TOOL = ROOT / "tools" / "derive_writer_registry.py"
DISPATCH = ROOT / "configs" / "displaypanelfeature_dispatch_map.tsv"
WRITERS = ROOT / "configs" / "displaypanelfeature_writer_features.tsv"
TEMPLATE = ROOT / "configs" / "displaypanelfeature_infiniti.xml.in"
SHIPPED = ROOT / "configs" / "displaypanelfeature_infiniti.xml"
REPOSITORY = ROOT.parent
DIRECT_WRITER_CONSUMERS = (
    REPOSITORY / "hidl" / "fingerprint" / "BiometricsFingerprint.h",
    REPOSITORY / "hidl" / "fingerprint" / "BiometricsFingerprint.cpp",
    REPOSITORY / "aod" / "include" / "oplus" / "aod" / "UltraLowPowerAod.h",
    REPOSITORY / "aod" / "UltraLowPowerAod.cpp",
)


class DerivationTool(Protocol):
    DerivationError: type[ValueError]

    @staticmethod
    def derive_registry(template: Path, dispatch: Path, writers: Path, output: Path) -> None: ...


@cache
def _tool() -> DerivationTool:
    spec = importlib.util.spec_from_file_location("derive_writer_registry", TOOL)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return cast(DerivationTool, cast(object, module))


def _feature_rows(path: Path) -> dict[str, dict[str, str]]:
    root = ET.parse(path).getroot()
    return {row.attrib["name"]: row.attrib for row in root.findall("feature")}


def test_production_derivation_reproduces_the_shipped_registry(tmp_path: Path) -> None:
    output = tmp_path / "registry.xml"
    _tool().derive_registry(TEMPLATE, DISPATCH, WRITERS, output)

    assert output.read_bytes() == SHIPPED.read_bytes()
    rows = _feature_rows(output)
    assert {
        name: (
            row["id"],
            row["direction"],
            row["producer"],
            row["status"],
            row["min"],
            row["max"],
        )
        for name, row in rows.items()
        if name in {"DimlayerHbm", "FpPress", "UltraLowPowerAod", "LowPwmAod"}
    } == {
        "DimlayerHbm": ("22", "both", "typed-client", "active", "0", "1"),
        "FpPress": ("28", "set", "typed-client", "active", "0", "1"),
        "UltraLowPowerAod": ("195", "both", "typed-client", "active", "0", "1"),
        "LowPwmAod": ("263", "both", "typed-client", "active", "0", "1"),
    }


def test_writer_spec_names_handlers_but_contains_no_route_ids() -> None:
    with WRITERS.open(newline="", encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream, delimiter="\t"))

    assert rows
    assert "feature_id" not in rows[0]
    assert {row["name"] for row in rows} == {
        "DimlayerHbm",
        "FpPress",
        "UltraLowPowerAod",
        "LowPwmAod",
    }
    assert all(not any(character.isdigit() for character in row["name"]) for row in rows)


def test_missing_dispatch_handler_refuses_instead_of_guessing(
    tmp_path: Path,
) -> None:
    lines = DISPATCH.read_text(encoding="utf-8").splitlines()
    mutated = tmp_path / "dispatch.tsv"
    _ = mutated.write_text(
        "\n".join(line for line in lines if "SetDisplayPanelFPPress" not in line) + "\n",
        encoding="utf-8",
    )

    with pytest.raises(_tool().DerivationError, match="SetDisplayPanelFPPress"):
        _tool().derive_registry(TEMPLATE, mutated, WRITERS, tmp_path / "registry.xml")


def test_ambiguous_dispatch_handler_refuses_instead_of_picking_one(
    tmp_path: Path,
) -> None:
    lines = DISPATCH.read_text(encoding="utf-8").splitlines()
    header, first, *rest = lines
    mutated = tmp_path / "dispatch.tsv"
    _ = mutated.write_text(
        "\n".join((header, first, first, *rest)) + "\n", encoding="utf-8"
    )

    with pytest.raises(_tool().DerivationError, match="duplicate feature id"):
        _tool().derive_registry(TEMPLATE, mutated, WRITERS, tmp_path / "registry.xml")


def test_direct_consumers_have_no_panel_device_fallback_writer() -> None:
    banned = (
        "/dev/oplus_display",
        "PANEL_IOCTL_SET_DIMLAYER_HBM",
        "PANEL_IOCTL_SET_FP_PRESS",
        "PANEL_IOCTL_GET_ULTRA_LOW_POWER_AOD",
        "PANEL_IOCTL_SET_ULTRA_LOW_POWER_AOD",
        "PANEL_IOCTL_SET_LOW_PWM_AOD",
    )
    for consumer in DIRECT_WRITER_CONSUMERS:
        text = consumer.read_text(encoding="utf-8")
        assert all(token not in text for token in banned), consumer

    server = (ROOT / "service.cpp").read_text(encoding="utf-8")
    backend = (ROOT / "IoctlPanelBackend.cpp").read_text(encoding="utf-8")
    assert "/dev/oplus_display" in server
    assert all(token in backend for token in banned[1:])
