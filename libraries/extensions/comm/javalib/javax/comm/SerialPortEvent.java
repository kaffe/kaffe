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

public class SerialPortEvent
  extends EventObject {

/**
 * @deprecated.
 */
public int eventType;

private boolean oldValue;
private boolean newValue;

public static final int DATA_AVAILABLE = 1;
public static final int OUTPUT_BUFFER_EMPTY = 2;
public static final int CTS = 3;
public static final int DSR = 4;
public static final int RI = 5;
public static final int CD = 6;
public static final int OE = 7;
public static final int PE = 8;
public static final int FE = 9;
public static final int BI = 10;

public SerialPortEvent(SerialPort src, int evt, boolean oval, boolean nval) {
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
