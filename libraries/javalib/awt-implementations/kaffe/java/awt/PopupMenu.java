package java.awt;


/**
 * class PopupMenu -
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author J.Mehlitz
 */
public class PopupMenu
  extends Menu
{
	final private static long serialVersionUID = -4620452533522760060L;
	private static int counter;
	PopupWindow wnd;

public PopupMenu() {
	super( null);
}

PopupMenu ( Menu m) {
	this(m.getLabel());
	items = m.items;
}

public PopupMenu( String label) {
	super( label);

	setName("popup" + counter++);
}

void dispose() {
	if ( (wnd != null) && (wnd.isShowing() )) {
		wnd.disposeAll();
		wnd = null;
	}
}

void requestFocus() {
	if ( (wnd != null) && (wnd.isShowing() ))
		wnd.requestFocus();
}

public void show( Component c, int x, int y) {
	Component pc = c;
	for ( ; pc.parent != null; pc = pc.parent){
		x += pc.x; y += pc.y;
	}
	x += pc.x;
	y += pc.y;
	
	wnd = new PopupWindow( c, (pc instanceof Frame) ? (Frame)pc : null, items);
	wnd.popupAt( x, y);
	
//	wnd.requestFocus();
}
}
