package java.awt;

import java.awt.event.InputEvent;
import java.util.Enumeration;
import kaffe.util.Ptr;

/**
 * Frame - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */
public class Frame
  extends Window
{
	String title;
	Image icon;
	boolean isResizable = true;
	BarMenu bMenu;
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
	
	this.title = (title == null) ? "" : title;	
	deco = frameDeco;
}

public Frame ( String title ) {
	this( null, title);
}

public int countComponents() {
	if ( bMenu == null )
		return nChildren;
	
	return nChildren-1;
}

Ptr createNativeWindow () {
	// This is the terminal class addNotify() part. DANGER: ptr isn't a real object

	// Insets seem to be set by the JDK during addNotify
	// (no need to create fresh objects since they are insets()-copied anyway <sigh>).
	// Note that the global inset objects might be changed if the native layer
	// has to compute the real deco offsets during window creation
	insets = (bMenu != null) ? menuFrameInsets : frameInsets;

	return Toolkit.wndCreateFrame( title, x + deco.x, y + deco.y,
                                 width - deco.width,
                                 height - deco.height,
	                               cursor.type, bgClr.nativeValue, isResizable);
}

public Component getComponent( int index) {
	if ( children == null )
		return null;
	if ( bMenu == null )
		return children[index];
	
	return children[index+1];
}

public Component[] getComponents() {
	if ( bMenu == null )
		return super.getComponents();
	
	Component ca[] = new Component[nChildren-1];
	
	if ( ca.length > 0 )
		System.arraycopy( children, 1, ca, 0, nChildren-1);
	
	return ca;
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
	return (bMenu != null) ? bMenu.mb : null;
}

public String getTitle() {
	return title;
}

public boolean isResizable () {
	return isResizable;
}

public void layout () {
	super.layout();

	if ( bMenu != null){
		bMenu.setBounds( frameDeco.x, frameDeco.y,
		                 width-(frameDeco.width), Defaults.MenuBarHeight);
  }		
}

/**
 * @deprecated, use getPreferredSize()
 */
public Dimension preferredSize () {
	Insets    in = Defaults.FrameInsets;
	Dimension d = super.preferredSize();
	
	d.width  += (in.left + in.right);
	d.height += (in.top + in.bottom);
	
	if ( bMenu != null ) {
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

static void setDecoInsets ( int top, int left, int bottom, int right ){
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
}

public void setIconImage ( Image icon ) {
	this.icon = icon;
	
	if ( nativeData != null )
		Toolkit.wndSetIcon( nativeData, icon.nativeData);
}

public void setMenuBar ( MenuBar mb ) {

	bMenu = new BarMenu( mb);
	
	// add BarMenu to children at position 0 ( without LayoutManager registration )
	if ( children == null )
		children= new Component[3];
	else if ( nChildren == children.length) {
		Component[] old = children;
		children = new Component[nChildren*2];
		System.arraycopy( old, 0, children, 1, nChildren);
	}
	else
		System.arraycopy( children, 0, children, 1, nChildren);
	
	children[0] = bMenu;
	nChildren++;
	bMenu.parent = this;

	if ( nativeData != null ) {
		insets = menuFrameInsets;
		doLayout();
	}
		
	for ( Enumeration e = mb.shortcuts(); e.hasMoreElements(); ) {
		MenuShortcut s = (MenuShortcut) e.nextElement();
		MenuItem mi = mb.getShortcutMenuItem( s);
		int mods = InputEvent.CTRL_MASK;
		if ( s.usesShiftMod )
			mods |= InputEvent.SHIFT_MASK;
		HotKeyHandler.addHotKey( bMenu, mi, s.key, mods, mi.getActionCommand() );
	}
}

public void setResizable ( boolean isResizable ) {
	this.isResizable = isResizable;
	
	if ( nativeData != null )
		Toolkit.wndSetResizable( nativeData, isResizable, x, y, width, height);
}

public void setTitle ( String newTitle ) {
	title = newTitle;
	
	if ( nativeData != null )
		Toolkit.wndSetTitle( nativeData, newTitle);
}
}
