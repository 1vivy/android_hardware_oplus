package com.oplus.cust;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/*
 * Stub implementation — OplusCfgFilePolicy is the OnePlus/Oplus "cust" customization
 * config-file locator. On OOS it lives in oplus-framework.jar and maps a config name to
 * files under the OEM cust partitions (/my_product/cust, /my_company, ...), which are NOT
 * present on LineageOS. Returning empty lists / null makes consumers — e.g.
 * com.oplus.aiunit's AssetsUnitStore.listFilesFromOS, and many other OnePlus system apps —
 * find no cust configs and degrade gracefully, instead of dying with
 * NoClassDefFoundError: com.oplus.cust.OplusCfgFilePolicy on class resolution.
 *
 * Public API mirrored exactly from the OOS 16.0.8.300 oplus-framework.jar definition
 * (descriptors must match for the precompiled consumers to link).
 */
public class OplusCfgFilePolicy {
    public static final int DEFAULT_SLOT = -2;

    public static String getCarrierId(int slot) {
        return "";
    }

    public static List getCfgFileList(String configName, String dir, int slot) {
        return new ArrayList();
    }

    public static List getCfgLevelList(String configName, int slot) {
        return new ArrayList();
    }

    public static File getCfgTopPriorityFile(String configName, String dir, int slot) {
        return null;
    }
}
