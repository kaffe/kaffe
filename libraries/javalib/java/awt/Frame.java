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

package java.awt;

import java.awt.event.InputEvent;
import java.util.Enumeration;
import kaffe.util.Ptr;

public class Frame
  extends Window
{
	String title;
	Image icon;
	boolean isResizable = true;
	BarMenu bMenu;

public Frame () {
	this( "");
}

Frame ( Frame owner, String title ) {
	super( owner);
	this.title = title;
}

public Frame ( String title ) {
	this.title = title;
}

Ptr createNativeWindow () {
	return Toolkit.wndCreateFrame( title, x, y, width, height,
	                               cursor.type, bgClr.nativeValue, isResizable);
}

public void doLayout () {
	super.doLayout();
	if ( bMenu != null)
		bMenu.setBounds( 0, 0, width-insets.right-insets.left, insets.top );
}

public Component getComponent( int index) {
	if ( children == null )
		return null;
	if ( bMenu == null )
		return children[index];
	
	return children[index+1];
}

public int getComponentCount() {
	if ( bMenu == null )
		return nChildren;
	
	return nChildren-1;
}

public Component[] getComponents() {
	if ( bMenu == null )
		return super.getComponents();
	
	Component ca[] = new Component[nChildren-1];
	
	if ( ca.length > 0 )
		System.arraycopy( children, 1, ca, 0, nChildren-1);
	
	return ca;
}

public Image getIconImage() {
	return icon;
}

public MenuBar getMenuBar () {
	return (bMenu != null) ? bMenu.mb : null;
}

public Dimension getPreferredSize () {
	Dimension d = super.getPreferredSize();
	
	d.width  += 2*Defaults.FrameBorderWidth;
	d.height += Defaults.TitleBarHeight + Defaults.BottomBarHeight;
	
	if ( bMenu != null )
		d.height += Defaults.MenuBarHeight;
		
	return d;
}

public String getTitle() {
	return title;
}

public boolean isResizable () {
	return isResizable;
}

public void remove ( MenuComponent mc ) {
}

public void setBounds ( int xNew, int yNew, int wNew, int hNew ) {
	// this is never called by a native toplevel resize

	x      = xNew;
	y      = yNew;
	width  = wNew;
	height = hNew;

	if ( nativeData != null )
		Toolkit.wndSetFrameBounds( nativeData, xNew, yNew, wNew, hNew);
	else
		doLayout();
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

	insets = new Insets( Defaults.MenuBarHeight, 0, 0, 0);
	
	if ( nativeData != null )
		doLayout();
		
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
