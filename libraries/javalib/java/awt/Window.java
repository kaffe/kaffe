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

package java.awt;

import java.lang.String;
import java.awt.BorderLayout;
import java.awt.event.FocusEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.peer.ComponentPeer;
import kaffe.util.Ptr;

public class Window
  extends Container
{
	Ptr nativeData;
	WindowListener wndListener;
	Frame owner;
	static Window dummy = new Window();

Window () {
	isVisible = false;
	
	fgClr = Defaults.WndForeground;
	bgClr = Defaults.WndBackground;
	font  = Defaults.WndFont;

	setLayout(new BorderLayout());
}

public Window ( Frame owner ) {
	this();
	this.owner = owner;
}

public void addNotify () {
	// even if this could be done in a central location, we defer this
	// as much as possible because it might involve polling (for non-threaded
	// AWTs), slowing down the startup time
	Toolkit.startDispatch();

	if ( nativeData == null ) {
		// Some native windowing systems require windows to be created in
		// the thread that does the event dispatching. We force a context
		// switch by means of a WMEvent in this case
		if ( Toolkit.isDispatchExclusive &&
		     (Thread.currentThread() != Toolkit.eventThread) ){
			WMEvent e =  WMEvent.getEvent( this, WMEvent.WM_CREATE);
			Toolkit.eventQueue.postEvent( e);

			while ( nativeData == null ) {
				synchronized ( e ) {
					try { e.wait(); } catch ( InterruptedException x ) {}
				} 
			}
		}
		else {
			// if we have an owner that has not been created yet, do it now
			if ( (owner != null) && (owner.nativeData == null) )
				owner.addNotify();
		
			// defer size setting as much as possible
			if ( (width == 0) || (height == 0) )
				setSize( getPreferredSize());
				
			if ( (nativeData = createNativeWindow()) == null )
				throw new AWTError( "native create failed: " + this);
			AWTEvent.registerSource( this, nativeData);
			
			super.addNotify();

			if ( hasToNotify( this, AWTEvent.WINDOW_EVENT_MASK, wndListener) ) {
				AWTEvent.sendEvent( WindowEvt.getEvent( this,
				                        WindowEvent.WINDOW_OPENED), false);
			}
		}
	}
}

public void addWindowListener ( WindowListener newListener ) {
	wndListener = AWTEventMulticaster.add( wndListener, newListener);
	eventMask |= AWTEvent.WINDOW_EVENT_MASK;
}

Ptr createNativeWindow () {
	return Toolkit.wndCreateWindow( (owner != null) ? owner.nativeData : null,
	                                x, y, width, height,
	                                cursor.type, bgClr.nativeValue);
}

ComponentPeer createPeer () {
	return Toolkit.singleton.createWindow( this);
}

public void dispose () {
	// we can't synchronously call removeNotify (i.e. bypass native destroy notification)
	// since there might still be some native events (already) queued which subsequently
	// would "loose" their source. However, we also have to make sure that wndDestroyWindow
	// is called just a single time (since many window managers react alergically on multiple
	// destroy requests). We "borrow" the x field for this purpose (which isn't used
	// hereafter, anyway)

	// prevent further drawing (might cause trouble for native windowing system)
	isVisible = false;

	if ( nativeData != null ) {
		// if there are resident Graphics objects used in respond to a focusLost,
		// we might get problems because of an already deleted window - we better
		// simulate sync what has to be processed anyway (this error typically shows
		// up in a KaffeServer context)
		if ( AWTEvent.activeWindow == this ){
			AWTEvent.sendEvent( FocusEvt.getEvent( AWTEvent.keyTgt,
			                                       FocusEvent.FOCUS_LOST), true);
		}

		Toolkit.wndDestroyWindow( nativeData);
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
		return NativeGraphics.getGraphics( this, nativeData,
		                                   NativeGraphics.TGT_TYPE_WINDOW,
		                                   -deco.x, -deco.y,
		                                   deco.x, deco.y,
		                                   width - deco.width,
		                                   height - deco.height,
		                                   fgClr, bgClr, font, false);
	}
	else {
		return null;
	}
}

public Container getParent () {
	return owner;
}

final public String getWarningString() {
	if (System.getSecurityManager().checkTopLevelWindow(this) == true) {
		return (null);
	}
	return (System.getProperty("awt.appletWarning"));
}

public void hide() {
	super.hide();
	Toolkit.wndSetVisible( nativeData, false);
}

public boolean isShowing () {
	return isVisible;
}

public void pack () {
	if ( nativeData == null ) {
		addNotify();
	}
	setSize( getPreferredSize());
	
	// this happens to be one of the automatic validation points
	validate();
}

protected void processWindowEvent ( WindowEvent event ) {
	if ( hasToNotify( this, AWTEvent.WINDOW_EVENT_MASK, wndListener) ) {
		switch ( event.getID() ) {
		case WindowEvent.WINDOW_OPENED:
			wndListener.windowOpened( event);
			break;
		case WindowEvent.WINDOW_CLOSING:
			wndListener.windowClosing( event);
			break;
		case WindowEvent.WINDOW_CLOSED:
			wndListener.windowClosed( event);
			break;
		case WindowEvent.WINDOW_ICONIFIED:
			wndListener.windowIconified( event);
			break;
		case WindowEvent.WINDOW_DEICONIFIED:
			wndListener.windowDeiconified( event);
			break;
		case WindowEvent.WINDOW_ACTIVATED:
			wndListener.windowActivated( event);
			break;
		case WindowEvent.WINDOW_DEACTIVATED:
			wndListener.windowDeactivated( event);
			break;
		}
	}
}

public void removeNotify () {
	if ( nativeData != null ) {
		// Some native windowing systems require windows to be destroyed from
		// the thread which created them. Even though this should be ensured
		// by calling removeNotify via the WindowEvt.dispatch(), it is more safe
		// to check (since this is a public method)
		if ( Toolkit.isDispatchExclusive &&
		     (Thread.currentThread() != Toolkit.eventThread) ){
			WMEvent e = WMEvent.getEvent( this, WMEvent.WM_DESTROY);
			Toolkit.eventQueue.postEvent( e);
			while ( nativeData == null ) {
				try { e.wait(); } catch ( InterruptedException x ) {}
			}
		}
		else {
			super.removeNotify();

			AWTEvent.unregisterSource( this, nativeData);
			nativeData = null;
			
			if ( hasToNotify( this, AWTEvent.WINDOW_EVENT_MASK, wndListener) ) {
				AWTEvent.sendEvent( WindowEvt.getEvent( this,
				                        WindowEvent.WINDOW_CLOSED), false);
			}
		}
	}
}

public void removeWindowListener ( WindowListener listener ) {
	wndListener = AWTEventMulticaster.remove( wndListener, listener);
}

public void requestFocus () {
	if ( (nativeData != null) && isVisible )
		Toolkit.wndRequestFocus( nativeData);
}

public void reshape ( int xNew, int yNew, int wNew, int hNew ) {
	// DEP - this should be in setBounds (the deprecated ripple effect!)
	// this is never called by a native toplevel resize

	// Some people don't trust the automatic validation and call validate() explicitly
	// right after a reshape. We wouldn't get this is we wait for the automatic
	// invalidation during ComponentEvt.getEvent() (hello again, SwingSet..)
	if ( (wNew != width) || (hNew != height) )
		invalidate();

	x      = xNew;
	y      = yNew;
	width  = wNew;
	height = hNew;

	if ( nativeData != null )
		Toolkit.wndSetBounds( nativeData, xNew +deco.x, yNew +deco.y,
		                                  wNew -deco.width, hNew -deco.height);
}

void setNativeCursor ( Cursor cursor ) {
	if ( nativeData != null )
		Toolkit.wndSetCursor( nativeData, cursor.type);
}

public void show() {
	if ( nativeData == null )
		addNotify();

	// this happens to be one of the automatic validation points, and it should
	// cause a layout *before* we get visible
	validate();

	super.show();
		
	// Some apps carelessly start to draw (or do other display related things)
	// immediately after a show(), which is usually not called from the
	// event dispatcher thread
	Toolkit.eventThread.show( this);
}

public void toBack () {
	if ( nativeData != null ) Toolkit.wndToBack( nativeData);
}

public void toFront () {
	if ( nativeData != null ) Toolkit.wndToFront( nativeData);
}

public void update ( Graphics g ) {
	paint( g);
	
	// we need this here because paint might be resolved (without calling
	// super.paint), and our childs wouldn't be painted that way
	paintChildren( g);
}
}
