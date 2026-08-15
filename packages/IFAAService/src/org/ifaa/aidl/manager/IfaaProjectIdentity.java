/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package org.ifaa.aidl.manager;

public final class IfaaProjectIdentity {
  private static final String PROJECT_24831 = "24831";
  private static final String MODEL_24831 = "ONEPLUS-R24831";

  private IfaaProjectIdentity() {}

  public static String resolve(String project, String model) {
    if (PROJECT_24831.equals(project) && MODEL_24831.equals(model)) {
      return MODEL_24831;
    }
    return null;
  }
}
