package java.awt;

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.Insets;
import java.awt.MenuBar;
import java.awt.MenuComponent;
import java.awt.Rectangle;
import java.awt.Toolkit;

/**
 * Frame - 
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
public class Frame
  extends Window
{
	String title;
	Image icon;
	MenuBar mBar;
	static Insets frameInsets;
	static Insets menuFrameInsets;
	static Rectangle frameDeco;

static {
	// these are just the guesses (to be overwritten by a subsequent
	// setFrameInsets() from the native layer)
	Insets guess = Defaults.FrameInsets;

	frameInsets = new Insets( guess.top, guess.left, guess.bottom, guess.right);
	menuFrameInsets = new Insets( guess.top + Defaults.MenuBarHeight, guess.left,
	                              guess.bottom, guess.right);	
	frameDeco = new Rectangle( guess.left, guess.top,
	                           (guess.left+guess.right), (guess.top+guess.bottom));

	Toolkit.wndSetFrameInsets( guess.top, guess.left, guess.bottom, guess.right);
}

public Frame () {
	this( null, null);
}

Frame ( Frame owner, String title ) {
	super( owner);

	flags |= IS_RESIZABLE;
	this.title = (title == null) ? "" : title;	
	deco = frameDeco;
}

public Frame ( String title ) {
	this( null, title);
}

void createNative() {
	// This is the terminal class addNotify() part. DANGER: ptr isn't a real object
	int u = x;
	int v = y;
	int w = width;
	int h = height;

	// Insets seem to be set by the JDK during addNotify
	// (no need to create fresh objects since they are insets()-copied anyway <sigh>).
	// Note that the global inset objects might be changed if the native layer
	// has to compute the real deco offsets during window creation
	insets = (mBar != null) ? menuFrameInsets : frameInsets;

	if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ) {
		// we just pretend to own the deco space, subtract it before going native
		u += deco.x;
		v += deco.y;
		w -= deco.width;
		h -= deco.height;
	}

	nativeData = Toolkit.wndCreateFrame( title, u, v, w, h,
	             		                 cursor.type, bgClr.getNativeValue(),
										 ((flags & IS_RESIZABLE) != 0));
}

void finishAddNotify() {
	if ( nativeData != null) {
		if ( mBar != null ){
			mBar.owner = this;
			mBar.addNotify();
			mBar.propagateOldEvents( ((flags & IS_OLD_EVENT) != 0));
			insets = menuFrameInsets;
			Toolkit.menuAssign( nativeData, mBar.nativeData, 0, 0);
		}
	}
}

/**
 * @deprecated, use Component.getCursor()
 */
public int getCursorType() {
	return (getCursor().getType());
}

public Image getIconImage() {
	return icon;
}

public MenuBar getMenuBar () {
	return mBar;
}

public String getTitle() {
	return title;
}

public boolean isResizable () {
	return ((flags & IS_RESIZABLE) != 0);
}

/**
 * @deprecated, use getPreferredSize()
 */
public Dimension preferredSize () {
	Dimension d = super.preferredSize();

	if ( mBar != null ) {
		d.height += Defaults.MenuBarHeight;
	}
	return (d);
}

public void remove ( MenuComponent mc ) {
}

/**
 * @deprecated, use Component.setCursor()
 */
public void setCursor(int cursorType) {
	setCursor(Cursor.getPredefinedCursor(cursorType));
}

static void setDecoInsets ( int top, int left, int bottom, int right, int srcIdx ){
	// this is the native callBack to set exact (calculated) Frame deco extends

	frameInsets.top    = top;
	frameInsets.left   = left;
	frameInsets.bottom = bottom;
	frameInsets.right  = right;

	menuFrameInsets.top  = top + Defaults.MenuBarHeight;
	menuFrameInsets.left = left;
	menuFrameInsets.bottom = bottom;
	menuFrameInsets.right = right;

	frameDeco.x = left;
	frameDeco.y = top;
	frameDeco.width = left + right;
	frameDeco.height = top + bottom;

	// if we got the correction in the context of a initial Window positioning
	// we have to make sure a subsequent ComponentEvt.getEvent() invalidates
	// this instance (which wouldn't be the case if we let its (faked) dimension alone)
	if ( srcIdx != -1 ) {
		Component src = AWTEvent.sources[srcIdx];
		src.width = src.height = 0;
	}
}

public void setIconImage ( Image icon ) {
	this.icon = icon;

	if ( nativeData != null )
		Toolkit.wndSetIcon( nativeData, icon.nativeData);
}

public void setMenuBar ( MenuBar mb ) {

	mBar = mb;

	if ( nativeData != null ) {
		if ( mBar != null ) {
			insets = menuFrameInsets;
			mb.owner = this;
			mb.addNotify();
			Toolkit.menuAssign( nativeData, mb.nativeData, 0, 0);
		}
		else {
			insets = frameInsets;
			Toolkit.menuAssign( nativeData, null, 0, 0);
		}
		doLayout();
	}

}

public void setResizable ( boolean isResizable ) {
	if ( isResizable )
		flags |= IS_RESIZABLE;
	else
		flags &= ~IS_RESIZABLE;

	if ( nativeData != null )
		Toolkit.wndSetResizable( nativeData, isResizable, x, y, width, height);
}

public void setTitle ( String newTitle ) {
	title = newTitle;

	if ( nativeData != null )
		Toolkit.wndSetTitle( nativeData, newTitle);
}
}
