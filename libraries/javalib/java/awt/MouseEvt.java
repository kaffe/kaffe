package java.awt;

import java.awt.Component;
import java.awt.Container;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.event.MouseEvent;
import java.util.Stack;

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
class MouseEvt
  extends MouseEvent
{
	int button;
	static int clicks;
	static MouseEvt cache;
	static boolean buttonPressed;
	static boolean mouseDragged;
	static boolean mouseGrabbed;
	static long lastPressed;
	static Point mousePos = new Point();
	static int xMouseTgt;
	static int yMouseTgt;
	static Component[] cFrom = new Component[10];
	static Component[] cTo = new Component[10];
	static int clickInterval;
	final static int BUTTON_MASK = BUTTON1_MASK | BUTTON2_MASK | BUTTON3_MASK;
	static Stack grabStack = new Stack();
	static Point nativePos = new Point();

static {
	clickInterval = Defaults.ClickInterval;
}

MouseEvt ( Component src, int evtId, long time, int modifiers,
	     int x, int y, int clickCount, boolean isPopupTrigger) {
	super( src, evtId, time, modifiers, x, y, clickCount, isPopupTrigger);
}

static void clickToFocus ( Component newKeyTgt ) {			
	// The JDK does not automatically set the focus on mouse clicks for lightweights,
	// (non-lightweights probably will be handled by the native window system), i.e.
	// if a component explicitly requests the focus in respond to a mouse click, the
	// focus events will be processed AFTER the mouse event. This is the opposite order
	// compared to native handling (for toplevels). We try to be compatible with
	// lightweight behavior
	
	//if ( newKeyTgt.isFocusTraversable() )
		//newKeyTgt.requestFocus();
}

static Component computeMouseTarget ( Container toplevel, int x, int y ) {
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
	// depending on the native window manager, we can get mouse events for windows
	// which already are in their dispose() processing (PopupWindow drag mode)
	if ( !cntr.isVisible ) return null;

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

protected void dispatch () {
	// We handle this on the assumption that native events follow the X-policy
	// (all events during press and release are routed to the same toplevel).
	// Don't mix this with our Java mouse handling: we also need to implement
	// toplevel internal drags/grabs (for lightweights), *and* we need to
	// override the native grabs for PopupWindow drag ops (where the popup
	// client toplevel gets all the native events). This corresponds to some
	// kind of a generalized "OwnerGrabButtonMask" behavior.

	Component  newTgt;

	//--- determine the target, handle enter/exit 
	if ( mouseGrabbed || mouseDragged ) {
		if ( source != mouseTgt ) { // lightweight / no-native grab
			x -= xMouseTgt;
			y -= yMouseTgt;
			source = mouseTgt;
		}
	}
	else if ( id != MOUSE_CLICKED ){
		if ( (source instanceof Container) && (source != root) ) {
			newTgt = (id == MOUSE_EXITED) ? null : computeMouseTarget( (Container)source, x, y);
			source = newTgt;
			x -= xMouseTgt;
			y -= yMouseTgt;
		}
		else {
			newTgt = (id == MOUSE_EXITED) ? null : (Component) source;
		}

		// this handles the enter/exit processing
		if ( newTgt != mouseTgt ) {
			transferMouse( this, mouseTgt, mousePos.x, mousePos.y, newTgt, x, y);
		}

		mouseTgt = newTgt;
	}

	//--- now dispatch all the remaining events
	if ( mouseTgt != null ) {
		switch ( id ) {
		
		case MOUSE_MOVED:
			if ( buttonPressed ) {             // this starts drag mode
				mouseDragged = true;
				id = MOUSE_DRAGGED;
			}
			mouseTgt.processEvent( this);
		  break;
		
		case MOUSE_PRESSED:
			PopupWindow.checkPopup( mouseTgt);    // the old highlander theme..
			if ( (mouseTgt != keyTgt) )  // decide upon focus policy
				clickToFocus( mouseTgt);
			
			buttonPressed = true;
			modifiers = updateInputModifier( button, true);

			if ( when - lastPressed < clickInterval )
				clicks++;
			else
				clicks = 1;

			accelHint = true;
			clickCount = clicks;
			lastPressed = when;
			
			mouseTgt.processEvent( this);
			
			// some apps might react on isPopupTrigger() for both pressed + released
			isPopupTrigger = false;
		  break;
		
		case MOUSE_RELEASED:
			accelHint = false;
			clickCount = clicks;
			buttonPressed = false;

			updateInputModifier( button, false);

			if ( !mouseDragged ) {
				// we can't do this sync because of possible recursion, but we
				// need to provide a clicked event as the next event to follow
				// a MOUSE_RELEASE
				postMouseClicked( mouseTgt, when, x, y, modifiers, clickCount, button);
			}
			else {
				// check if we have to generate a enter event for a sibling
				sendMouseEnterEvent( nativeSource, nativePos.x, nativePos.y, false);
				mouseDragged = false;
			}
			mouseTgt.processEvent( this);
		  break;
		
		case MOUSE_CLICKED:
			clickCount = clicks;
			mouseTgt.processEvent( this);
			break;
		}

		// mousePos (used by transferMouse) always contains the LAST mouse pos
		// (relative to mouseTgt) in order to enable "clean" (non-continuos
		// move) exits
		mousePos.x = x;
		mousePos.y = y;
	}
	else {                            // mouseTgt == null -> toplevel exit
		mousePos.x = mousePos.y = 0;
	}

	recycle();
}

static synchronized MouseEvt getEvent ( Component source, int id, long when, int mods,
				            int x, int y, int clickCount, boolean isPopupTrigger ) {
	if ( cache == null ) {
		return new MouseEvt( source, id, when, mods, x, y, clickCount, isPopupTrigger);
	}
	else {
		MouseEvt e = cache;
		cache = (MouseEvt) e.next;
		e.next = null;
		
		e.source = source;
		e.id = id;
		e.when = when;
		e.x = x;
		e.y = y;
		e.clickCount = clickCount;
		e.isPopupTrigger = isPopupTrigger;

		return e;
	}
}

static synchronized MouseEvt getEvent ( int srcIdx, int id, int button, int x, int y ) {
	MouseEvt    e;
	Component   source = sources[srcIdx];
	long        when = System.currentTimeMillis();
	Rectangle   d = source.deco;

	x += d.x;
	y += d.y;

	nativeSource = source;
	nativePos.x = x;
	nativePos.y = y;

	if ( cache == null ) {
		e = new MouseEvt( source, id, when, inputModifier, x, y, 0, (button == 3));
	}
	else {
		e = cache;
		cache = (MouseEvt) e.next;
		e.next = null;
			
		e.source     = source;
		e.id         = id;
		e.when       = when;
		e.modifiers  = inputModifier;
		e.x          = x;
		e.y          = y;
		e.clickCount = 0;
		e.isPopupTrigger = (button == 3);	
	}

	e.button = button;
	return e;
}

static Component getGrab() {
	return (grabStack.isEmpty()) ? null : (Component)grabStack.peek();
}

static void grabMouse ( Component grab ) {
	// note that grabs currently don't work with Windows being moved
	// explicitly during the grab (since this requires the xMouseTgt,yMouseTgt
	// to be recomputed)
	
	synchronized ( MouseEvt.class ) {
		if ( nativeSource == null )
			return;

		Component p;
		int x = 0, y=0;
		for ( p=grab; p != null; p = p.parent ){
			x += p.x;
			y += p.y;
		}

		xMouseTgt = x - nativeSource.x;
		yMouseTgt = y - nativeSource.y;
		mouseTgt  = grab;

		nativeSource.setNativeCursor( grab.cursor);

		grabStack.push( grab);
		mouseGrabbed = true;
	}
}

static void moveMouseTgt ( int dx, int dy ) {
	xMouseTgt += dx;
	yMouseTgt += dy;
}

static void postMouseClicked ( Object source, long when, int x, int y,
			                  int modifiers, int clickCount, int button ) {
	MouseEvt e = getEvent( (Component)source, MOUSE_CLICKED, when,
			                                         modifiers, x, y, clickCount,
			                                         (button == 3));		
	e.button = button;
	
	AWTEvent.sendEvent( e, false);
}

protected void recycle () {
	synchronized ( MouseEvt.class ) {
		source = null;

		next  = cache;	
		cache = this;
	}
}

static void releaseMouse () {
	Cursor curs;
	Component grab;

	synchronized ( MouseEvt.class ) {
		if ( !mouseGrabbed )
			return;

		grabStack.pop();
		
		if ( grabStack.isEmpty() ){
			mouseTgt = null;
			xMouseTgt = yMouseTgt = 0;
			curs = nativeSource.cursor;
			mouseGrabbed = false;
		}
		else {
			grab = (Component)grabStack.peek();
			xMouseTgt = grab.x - nativeSource.x;
			yMouseTgt = grab.y - nativeSource.y;
			mouseTgt  = grab;
			curs = grab.cursor;
		}

		nativeSource.setNativeCursor( curs);
	}
}

static void sendMouseEnterEvent ( Component src, int x, int y, boolean sync ) {
	MouseEvt e = getEvent( src, MouseEvent.MOUSE_ENTERED,
	                              System.currentTimeMillis(),
	                              0, x, y,
	                              0, false);
	
	if ( sync )
		e.dispatch();
	else
		Toolkit.eventQueue.postEvent( e);
}

void setMouseEvent ( long time, int mods, int xNew, int yNew, int click, boolean popUp ) {
	when = time;
	modifiers = mods;
	x = xNew;
	y = yNew;
	clickCount = click;
	isPopupTrigger = popUp;
}

protected void setXY ( int xNew, int yNew ) {
	x = xNew;
	y = yNew;
}

static void transferMouse ( MouseEvt e,
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
	int    origX      = e.x;
	int    origY      = e.y;

	// Work out depths of from and to and adjust arrays accordingly.
	// (we have to inform all non-shared parents, so there is no way
	// around it)
	int df = 1;
	for (Component c = from; c != null; c = c.parent, df++);
	int dt = 1;
	for (Component c = to; c != null; c = c.parent, dt++);
	if (dt > df) {
		df = dt;
	}
	if (df > cFrom.length) {
		cFrom = new Component[df];
		cTo = new Component[df];
	}

	// init the from[], to[] stacks and get the first common parent (if any)
	for ( n=0, cFrom[0] = from; from != null; from = from.parent ) {
		cFrom[++n] = from;
	}
	for ( m=0, cTo[0] = to; to != null; to = to.parent ) {
		cTo[++m] = to;
	}
	for ( ; cFrom[n] == (to = cTo[m]); n--, m-- ) {
		cFrom[n] = null;
		cTo[m] = null;
	}

	// send EXITED to all from[] components (inside out)
	e.id = MouseEvent.MOUSE_EXITED;
	for ( i=1; i <= n; i++ ) {
		e.setXY( x, y);
		from = cFrom[i];
		e.source = from;
		
	  //System.out.println( "mouseExit:  " + from.getClass() + ": " + x + ',' + y);
		from.processEvent( e);
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
		to = cTo[i];
		e.source = to;
		
		x -= to.x;
		y -= to.y;

		e.x = x;
		e.y = y;
		//System.out.println( "mouseEnter: " + to.getClass() + ": " + x + ',' + y);
		to.processEvent( e);
	}

	// set cursor
	if ( to != null ){
		to.setCursor( to.cursor);
	}

	// clean up stacks (to avoid memory leaks)
	for ( i=0; i<n; i++ ) {
		cFrom[i] = null;
	}
	for ( i=0; i<m; i++ ) {
		cTo[i] = null;
	}
	
	// restore state
	e.source = origSource;
	e.id     = origId;
	e.x      = origX;
	e.y      = origY;
}

static int updateInputModifier ( int button, boolean pressed ) {
	if ( pressed ) {
		switch ( button ) {
		case 1:  inputModifier |= BUTTON1_MASK; break;
		case 2:  inputModifier |= BUTTON2_MASK; break;
		case 3:  inputModifier |= BUTTON3_MASK;
		}
	}
	else {
		switch ( button ) {
		case 1:  inputModifier &= ~BUTTON1_MASK; break;
		case 2:  inputModifier &= ~BUTTON2_MASK; break;
		case 3:  inputModifier &= ~BUTTON3_MASK;
		}
	}
	
	return inputModifier;
}
}
