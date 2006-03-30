package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.PaintEvent;
import java.awt.event.WindowEvent;
import java.awt.event.TextEvent;
import java.awt.event.FocusEvent;
import java.awt.event.InputMethodEvent;
import java.awt.event.AdjustmentEvent;
import java.awt.event.ContainerEvent;
import java.awt.event.InvocationEvent;
import java.awt.event.ItemEvent;
import java.awt.event.KeyEvent;
import java.util.EventObject;

import org.kaffe.awt.FocusHook;
import org.kaffe.awt.KeyHook;
import org.kaffe.awt.MouseHook;

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
	public static final long COMPONENT_EVENT_MASK = 0x01;
	public static final long CONTAINER_EVENT_MASK = 0x02;
	public static final long FOCUS_EVENT_MASK = 0x04;
	public static final long KEY_EVENT_MASK = 0x08;
	public static final long MOUSE_EVENT_MASK = 0x10;
	public static final long MOUSE_MOTION_EVENT_MASK = 0x20;
	public static final long WINDOW_EVENT_MASK = 0x40;
	public static final long ACTION_EVENT_MASK = 0x80;
	public static final long ADJUSTMENT_EVENT_MASK = 0x100;
	public static final long ITEM_EVENT_MASK = 0x200;
	public static final long TEXT_EVENT_MASK = 0x400;
        public static final long INPUT_METHOD_EVENT_MASK = 0x800;
	public static final int RESERVED_ID_MAX = 1999;
	  /**
   * Mask for paint events.
   * @since 1.3
   */
  public static final long PAINT_EVENT_MASK = 0x02000;

  /**
   * Mask for invocation events.
   * @since 1.3
   */
  public static final long INVOCATION_EVENT_MASK = 0x04000;
  
    /**
   * Mask for mouse wheel events.
   * @since 1.4
   */
  public static final long MOUSE_WHEEL_EVENT_MASK = 0x20000;

  /**
   * Mask for window state events.
   * @since 1.4
   */
  public static final long WINDOW_STATE_EVENT_MASK = 0x40000;
  
    /**
   * Mask for window focus events.
   * @since 1.4
   */
  public static final long WINDOW_FOCUS_EVENT_MASK = 0x80000;
  
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

  /**
   * Converts an event id to the appropriate event mask.
   *
   * @param id the event id
   *
   * @return the event mask for the specified id
   */
  static long eventIdToMask(int id)
  {
    long mask = 0;
    switch (id)
    {
      case ActionEvent.ACTION_PERFORMED:
        mask = ACTION_EVENT_MASK;
        break;
      case AdjustmentEvent.ADJUSTMENT_VALUE_CHANGED:
        mask = ADJUSTMENT_EVENT_MASK;
        break;
      case ComponentEvent.COMPONENT_MOVED:
      case ComponentEvent.COMPONENT_RESIZED:
      case ComponentEvent.COMPONENT_SHOWN:
      case ComponentEvent.COMPONENT_HIDDEN:
        mask = COMPONENT_EVENT_MASK;
        break;
      case ContainerEvent.COMPONENT_ADDED:
      case ContainerEvent.COMPONENT_REMOVED:
        mask = CONTAINER_EVENT_MASK;
        break;
      case FocusEvent.FOCUS_GAINED:
      case FocusEvent.FOCUS_LOST:
        mask = FOCUS_EVENT_MASK;
        break;
      case InputMethodEvent.INPUT_METHOD_TEXT_CHANGED:
      case InputMethodEvent.CARET_POSITION_CHANGED:
        mask = INPUT_METHOD_EVENT_MASK;
        break;
      case InvocationEvent.INVOCATION_DEFAULT:
        mask = INVOCATION_EVENT_MASK;
        break;
      case ItemEvent.ITEM_STATE_CHANGED:
        mask = ITEM_EVENT_MASK;
        break;
      case KeyEvent.KEY_TYPED:
      case KeyEvent.KEY_PRESSED:
      case KeyEvent.KEY_RELEASED:
        mask = KEY_EVENT_MASK;
        break;
      case MouseEvent.MOUSE_CLICKED:
      case MouseEvent.MOUSE_PRESSED:
      case MouseEvent.MOUSE_RELEASED:
        mask = MOUSE_EVENT_MASK;
        break;
      case MouseEvent.MOUSE_MOVED:
      case MouseEvent.MOUSE_ENTERED:
      case MouseEvent.MOUSE_EXITED:
      case MouseEvent.MOUSE_DRAGGED:
        mask = MOUSE_MOTION_EVENT_MASK;
        break;
      case MouseEvent.MOUSE_WHEEL:
        mask = MOUSE_WHEEL_EVENT_MASK;
        break;
      case PaintEvent.PAINT:
      case PaintEvent.UPDATE:
        mask = PAINT_EVENT_MASK;
        break;
      case TextEvent.TEXT_VALUE_CHANGED:
        mask = TEXT_EVENT_MASK;
        break;
      case WindowEvent.WINDOW_OPENED:
      case WindowEvent.WINDOW_CLOSING:
      case WindowEvent.WINDOW_CLOSED:
      case WindowEvent.WINDOW_ICONIFIED:
      case WindowEvent.WINDOW_DEICONIFIED:
      case WindowEvent.WINDOW_ACTIVATED:
      case WindowEvent.WINDOW_DEACTIVATED:
        mask = WINDOW_EVENT_MASK;
        break;
      case WindowEvent.WINDOW_GAINED_FOCUS:
      case WindowEvent.WINDOW_LOST_FOCUS:
        mask = WINDOW_FOCUS_EVENT_MASK;
        break;
      case WindowEvent.WINDOW_STATE_CHANGED:
        mask = WINDOW_STATE_EVENT_MASK;
        break;
      default:
        mask = 0;
    }
    return mask;
  }

}
