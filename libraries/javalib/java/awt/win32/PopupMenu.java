package java.awt;

import java.awt.Component;
import java.awt.Menu;
import java.awt.Toolkit;


/**
 * class PopupMenu -
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
public class PopupMenu
  extends Menu
{
public PopupMenu() {
	super( "");
}

PopupMenu ( Menu m) {
	super(m.label);
	items = m.items;
}

public PopupMenu( String label) {
	super( label);
}

public void show( Component c, int x, int y) {
	owner = c;
	parent = null;

	addNotify();

	for ( ; c != null; c = c.parent) {
		if ( c instanceof NativeContainer ) {
			Toolkit.menuAssign( ((NativeContainer)c).nativeData, nativeData, x, y);
			return;
		}
		else if ( c instanceof NativeComponent ) {
			Toolkit.menuAssign( ((NativeComponent)c).nativeData, nativeData, x, y);
			return;
		}
		x += c.x;
		y += c.y;
	}
}
}
