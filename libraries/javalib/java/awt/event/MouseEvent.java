/*
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

package java.awt.event;

import java.awt.Component;
import java.awt.Event;
import java.awt.Point;

public class MouseEvent
  extends InputEvent
{
	protected int x;
	protected int y;
	protected int clickCount;
	protected boolean isPopupTrigger;
	
	protected int button;
	final public static int BUTTON1 = 1;
	final public static int BUTTON2 = 2;
	final public static int BUTTON3 = 3;
	final public static int NOBUTTON = 0;
	final public static int MOUSE_FIRST = 500;
	final public static int MOUSE_LAST = 507;
	final public static int MOUSE_CLICKED = MOUSE_FIRST;
	final public static int MOUSE_PRESSED = MOUSE_FIRST + 1;
	final public static int MOUSE_RELEASED = MOUSE_FIRST + 2;
	final public static int MOUSE_MOVED = MOUSE_FIRST + 3;
	final public static int MOUSE_ENTERED = MOUSE_FIRST + 4;
	final public static int MOUSE_EXITED = MOUSE_FIRST + 5;
	final public static int MOUSE_DRAGGED = MOUSE_FIRST + 6;
	final public static int MOUSE_WHEEL = MOUSE_FIRST + 7;

	private static final String [] MOUSE_EVENTS = {
		"MOUSE_CLICKED",
		"MOUSE_PRESSED",
		"MOUSE_RELEASED",
		"MOUSE_MOVED",
		"MOUSE_ENTERED",
		"MOUSE_EXITED",
		"MOUSE_DRAGGED",
	};

	private static final long serialVersionUID = -991214153494842848L;

public MouseEvent ( Component src, int evtId, long time, int modifiers,
	     int x, int y, int clickCount, boolean isPopupTrigger) {
	super( src, evtId);
	
	this.when = time;
	this.modifiers = modifiers;

	this.x = x;
	this.y = y;
	this.clickCount = clickCount;
	this.isPopupTrigger = isPopupTrigger;

	if ( ( modifiers & BUTTON1_MASK ) > 0 )
		this.button = BUTTON1;
	else if ( ( modifiers & BUTTON2_MASK ) > 0 )
		this.button = BUTTON2;
	else if ( ( modifiers & BUTTON3_MASK) > 0 )
		this.button = BUTTON3;
	else
		this.button = NOBUTTON;
}

public MouseEvent ( Component src, int evtId, long time, int modifiers,
		int x, int y, int clickCount, boolean isPopupTrigger, 
		int button) {
	super( src, evtId);
	
	this.when = time;
	this.modifiers = modifiers;

	this.x = x;
	this.y = y;
	this.clickCount = clickCount;
	this.isPopupTrigger = isPopupTrigger;

	this.button = button;
}

public int getButton() {
	return button;
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
	return MOUSE_EVENTS[getID() - MOUSE_FIRST]
		+ ",(" + getX() + "," + getY() + ")"
		+ ",modifiers=" + InputEvent.getModifiersAsString(getModifiers()) 
		+ ",clickCount=" + getClickCount();
}

public void translatePoint ( int x, int y ) {
	this.x += x;
	this.y += y;
}
}
