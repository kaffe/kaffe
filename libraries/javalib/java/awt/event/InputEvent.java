package java.awt.event;

import java.awt.Component;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */


public class InputEvent
  extends ComponentEvent
{
	protected int modifiers;
	protected long when;
	final public static int SHIFT_MASK = 1;
	final public static int CTRL_MASK = 2;
	final public static int META_MASK = 4;
	final public static int ALT_MASK = 8;
	final public static int BUTTON1_MASK = 16;
	final public static int BUTTON2_MASK = ALT_MASK;
	final public static int BUTTON3_MASK = META_MASK;

InputEvent ( Component src, int evtId ) {
  super( src, evtId);
}

public int getModifiers() {
	return modifiers;
}

public long getWhen() {
	return when;
}

public boolean isAltDown() {
	return ((modifiers & ALT_MASK) != 0);
}

public boolean isControlDown() {
	return ((modifiers & CTRL_MASK) != 0);
}

public boolean isMetaDown() {
	return ((modifiers & META_MASK) != 0);
}

public boolean isShiftDown() {
	return ((modifiers & SHIFT_MASK) != 0);
}
}
