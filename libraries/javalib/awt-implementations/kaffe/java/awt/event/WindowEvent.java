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
