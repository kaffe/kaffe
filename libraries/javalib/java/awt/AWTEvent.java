package java.awt;

import java.lang.String;
import java.awt.event.ActionEvent;
import java.awt.event.AdjustmentEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ContainerEvent;
import java.awt.event.FocusEvent;
import java.awt.event.ItemEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.TextEvent;
import java.awt.event.WindowEvent;
import java.util.EventObject;
import java.util.Stack;
import kaffe.util.Ptr;

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
	protected boolean consumed = false;
	protected AWTEvent next;
	final public static int COMPONENT_EVENT_MASK = 0x01;
	final public static int CONTAINER_EVENT_MASK = 0x02;
	final public static int FOCUS_EVENT_MASK = 0x04;
	final public static int KEY_EVENT_MASK = 0x08;
	final public static int MOUSE_EVENT_MASK = 0x10;
	final public static int MOUSE_MOTION_EVENT_MASK = 0x20;
	final public static int WINDOW_EVENT_MASK = 0x40;
	final public static int ACTION_EVENT_MASK = 0x80;
	final public static int ADJUSTMENT_EVENT_MASK = 0x100;
	final public static int ITEM_EVENT_MASK = 0x200;
	final public static int TEXT_EVENT_MASK = 0x400;
	final public static int RESERVED_ID_MAX = 1999;
	final static int DISABLED_MASK = 0x80000000;
	protected static Component keyTgt;
	protected static Window activeWindow;
	protected static Component mouseTgt;
	protected static int inputModifier;
	protected static boolean accelHint;
	protected static Component[] sources;
	static int nSources;
	protected static Object evtLock = new Object();
	protected static RootWindow root;
	protected static Component nativeSource;

static {
	sources = Toolkit.evtInit();
}

protected AWTEvent ( Object source, int id ) {
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

static int getID ( AWTEvent evt ) {
	return evt.id;
}

static Object getSource ( AWTEvent evt ) {
	return evt.source;
}

protected static Component getToplevel ( Component c ) {
	while ( ! (c instanceof Window) )
		c = c.parent;
		
	return c;
}

protected Event initOldEvent ( Event e ) {
	// this is the generic version, to be resolved by the relevant subclasses
	return null;
}

protected boolean isConsumed () {
	return consumed;
}

public String paramString () {
	return "";
}

protected void recycle () {
	source = null;
	next = null;
}

static void registerSource ( Component c, Ptr nativeData ) {
	int idx = Toolkit.evtRegisterSource( nativeData);
	sources[idx] = c;

	if ( ++nSources	== 1 )
		Toolkit.startDispatch();
}

protected static void sendEvent ( AWTEvent e, boolean sync ) {
    if ( sync )
      e.dispatch();
    else
      Toolkit.eventQueue.postEvent( e);
}

void setSource ( Object newSource ) {
	source = newSource;
}

public String toString () {
	return getClass().getName() + ':' + paramString() + ", source: " + source;
}

static void unregisterSource ( Component c, Ptr nativeData ) {
	int idx = Toolkit.evtUnregisterSource( nativeData);
	sources[idx] = null;

	if ( c == nativeSource ) // just a matter of safety (avoid temp garbage)
		nativeSource = null;

	if ( (--nSources	== 0) && Defaults.AutoStop ) {
		// give the SecurityManager a chance to step in before
		// closing down the Toolkit
		System.getSecurityManager().checkExit( 0);
		
		Toolkit.terminate();
		System.exit( 0); // not strictly required (if there are no persistent daemons)
	}
}
}
