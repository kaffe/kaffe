package java.awt;

import java.awt.event.FocusEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.util.Vector;

/**
 * class PopupWindow -
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author J.Mehlitz
 */
class PopupWindow
  extends Window
{
	MenuItem selection;
	static int xOffs = 8;
	Vector items;
	Component client;
	PopupWindow sub;
	static boolean skipRelease;

public PopupWindow ( Component client, Frame fr, Vector items) {
	super( fr);
	this.client = client;
	this.items  = items;
}

public void dispose () {
	disposeSubMenus();
	super.dispose();

	if ( this == MouseEvt.getGrab() )
		MouseEvt.releaseMouse( client);
}

void disposeAll() {
	// Window dispose will cause focus changes, save current state
	Component keyTgt = AWTEvent.keyTgt;

	PopupWindow p = rootWnd();
	p.dispose();

	// otherwise, the focus probably has been set explicitly (by a
	// triggered action)
	if ( keyTgt instanceof PopupWindow ){
		p.client.requestFocus();
	}
}

void disposeSubMenus() {
	PopupWindow pop;

	for ( pop = sub; pop != null; pop = pop.sub)
		pop.dispose();

	sub = null;
}

int getY ( MenuItem mi) {
	int s = items.size();
	int y = 1;

	for ( int i=0; i<s; i++) {
		MenuItem cmi = (MenuItem)items.elementAt( i);
		if ( cmi == mi )
			return y;
		y += cmi.getHeight();
	}

	return -1;
}

boolean isMaster ( Component c ) {
	PopupWindow pop = this;

	while ( pop.client instanceof PopupWindow ) {
		if ( pop.client == c )
			return true;
		pop = (PopupWindow) pop.client;
	}

	return false;
}

MenuItem itemAt ( int yPos) {
	int s = items.size();
	int y = 0;

	for ( int i=0; i<s; i++) {
		MenuItem mi = (MenuItem)items.elementAt( i);
		y += mi.getHeight();
		if ( y > yPos)
			return mi;
	}

	return null;
}

boolean openSubMenu () {
	if ( sub != null )
		return false;

	if ( ( selection == null) || ( ! (selection instanceof Menu) ) )
		return false;

	Vector v = ((Menu)selection).items;
	if ( (v == null) || (v.size() == 0) )
		return false;

	sub = new PopupWindow( this, owner, v );
	sub.popupAt( x+width, y+getY( selection));
	return true;
}

public void paint( Graphics g) {
	int s = items.size();
	int y = 1;   // border width

	g.setColor( Color.black);
	g.drawRect( 0, 0, width-1, height-1);
	g.setColor( bgClr);
	g.fill3DRect( 1, 1, width-2, height-2, true);

	for ( int i=0; i<s; i++) {
		MenuItem mi = (MenuItem)items.elementAt( i);
		y += paintItem( mi, g, y);
	}
}

int paintItem ( MenuItem mi, Graphics g, int y) {

	int ih = mi.paint( g, 1, xOffs, y, width-3, 0, bgClr, fgClr, mi == selection);

	// draw the submenu mark
	if ( mi instanceof Menu) {
		int my = y + ih/2;
		int mx = width - 5;
		for ( int i=0; i<4; i++)
			g.drawLine( mx-2*i, my-i, mx-2*i, my+i);
	}

	return ih;
}

public void popupAt ( int x, int y ) {
	Dimension wd = Toolkit.singleton.getScreenSize();
	Dimension md = getPreferredSize();

	if ( x + md.width > wd.width)
		x = wd.width - md.width;

	if ( y + md.height > wd.height)
		y = wd.height - md.height;

	setBounds( x, y, md.width, md.height);
	setVisible( true);

	if ( !(client instanceof PopupWindow) ){
		skipRelease = true;
		requestFocus();
	}

	// in case we still have a pressed button, let the popup grab the mouse
	// so that we can handle drags
	if ( MouseEvt.buttonPressed )
		MouseEvt.grabMouse( this);
}

/**
 * @deprecated, use getPreferredSize()
 */
public Dimension preferredSize() {
	int s = (items != null) ? items.size() : 0;
	int th = 0;
	int tw = 0;
	int cs = 2;

	for ( int i=0; i<s; i++) {
		MenuItem mi = (MenuItem)items.elementAt( i);
		if ( mi instanceof Menu)
			cs = 3;
		tw = Math.max( tw, mi.getWidth() );
		th += mi.getHeight();
	}

	return new Dimension( tw+cs*xOffs +4, th +4);  // border width
}

void process ( FocusEvent evt ) {
	if ( evt.id == evt.FOCUS_GAINED ){
		disposeSubMenus();
	}
	else if ( (AWTEvent.keyTgt != sub) && !isMaster(AWTEvent.keyTgt)) {
		disposeAll();
	}
}

void process ( KeyEvent evt ) {
	// we have to process our own keys first to make sure they don't get passed
	// to our owner
	switch ( evt.id ){
	case KeyEvent.KEY_PRESSED:
		switch( evt.getKeyCode() ) {
		case KeyEvent.VK_ENTER:
			processSelection();
			evt.consumed = true;
			break;
		case KeyEvent.VK_UP:
			if ( selectPrev() )
				evt.consumed = true;
			break;
		case KeyEvent.VK_DOWN:
			if ( selectNext() )
				evt.consumed = true;
			break;
		case KeyEvent.VK_ESCAPE:
			disposeAll();
			evt.consumed = true;
			break;
		case KeyEvent.VK_LEFT:
			if ( selectUpper() )
				evt.consumed = true;
			break;
		case KeyEvent.VK_RIGHT:
			if ( selectLower() )
				evt.consumed = true;
			break;
		}
	}

	super.process( evt);
}

void processMotion ( MouseEvent evt ) {
	int u = evt.getX();
	int v = evt.getY();
	MenuItem mi;

	switch ( evt.id ){

	case MouseEvent.MOUSE_MOVED:
		mi = itemAt( v);

		if ( (sub != null) && (mi != selection) )
			requestFocus();

		selectItem( mi);
		break;

	case MouseEvent.MOUSE_DRAGGED:
		skipRelease = false;

		if ( contains( u, v) ) {
			selectItem( itemAt( v ));

			if ( (selection != null) && (selection instanceof Menu) )
				openSubMenu();
		}
		else {
			if ( client instanceof PopupWindow ) {
				PopupWindow master = (PopupWindow) client;
				u -= (master.x - x);
				v -= (master.y - y);
				if ( master.contains( u, v) && (master.itemAt( v) != master.selection) ){
					master.disposeSubMenus();
				}
			}
		}
		break;
	}
}

void processMouse ( MouseEvent evt ) {
	int u = evt.getX();
	int v = evt.getY();

	super.processMouse( evt);

	switch ( evt.id ){

	case MouseEvent.MOUSE_PRESSED:
		selectItem( itemAt( v));
		break;

	case MouseEvent.MOUSE_RELEASED:
		if ( MouseEvt.getGrab() == this )
			MouseEvt.releaseMouse(null);

		if ( skipRelease ) {
			skipRelease = false;
			break;
		}

		if ( contains( u, v) && (selection != null) ){  // action!
			processSelection();
		}
		else {
			disposeAll();
		}
		break;

	case MouseEvent.MOUSE_ENTERED:
		requestFocus();
		break;
	}
}

void processSelection () {
	if ( selection == null)
		return;

	if ( selection instanceof Menu){
		selectLower();
	}
	else {
		disposeAll(); // clean up before doing any popup triggered actions
		selection.handleShortcut( null);
	}
}

PopupWindow rootWnd () {
	for ( Component c = this; c != null; ) {
		if ( c instanceof PopupWindow){
			PopupWindow pc = (PopupWindow) c;
			if ( ! (pc.client instanceof PopupWindow ) )
				return pc;
			c = pc.client;
		}
	}
	return null;
}

boolean selectItem ( MenuItem nmi) {
	if ( (nmi == null) || (nmi.isSeparator()) || (nmi == selection) )
		return false;

	Graphics g = getGraphics();
	if ( g != null ) {
		if ( selection != null){
			MenuItem cmi = selection;
			selection = null;
			paintItem( cmi, g, getY( cmi) );
		}

		selection = nmi;
		paintItem( selection, g, getY( selection) );

		g.dispose();
	}

	return true;
}

boolean selectLower() {
	if ( sub == null ) {
		if ( selection instanceof Menu){
			disposeSubMenus();
			openSubMenu();
		}
		else {
			return false;
		}
	}

	sub.requestFocus();
	Vector v = sub.items;
	if ( (v != null) && (v.size() > 0) ) {
		sub.selectItem ((MenuItem) v.firstElement());
		return true;
	}

	return false;
}

boolean selectNext () {
	int isz = (items != null) ? items.size() : 0;
	int ci = 0;

	if ( isz == 0 )
		return false;

	if ( selection != null )
		ci = items.indexOf( selection) + 1;

	for ( int idx = ci; idx<isz; idx++) {
		MenuItem mi = (MenuItem)items.elementAt( idx);
		if ( ! mi.isSeparator() ){
			selectItem( mi);
			return true;
		}
	}

	return false;
}

boolean selectPrev () {
	int isz = (items != null) ? items.size() : 0;
	int ci = isz-1;

	if ( isz == 0 )
		return false;

	if ( selection != null )
		ci = items.indexOf( selection) -1;

	for ( int idx = ci; idx>=0; idx--) {
		MenuItem mi = (MenuItem)items.elementAt( idx);
		if ( ! mi.isSeparator() ){
			selectItem( mi);
			return true;
		}
	}

	return false;
}

boolean selectUpper() {
	if ( ! (client instanceof PopupWindow))
		return false;

	PopupWindow p = (PopupWindow)client;
	p.requestFocus();

	return true;
}
}
