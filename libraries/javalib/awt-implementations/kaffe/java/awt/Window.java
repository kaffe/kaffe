package java.awt;

import java.awt.event.FocusEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.peer.ComponentPeer;

import gnu.classpath.Pointer;

/**
 * Window -
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class Window
  extends Container
{
	Pointer nativeData;
	WindowListener wndListener;
	Frame owner;
	static Window dummy = new Window();
	private static int counter;
	final private static long serialVersionUID = 4497834738069338734L;

Window () {
	// windows aren't visible per se, but they are exposed, colored and fontified
	flags = (IS_PARENT_SHOWING | IS_BG_COLORED | IS_FG_COLORED | IS_FONTIFIED);

	fgClr = Defaults.WndForeground;
	bgClr = Defaults.WndBackground;
	font  = Defaults.WndFont;

	setLayout(new BorderLayout());
	setName("win" + counter++);
}

public Window ( Frame owner ) {
	this();
	this.owner = owner;
}

public void addNotify () {
	if ( nativeData != null )  // don't do it twice
		return;

	// if we have an owner that has not been created yet, do it now
	if ( (owner != null) && (owner.nativeData == null) )
		owner.addNotify();

	// create the native object (this might involve a thread switch)
	Toolkit.createNative( this);

	if ( nativeData == null ){
		throw new AWTError( "native create failed: " + this);
	}

	// enable mapping of native events to Java Components
	AWTEvent.registerSource( this, nativeData);

	// addNotify childs and set flags. Be aware of that childs might be native, too
	// (i.e. need a native parent before they can be addNotified by themselves)
	super.addNotify();

/*** so far, JDK doesn't set the right size in addNotify, so we skip it, too
	// since setSize() triggers a layout, avaoid to do it before we know the right size.
	if ( (width == 0) || (height == 0) ) {
		// Bad! We have to defer setting a native size up to this point, because
		// getPreferredSize() might use a careless LayoutManager who doesn't check for
		// null peers. It really would be nice if we would already have the right coords
		// when creating the native window
	  setSize( getPreferredSize());
	}
****/
}

public void addWindowListener ( WindowListener newListener ) {
	wndListener = AWTEventMulticaster.add( wndListener, newListener);
}

void cleanUpNative () {
	if ( nativeData != null ) {
		AWTEvent.unregisterSource( this, nativeData);
		nativeData = null;
	}
}

void createNative () {
	nativeData = Toolkit.wndCreateWindow( (owner != null) ? owner.nativeData : null,
	                                x, y, width, height,
	                                cursor.type, bgClr.getNativeValue());
}

void destroyNative () {
	Toolkit.wndDestroyWindow( nativeData);

	cleanUpNative();
}

public void dispose () {
	// we can't synchronously call removeNotify (i.e. bypass native destroy notification)
	// since there might still be some native events (already) queued which subsequently
	// would "loose" their source. However, we also have to make sure that wndDestroyWindow
	// is called just a single time (since many window managers react alergically on multiple
	// destroy requests). We "borrow" the x field for this purpose (which isn't used
	// hereafter, anyway)

	// prevent further drawing (might cause trouble for native windowing system)
	// we don't use hide(), since this would trigger superfluous native action
	flags &= ~IS_VISIBLE;

	if ( nativeData != null ) {
		removeNotify();
	}
}

/**
 * Handle application resources.
 */
public void freeResource() {
	dispose();
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeAll( getClass(), true);
}

public Component getFocusOwner () {
	return ( this == AWTEvent.activeWindow ) ? AWTEvent.keyTgt : null;
}

public Graphics getGraphics () {
	if ( nativeData != null ){
		int u = 0, v = 0;

		if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ) {
			u -= deco.x;
			v -= deco.y;
		}

		// note that we have to clip against possible Frame menubars, too
		// (hence we partly have to use insets, not completely relying on deco)
		return NativeGraphics.getGraphics( this, nativeData,
		                                   NativeGraphics.TGT_TYPE_WINDOW,
		                                   u, v, insets.left, insets.top,
		                                   width - deco.width,
		                                   height - (insets.top + insets.bottom),
		                                   fgClr, bgClr, font, false);
	}
	else {
		return null;
	}
}

Pointer getNativeData () {
	return nativeData;
}

public Container getParent () {
	return owner;
}

public ComponentPeer getPeer () {
	// this is just a dummy, i.e. we share a single peer object that can be used
	// ONLY to "(getPeer() != null)" check if we already passed addNotify()
	return ((flags & IS_ADD_NOTIFIED) != 0) ? Toolkit.windowPeer : null;
}

final public String getWarningString() {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null && sm.checkTopLevelWindow(this)) {
		return (null);
	}
	return (System.getProperty("awt.appletWarning"));
}

public void hide() {
	super.hide();

	if ( nativeData != null ){
		Toolkit.wndSetVisible( nativeData, false);
	}
}

public boolean isShowing () {
	return ((flags & IS_VISIBLE) != 0);
}

public void pack () {
	if ( nativeData == null ) {
		addNotify();
	}
	setSize( getPreferredSize());

	// this happens to be one of the automatic validation points
	validate();
}

void process ( FocusEvent event ) {
	Component c;

	super.process( event);

	if ( event.id == FocusEvent.FOCUS_GAINED ) {
		// Set focus on first child which can handle it. We do this automatic focus forwarding
		// ONLY if there are no other pending requests, and we do it sync (we are already in the
		// thread which processed the FOCUS_GAINED) to avoid any interference with popup focus
		// transitions. This is because we might otherwise get a out-of-order focus event:
		// (popup1-lost -> owner-gained ->post forward , popup2-gained, forward-gained ->popup2-lost)
		if ( (nChildren > 0) && !Toolkit.eventQueue.hasPendingEvents( null, FocusEvt.FOCUS_GAINED) ) {
			c = ShortcutHandler.focusNext( this);
			if ( (c != null) && (c != this) ) {
				AWTEvent.sendEvent( FocusEvt.getEvent( c, FocusEvt.FOCUS_GAINED, false), true);
			}
		}
	}
}

void process ( WindowEvent e ) {
	if ( (wndListener != null) || (eventMask & AWTEvent.WINDOW_EVENT_MASK) != 0)
		processEvent( e);
}

protected void processWindowEvent ( WindowEvent event ) {
	// This is a artificial constraint - we could happily emit ACTIVATED/DEACTIVATED
	// events with out popup focus mechanism, but the JDK class docu says it just
	// handles OPENED/CLOSED.
	if ( wndListener != null ) {
		switch ( event.id ) {
		case WindowEvent.WINDOW_OPENED:
			wndListener.windowOpened( event);
			break;
		case WindowEvent.WINDOW_CLOSED:
			wndListener.windowClosed( event);
			break;
		}
	}
}

public void removeNotify () {
	if ( (flags & IS_ADD_NOTIFIED) != 0 ) {

		// use this rather than nativeData, because the sync FOCUS_LOST might get us recursive
    flags &= ~IS_ADD_NOTIFIED;

		// if there are resident Graphics objects used in respond to a focusLost,
		// we might get problems because of an already deleted window - we better
		// simulate sync what has to be processed anyway (this error typically shows
		// up in a KaffeServer context)
		if ( (AWTEvent.activeWindow == this) && (AWTEvent.keyTgt != null) ){
			AWTEvent.sendEvent( FocusEvt.getEvent( AWTEvent.keyTgt,
			                                       FocusEvent.FOCUS_LOST, false), true);
		}

		super.removeNotify();

		// this might cause a context switch, since we have to do it sync
		// (to prevent double-destroys for things like the swing popup
		// removeNotify jitter)
		Toolkit.destroyNative( this);

		if ( (wndListener != null) || (eventMask & AWTEvent.WINDOW_EVENT_MASK) != 0 ){
			AWTEvent.sendEvent( WindowEvt.getEvent( this,
						                        WindowEvent.WINDOW_CLOSED), false);
		}
	}
}

public void removeWindowListener ( WindowListener listener ) {
	wndListener = AWTEventMulticaster.remove( wndListener, listener);
}

public void requestFocus () {
	if ( (nativeData != null) && ((flags & IS_VISIBLE) != 0) ){
		Toolkit.wndRequestFocus( nativeData);
	}
}

public void reshape ( int xNew, int yNew, int wNew, int hNew ) {
	// DEP - this should be in setBounds (the deprecated ripple effect!)
	// this is never called by a native toplevel resize

	if ( (xNew == x) && (yNew == y) && (wNew == width) && (hNew == height) )
		return;  // avoid flicker of redundant reshapes

	// Some people don't trust the automatic validation and call validate() explicitly
	// right after a reshape. We wouldn't get this is we wait for the automatic
	// invalidation during ComponentEvt.getEvent() (hello again, SwingSet..)
	if ( (wNew != width) || (hNew != height) )
		invalidate();

	x      = xNew;
	y      = yNew;
	width  = wNew;
	height = hNew;

	if ( nativeData != null ) {
		if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ){
			// we have to fake a bit with native coordinates, since we pretend to own the
			// whole real estate of the toplevel (including the deco), but we don't in reality
			xNew += deco.x;
			yNew += deco.y;
			wNew -= deco.width;
			hNew -= deco.height;
		}

		Toolkit.wndSetBounds( nativeData, xNew, yNew, wNew, hNew, ((flags & IS_RESIZABLE) != 0));
	}
}

public void setBackground ( Color clr ) {
	// we can't nullify this (rermember the "bgClr != null" invariant)
	if ( clr != null ) {
		bgClr = clr;
		propagateBgClr( clr);

		if ( isShowing() )
			repaint();
	}
}

public void setFont ( Font fnt ) {
	// we can't nullify this (remember the "font != null" invariant)
	if ( fnt != null ) {
		font = fnt;
		propagateFont( fnt);

		if ( isShowing() )
			repaint();
	}
}

public void setForeground ( Color clr ) {
	// we can't nullify this (rermember the "fgClr != null" invariant)
	if ( clr != null ) {
		fgClr = clr;
		propagateFgClr( clr);

		if ( isShowing() )
			repaint();
	}
}

void setNativeCursor ( Cursor cursor ) {
	if ( nativeData != null )
		Toolkit.wndSetCursor( nativeData, cursor.type);
}

public void show() {
	if ( nativeData == null ){
		addNotify();
	}

	// this happens to be one of the automatic validation points, and it should
	// cause a layout *before* we get visible
	validate();

	if ( (flags & IS_VISIBLE) != 0 ) {
		toFront();
	}
	else {
		super.show();

		// Some apps carelessly start to draw (or do other display related things)
		// immediately after a show(), which is usually not called from the
		// event dispatcher thread. If we don't wait until the window is mapped, this
		// output is lost. But if we do, we might get into trouble with things like
		// swing (with its show->removeNotify->show jitter for popups). Since it isn't
		// specified, and the JDK does not provide reliable sync, we skip it for now
		// (local dispatching should be kept to a minimum)
		Toolkit.wndSetVisible( nativeData, true);

		// the spec says that WINDOW_OPENED is delivered the first time a Window
		// is shown, and JDK sends this after it got shown, so this is the place
		if ( (flags & IS_OPENED) == 0 ) {
			flags |= IS_OPENED;

			if ( (wndListener != null) || (eventMask & AWTEvent.WINDOW_EVENT_MASK) != 0 ){
				AWTEvent.sendEvent( WindowEvt.getEvent( this,
				                        WindowEvent.WINDOW_OPENED), false);
			}
		}
	}
}

public void toBack () {
	if ( nativeData != null ) Toolkit.wndToBack( nativeData);
}

public void toFront () {
	if ( nativeData != null ) Toolkit.wndToFront( nativeData);
}
}
