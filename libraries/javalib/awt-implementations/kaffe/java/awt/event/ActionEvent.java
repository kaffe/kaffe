package java.awt.event;

import java.awt.AWTEvent;
import java.awt.Event;

/**
 * class ActionEvent - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class ActionEvent
  extends AWTEvent
{
	protected int mods;
	protected String cmd;
	final public static int SHIFT_MASK = InputEvent.SHIFT_MASK;
	final public static int CTRL_MASK = InputEvent.CTRL_MASK;
	final public static int META_MASK = InputEvent.META_MASK;
	final public static int ALT_MASK = InputEvent.ALT_MASK;
	final public static int ACTION_FIRST = 1001;
	final public static int ACTION_LAST = 1001;
	final public static int ACTION_PERFORMED = ACTION_FIRST;
	private static final long serialVersionUID = -7671078796273832149L;

public ActionEvent ( Object src, int evtId, String cmdString ) {
	this( src, evtId, cmdString, 0);
}

public ActionEvent ( Object src, int evtId, String cmdString, int modifiers ) {
	super( src, evtId);
	
	mods = modifiers;
	cmd = cmdString;
}

public String getActionCommand () {
	return cmd;
}

public int getModifiers () {
	return mods;
}

protected Event initOldEvent ( Event e ) {
	e.target = source;
	e.id = id;
	e.arg = cmd;
	
	return e;
}

public String paramString() {
	String s;
	
	if ( id == ACTION_PERFORMED )
		s = "ACTION_PERFORMED";
	else
		s = "ACTION: " + id;
		
	return s + ", mods: " + mods + ", cmd: " + cmd;
}
}
