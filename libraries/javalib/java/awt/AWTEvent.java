package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.AdjustmentEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ContainerEvent;
import java.awt.event.FocusEvent;
import java.awt.event.ItemEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.PaintEvent;
import java.awt.event.TextEvent;
import java.awt.event.WindowEvent;
import java.util.EventObject;
import kaffe.util.Ptr;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
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
	protected static int inputModifier;
	final protected static int BUTTON_MASK = MouseEvent.BUTTON1_MASK |
			       MouseEvent.BUTTON2_MASK |
			       MouseEvent.BUTTON3_MASK;
	protected static boolean buttonPressed;
	protected static boolean mouseDragged;
	protected static int clickInterval;
	protected static Point mousePos = new Point();
	protected static Component mouseTgt;
	protected static int xMouseTgt;
	protected static int yMouseTgt;
	static Component[] cFrom = new Component[10];
	static Component[] cTo = new Component[10];
	protected static Component keyTgt;
	protected static Component keyTgtRequest;
	protected static Window activeWindow;
	protected static Window newActiveWindow;
	protected static FocusEvent focusEvtCache;
	protected static KeyEvent keyEvtCache;
	protected static MouseEvent mouseEvtCache;
	protected static PaintEvent paintEvtCache;
	protected static ComponentEvent cmpEvtCache;
	protected static WindowEvent wndEvtCache;
	protected static ActionEvent actEvtCache;
	protected static ItemEvent itmEvtCache;
	protected static AdjustmentEvent adjEvtCache;
	protected static boolean accelHint;
	protected static Component[] sources;
	static int nSources;
	protected static Object evtLock = new Object();
	protected static RootWindow root;
	protected static Window popup;

static {
	clickInterval = Defaults.ClickInterval;

	focusEvtCache = new FocusEvent( Window.dummy, 0);
	wndEvtCache = new WindowEvent( Window.dummy, 0);
	keyEvtCache = new KeyEvent( Window.dummy, 0, 0, 0, 0 );
	mouseEvtCache = new MouseEvent( Window.dummy, 0, 0, 0, 0, 0, 0, false);
	paintEvtCache = new PaintEvent( Window.dummy, 0, new Rectangle());
	cmpEvtCache = new ComponentEvent( Window.dummy, 0);
	actEvtCache = new ActionEvent( Window.dummy, 0, null);
	itmEvtCache = new ItemEvent( null, 0, null, 0);
	adjEvtCache = new AdjustmentEvent( null, 0, 0, 0);
	
	sources = Toolkit.evtInit();
	
	root = RootWindow.getDefaultRootWindow();
}

protected AWTEvent ( Object source, int id ) {
	super( source);
	
	this.id = id;
}

protected static void checkActiveWindow( Component c) {
	Component top = c.getToplevel();

	if ( (top != activeWindow) && (top != null) ) {
		FocusEvent e = getFocusEvent( top, FocusEvent.FOCUS_GAINED);
		e.dispatch();
	}
}

protected static boolean checkPopup ( Object src ) {
	if ( popup == null ) {
		return true;
	}
	if ( (src instanceof PopupWindow) && (((PopupWindow)src).rootWnd() == popup) ) {
		return true;
	}

	popup.dispose();
	popup = null;
	return false;
}

protected static Component computeMouseTarget ( Container toplevel, int x, int y ) {
	Container cntr;
	Component c;
	int       i, u, v;

  // Check no change / child change first
	if ( mouseTgt != null ) {
		u = x - xMouseTgt;
		v = y - yMouseTgt;

		if ( mouseTgt.contains( u, v ) ) {
			if ( mouseTgt instanceof Container ){
				cntr = (Container)mouseTgt;
				x = u;
				y = v;
			}
			else {
				return mouseTgt;                      // shortcut, no change
			}
		}
		else {
			cntr = toplevel; xMouseTgt = 0; yMouseTgt = 0;
		}
	}
	else {
		cntr = toplevel; xMouseTgt = 0; yMouseTgt = 0;
	}

	// This is a workaround for no-native-wm Frames with childs extending the Frame size
	// (the Frame deco border is no separate Component, just "protected" by the Frame insets)
	// Note that we cannot include the top because this would disable Menubars, but since
	// the Menubar is a child (and most layouts are top/left aligned anyway), we can ignore this
	if ( cntr.insets != Insets.noInsets ) {
		if ( (x >= (cntr.width - cntr.insets.right)) || (y >= (cntr.height - cntr.insets.bottom)) )
			return cntr;
	}

	for ( i=0; i<cntr.nChildren; ) {
		c = cntr.children[i];
		u = x - c.x; v = y - c.y;

		if ( c.contains( u, v) ){                 // contains() might be reimplemented
			xMouseTgt += c.x; yMouseTgt += c.y;
			if ( c instanceof Container ){
				x = u; y = v;
				cntr= (Container) c;
				i=0;
				continue;
			}
			else {
				return c;
			}
		}
		i++;
	}

	return cntr;
}

public void consume () {
	consumed = true;
}

protected void dispatch () {
	// standard processing of non-system events
	((Component)source).dispatchEventImpl( this);
}

protected static ActionEvent getActionEvent ( Object source, int id ) {
	synchronized ( evtLock ) {
		if ( actEvtCache == null ) {
			return new ActionEvent( source, id, null);
		}
		else {
			ActionEvent e = actEvtCache;
			actEvtCache = (ActionEvent) e.next;
			e.next = null;
		
			e.source = source;
			e.id = id;

			return e;
		}
	}
}

protected static AdjustmentEvent getAdjustmentEvent ( Adjustable source, int id ) {
	synchronized ( evtLock ) {
		if ( adjEvtCache == null ) {
			return new AdjustmentEvent( source, id, 0, 0);
		}
		else {
			AdjustmentEvent e = adjEvtCache;
			adjEvtCache = (AdjustmentEvent) e.next;
			e.next = null;
		
			e.source = source;
			e.id = id;

			return e;
		}
	}
}

protected static ComponentEvent getComponentEvent ( Component source, int id ) {
	synchronized ( evtLock ) {
		if ( cmpEvtCache == null ) {
			return new ComponentEvent( source, id);
		}
		else {
			ComponentEvent e = cmpEvtCache;
			cmpEvtCache = (ComponentEvent) e.next;
			e.next = null;
		
			e.source = source;
			e.id = id;

			return e;
		}
	}
}

protected static FocusEvent getFocusEvent ( Component source, int id ) {
	synchronized ( evtLock ) {
		if ( focusEvtCache == null ) {
			return new FocusEvent( source, id);
		}
		else {
			FocusEvent e = focusEvtCache;
			focusEvtCache = (FocusEvent) e.next;
		
			e.source = source;
			e.id = id;
			e.next = null;

			return e;
		}
	}
}

public int getID () {
	return id;
}

static int getID ( AWTEvent evt ) {
	return evt.id;
}

protected static ItemEvent getItemEvent ( ItemSelectable source, int id ) {
	synchronized ( evtLock ) {
		if ( itmEvtCache == null ) {
			return new ItemEvent( source, id, null, 0);
		}
		else {
			ItemEvent e = itmEvtCache;
			itmEvtCache = (ItemEvent) e.next;
			e.next = null;
		
			e.source = source;
			e.id = id;

			return e;
		}
	}
}

protected static MouseEvent getMouseEvent ( Component source, int id, long when, int mods,
                                int x, int y, int clickCount, boolean isPopupTrigger ) {
	synchronized ( evtLock ) {
		if ( mouseEvtCache == null ) {
			return new MouseEvent( source, id, when, mods, x, y, clickCount, isPopupTrigger);
		}
		else {
			MouseEvent e = mouseEvtCache;
			mouseEvtCache = (MouseEvent) e.next;
			e.next = null;
		
			e.source = source;
			e.id = id;

			e.setMouseEvent( when, mods, x, y, clickCount, isPopupTrigger);

			return e;
		}
	}
}

protected static PaintEvent getPaintEvent ( Component source, int id, Rectangle updateRect ) {
	synchronized ( evtLock ) {
		if ( paintEvtCache == null ) {
			return new PaintEvent( source, id, updateRect);
		}
		else {
			PaintEvent e = paintEvtCache;
			paintEvtCache = (PaintEvent) e.next;
			e.next = null;
		
			e.source = source;
			e.id = id;
			e.setUpdateRect( updateRect);

			return e;
		}
	}
}

static Object getSource ( AWTEvent evt ) {
	return evt.source;
}

protected static Component getToplevel ( Component c ) {
	while ( ! (c instanceof Window) )
		c = c.parent;
		
	return c;
}

protected static WindowEvent getWindowEvent ( Window source, int id ) {
	synchronized ( evtLock ) {
		if ( wndEvtCache == null ) {
			return new WindowEvent( source, id);
		}
		else {
			WindowEvent e = wndEvtCache;
			wndEvtCache = (WindowEvent) e.next;
		
			e.source = source;
			e.id = id;
			e.next = null;
	
			return e;
		}
	}
}

protected boolean isConsumed () {
	return consumed;
}

public String paramString () {
	return "";
}

protected static void processActionEvent ( ActionEvent e ) {
	Object src = e.source;
	
	if ( src instanceof Component )
		((Component)src).processActionEvent( e);
	else if ( src instanceof MenuItem )
		((MenuItem)src).processActionEvent( e);
}

protected static void processAdjustmentEvent ( AdjustmentEvent e ) {
	((Component)e.source).processAdjustmentEvent(e);
}

protected static void processComponentEvent ( ComponentEvent e ) {
	((Component)e.source).processComponentEvent(e);
}

protected static void processContainerEvent ( ContainerEvent e ) {
	((Component)e.source).processContainerEvent(e);
}

protected static void processFocusEvent ( FocusEvent e ) {
	((Component)e.source).processFocusEvent(e);
}

protected static void processItemEvent ( ItemEvent e ) {
	Object src = e.source;
	
	if ( src instanceof Component)
		((Component)src).processItemEvent( e);
	else if ( src instanceof CheckboxMenuItem )
		((CheckboxMenuItem)src).processItemEvent( e);
}

protected static void processKeyEvent ( KeyEvent e ) {
	((Component)e.source).processKeyEvent(e);

	if ( ! e.consumed )
	    HotKeyHandler.handle( e);
}

protected static void processMouseEvent ( MouseEvent e ) {
	((Component)e.source).processMouseEvent(e);
}

protected static void processMouseMotionEvent ( MouseEvent e ) {
	((Component)e.source).processMouseMotionEvent(e);
}

protected static void processPaintEvent ( PaintEvent e ) {
	((Component)e.source).processPaintEvent( e);
}

protected static void processWindowEvent ( WindowEvent e ) {
	((Component)e.source).processWindowEvent(e);
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

static void resetPopup ( Window oldPopup ) {

	// just reset if this is the one that is still active
	if ( (popup != null) && (popup == oldPopup) ) {
		popup = null;
	}
}

protected static void sendFocusEvent ( Component src, boolean focusGained, boolean sync ) {
	int id = focusGained ? FocusEvent.FOCUS_GAINED : FocusEvent.FOCUS_LOST;
	FocusEvent e = getFocusEvent( src, id);
	
	if ( sync )
		e.dispatch();
	else
		Toolkit.eventQueue.postEvent( e);
}

protected static void sendMouseEnterEvent ( Component src, boolean sync ) {
	MouseEvent e = getMouseEvent( src, MouseEvent.MOUSE_ENTERED,
	                              System.currentTimeMillis(),
	                              0, mousePos.x + xMouseTgt, mousePos.y + yMouseTgt,
	                              0, false);
	
	if ( sync )
		e.dispatch();
	else
		Toolkit.eventQueue.postEvent( e);
}

protected static void sendPaintEvent ( Component src, Rectangle r, boolean sync ) {
	PaintEvent e = getPaintEvent( src, PaintEvent.PAINT, r);
	
	if ( sync )
		e.dispatch();
	else
		Toolkit.eventQueue.postEvent( e);
}

protected static void sendWindowEvent ( Window src, int id, boolean sync ) {
	WindowEvent e = getWindowEvent( src, id);
	
	if ( sync )
		e.dispatch();
	else
		Toolkit.eventQueue.postEvent( e);
}

protected void setActionEvent ( String cmd, int mods ) {
}

protected void setAdjustmentEvent ( int adjType, int adjVal ) {
}

protected void setItemEvent ( Object obj , int op ) {
}

protected void setMouseEvent ( long time, int mods, int xNew, int yNew, int click, boolean popUp ) {
	// set protected MouseEvent fields
}

static void setPopup ( Window newPopup ) {
	if ( popup != null ) { // it has to be one
		popup.dispose();
	}
	
	popup = newPopup;
}

void setSource ( Object newSource ) {
	source = newSource;
}

protected void setXY ( int x, int y ) {
}

public String toString () {
	return getClass().getName() + ':' + paramString() + ", source: " + source;
}

protected static void transferMouse ( MouseEvent e,
		     Component from, int xFrom, int yFrom,
		     Component to, int xTo, int yTo ) {
	// We have to handle both inter-/intra- toplevel transfers, exits and enters have to
	// be fully symmetrical (enter A, enter A.B -> exit A.B, exit A). Note that for intra
	// transfers, the first common parent does not have to be a toplevel container.
	// We also have to handle non-contiguous mouse moves (e.g. acceleration or wraps),
	// with exits always using the last known (inside) coordinate of the lastTarget.
	// Last, not least, we also have to handle 'null' from / to targets (toplevel enter/exits)

	int i, n, m;
	int x  = xFrom;
	int y  = yFrom;

	// save event state (we "re-use" it in order to save memory)
	Object origSource = e.source;
	int    origId     = e.id;
	int    origX      = e.getX();
	int    origY      = e.getY();

	// init the from[], to[] stacks and get the first common parent (if any)
	for ( n=0, cFrom[0] = from; from != null; from = from.parent )
		cFrom[++n] = from;
	for ( m=0, cTo[0] = to; to != null; to = to.parent )
		cTo[++m] = to;
	for ( ; cFrom[n] == (to = cTo[m]); n--, m-- )
		cFrom[n] = cTo[m] = null;

	// send EXITED to all from[] components (inside out)
	e.id = MouseEvent.MOUSE_EXITED;
	for ( i=1; i <= n; i++ ) {
		e.setXY( x, y);
		// e.source = from = cFrom[i];
		from = cFrom[i];
		e.source = from;
		
	  //System.out.println( "mouseExit:  " + from.getClass() + ": " + x + ',' + y);
		from.processMouseEvent( e);
		x += from.x; y += from.y;
	}

	// send ENTERED to all to[] components (outside in, hence we have to adapt x,y first)
	e.id = MouseEvent.MOUSE_ENTERED;
	if ( cFrom[n] != cTo[m] ) {
		for ( i=1, x=xTo, y=yTo; i<=m; i++ ) {
			x += cTo[i].x; y += cTo[i].y;
		}
	}
	for ( i=m; i > 0; i-- ) {
		// e.source = to = cTo[i];
		to = cTo[i];
		e.source = to;
		
		x -= to.x; y -= to.y;
		e.setXY( x, y);
		//System.out.println( "mouseEnter: " + to.getClass() + ": " + x + ',' + y);
		to.processMouseEvent( e);
	}

	// set cursor
	if ( to != null ){
		to.setCursor( to.cursor);
	}

	// clean up stacks (to avoid memory leaks)
	for ( i=0; i<n; i++ ) cFrom[i] = null;
	for ( i=0; i<m; i++ ) cTo[i] = null;
	
	// restore state
	e.source = origSource;
	e.id     = origId;
	e.setXY( origX, origY);
}

protected static void triggerPopup ( MouseEvent e ) {
	Component s = (Component) e.source;
	
	if ( (s.popups != null) && (s.popups.size() > 0) ) {
		PopupMenu p = (PopupMenu)s.popups.firstElement();
		p.show( s, e.getX(), e.getY());
	}
}

static void unregisterSource ( Component c, Ptr nativeData ) {
	int idx = Toolkit.evtUnregisterSource( nativeData);
	sources[idx] = null;

	if ( (--nSources	== 0) && Defaults.AutoStop ) {
		// give the SecurityManager a chance to step in before
		// closing down the Toolkit
		System.getSecurityManager().checkExit( 0);
		
		Toolkit.terminate();
		System.exit( 0); // not strictly required (if there are no persistent daemons)
	}
}

protected static void updateToplevelBounds ( Component c, int x, int y, int width, int height ) {
	c.x = x;
	c.y = y;

	if ( (width != c.width) || (height != c.height) ) {
		c.width = width;
		c.height = height;
		
		// we don't want to repaint during the layout since we get a native
		// expose anyway
		c.isVisible = false;
		c.doLayout();
		c.isVisible = true;
	}
}
}
