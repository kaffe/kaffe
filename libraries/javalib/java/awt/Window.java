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

import java.awt.event.PaintEvent;
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
}

public Window ( Frame owner ) {
	this();

	this.owner = owner;
}

public void addNotify () {
	if ( nativeData == null ) {
		// some native windowing systems require windows to be created in
		// the thread that does the event dispatching
		if ( Toolkit.isWrongThread() ){
			WMEvent e =  new WMEvent( this, WMEvent.WM_CREATE);
			Toolkit.eventQueue.postEvent( e);
			while ( nativeData == null ) {
				try { e.wait(); } catch ( InterruptedException x ) {}
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

			if ( hasToNotify( AWTEvent.WINDOW_EVENT_MASK, wndListener) ) {
				Toolkit.eventQueue.postEvent( new WindowEvent( this,
				                                             WindowEvent.WINDOW_OPENED));
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
			AWTEvent.sendFocusEvent( AWTEvent.keyTgt, false, true);
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

LayoutManager getDefaultLayout() {
	return new BorderLayout();
}

public Component getFocusOwner () {
	return ( this == AWTEvent.activeWindow ) ? AWTEvent.keyTgt : null;
}

public Container getParent () {
	return owner;
}

public boolean isShowing () {
	return isVisible;
}

public void pack () {
	if ( (width == 0) || (height == 0) )
		setSize( getPreferredSize());
		
	if ( nativeData == null )
		addNotify();
}

void processPaintEvent ( PaintEvent e ) {
	if ( isVisible && (width > 0) ) {
		Rectangle r = e.getUpdateRect();
		Graphics g = Graphics.getGraphics( this,  0,0,
                                       r.x, r.y, r.width, r.height,
                                       fgClr, bgClr, font, true);

		if ( g != null ) {
			paint( g);
			g.dispose();
		}
	}

}

protected void processWindowEvent ( WindowEvent event ) {
	if ( hasToNotify( AWTEvent.WINDOW_EVENT_MASK, wndListener) ) {
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
		// some native windowing systems require windows to be created in
		// the thread that does the event dispatching
		if ( Toolkit.isWrongThread() ){
			WMEvent e = new WMEvent( this, WMEvent.WM_DESTROY);
			Toolkit.eventQueue.postEvent( e);
			while ( nativeData == null ) {
				try { e.wait(); } catch ( InterruptedException x ) {}
			}
		}
		else {
			super.removeNotify();

			AWTEvent.unregisterSource( this, nativeData);
			nativeData = null;
		}
	}
}

public void removeWindowListener ( WindowListener listener ) {
	wndListener = AWTEventMulticaster.remove( wndListener, listener);
}

public void repaint ( int x, int y, int width, int height ) {
	if ( isVisible && (width > 0) ) {
		Graphics g = Graphics.getGraphics( this,  0,0,
                                       x, y, width, height,
                                       fgClr, bgClr, font, true);
		if ( g != null ) {
			update( g);
			g.dispose();
		}
	}
}

public void requestFocus () {
	if ( (nativeData != null) && isVisible )
		Toolkit.wndRequestFocus( nativeData);
}

public void setBounds ( int xNew, int yNew, int wNew, int hNew ) {
	// this is never called by a native toplevel resize

	x      = xNew;
	y      = yNew;
	width  = wNew;
	height = hNew;

	if ( nativeData != null )
		Toolkit.wndSetWindowBounds( nativeData, xNew, yNew, wNew, hNew);
	else
		doLayout();
}

void setNativeCursor ( Cursor cursor ) {
	if ( nativeData != null )
		Toolkit.wndSetCursor( nativeData, cursor.type);
}

public void setVisible ( boolean showIt ) {
	if ( nativeData == null )
		addNotify();

	super.setVisible( showIt);
		
	// There is a subtle problem with some apps doing lengthy sync ops
	// or even explicit drawing directly after issuing a setVisible( true)
	// the dispatcher has no chance to process a pending native paint event
	// until this sync processing is finished, possibly causing inconsistent
	// screen representations. Note that we can't directly call wndSetVisible
	// since we don't know if we are in the dsptcher thread (if not, the
	// high priority dispatcher might process a paint before we start to wait
	// for it, extending the wait until doomesday
	if ( showIt )
		Toolkit.eventThread.show( this);
	else
		Toolkit.wndSetVisible( nativeData, showIt);
}

public void show()
{
	setVisible(true);
}

public void toBack () {
	if ( nativeData != null ) Toolkit.wndToBack( nativeData);
}

public void toFront () {
	if ( nativeData != null ) Toolkit.wndToFront( nativeData);
}
}
