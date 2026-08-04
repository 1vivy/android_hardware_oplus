/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.telephony.ims.aidl;

import android.os.Bundle;

oneway interface IImsRilInd {
    void onImsRilInd(int phoneId, int eventId, in Bundle bundle);
}
