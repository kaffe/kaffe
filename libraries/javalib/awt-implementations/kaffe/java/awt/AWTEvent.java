package java.awt;

import java.awt.event.MouseEvent;
import java.util.EventObject;

import kaffe.awt.FocusHook;
import kaffe.awt.KeyHook;
import kaffe.awt.MouseHook;

import gnu.classpath.Pointer;

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
public class AWTEvent
  extends EventObject
{
	protected int id;
	protected boolean consumed;
	protected AWTEvent next;
	final private static long serialVersionUID = -1825314779160409405L;
	final public static long COMPONENT_EVENT_MASK = 0x01;
	final public static long CONTAINER_EVENT_MASK = 0x02;
	final public static long FOCUS_EVENT_MASK = 0x04;
	final public static long KEY_EVENT_MASK = 0x08;
	final public static long MOUSE_EVENT_MASK = 0x10;
	final public static long MOUSE_MOTION_EVENT_MASK = 0x20;
	final public static long WINDOW_EVENT_MASK = 0x40;
	final public static long ACTION_EVENT_MASK = 0x80;
	final public static long ADJUSTMENT_EVENT_MASK = 0x100;
	final public static long ITEM_EVENT_MASK = 0x200;
	final public static long TEXT_EVENT_MASK = 0x400;
        public static final long INPUT_METHOD_EVENT_MASK = 0x800;
	final public static int RESERVED_ID_MAX = 1999;
	final static long DISABLED_MASK = 0x80000000;
	final static long TEMP_DISABLED_MASK = 0x40000000;
	static Component keyTgt;
	static Window activeWindow;
	static Component mouseTgt;
	protected static int inputModifier;
	protected static Component[] sources;
	private static int nSources;
	protected static RootWindow root;
	protected static Component nativeSource;
	protected static KeyHook keyHook;
	protected static MouseHook mouseHook;
	protected static FocusHook focusHook;

static {
	sources = Toolkit.evtInit();
}

public AWTEvent(Event event) {
	this(event.target, event.id);
}

public AWTEvent ( Object source, int id ) {
	super( source);

	this.id = id;
}

public void consume () {
	consumed = true;
}

protected void dispatch () {
	// standard processing of non-system events
	((Component)source).processEvent( this);
}

public int getID () {
	return id;
}

protected static Component getToplevel ( Component c ) {
	// Note that this will fail in case 'c' is already removeNotified (has no parent,
	// anymore). But returning 'null' would just shift the problem into the caller -
	// a dispatch() method - and that would slow down dispatching. Since it also would be
	// difficult to decide what to do (because of inconsistent global state), we prefer a
	// clean cut and rely on no events being dispatched on removeNotified Components

	while ( c.parent != null ) c = c.parent;

	return c;
}

protected Event initOldEvent ( Event e ) {
	// this is the generic version, to be resolved by the relevant subclasses
	return null;
}

protected boolean isConsumed () {
	return consumed;
}

protected boolean isLiveEventFor( Object src ) {
	return false;
}

protected boolean isObsoletePaint( Object src, int x, int y, int w, int h ) {
	return false;
}

public String paramString () {
	return "";
}

protected void recycle () {
	source = null;
	next = null;
}

static void registerSource ( Component c, Pointer nativeData ) {
	int idx = Toolkit.evtRegisterSource( nativeData);
	sources[idx] = c;

	if ( ++nSources	== 1 ) {
		Toolkit.startDispatch();
	}
}

protected MouseEvent retarget ( Component target, int dx, int dy ) {
	return null;
}

protected static void sendEvent ( AWTEvent e, boolean sync ) {
    if ( sync )
      e.dispatch();
    else
      Toolkit.eventQueue.postEvent( e);
}

static void setSource ( AWTEvent evt, Object newSource ) {
	evt.source = newSource;
}

public String toString () {
	StringBuffer result = new StringBuffer(getClass().getName());

	result.append('[').append(paramString()).append("] on ");

	Object src = getSource();
	if (src instanceof Component) {
		result.append(((Component) src).getName());
	}
	else if (src instanceof MenuComponent) {
		result.append(((MenuComponent) src).getName());
	}
	else {
		result.append(src);
	}

	return result.toString();
}

static void unregisterSource ( Component c, Pointer nativeData ) {
	int idx = Toolkit.evtUnregisterSource( nativeData);
	sources[idx] = null;

	if ( c == nativeSource ) // just a matter of safety (avoid temp garbage)
		nativeSource = null;

	if ( --nSources	== 0 ) {
		if ( Defaults.AutoStop ) {
			// give the SecurityManager a chance to step in before
			// closing down the Toolkit
			SecurityManager sm = System.getSecurityManager();
			if (sm != null)
				sm.checkExit( 0);

			Toolkit.terminate();
			System.exit( 0); // not strictly required (if there are no persistent daemons)
		}
		else {
			// no need to continue dispatching (but a subsequent registerSource might
			// restart to dispatch, again)
			Toolkit.stopDispatch();
		}
	}
}
}
