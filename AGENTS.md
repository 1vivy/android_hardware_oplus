# android_hardware_oplus — agent carrier

<!-- rom-ops:carrier
project = "android_hardware_oplus"
seeded_at_ref = "crdroid/16.0"
seeded_at_oid = "76d4d0c3c50775edb3cb203874e1aa8270f5e72d"
date = "2026-08-15"
-->

## Upstream distillation

### Role and ownership map

This repository is the OPLUS device-support and vendor-HAL integration layer. It owns source-built OPLUS HAL adapters, frozen vendor interface descriptions, framework compatibility stubs, device-facing utilities, optional LineageOS/crDroid features, overlays, and the SELinux/VINTF declarations that let those pieces coexist with stock vendor services.

- `aidl/` contains service implementations: OPLUS `commondcs`, `osense.client`, and `performance`; Android `ir` and `vibrator`; and LineageOS `livedisplay`, `powershare`, and `touch`. Each service's `Android.bp`, init `.rc`, and VINTF fragment form one deployment unit.
- `interfaces/` owns stable OPLUS AIDL source under `interfaces/aidl/`, frozen API snapshots under `interfaces/aidl_api/`, and the three source HIDL packages under `interfaces/oplus/hardware/`. `interfaces/update-makefiles.sh` is the HIDL regeneration entry point.
- `hidl/fingerprint/` implements the Android fingerprint 2.3 service over OPLUS fingerprint 2.1; `fingerprint/` supplies SurfaceFlinger UDFPS support and AIDL/HIDL compatibility shims.
- `audio_amplifier/`, `power/`, and `sensors/` are integration libraries rather than standalone vendor contracts: `audio_amplifier.qcom`, `power-ext-oplus`, `sensors.oplus`, and `sensors.ssc_custom_flag` plug into Qualcomm/Android host HALs.
- `kernel-headers/` exports `kernel_headers.oplus` (`oplus_display_panel.h`, `oplus_ir_core.h`). `folio-daemon/`, `nrmodeswitcher/`, `wvmkiller/`, `uah-client/`, and `usb/` provide cover-state, NR-mode, DRM cleanup, UAH client, and USB-composition support.
- `oplus-fwk/` builds the installable boot-jar compatibility library `oplus-fwk`; its Java/AIDL stubs and wrappers satisfy OPLUS application/framework ABI expectations. `oplus-fwk/oplus-fwk.mk` adds it to `PRODUCT_PACKAGES` and `PRODUCT_BOOT_JARS`.
- `packages/` owns `OplusDoze`, `OplusEuicc`/`EuiccPolicyOplus`, `IFAAService`, `KeyHandler`, `OplusPen`, and `OplusEsimSwitcher`. `ltpo/` owns the crDroid system-ext app `OplusLtpo`.
- `overlay/generic/` and `overlay/qssi/` own framework, SettingsProvider, SystemUI, telephony, and Wi-Fi RROs. Their `generic.mk` and `qssi.mk` add package sets and Soong namespaces.
- `sepolicy/qti/` and `vintf/device_framework_matrix.xml` are compatibility ownership, not ancillary documentation: they label vendor binaries/devices/services and admit optional stock OPLUS, Pixelworks, and ZEKU contracts.

### Interface surfaces

`interfaces/Android.bp` declares these stable AIDL modules (the interface types are named exactly as their source `.aidl` files):

- `vendor.oplus.hardware.appradioaidl` V1: `IOplusAppRadio`, `IOplusAppRadioIndication`, `IOplusAppRadioResponse`.
- `vendor.oplus.hardware.biometrics.fingerprintpay` V1: `IFingerprintPay`.
- `vendor.oplus.hardware.camera.aon` V1: `IAONService`, `IAONServiceCallBack`.
- `vendor.oplus.hardware.camera_rfi` V1-V3: `ICameraRfiService`, `ICameraRfiCallback`.
- `vendor.oplus.hardware.cameraextension` V1: `ICameraExtensionService`.
- `vendor.oplus.hardware.cammidasservice` V1: `IMIDASService`.
- `vendor.oplus.hardware.charger` frozen V1-V3 and V5-V11: `ICharger`.
- `vendor.oplus.hardware.commondcs` V1: `ICommonDcsAidlHalService`.
- `vendor.oplus.hardware.communicationcenter` V1-V2: `IDmtpService`, `IDmtpClient`.
- `vendor.oplus.hardware.cwb` V1-V2: `ICwbService`, `ICwbCallback`.
- `vendor.oplus.hardware.displaycolorfeature` V1: `IDisplayColorFeature`.
- `vendor.oplus.hardware.display.MixLut3D` V1-V2: `IMixLut3D`.
- `vendor.oplus.hardware.displaypanelfeature` V1: `IDisplayPanelFeature`.
- `vendor.oplus.hardware.ims` V1: `IOplusImsRadio`, `IOplusImsRadioIndication`, `IOplusImsRadioResponse`.
- `vendor.oplus.hardware.esim` V1: `IOplusEsim`.
- `vendor.oplus.hardware.olc2` V1-V3: `IOplusLogCore`, `IOplusLogCoreEventCallback`, `IGaiaEventListener`.
- `vendor.oplus.hardware.osense.client` V1: `IOsenseAidlHalReporter`.
- `vendor.oplus.hardware.performance` V1: `IPerformance`.
- `vendor.oplus.hardware.pogo_keyboard` V1: `IPogoKeyboard`.
- `vendor.oplus.hardware.radio` V1-V2: `IOplusRadio`, `IOplusRadioIndication`, `IOplusRadioResponse`, `IOplusCarrierIdentifierIndication`, `IOplusCarrierIdentifierResponse`.
- `vendor.oplus.hardware.secure_element` V1: `ISecureElement`.
- `vendor.oplus.hardware.sendextcamcmd` V1-V2: `ISendExtCamCmdService`.
- `vendor.oplus.hardware.stability.oplus_project` V1-V3: `IOplusProject`.
- `vendor.oplus.hardware.subsys_imsrtp` V1: `IImsRtp`, `IImsRtpIndication`, `IImsRtpResponse`.
- Module `vendor.oplus.hardware.subsys` V1-V5 sources package `vendor.oplus.hardware.subsys_interface.subsys`: `ISubsys`, `ISubsysIndication`, `ISubsysResponse`, `ISubsysIccMetaCallback`, `IImsOrtcIndication`, `IImsOrtcResponse`.
- Module `vendor.oplus.hardware.subsys_radio` V1-V9 sources package `vendor.oplus.hardware.subsys_interface.subsys_radio`: `ISubsysRadio`, `ISubsysRadioIndication`, `ISubsysRadioResponse`, `IUimAuthenticateCallback`.
- `vendor.oplus.hardware.touch` V1-V2: `IOplusTouch`.
- `vendor.oplus.hardware.urcc` V1: `IUrcc`, `IUrccCallback`.

The same build file supplies `-ndk_platform` compatibility aliases for selected frozen libraries. Parcelable, enum, and union companions live beside each interface and in matching `aidl_api/<module>/<version>/` snapshots; changing a stable contract requires updating the source and frozen API together.

Source HIDL packages are `vendor.oplus.hardware.biometrics.fingerprint@2.1` (`IBiometricsFingerprint`, `IBiometricsFingerprintClientCallbackEx`), `vendor.oplus.hardware.cwb@1.0` (`ICwbService`, `ICwbCallback`, `types.hal`), and `vendor.oplus.hardware.displaypanelfeature@1.0` (`IDisplayPanelFeature`). Root `Android.bp` also declares prebuilt HIDL surfaces `vendor.oplus.hardware.cameraextension@1.0::ICameraExtensionService`, `vendor.pixelworks.hardware.feature@1.0::IIrisFeature`, and `vendor.zeku.hardware.explorer@1.0::IMmsProvider`.

Implemented service contracts are:

- `vendor.oplus.hardware.commondcs-service` -> `vendor.oplus.hardware.commondcs.ICommonDcsAidlHalService/default`.
- `vendor.oplus.hardware.osense.client-service` -> `vendor.oplus.hardware.osense.client.IOsenseAidlHalReporter/default`.
- `vendor.oplus.hardware.performance-service` -> `vendor.oplus.hardware.performance.IPerformance/default`.
- `android.hardware.ir-service.oplus` -> `android.hardware.ir.IConsumerIr/default`.
- `vendor.qti.hardware.vibrator.service.oplus` and `android.hardware.vibrator.service.oplus-richtap` -> `android.hardware.vibrator.IVibrator/default` (alternative implementations, not co-installed providers).
- `vendor.lineage.livedisplay-service.oplus` -> optional `vendor.lineage.livedisplay.IAdaptiveBacklight`, `IAntiFlicker`, `IDisplayModes`, `IPictureAdjustment`, and `ISunlightEnhancement`, all `/default`.
- `vendor.lineage.powershare-service.oplus` -> `vendor.lineage.powershare.IPowerShare/default`.
- `vendor.lineage.touch-service.oplus` -> optional `vendor.lineage.touch.IGloveMode`, `IHighTouchPollingRate`, and `ITouchscreenGesture`, all `/default`; it can consume stock `vendor.oplus.hardware.touch.IOplusTouch/default`.
- `android.hardware.biometrics.fingerprint@2.3-service.oplus` implements Android HIDL `IBiometricsFingerprint@2.3/default` while consuming the OPLUS 2.1 extension.

`vintf/device_framework_matrix.xml` additionally admits optional vendor-provided AIDL/HIDL instances that this repository does not implement, including OPLUS app-radio, camera, charger, communication, CWB, display, eSIM, IMS, sensor, radio/satellite, secure-element, subsystem, touch and URCC families, plus Pixelworks display/feature and ZEKU explorer. Keep this matrix synchronized with stock vendor manifests; it intentionally contains compatibility names such as `IAppRadioStable`, `IImsStable`, and `IRadioStable` that differ from the source AIDL type names.

### Build graph and extension points

- Root `Android.bp` establishes the Soong namespace and prebuilt HIDL declarations. Every native/app subtree has its own `Android.bp`; `interfaces/Android.bp` is the stable-AIDL hub and `kernel-headers/Android.bp` exports device UAPI.
- Make entry points are only `oplus-fwk/oplus-fwk.mk`, `overlay/generic/generic.mk`, `overlay/qssi/qssi.mk`, and `sepolicy/qti/SEPolicy.mk`; product/device trees opt into this repository through those files and `PRODUCT_SOONG_NAMESPACES`/`PRODUCT_PACKAGES`.
- The established LineageOS extension pattern is a small vendor adapter around shared Lineage interfaces, selected by Soong config and paired with conditional VINTF fragments. `OPLUS_LINEAGE_LIVEDISPLAY_HAL` controls `ENABLE_AB`, `ENABLE_AF`, `ENABLE_DM`, `ENABLE_PA`, and `ENABLE_SE`; defaults enable picture adjustment and sunlight enhancement. Adaptive backlight, anti-flicker, and sunlight use `/dev/oplus_display`; display modes/picture adjustment use shared SDM components.
- `OPLUS_LINEAGE_TOUCH_HAL` controls glove mode, high-touch polling, touchscreen gestures, a device include path, and whether to bridge through stock `IOplusTouch`; `OPLUS_LINEAGE_VIBRATOR_HAL` supplies a device include path and optional effect streaming. These are the precedents for device variation: Soong variables plus shared source, not copied per-device implementations.
- Lineage-facing apps use `org.lineageos.*` packages and `org.lineageos.settings.resources`; `OplusLtpo` is the existing crDroid-specific extension (`com.crdroid.ltpo.oplus`) and is a platform-signed privileged system-ext settings surface.
- `nrmodeswitcher` demonstrates consuming `vendor.oplus.hardware.radio-V2-ndk`; `power-ext-oplus` demonstrates a host HAL extension consuming `vendor.oplus.hardware.touch-V2-ndk`; `IFAAService` and `OplusEsimSwitcher` demonstrate Java clients of frozen vendor AIDL.

### SELinux and VINTF touchpoints

`sepolicy/qti/SEPolicy.mk` derives the Qualcomm platform suffix from `SEPOLICY_PATH`, adds `vendor/` to `BOARD_VENDOR_SEPOLICY_DIRS`, `private/` and `public/` to system-ext policy, includes platform-specific directories, adds `common-um` except on `legacy-um`, and includes Lineage `libperfmgr` policy.

- `public/attributes`, `public/type.te`, `public/service.te`, `public/property.te`, and `public/te_macros` export cross-partition HAL attributes/types. `private/` owns system/system-ext app and service policy, service/property/file contexts, and compatibility CIL. `vendor/` owns HAL domains, daemon policy, service/hwservice/property/file/genfs contexts, ioctl definitions, and `legacy-um`, `common-um`, `sm8450`, and `sm8550` variants.
- For display specifically, `vendor/file_contexts` labels `/dev/oplus_display`, the Lineage LiveDisplay binary, and the stock display-panel-feature binary. `vendor/service_contexts` labels `IDisplayColorFeature/default` and `IDisplayPanelFeature/default`; `public/attributes` exports `vendor_hal_oplus_displaypanelfeature`; `hal_lineage_livedisplay_qti.te` grants the graphics-device access; and `hal_oplus_displaypanelfeature_aidl.te` defines/confines the stock AIDL server and service ownership.
- Other policy is similarly organized by real producer/consumer (`hal_oplus_*`, fingerprint, radio/subsystem, touch daemon, vibrator, sensors, camera, charger, apps/daemons). Add a service label, executable/device label, domain transition, and least-privilege access together rather than solving denials in an unrelated domain.

### Upstream commit conventions

The pinned log uses short, imperative subjects with a subsystem prefix: `oplus-fwk: Add ...`, `interfaces: Introduce ...`, `sepolicy: Address ...`, `overlay: qssi: Update ...`, or a component such as `KeyHandler:`. Nested scope is common (`sepolicy: qti:`); interface changes explicitly say `Introduce`, `Add`, `Update`, or `Bump` and name the contract/version. Generated localization commits use the exact subject `Automatic translation import`. Source headers predominantly use SPDX identifiers and LineageOS/crDroid copyright attribution; preserve the local file's established header style.

## Our deltas

Against `76d4d0c3c50775edb3cb203874e1aa8270f5e72d`, topic HEAD `e76be9e8088b78d7ded54b7826dfb68d8d8283db` changes 324 tracked paths (18,364 insertions, 349 deletions):

- Display is re-architected around a new typed `displaypanelfeature/` layer. It adds `liboplus_displaypanelfeature_config`, `liboplus_displaypanelfeature_client`, `liboplus_displaypanelfeature_aidl_transport`, `displaypanelfeature-publisher`, and `adfr-floor-daemon`, an Infiniti XML registry/config, AIDL transport, property/sysfs publication, ADFR floor policy, host tests, init services, and corresponding SELinux domains.
- `aidl/livedisplay/` adds DC-dimming support and `PanelFeature` adapters, links the typed display-panel client/transport, routes adaptive backlight, anti-flicker, display modes, sunlight enhancement, and DC dimming through that contract, and degrades unavailable SDM picture adjustment instead of aborting the whole service. `ltpo/` becomes a Java client of `vendor.oplus.hardware.displaypanelfeature-V1-java`, gains masked per-device ADFR configuration and an AOSP active-mode bridge, and adds a host test.
- New display-adjacent support comprises `oplus_ulp_aod`, recovery `hbp-setup`, expanded OPLUS display/HBP kernel headers, and the `android.hardware.health-service.oplus` battery-health provider with device-selected `oplus_chg` telemetry paths. Folio handling gains parsed magnetic-cover state. All receive matching file/property/genfs/service labels and confined domains.
- Non-display additions include QTI vibrator effect-stream wiring, a read-only OPLUS subsystem-radio probe with tests, Plus Key short/long actions, OnePlus 15 eSIM resources, extra framework-matrix FIDO/CryptoEng declarations, and extensive `oplus-fwk` ABI/runtime expansion (camera/media, access control, radio/IMS adapters, framework wrappers, crypto, UI, and compatibility classes).
- `interfaces/Android.bp` enables Java backends for app-radio, IMS, and display-panel-feature; `kernel_headers.oplus` becomes vendor- and recovery-available. Existing powershare init ordering and broad OPLUS/QTI policy are adjusted for the new consumers and devices.

## Known defects

### DEF-DPF-01 — LiveDisplay still opens the raw panel device
After the DPF collapse, `aidl/livedisplay/DcDimming.cpp` still accounted for one direct `/dev/oplus_display` open. Done means routing DC dimming through DPF features 23/199/201 and measuring zero raw opens below `aidl/livedisplay/`. See `docs/history/DEFECTS-2026-08.md` for the full investigation.

### DEF-DPF-03 — `aod/UltraLowPowerAod.cpp` is a second direct panel writer
Observed 2026-08-15 while landing the open DPF server. `aod/UltraLowPowerAod.cpp` declares
`kPanelDevice = "/dev/oplus_display"` and issues `PANEL_IOCTL_GET/SET_ULTRA_LOW_POWER_AOD`
and `PANEL_IOCTL_SET_LOW_PWM_AOD` directly, so the panel has two writers even after the DPF
collapse — the condition DEF-DPF-01 closed for `aidl/livedisplay/` still holds here. It
collides concretely with the new server: `IoctlPanelBackend` binds `LowPwmAod`, and the
generated registry carries a `LowPwmAod` row, so both paths can drive that same ioctl.

NOT fixed in that session, deliberately. The generated map names `LowPwmAod` but names no
row for `PANEL_IOCTL_*_ULTRA_LOW_POWER_AOD`, so routing the whole file through DPF would
require INVENTING a feature id, and routing only `LowPwmAod` would split one file across two
writer paths — strictly worse than one honest direct writer. Done means either the RE names
the ULP AOD route so the whole file moves behind the typed client, or the file is declared
the owner of that ioctl and the DPF `LowPwmAod` binding is withdrawn so exactly one writer
remains. Verify with: zero `/dev/oplus_display` opens outside `displaypanelfeature/`, and no
kernel request reachable from both a registry row and a direct caller.

### DEF-EUICC-01 — competing eUICC surfaces can drive a phone-process bind loop
A flashed build emitted 3,822 `EuiccConnector` lines in a 4,000-line window while `com.android.phone` used 74.9% CPU and thermal status reached severe; the artifact packaged several eUICC surfaces at once. Done means the ROM composition selects one stable provider/policy path and a sustained device run shows no bind loop. See `docs/history/DEFECTS-2026-08.md` for the full investigation.

### DEF-DPF-02 — missing DPF Binder permission made LiveDisplay unavailable
Under enforcing policy, `hal_lineage_livedisplay_qti` was denied Binder calls to `hal_oplus_displaypanelfeature_aidl`, producing an empty descriptor and dead features 4/12/23/199/201. Done means the explicit client/server `binder_call` pair is present and an enforcing restart yields no denial, empty descriptor, or unavailable-feature signature. See `docs/history/DEFECTS-2026-08.md` for the full investigation.

### DEF-COL-01 — colour profile writes do not produce a panel change
The complete UI-to-DPF call path reaches feature 4, but the ODM service returns `result=-1`; gen 8 still showed no colour-mode change. Done means selecting a profile under enforcing policy produces a measured seed or LiveDisplay-mode delta joined to a framebuffer change, not merely a successful Binder call. See `docs/history/DEFECTS-2026-08.md` for the full investigation.

### DEF-EDR-01 — stock camera preview is overexposed
The camera preview misapplies EDR/HDR headroom while Gallery ProXDR renders the reference correctly; idle HDR capability itself is present. Done requires deterministic headroom/state movement with camera requests and physical preview validation against the Gallery reference. See `docs/history/DEFECTS-2026-08.md` for the full investigation.

### DEF-AOD-01 — AOD refresh floor remains at 60 Hz
The shipped path lacks a complete AOD policy/consumer chain and does not reach the 1 Hz floor achieved by stock. Done means one automated sleep/wake cycle records the expected DPF power edges and physically measures `test_te` below 60 in AOD and rising after wake. See `docs/history/DEFECTS-2026-08.md` for the full investigation.

## Owner rulings

### Use open adapters and only named compatibility re-stubs (2026-08-04)
Device capability belongs behind AOSP/Lineage/crDroid interfaces; OPLUS framework stubs are allowed only as a separate compatibility layer for a named OOS consumer. Vendor-only flags are not a product API. See `docs/history/DIRECTIVES-2026-08.md`.

### Become the DPF service (2026-08-12)
This repository must implement the centralized display-panel-feature service and own the panel ioctl/sysfs path; the ODM service is RE corpus, not a runtime dependency. OPLUS configuration is parsed into derived mappings rather than copied into a hand-maintained ID registry. See `docs/history/DIRECTIVES-2026-08.md`.

### Our vibrator remains the principal owner (2026-08-13)
The standard vibrator implementation is the sole principal writer. The OPLUS linear-motor surface is only a compatibility re-stub for named ICP camera/gallery consumers, never a second owner or product API precedent. See `docs/history/DIRECTIVES-2026-08.md`.

### Our performance stack has precedence (2026-08-13)
Retain our thermal/performance ownership and re-author useful OPLUS techniques on that surface; do not restore the OOS ROM-layer thermal or performance stack. ADPF, Power HAL hint sessions, and QTI Thermal remain actuators rather than competing policy owners. See `docs/history/DIRECTIVES-2026-08.md`.
