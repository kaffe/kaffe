/*
 * Java comm library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.comm;

import java.util.EventObject;

public class ParallelPortEvent
  extends EventObject {

/**
 * @deprecated.
 */
public int eventType;

private boolean oldValue;
private boolean newValue;

public static final int PAR_EV_BUFFER = 1;
public static final int PAR_EV_ERROR = 2;

public ParallelPortEvent(ParallelPort src, int evt, boolean oval, boolean nval) {
	super(src);
	eventType = evt;
	oldValue = oval;
	newValue = nval;
}

public int getEventType() {
	return (eventType);
}

public boolean getNewValue() {
	return (newValue);
}

public boolean getOldValue() {
	return (oldValue);
}

}
