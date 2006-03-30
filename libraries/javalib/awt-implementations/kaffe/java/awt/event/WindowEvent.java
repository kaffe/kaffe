package java.awt.event;

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
public class WindowEvent
  extends ComponentEvent
{
	final public static int WINDOW_FIRST = 200;
	final public static int WINDOW_OPENED = 200;
	final public static int WINDOW_CLOSING = 201;
	final public static int WINDOW_CLOSED = 202;
	final public static int WINDOW_ICONIFIED = 203;
	final public static int WINDOW_DEICONIFIED = 204;
	final public static int WINDOW_ACTIVATED = 205;
	final public static int WINDOW_DEACTIVATED = 206;
	final public static int WINDOW_LAST = 206;
	
	  /**
   * This is the id for a window becoming the focused window.
   *
   * @since 1.4
   */
  public static final int WINDOW_GAINED_FOCUS = 207;

  /**
   * This is the id for a window losing all focus.
   *
   * @since 1.4
   */
  public static final int WINDOW_LOST_FOCUS = 208;

  /**
   * This is the id for a window state change, such as maximization.
   *
   * @since 1.4
   */
  public static final int WINDOW_STATE_CHANGED = 209;
  
	private static final long serialVersionUID = -1567959133147912127L;

public WindowEvent ( Window  src, int evtId ) {
	super( src, evtId);
}

public Window getWindow () {
	return (Window) source;
}

public String paramString() {
	switch ( id ) {   
	case WINDOW_OPENED:		    return "WINDOW_OPENED";
	case WINDOW_CLOSING:		  return "WINDOW_CLOSING";
	case WINDOW_CLOSED:		    return "WINDOW_CLOSED";
	case WINDOW_ICONIFIED:	  return "WINDOW_ICONIFIED";
	case WINDOW_DEICONIFIED:  return "WINDOW_DEICONIFIED";
	case WINDOW_ACTIVATED:		return "WINDOW_ACTIVATED";
	case WINDOW_DEACTIVATED:	return "WINDOW_DEACTIVATED";
	}
	
	return "?";
}
}
