/*
 * Copyright (C) 2024 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.oplus.ims.stub;

import android.content.Context;
import android.util.Log;

import com.oplus.ims.IImsExt;

import java.util.concurrent.Executor;

public class ImsServiceControllerExt {
    private static final String LOG_TAG = "ImsServiceControllerExt";
    private static ImsServiceControllerExt sInstance;

    private final ImsExtStub mImsExtStub;

    protected ImsServiceControllerExt(Context context, Executor executor) {
        mImsExtStub = new ImsExtStub();
    }

    public static synchronized ImsServiceControllerExt make(Context context, Executor executor) {
        if (sInstance == null) {
            sInstance = new ImsServiceControllerExt(context, executor);
        }
        return sInstance;
    }

    public static ImsServiceControllerExt getInstance() {
        if (sInstance == null) {
            Log.e(LOG_TAG, "getInstance before make()");
        }
        return sInstance;
    }

    public IImsExt getIImsExt() {
        return mImsExtStub;
    }

    public static class ImsExtStub extends IImsExt.Stub {}
}
