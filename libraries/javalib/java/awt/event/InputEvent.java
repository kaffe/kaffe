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

	private static final long serialVersionUID = -2482525981698309786L;

	public static final int SHIFT_MASK = 1;
	public static final int SHIFT_DOWN_MASK = 64;
	public static final int CTRL_MASK = 2;
	public static final int CTRL_DOWN_MASK = 128;
	public static final int META_MASK = 4;
	public static final int META_DOWN_MASK = 256;
	public static final int ALT_MASK = 8;
	public static final int ALT_DOWN_MASK = 512;
	public static final int ALT_GRAPH_MASK = 32;
	public static final int ALT_GRAPH_DOWN_MASK = 8192;
	public static final int BUTTON1_MASK = 16;
	public static final int BUTTON1_DOWN_MASK = 1024;
	public static final int BUTTON2_MASK = ALT_MASK;
	public static final int BUTTON2_DOWN_MASK = 2048;
	public static final int BUTTON3_MASK = META_MASK;
	public static final int BUTTON3_DOWN_MASK = 4096;

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

public boolean isAltGraphDown() {
	return ((modifiers & ALT_GRAPH_MASK) != 0);
}

public boolean isConsumed() {
	return consumed;
}

public boolean isControlDown() {
	return ((modifiers & CTRL_MASK) != 0);
}

protected boolean isLiveEventFor ( Object src ) {
	return (src == source);
}

public boolean isMetaDown() {
	return ((modifiers & META_MASK) != 0);
}

public boolean isShiftDown() {
	return ((modifiers & SHIFT_MASK) != 0);
}
}
