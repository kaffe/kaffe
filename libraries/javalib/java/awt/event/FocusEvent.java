package java.awt.event;

import java.awt.Component;
import java.awt.Frame;
import java.awt.Window;

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
	boolean isTemporary;
	final public static int FOCUS_FIRST = 1004;
	final public static int FOCUS_LAST = 1005;
	final public static int FOCUS_GAINED = FOCUS_FIRST;
	final public static int FOCUS_LOST = FOCUS_FIRST + 1;

public FocusEvent ( Component src, int evtId ) {
	super( src, evtId);
}

public FocusEvent ( Component src, int evtId, boolean isTemporary ) {
	super( src, evtId);
	this.isTemporary = isTemporary;
}

protected void dispatch () {
	WindowEvent  we;
	Component    src = (Component) source;

	if ( id == FOCUS_GAINED ) {
		if ( keyTgtRequest != null ) {
			src = keyTgtRequest;
			keyTgtRequest = null;
		}
		
		if ( keyTgt == src ) return; // nothing to do
		
		if ( (keyTgt != null) && (keyTgt != src) ) {
			id = FOCUS_LOST;
			source = keyTgt;
			keyTgt = src;
			processFocusEvent( this);
		  id = FOCUS_GAINED;
		}

		// we should make the next active window known (within java.awt)
		// prior to sending the DEACTIVATED messages because RootWindow
		// instances might have to react on this
		newActiveWindow = (Window) getToplevel( src);
		if ( (activeWindow != null) && (newActiveWindow != activeWindow)
		       && ((newActiveWindow instanceof Frame) || (newActiveWindow == root)) ) {
			we = getWindowEvent( activeWindow, WindowEvent.WINDOW_DEACTIVATED);
			processWindowEvent( we);
		}

		keyTgt = src;
		source = src;
		processFocusEvent( this);
		
		if ( (newActiveWindow != activeWindow) && (newActiveWindow instanceof Frame) ) {
			we = getWindowEvent( newActiveWindow, WindowEvent.WINDOW_ACTIVATED);
			processWindowEvent( we);
			activeWindow = (Window) newActiveWindow;
		}
		else if ( newActiveWindow == root )
			activeWindow = null;
	}
	else if ( id == FOCUS_LOST ) {
		if ( src == activeWindow ) { // native generated focus lost
			we = getWindowEvent( activeWindow, WindowEvent.WINDOW_DEACTIVATED);
			processWindowEvent( we);
			activeWindow = null;
		}

		if ( keyTgt != null ) {
			source = keyTgt;
			keyTgt = null;
			inputModifier = 0; // just a matter of safety (a reset point)

			processFocusEvent( this);			
		}
	}
	
	recycle();
}

static FocusEvent getFocusEvent ( int srcIdx, int id ) {
	return getFocusEvent( sources[srcIdx], id);
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
	
	return s;
}

protected void recycle () {
	synchronized ( evtLock ) {
		source = null;

		next = focusEvtCache;	
		focusEvtCache = this;
	}
}
}
