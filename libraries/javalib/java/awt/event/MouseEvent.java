package java.awt.event;

import java.awt.Component;
import java.awt.Container;
import java.awt.Event;
import java.awt.Point;
import java.awt.Toolkit;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class MouseEvent
  extends InputEvent
{
	protected int x;
	protected int y;
	protected int clickCount;
	protected boolean isPopupTrigger;
	final public static int MOUSE_FIRST = 500;
	final public static int MOUSE_LAST = 506;
	final public static int MOUSE_CLICKED = MOUSE_FIRST;
	final public static int MOUSE_PRESSED = MOUSE_FIRST + 1;
	final public static int MOUSE_RELEASED = MOUSE_FIRST + 2;
	final public static int MOUSE_MOVED = MOUSE_FIRST + 3;
	final public static int MOUSE_ENTERED = MOUSE_FIRST + 4;
	final public static int MOUSE_EXITED = MOUSE_FIRST + 5;
	final public static int MOUSE_DRAGGED = MOUSE_FIRST + 6;

public MouseEvent ( Component src, int evtId, long time, int modifiers,
	     int x, int y, int clickCount, boolean isPopupTrigger) {
	super( src, evtId);
	
	this.when = time;
	this.modifiers = modifiers;

	this.x = x;
	this.y = y;
	this.clickCount = clickCount;
	this.isPopupTrigger = isPopupTrigger;
}

public int getClickCount() {
	return clickCount;
}

public Point getPoint() {
	return new Point( x, y);
}

public int getX() {
	return x;
}

public int getY() {
	return y;
}

protected Event initOldEvent ( Event e ) {
	e.target = source;
	e.id = id;
	
	e.when = when;
	e.modifiers = modifiers;
	e.x = x;
	e.y = y;
	e.clickCount = clickCount;
	
	return e;
}

public boolean isPopupTrigger() {
	return isPopupTrigger;
}

public String paramString() {
	String s;

	switch ( id ) {
	case MOUSE_PRESSED:		s = "MOUSE_PRESSED";		break;
	case MOUSE_RELEASED:	s = "MOUSE_RELEASED";		break;
	case MOUSE_CLICKED:		s = "MOUSE_CLICKED";		break;
	case MOUSE_ENTERED:		s = "MOUSE_ENTERED";		break;
	case MOUSE_EXITED:		s = "MOUSE_EXITED";		  break;
	case MOUSE_MOVED:		  s = "MOUSE_MOVED";		  break;
	case MOUSE_DRAGGED:		s = "MOUSE_DRAGGED";		break;
	default:		          s = "unknown type";
	}
	return s + ",(" + x + "," + y + ")" + ",mods=" + modifiers + ",clickCount="
     	     + clickCount;
}

public void translatePoint ( int x, int y ) {
	this.x += x;
	this.y += y;
}
}
