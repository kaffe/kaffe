/**
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

package java.awt;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.Vector;

class BarMenu
  extends Component
{
	static int Dx = 5;
	Menu selection;
	MenuBar mb;
	PopupMenu current;

class BarMenuListener
  implements KeyListener, MouseListener, MouseMotionListener
{
public BarMenuListener () {
	addMouseListener( this);
	addMouseMotionListener( this);
	addKeyListener( this);
}

public void keyPressed ( KeyEvent evt ) {
	int cc = evt.getKeyCode();
	switch( cc) {
		case KeyEvent.VK_ENTER:
			processSelection();
			break;
		case KeyEvent.VK_LEFT:
			selectPrev();
			break;
		case KeyEvent.VK_RIGHT:
			selectNext();
			break;
	}
}

public void keyReleased ( KeyEvent evt ) {
}

public void keyTyped ( KeyEvent evt ) {
}

public void mouseClicked ( MouseEvent evt ) {
	//processSelection();
}

public void mouseDragged ( MouseEvent evt ) {
}

public void mouseEntered ( MouseEvent evt ) {
	//requestFocus();
	selectMenu( menuAt( evt.getX() ));
}

public void mouseExited ( MouseEvent evt ) {
	selectMenu( null);
}

public void mouseMoved ( MouseEvent evt ) {
	selectMenu( menuAt( evt.getX() ));
}

public void mousePressed ( MouseEvent evt ) {
	processSelection();
}

public void mouseReleased ( MouseEvent evt ) {
}
}

public BarMenu ( MenuBar mb) {
	this.mb = mb;
	mb.bMenu = this;
	
	fgClr = Defaults.MenuTxtClr;
	bgClr = Defaults.MenuBgClr;
	font  = Defaults.MenuFont;

	new BarMenuListener();
}

public void addNotify () {
	super.addNotify();

	mb.propagateOldEvents( oldEvents);
}

void disposeCurrent () {
	if ( current != null){
		current.dispose();
		current = null;
	}
}

public Graphics getGraphics () {
	// we can't use Component.getGraphics() here, because this is clipped on the
	// insetted parents. Since Frame insets include the BarMenu, we need to bypass
	// this (to avoid getting clipped away)
	return NativeGraphics.getGraphics( parent, ((Window)parent).nativeData,
	                                   NativeGraphics.TGT_TYPE_WINDOW,
	                                   0, 0,
	                                   0, 0, width, height,
	                                   parent.fgClr, parent.bgClr, parent.font, false);
}

int getX ( Menu m) {
	int sz = mb.menus.size();
	int x0 = 0;
	
	for ( int i=0; i<sz; i++) {
		Menu cm = (Menu)mb.menus.elementAt( i);
		if ( cm == m)
			return x0;
		x0 += cm.getWidth()+2*Dx;
	}
	
	return -1;		
}

Menu menuAt ( int x) {
	int sz = mb.menus.size();
	int x0 = Dx;
	
	for ( int i=0; i<sz; i++) {
		Menu m = (Menu)mb.menus.elementAt( i);
		int mw = m.getWidth();
		x0 += mw+Dx;
		if ( x0 > x)
			return m;
		x0 += Dx;
	}
	
	return null;
}

public void paint ( Graphics g) {
	int sz = mb.menus.size();
	int x0 = Dx;
	int y0;

	g.setColor( getBackground());
	g.fill3DRect( 0, 0, width, height, true);
	
	for ( int i=0; i<sz; i++) {
		Menu m = (Menu)mb.menus.elementAt( i);
		int mw = m.getWidth();
		
		y0 = (height - m.fm.getHeight()) / 2;
		m.paint( g, x0, y0, mw+2*Dx, bgClr, fgClr, selection == m);
		x0 += mw+2*Dx;
	}
}

void paintMenu ( Graphics g, Menu m) {
	int sz = mb.menus.size();
	int x0 = Dx;
	int y0;
	
	for ( int i=0; i<sz; i++) {
		Menu cm = (Menu)mb.menus.elementAt( i);
		int mw = cm.getWidth();
		y0 = (height - cm.fm.getHeight()) / 2;

		if ( m == cm) {
			cm.paint( g, x0, y0, mw+2*Dx, bgClr, fgClr, selection == cm);
			return;
		}
		x0 += mw+2*Dx;
	}
		
}

void processSelection () {
	if ( selection != null) {
		disposeCurrent();
		if ( selection.getItemCount() > 0) {
			current = new PopupMenu( selection);
			int x = getX( selection) + 1;
			int y = height + 1;
			current.show( this, x, y);
			current.requestFocus();
		}
		else
			selection.process();
	}
		
}

void selectMenu ( Menu m) {
	Menu ms;
	Graphics g = getGraphics();

	if ( m == null) {
		if ( selection != null) {
			ms = selection;
			selection = null;
			paintMenu( g, ms);
		}
	}
	else if ( m != selection) {
		ms = selection;
		selection = m;
		paintMenu( g, ms);
		paintMenu( g, selection);
	}

	g.dispose();
}

void selectNext () {
	try {
		if ( selection == null)
			selectMenu( (Menu)mb.menus.firstElement());
		else {
			int nIdx = mb.menus.indexOf( selection) + 1;
			selectMenu( (Menu)mb.menus.elementAt( nIdx));
		}
	}
	catch ( Exception e) {}
}

void selectPrev () {
	try {
		if ( selection == null)
			selectMenu( (Menu)mb.menus.lastElement());
		else {
			int nIdx = mb.menus.indexOf( selection) - 1;
			selectMenu( (Menu)mb.menus.elementAt( nIdx));
		}
	}
	catch ( Exception e) {}
}

void setMenus ( MenuBar mb) {
	this.mb = mb;
	if ( isShowing() )
		repaint();
}
}
