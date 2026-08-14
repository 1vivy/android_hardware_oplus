# PROJECT KNOWLEDGE BASE
**Project:** android_hardware_oplus
**Seeded-at-ref:** crdroid/16.0
**Seeded-at-oid:** 76d4d0c3c50775edb3cb203874e1aa8270f5e72d
**Seeded-at-date:** 2026-08-14
**Generated-policy-sha256:** 831fceec497e89cad3d18f57f71d7d9fbc2bf2498062efbc079821f326bd17f0

## UPSTREAM DISTILLATION

### Scope and ownership

This project is the open Oplus hardware and compatibility layer. It contains Lineage HAL implementations, Oplus interface declarations, framework shims, device apps, overlays, native daemons, and QTI/Oplus sepolicy. Device trees select and configure these modules; proprietary repositories supply measured backend blobs where an open implementation still delegates to one.

### Module map

- `aidl/`: Lineage-facing commondcs, IR, LiveDisplay, Osense, performance, PowerShare, touch, and vibrator service implementations. Each service has a local `Android.bp` and native `service.cpp` entry point.
- `interfaces/`: Oplus HIDL/AIDL definitions and frozen `aidl_api` snapshots. Update API snapshots with the interface tooling; do not hand-edit frozen API output independently of the source declaration.
- `fingerprint/` and `hidl/fingerprint/`: UDFPS extension and compatibility shims. `sensors/`: Sensors sub-HAL. `power/`: Oplus power-mode adapter.
- `ltpo/`: framework/app-side LTPO integration. `oplus-fwk/`: Oplus framework compatibility classes and product makefile.
- `packages/`: Doze, eSIM/Euicc, IFAA, KeyHandler, and Pen applications.
- `nrmodeswitcher/`, `folio-daemon/`, `wvmkiller/`, `uah-client/`, `usb/`, and `audio_amplifier/`: focused native services or configuration surfaces.
- `overlay/`: generic and QSSI resource overlays. `kernel-headers/`: imported UAPI needed by native implementations.

### Build and interface entry points

Root `Android.bp` exports the namespace and names prebuilt compatibility interfaces. Subsystems own their own build files. AIDL service implementations register from their `service.cpp`; interface source and frozen API live under `interfaces/`. The root `README.md` documents supported typed Soong variables for LiveDisplay, touch, and vibrator. Prefer these variables over product properties or device-specific forks of implementation code.

### SePolicy and product integration

`sepolicy/qti/public`, `private`, and `vendor` form one policy surface; `SEPolicy.mk` wires it into products. New services require the complete join: build module, init rc, VINTF where applicable, service or hwservice context, domain/type policy, device package selection, and tests. Keep policy with the narrowest owning implementation and derive labels from real service names.

### Lineage/crDroid extension precedents

The preferred boundary is a standard or `vendor.lineage.*` typed HAL backed by a narrow Oplus adapter. Existing LiveDisplay, PowerShare, touch, vibrator, IR, and sensor implementations are precedents. Oplus compatibility APIs under `interfaces/` and `oplus-fwk/` are compatibility ingress, not justification for exposing raw feature IDs, packed values, sysfs paths, or vendor revisions to new open consumers.

### Conventions and verification

Subjects are subsystem-scoped (`livedisplay: ...`, `oplus-fwk: ...`) and use imperative descriptions. Preserve SPDX headers, local formatting, and API snapshot consistency. Search for the closest existing service and its build, rc, VINTF, context, domain, device-selection, and test path before adding a module. Verify the affected native/app tests and module graph; service registration, policy, and hardware behavior require their real integration surfaces and cannot be inferred from compilation alone.

## OUR DELTAS

None at seed. Later entries must name topic commit OIDs and must not rewrite upstream truth.
