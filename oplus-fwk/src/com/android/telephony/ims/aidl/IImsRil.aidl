/*
 * Copyright (C) 2024 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package android.telephony.ims.aidl;

import android.os.Message;

import com.oplus.ims.IImsExt;

import android.telephony.ims.aidl.IImsRilInd;

interface IImsRil {
    void unRegisterIndication(IImsRilInd ind);
    void registerIndication(IImsRilInd ind);
    void commonReqToIms(int phoneId, int requestId, in Message reqComplete);
    IImsExt getIImsExtBinder();
    boolean switchToPhysicalIms();
    boolean switchToVirtualIms(int toSlot);
}
