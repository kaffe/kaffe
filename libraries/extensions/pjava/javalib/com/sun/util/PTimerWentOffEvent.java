/*
 * PTimerWentOffEvent.java
 *
 * PersonalJava API 1.1
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */

package com.sun.util;

import java.util.EventObject;

public class PTimerWentOffEvent
  extends EventObject {

private PTimerSpec spec;

public PTimerWentOffEvent(PTimer source, PTimerSpec spec) {
	super(source);
	this.spec = spec;
}

public PTimerSpec getTimerSpec() {
	return (spec);
}

}
