package java.awt.event;

import java.awt.Component;
import java.awt.Event;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
public class FocusEvent
  extends ComponentEvent
{
	protected boolean isTemporary;
	final public static int FOCUS_FIRST = 1004;
	final public static int FOCUS_LAST = 1005;
	final public static int FOCUS_GAINED = FOCUS_FIRST;
	final public static int FOCUS_LOST = FOCUS_FIRST + 1;
	private static final long serialVersionUID = 523753786457416396L;

public FocusEvent ( Component src, int evtId ) {
	super( src, evtId);
}

public FocusEvent ( Component src, int evtId, boolean isTemporary ) {
	super( src, evtId);
	this.isTemporary = isTemporary;
}

protected Event initOldEvent ( Event e ) {
	e.target = source;
	e.id = id;
	
	return e;
}

public boolean isTemporary() {
	return isTemporary;
}

public String paramString() {
	String s;

	switch ( id ) {
		case FOCUS_GAINED:  s = "FOCUS_GAINED"; break;
		case FOCUS_LOST:    s = "FOCUS_LOST"; break;
		default:            s = "unknown type"; break;
	}
	
	if ( isTemporary )
		s += ", temporary";
	
	return s;
}
}
