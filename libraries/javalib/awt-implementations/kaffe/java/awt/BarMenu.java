package java.awt;

import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

/**
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class BarMenu
  extends Component
  implements ShortcutConsumer
{
	private static int Dx = 5;
	private Menu selection;
	MenuBar mb;
	private PopupMenu current;
	private WindowAdapter wa;

public BarMenu ( MenuBar mb) {
	this.mb = mb;
	
	setForeground( Defaults.MenuTxtClr);
	setBackground( Defaults.MenuBgClr);
	setFont( Defaults.MenuFont);

//	new BarMenuListener();
}

public void addNotify () {
	if ( (flags & IS_ADD_NOTIFIED) == 0 ) {
		super.addNotify();

		mb.owner = getToplevel();
		mb.parent = this;
		mb.addNotify();
		mb.propagateOldEvents( ((flags & IS_OLD_EVENT) != 0));

		registerHandlers();
	}
}

void disposeCurrent () {
	if ( current != null){
		unlinkCurrent();
		current.dispose();
		current = null;
	}
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeAll( getClass(), false);
}

public Graphics getGraphics () {
	int u = x;
	int v = y;
	
	if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ){
		u -= parent.deco.x;
		v -= parent.deco.y;
	}

	// we can't use Component.getGraphics() here, because this is clipped on the
	// insetted parents. Since Frame insets include the BarMenu, we need to bypass
	// this (to avoid getting clipped away)
	return NativeGraphics.getGraphics( parent, ((Window)parent).nativeData,
	                                   NativeGraphics.TGT_TYPE_WINDOW,
	                                   u, v, 0, 0, width, height,
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

public void handleShortcut( MenuShortcut ms) {
	switch ( ms.keyCode) {
		case KeyEvent.VK_LEFT:
			selectPrev();
			break;
		case KeyEvent.VK_RIGHT:
			selectNext();
			break;
		case KeyEvent.VK_ENTER:
			processSelection();
			break;
	}
}

public boolean isFocusTraversable () {
	return false;
//	return super.isFocusTraversable();
}

Menu menuAt ( int x) {
	int sz = mb.menus.size();
	int x0 = Dx;
	
	for ( int i=0; i<sz; i++) {
		Menu m = (Menu)mb.menus.elementAt( i);
		int mw = m.getWidth();
		x0 += mw+Dx;
		if ( x0 > x) {
			return m;
		}
		x0 += Dx;
	}
	
	return null;
}

boolean openSelection () {
	if ( (selection == null) || ((selection.getItemCount() == 0)) )
		return false;
	
	disposeCurrent();
	current = new PopupMenu( selection);
	current.show( this, getX( selection), height);

	MenuShortcut s1 = new MenuShortcut( this, KeyEvent.VK_LEFT, 0);
	MenuShortcut s2 = new MenuShortcut( this, KeyEvent.VK_RIGHT, 0);
	ShortcutHandler.addShortcut( s1, current.wnd, this);
	ShortcutHandler.addShortcut( s2, current.wnd, this);
	current.wnd.addWindowListener( wa);

//System.out.println( "linked: " + current.wnd);
	return true;		
}

public void paint ( Graphics g) {
	int sz = mb.menus.size();
	int dx2 = 2 * Dx;
	int x0 = 1;
	int y0 = 1;
	int mh = height - 2;

	g.setColor( getBackground());
	g.fill3DRect( 0, 0, width, height, true);
	
	for ( int i=0; i<sz; i++) {
		Menu m = (Menu)mb.menus.elementAt( i);
		int mw = m.getWidth() +dx2;

		m.paint( g, x0, Dx, y0, mw, mh, bgClr, fgClr, selection == m);
		x0 += mw;
	}
}

void paintMenu ( Graphics g, Menu m) {
	int sz = mb.menus.size();
	int x0 = 1;
	int dx2 = 2 * Dx;
	int y0 = 1;
	int mh = height - 2;
	
	for ( int i=0; i<sz; i++) {
		Menu cm = (Menu)mb.menus.elementAt( i);
		int mw = cm.getWidth() + dx2;

		if ( m == cm) {
			cm.paint( g, x0, Dx, y0, mw, mh, bgClr, fgClr, selection == cm);
			return;
		}
		x0 += mw;
	}
		
}

void processSelection () {
	if ( selection != null) {
		if ( !openSelection() ) {
			selection.handleShortcut( null);
			selectMenu( null);
		}
	}	
}

void registerHandlers () {
	MenuShortcut s1 = new MenuShortcut( this, KeyEvent.VK_LEFT, 0);
	MenuShortcut s2 = new MenuShortcut( this, KeyEvent.VK_RIGHT, 0);
	MenuShortcut s3 = new MenuShortcut( this, KeyEvent.VK_ENTER, 0);
	ShortcutHandler.addShortcut( s1, this, this);
	ShortcutHandler.addShortcut( s2, this, this);
	ShortcutHandler.addShortcut( s3, this, this);
	
	MouseMotionAdapter mma = new MouseMotionAdapter() {
		public void mouseMoved( MouseEvent evt) {
			if ( (AWTEvent.keyTgt == evt.getSource()) || (current != null) )
				selectMenu( menuAt( evt.getX() ));
		}
	};
	MouseAdapter ma = new MouseAdapter() {
		public void mousePressed( MouseEvent evt) {
			requestFocus();
			selectMenu( menuAt( evt.getX() ));
		}
		public void mouseReleased( MouseEvent evt ) {
			processSelection();
		}
		public void mouseExited( MouseEvent evt ) {
			if ( current == null )
				selectMenu( null);
		}
	};
	
	wa = new WindowAdapter() {
		public void windowClosed( WindowEvent evt) {
			selectMenu( null);
		}
	};
	
	addMouseMotionListener( mma);
	addMouseListener( ma);
}

public void removeNotify () {
	super.addNotify();
	mb.removeNotify();
	
	ShortcutHandler.removeShortcuts( this);
}

void selectMenu ( Menu m) {
	Menu ms = selection;
	
	if ( m == selection )
		return;
	
	Graphics g = getGraphics();

	if ( m == null ) {
		disposeCurrent();
		selection = null;
		paintMenu( g, ms);
	}
	else if ( selection == null ) {
		selection = m;
		paintMenu( g, m);
		openSelection();
	}
	else {
		selection = m;
		paintMenu( g, ms);
		paintMenu( g, m);
		disposeCurrent();
		openSelection();
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

void unlinkCurrent () {
	if ( (current != null) && (current.wnd != null ) ) {
		ShortcutHandler.removeShortcuts( current.wnd);
		current.wnd.removeWindowListener( wa);
//System.out.println( "unlinked: " + current.wnd);
	}
}
}
