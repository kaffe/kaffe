/**
 * Event - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @deprecated
 * This class has been deprecated - use AWTEvent and it's subclasses.
 */

package java.awt;

public class Event implements java.io.Serializable
{
	public Object arg;
	public int clickCount;
	private boolean consumed;	// needed for serial form
	public Event evt;
	public int id;
	public int key;
	public int modifiers;
	public Object target;
	public int x;
	public int y;
	public long when;
	Event next;
	private static Event cache;
	private static final long serialVersionUID = 5488922509400504703L;
	final public static int SHIFT_MASK = 1;
	final public static int CTRL_MASK = 2;
	final public static int META_MASK = 4;
	final public static int ALT_MASK = 8;
	final public static int HOME = 1000;
	final public static int END = 1001;
	final public static int PGUP = 1002;
	final public static int PGDN = 1003;
	final public static int UP = 1004;
	final public static int DOWN = 1005;
	final public static int LEFT = 1006;
	final public static int RIGHT = 1007;
	final public static int F1 = 1008;
	final public static int F2 = 1009;
	final public static int F3 = 1010;
	final public static int F4 = 1011;
	final public static int F5 = 1012;
	final public static int F6 = 1013;
	final public static int F7 = 1014;
	final public static int F8 = 1015;
	final public static int F9 = 1016;
	final public static int F10 = 1017;
	final public static int F11 = 1018;
	final public static int F12 = 1019;
	final public static int PRINT_SCREEN = 1020;
	final public static int SCROLL_LOCK = 1021;
	final public static int CAPS_LOCK = 1022;
	final public static int NUM_LOCK = 1023;
	final public static int PAUSE = 1024;
	final public static int INSERT = 1025;
	final public static int ENTER = 10;
	final public static int BACK_SPACE = 8;
	final public static int TAB = 9;
	final public static int ESCAPE = 27;
	final public static int DELETE = 127;
	final public static int WINDOW_DESTROY = 201;
	final public static int WINDOW_EXPOSE = 202;
	final public static int WINDOW_ICONIFY = 203;
	final public static int WINDOW_DEICONIFY = 204;
	final public static int WINDOW_MOVED = 205;
	final public static int KEY_PRESS = 401;
	final public static int KEY_RELEASE = 402;
	final public static int KEY_ACTION = 403;
	final public static int KEY_ACTION_RELEASE = 404;
	final public static int MOUSE_DOWN = 501;
	final public static int MOUSE_UP = 502;
	final public static int MOUSE_MOVE = 503;
	final public static int MOUSE_ENTER = 504;
	final public static int MOUSE_EXIT = 505;
	final public static int MOUSE_DRAG = 506;
	final public static int SCROLL_LINE_UP = 601;
	final public static int SCROLL_LINE_DOWN = 602;
	final public static int SCROLL_PAGE_UP = 603;
	final public static int SCROLL_PAGE_DOWN = 604;
	final public static int SCROLL_ABSOLUTE = 605;
	final public static int SCROLL_BEGIN = 606;
	final public static int SCROLL_END = 607;
	final public static int LIST_SELECT = 701;
	final public static int LIST_DESELECT = 702;
	final public static int ACTION_EVENT = 1001;
	final public static int LOAD_FILE = 1002;
	final public static int SAVE_FILE = 1003;
	final public static int GOT_FOCUS = 1004;
	final public static int LOST_FOCUS = 1005;
	final public static int EVENT_FLAG_ACTION = 0x01;
	final public static int EVENT_FLAG_FOCUS = 0x02;
	final public static int EVENT_FLAG_KEY = 0x04;
	final public static int EVENT_FLAG_MOUSE = 0x08;
	final public static int EVENT_FLAG_MOUSEMOTION = 0x10;
	final public static int EVENT_FLAG_ALL = 0x1F;

public Event(Object target, int id, Object arg) {
	this(target, 0, id, 0, 0, 0, 0, arg);
}

public Event(Object target, long when, int id, int x, int y, int key, int modifiers) {
	this(target, when, id, x, y, key, modifiers, null);
}

public Event(Object target, long when, int id, int x, int y, int key, int modifiers, Object arg) {
	this.target = target;
	this.when = when;
	this.id = id;
	this.x = x;
	this.y = y;
	this.key = key;
	this.modifiers = modifiers;
	this.arg = arg;
}

public boolean controlDown() {
	if ((modifiers & CTRL_MASK) == 0) {
		return (false);
	}
	else {
		return (true);
	}
}

/*
 * Construct an 1.0 Event from a 1.1 Event.
 */
static synchronized Event getEvent ( AWTEvent evt ) {
	Event e;

	if ( cache == null ) {
		e = new Event(null, 0, null);
	}
	else {
		e = cache;
		cache = e.next;
		e.next = null;
	}

	return evt.initOldEvent( e);
}

public boolean metaDown() {
	if ((modifiers & META_MASK) == 0) {
		return (false);
	}
	else {
		return (true);
	}
}

protected String paramString() {
	return ("id=" + id + ",x=" + x + ",y=" + y
		+ ",target=" + target + ",arg=" + arg);
}

public void recycle() {
	synchronized ( Event.class ) {
		arg = null;
		evt = null;
		target = null;
		this.next = cache;
		cache = this;
	}
}

public boolean shiftDown() {
	if ((modifiers & SHIFT_MASK) == 0) {
		return (false);
	}
	else {
		return (true);
	}
}

public String toString() {
	return getClass().getName() + '[' + paramString() + ']';
}

public void translate(int x, int y) {
	this.x += x;
	this.y += y;
}
}
