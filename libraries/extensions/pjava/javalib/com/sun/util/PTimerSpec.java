/*
 * PTimerSpec
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

import java.util.Vector;
import java.util.Enumeration;

public class PTimerSpec {

protected Object listeners;

private boolean repeat;
private boolean regular;
private boolean absolute;
private long time;

public PTimerSpec() {
}

public void setAbsolute(boolean absolute) {
	this.absolute = absolute;
}

public boolean isAbsolute() {
	return (absolute);
}

public void setRepeat(boolean repeat) {
	this.repeat = repeat;
}

public boolean isRepeat() {
	return (repeat);
}

public void setRegular(boolean regular) {
	this.regular = regular;
}

public boolean isRegular() {
	return (regular);
}

public void setTime(long time) {
	this.time = time;
}

public long getTime() {
	return (time);
}

public void addPTimerWentOffListener(PTimerWentOffListener l) {
	if (listeners == null) {
		listeners = new Vector(1);
	}
	((Vector)listeners).addElement(l);
}

public void removePTimerWentOffListener(PTimerWentOffListener l) {
	if (listeners != null) {
		((Vector)listeners).removeElement(l);
	}
}

public void setAbsoluteTime(long when) {
	absolute = true;
	time = when;
	repeat = false;
}

public void setDelayTime(long delay) {
	absolute = false;
	time = delay;
	repeat = false;
}

public void notifyListeners(PTimer source) {
	if (listeners != null) {
		Enumeration e = ((Vector)listeners).elements();
		PTimerWentOffEvent ev = new PTimerWentOffEvent(source, this);
		while (e.hasMoreElements()) {
			((PTimerWentOffListener)e.nextElement()).timerWentOff(ev);
		}
	}
}

}
