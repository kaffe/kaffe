package java.awt;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
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
	static Window current;

class PopupListener
  implements MouseListener, MouseMotionListener, KeyListener
{
public PopupListener () {
	addMouseListener( this);
	addMouseMotionListener( this);
	addKeyListener( this);
}

public void keyPressed ( KeyEvent evt ) {
	int cc = evt.getKeyCode();
	switch( cc) {
		case KeyEvent.VK_ENTER:
			processSelection();
			evt.consume();
			break;
		case KeyEvent.VK_UP:
			if ( selectPrev() )
				evt.consume();
			break;
		case KeyEvent.VK_DOWN:
			if ( selectNext() )
				evt.consume();
			break;
		case KeyEvent.VK_ESCAPE:
			disposeAll();
			evt.consume();
			break;
		case KeyEvent.VK_LEFT:
			if ( selectUpper() )
				evt.consume();
			break;
		case KeyEvent.VK_RIGHT:
			if ( selectLower() )
				evt.consume();
			break;
	}
}

public void keyReleased ( KeyEvent evt ) {
}

public void keyTyped ( KeyEvent evt ) {
}

public void mouseClicked ( MouseEvent evt ) {
}

public void mouseDragged ( MouseEvent evt ) {
	int u = evt.getX();
	int v = evt.getY();

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
			if ( master.contains( u, v) && (master.itemAt( v) != master.selection) )
				master.disposeSubMenus();
		}
	}
}

public void mouseEntered ( MouseEvent evt ) {
	requestFocus();
}

public void mouseExited ( MouseEvent evt ) {
}

public void mouseMoved ( MouseEvent evt ) {
	selectItem( itemAt( evt.getY() ));
}

public void mousePressed ( MouseEvent evt ) {
	selectItem( itemAt( evt.getY() ));
}

public void mouseReleased ( MouseEvent evt ) {
	if ( contains( evt.getX(), evt.getY()) && (selection != null) )  // action!
		processSelection();
	else {
		if ( PopupWindow.this == MouseEvt.getGrab() )
			MouseEvt.releaseMouse();
		else
			disposeAll();
	}
}
}

public PopupWindow ( Component client, Frame fr, Vector items) {
	super( fr);
	this.client = client;
	this.items  = items;
	
	new PopupListener();

	if ( !(client instanceof PopupWindow) )
		setPopup( this);
}

static boolean checkPopup ( Object src ) {
	if ( current == null ) {
		return true;
	}
	if ( (src != null) &&
	     (src instanceof PopupWindow) && (((PopupWindow)src).rootWnd() == current) ) {
		return true;
	}

	current.dispose();
	current = null;
	return false;
}

static boolean closePopup () {
	if ( current == null ) {
		return false;
	}

	current.dispose();
	current = null;
	return true;
}

public void dispose () {
	if ( !(client instanceof PopupWindow) )
		resetPopup( this);

	disposeSubMenus();		
	super.dispose();

	if ( this == MouseEvt.getGrab() )
		MouseEvt.releaseMouse();
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
	for ( PopupWindow p = sub; p != null; p = p.sub)
		p.dispose();
	sub = null;
}

int getY ( MenuItem mi) {
	int s = items.size();
	int y = 2;
	
	for ( int i=0; i<s; i++) {
		MenuItem cmi = (MenuItem)items.elementAt( i);
		if ( cmi == mi )
			return y;
		y += cmi.getHeight();
	}

	return -1;
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
	int x = xOffs;
	int y = 2;   // border width

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
	int ih = mi.paint( g, xOffs, y +2, width, bgClr, fgClr, mi == selection);

	// draw the submenu mark
	if ( mi instanceof Menu) {
		int my = y + ih/2;
		int mx = width - 4;
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

void processSelection () {
	if ( selection == null)
		return;
	if ( selection instanceof Menu){
		disposeSubMenus();
		openSubMenu();
	}
	else {
		disposeAll(); // clean up before doing any popup triggered actions
		selection.handleShortcut( null);
	}
}

static void resetPopup ( Window oldPopup ) {
	// just reset if this is the one that is still active
	if ( (current != null) && (current == oldPopup) ) {
		current = null;
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

	if ( selection != null){
		MenuItem cmi = selection;
		selection = null;
		paintItem( cmi, g, getY( cmi) );
	}
	
	selection = nmi;
	paintItem( selection, g, getY( selection) );
		
	g.dispose();
	return true;	
}

boolean selectLower() {
	if ( sub == null )
		return false;

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
	dispose();
	p.requestFocus();
	p.sub = null;
	
	return true;
}

static void setPopup ( Window newPopup ) {
	if ( current != null ) { // it has to be one
		current.dispose();
	}
	
	current = newPopup;
}
}
