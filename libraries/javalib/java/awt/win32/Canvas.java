package java.awt;

import java.awt.Toolkit;


/**
 * Canvas - 
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
public class Canvas
  extends NativeComponent
{
	final private static long serialVersionUID = -2284879212465893870L;

public Canvas() {
	// Canvases usually get their own update events, not being updated
	// sync within their parents
	flags |= IS_ASYNC_UPDATED | IS_BG_COLORED;
	bgClr = Defaults.WndBackground;
}

/**
 * Sun's version of this class apparently has this method. So we put
 * one here so applets compiled against kaffe's version of this class
 * will call the right method when they do super.addNotify().
 */
public void addNotify() {
	if ( nativeData == null ) {
		Toolkit.createNative(this);
		super.addNotify();
	}
}

void createNative() {
	nativeData = Toolkit.widgetCreateWidget( getParentData());
}

public boolean isFocusTraversable () {
	// for some obscure reason, Canvases are not focusTraversable by default
	return false;
}
}
