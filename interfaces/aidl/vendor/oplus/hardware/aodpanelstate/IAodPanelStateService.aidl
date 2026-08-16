/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package vendor.oplus.hardware.aodpanelstate;

import vendor.oplus.hardware.aodpanelstate.AodPanelEdge;

@PermissionManuallyEnforced
interface IAodPanelStateService {
    boolean send(long generation, AodPanelEdge edge);
}
