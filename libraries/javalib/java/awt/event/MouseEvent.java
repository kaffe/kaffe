package java.awt.event;

import java.awt.Component;
import java.awt.Container;
import java.awt.Point;
import java.awt.Toolkit;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
public class MouseEvent
  extends InputEvent
{
	protected int x;
	protected int y;
	protected int clickCount;
	protected boolean isPopupTrigger;
	int button;
	final public static int MOUSE_FIRST = 500;
	final public static int MOUSE_LAST = 506;
	final public static int MOUSE_CLICKED = MOUSE_FIRST;
	final public static int MOUSE_PRESSED = MOUSE_FIRST + 1;
	final public static int MOUSE_RELEASED = MOUSE_FIRST + 2;
	final public static int MOUSE_MOVED = MOUSE_FIRST + 3;
	final public static int MOUSE_ENTERED = MOUSE_FIRST + 4;
	final public static int MOUSE_EXITED = MOUSE_FIRST + 5;
	final public static int MOUSE_DRAGGED = MOUSE_FIRST + 6;
	static long lastPressed;
	static int clicks;

public MouseEvent ( Component src, int evtId, long time, int modifiers,
	     int x, int y, int clickCount, boolean isPopupTrigger) {
	super( src, evtId);
	
	this.when = when;
	this.modifiers = modifiers;

	this.x = x;
	this.y = y;
	this.clickCount = clickCount;
	this.isPopupTrigger = isPopupTrigger;
}

protected void dispatch () {
	Component newTgt;

	if ( mouseDragged ) {
		Object newTop = source;
		int    newX = x, newY = y;
	
		// we might get here in a pseudo drag if a PopupWindow grabbed the mouse
		// (a native click release has another source than a drag release!)
		if ( source != mouseTgt ) {
			x -= xMouseTgt;
			y -= yMouseTgt;
			source = mouseTgt;
		}

		if ( id == MOUSE_MOVED ) {
			id = MOUSE_DRAGGED;
			processMouseMotionEvent( this);
		}
		else if ( id == MOUSE_RELEASED ) {
			accelHint = false;
			updateInputModifier( button, false);

			buttonPressed = false;
			processMouseEvent( this);
			mouseDragged = false;
				
			x = newX;
			y = newY;
			if ( (newTgt = computeMouseTarget( (Container)newTop, x, y)) != mouseTgt ) {
				x -= xMouseTgt;
				y -= yMouseTgt;
				transferMouse( this, mouseTgt, mousePos.x, mousePos.y, newTgt, x, y);
				mouseTgt = newTgt;
			}
		}
	}
	else {
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
		if ( newTgt != mouseTgt )
			transferMouse( this, mouseTgt, mousePos.x, mousePos.y, newTgt, x, y);

		mouseTgt = newTgt;

		if ( newTgt == null ) {              // toplevel exit
			mousePos.x = mousePos.y = 0;
			source = null;
			return;
		}

		if ( id == MOUSE_MOVED ) {
			if ( buttonPressed ) {
				mouseDragged = true;
				id = MOUSE_DRAGGED;
			}
			processMouseMotionEvent( this);
		}
		else if ( id == MOUSE_PRESSED ) {
			checkPopup( mouseTgt);         // the old highlander theme..
			if ( (mouseTgt != keyTgt) )    // pass this to java.awt to decide upon policy
				clickToFocus( newTgt);
			
			buttonPressed = true;
			modifiers = updateInputModifier( button, true);

			if ( when - lastPressed < clickInterval )
				clicks++;
			else
				clicks = 1;

			clickCount = clicks;
			lastPressed = when;
			
			processMouseEvent( this);
			
			// some apps might react on isPopupTrigger() for both pressed + released
			isPopupTrigger = false;
		}
		else if ( id == MOUSE_RELEASED ) {
			accelHint = false;
			clickCount = clicks;
			updateInputModifier( button, false);

			// we can't do this sync because of possible recursion, but we need to provide
			// a clicked event as the next event to follow a MOUSE_RELEASE
			postMouseClicked( mouseTgt, when, x, y, modifiers, clickCount, button);				
			buttonPressed = false;
			processMouseEvent( this);
		}
		else if ( id == MOUSE_CLICKED ) {
			clickCount = clicks;
			processMouseEvent( this);
		}
	}

	// mousePos (used by transferMouse) always contains the LAST mouse pos
	// in order to enable "clean" (non-continuos move) exits
	mousePos.x = x;
	mousePos.y = y;
	
	recycle();
}

public int getClickCount() {
	return clickCount;
}

static MouseEvent getMouseEvent ( int srcIdx, int id, int button, int x, int y ) {
	MouseEvent e;
	Component source = sources[srcIdx];
	long       when = System.currentTimeMillis();

	synchronized ( evtLock ) {
		if ( mouseEvtCache == null ) {
			e = new MouseEvent( source, id, when, inputModifier, x, y, 0, (button == 3));
		}
		else {
			e = mouseEvtCache;
			mouseEvtCache = (MouseEvent) e.next;
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
	}

	e.button = button;

	return e;
}

public Point getPoint() {
	return new Point( x, y);
}

public int getX() {
	return x;
}

public int getY() {
	return y;
}

public boolean isPopupTrigger() {
	return isPopupTrigger;
}

public String paramString() {
	String s;

	switch ( id ) {
	case MOUSE_PRESSED:		s = "MOUSE_PRESSED";		break;
	case MOUSE_RELEASED:	s = "MOUSE_RELEASED";		break;
	case MOUSE_CLICKED:		s = "MOUSE_CLICKED";		break;
	case MOUSE_ENTERED:		s = "MOUSE_ENTERED";		break;
	case MOUSE_EXITED:		s = "MOUSE_EXITED";		  break;
	case MOUSE_MOVED:		  s = "MOUSE_MOVED";		  break;
	case MOUSE_DRAGGED:		s = "MOUSE_DRAGGED";		break;
	default:		          s = "unknown type";
	}
	return s + ",(" + x + "," + y + ")" + ",mods=" + modifiers + ",clickCount="
     	     + clickCount;
}

static void postMouseClicked ( Object source, long when, int x, int y,
			                  int modifiers, int clickCount, int button ) {
	MouseEvent e = getMouseEvent( (Component)source, MOUSE_CLICKED, when,
			                          modifiers, x, y, clickCount,
			                          (button == 3));		
	e.button = button;
	
	Toolkit.getDefaultToolkit().getSystemEventQueue().postEvent( e);
}

protected void recycle () {
	synchronized ( evtLock ) {
		source = null;

		next = mouseEvtCache;	
		mouseEvtCache = this;
	}
}

protected void setMouseEvent ( long time, int mods, int xNew, int yNew, int click, boolean popUp ) {
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

public void translatePoint ( int x, int y ) {
	this.x += x;
	this.y += y;
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
