/*
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

package java.awt;

import java.awt.event.MouseEvent;
import java.util.Stack;

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
	static Point pressedPos = new Point();

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

	// note that the JDK allows components which are not isFocusTraversable() to gain
	// the focus by means of explicit requestFocus() (not very intuitive) OR by means of
	// mouseclicks (even on components which are not mouse aware, which sounds silly)

/* XXX
	if ( ((newKeyTgt.flags & Component.IS_NATIVE_LIKE) != 0) && newKeyTgt.isFocusTraversable() )
		newKeyTgt.requestFocus();	
 */
}

static Component computeMouseTarget ( Container toplevel, int x, int y ) {
	Container  cntr;
	Component  c, tgt;
	int        i, xm = 0, ym = 0;
	
	tgt = cntr = toplevel;

	// This is a workaround for no-native-wm Frames with childs extending the Frame size
	// (the Frame deco border is no separate Component, just "protected" by the Frame insets)
	if ( (root != null) && (x < cntr.deco.x) || (y < cntr.deco.y) ||
			 (x > (cntr.width - cntr.insets.right)) || (y > (cntr.height - cntr.insets.bottom)) ){
		return cntr;
	}

	i = 0;
	while (i < cntr.nChildren) {
		c = cntr.children[i];

		if ( ((c.flags & Component.IS_SHOWING) == Component.IS_SHOWING) &&
		     (x >= c.x) && (y >= c.y) && (x <= (c.x+c.width)) && (y <= (c.y+c.height)) ) {

			int u = x - c.x;
			int v = y - c.y;

			if ( c.contains( u, v) ){  // contains() might be reimplemented

				xm += c.x; ym += c.y;

				// IS_MOUSE_AWARE is a uggly construct to sum up the nice little chain of discriminants:
				//     !disabled && (mouseListener || motionListener || mouseEventMask || motionEventMask || oldEvents)
				// We assume here that disabled comps don't emit events

				// If it's a container, reset loop for new
				// container and continue.
				if ( c instanceof Container ){
					// reset loop for new container
					tgt = cntr = (Container) c;					
					x = u;
					y = v;
					i = 0;

					continue;
				}
				else {
					// Else it's a leaf of the component tree,
					// so it gets the event, whether it cn handle
					// it, or not.
					tgt = c;

					break;
				}
			}
		}
		i++;
	}

	xMouseTgt = xm;
	yMouseTgt = ym;

	return tgt;
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

	//--- the first phase handles mode setting and retargeting
	if ( mouseGrabbed || mouseDragged || (buttonPressed && (id == MOUSE_MOVED)) ) {
		// These modes (grab is a generalized drag) don't change the mouseTgt
		// Note that the first mouseMove after a press is already handled like a drag
		// (which is initiated by this), but releases might get a different mouseTgt
		if ( source != AWTEvent.mouseTgt ) {
			x -= xMouseTgt;
			y -= yMouseTgt;
			source = AWTEvent.mouseTgt;
		}
	}
	else if ( id == MOUSE_CLICKED ){
		// that one was synthetic, don't change any event fields
	}
	else {
		// determine new target and do exit/enter processing
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
		if ( newTgt != AWTEvent.mouseTgt ) {
			transferMouse( this, AWTEvent.mouseTgt, mousePos.x, mousePos.y, newTgt, x, y);
		}

		AWTEvent.mouseTgt = newTgt;
	}

	//--- now dispatch all the remaining events
	if ( AWTEvent.mouseTgt != null ) {
		switch ( id ) {
		
		case MOUSE_MOVED:
			if ( buttonPressed ){
				mouseDragged = true;
				id = MOUSE_DRAGGED;
			}
			AWTEvent.mouseTgt.processMotion( this);
		  break;
		
		case MOUSE_PRESSED:
			if ( (AWTEvent.mouseTgt != AWTEvent.keyTgt) )  // decide upon focus policy
				clickToFocus( AWTEvent.mouseTgt);
			
			buttonPressed = true;
			modifiers = updateInputModifier( button, true);
			
			// save current Position so that we can later-on check against ClickDistance
			pressedPos.x = x;
			pressedPos.y = y;

			if ( when - lastPressed < clickInterval )
				clicks++;
			else
				clicks = 1;

			clickCount = clicks;
			lastPressed = when;

			AWTEvent.mouseTgt.processMouse( this);

			// some apps might react on isPopupTrigger() for both pressed + released
			isPopupTrigger = false;
		  break;
		
		case MOUSE_RELEASED:
			clickCount = clicks;
			buttonPressed = false;

			updateInputModifier( button, false);

			if ( mouseDragged ) {
				// check if we have to generate a enter event for a sibling
				sendMouseEnterEvent( nativeSource, nativePos.x, nativePos.y, false);
				mouseDragged = false;
			}

			// some display systems can't position the mouse exactly - we tolerate
			// every release within the Defaults.ClickDistance radius as a valid click
			if ( !mouseDragged ||
			     ((Math.abs(pressedPos.x - x) <= Defaults.ClickDistance) &&
					  (Math.abs(pressedPos.y - y) <= Defaults.ClickDistance))   ) {
				// we can't do this sync because of possible recursion, but we
				// need to provide a clicked event as the next event to follow
				// a MOUSE_RELEASE
				postMouseClicked( AWTEvent.mouseTgt, when, x, y, modifiers, clickCount, button);
			}

			AWTEvent.mouseTgt.processMouse( this);
		  break;
		
		case MOUSE_CLICKED:
			clickCount = clicks;
			AWTEvent.mouseTgt.processMouse( this);
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

	if ( (Defaults.RecycleEvents & AWTEvent.MOUSE_EVENT_MASK) != 0 )	recycle();
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
		e.consumed = false;
		e.when = when;
		e.modifiers = mods;
		e.x = x;
		e.y = y;
		e.clickCount = clickCount;
		e.isPopupTrigger = isPopupTrigger;
		e.button = 0;

		if ( mods != 0 ) {
			if ( (mods & BUTTON1_MASK) != 0 )
				e.button = 1;
			else if ( (mods & BUTTON2_MASK) != 0 )
				e.button = 2;
			else if ( (mods & BUTTON3_MASK) != 0 )
				e.button = 3;
		}
		
		return e;
	}
}

static synchronized MouseEvt getEvent ( int srcIdx, int id, int button, int x, int y ) {
	MouseEvt    e;
	Component   source = sources[srcIdx];
	long        when = System.currentTimeMillis();
	boolean     popupTrigger = (button == 3) && (id == MOUSE_PRESSED);
	
	if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ) {
		Rectangle   d = source.deco;
		x += d.x;
		y += d.y;
	}

	nativeSource = source;
	nativePos.x = x;
	nativePos.y = y;

	if ( cache == null ) {
		e = new MouseEvt( source, id, when, inputModifier, x, y, 0, popupTrigger);
	}
	else {
		e = cache;
		cache = (MouseEvt) e.next;
		e.next = null;
			
		e.source     = source;
		e.id         = id;
		e.consumed   = false;
		e.when       = when;
		e.modifiers  = inputModifier;
		e.x          = x;
		e.y          = y;
		e.clickCount = 0;
		e.isPopupTrigger = popupTrigger;	
	}

	e.button = button;

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// this is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer
		Toolkit.eventQueue.postEvent( e);
	}

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
		AWTEvent.mouseTgt  = grab;

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

static void releaseMouse ( Component c ) {
	Cursor curs = null;
	Component grab = (Component)grabStack.peek();

	synchronized ( MouseEvt.class ) {
		if ( !mouseGrabbed )
			return;

		while ( grab != c ) {
			grabStack.pop();
		
			if ( grabStack.isEmpty() ){
				AWTEvent.mouseTgt = null;
				xMouseTgt = yMouseTgt = 0;
				mouseGrabbed = false;
				curs = nativeSource.cursor;
				break;
			}

			grab = (Component)grabStack.peek();
			xMouseTgt = grab.x - nativeSource.x;
			yMouseTgt = grab.y - nativeSource.y;
			AWTEvent.mouseTgt  = grab;
			curs = grab.cursor;
		}
		
		if ( curs != null )
			nativeSource.setNativeCursor( curs);	
	}
}

protected MouseEvent retarget ( Component target, int dx, int dy ) {
	source = target;
	x += dx;
	y += dy;
	
	return this;
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
		from.processMouse( e);
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
		to.processMouse( e);
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
