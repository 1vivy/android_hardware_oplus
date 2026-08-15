/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package org.ifaa.aidl.manager;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

import org.junit.Test;

public class IfaaProjectIdentityTest {
  @Test
  public void exact24831IdentityIsAccepted() {
    assertEquals("ONEPLUS-R24831", IfaaProjectIdentity.resolve("24831", "ONEPLUS-R24831"));
  }

  @Test
  public void unsetIdentityIsRejected() {
    assertNull(IfaaProjectIdentity.resolve("", ""));
    assertNull(IfaaProjectIdentity.resolve("24831", ""));
    assertNull(IfaaProjectIdentity.resolve("", "ONEPLUS-R24831"));
  }

  @Test
  public void project24863IsRejectedWithoutIndependentIdentityProof() {
    assertNull(IfaaProjectIdentity.resolve("24863", "ONEPLUS-R24831"));
    assertNull(IfaaProjectIdentity.resolve("24863", "ONEPLUS-R24863"));
  }

  @Test
  public void unknownAndMismatchedProjectsAreRejected() {
    assertNull(IfaaProjectIdentity.resolve("99999", "ONEPLUS-R24831"));
    assertNull(IfaaProjectIdentity.resolve("24831", "ONEPLUS-R24863"));
    assertNull(IfaaProjectIdentity.resolve("24831-extra", "ONEPLUS-R24831"));
  }
}
