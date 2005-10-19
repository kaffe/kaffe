package java.awt;

import java.awt.Menu;
import java.awt.MenuComponent;
import java.awt.MenuContainer;
import java.awt.MenuItem;
import java.awt.MenuShortcut;
import java.awt.Toolkit;
import java.util.Enumeration;
import java.util.Vector;

import org.kaffe.util.Ptr;

/**
 * class MenuBar -
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
public class MenuBar
  extends MenuComponent
  implements MenuContainer
{
	Vector menus = new Vector();
	Menu helpMenu;
	Ptr nativeData;

public MenuBar() {
}

public Menu add( Menu m) {
	menus.addElement( m);

	if ( nativeData != null ) {
		m.parent = this;
		m.owner = owner;
		m.addNotify();
		Toolkit.menuInsertItem( nativeData, m.nativeData, m.label, -1, m);
	}
	updateView();
	return (m);
}

public void addNotify() {
	// PM are we in th the right thread??

	if ( nativeData == null ) {
		nativeData = Toolkit.menuCreateMenu( false);
		int ms = menus.size();
		for ( int i=0; i<ms; i++) {
			Menu m = (Menu)menus.elementAt(i);
			m.parent = this;
			m.owner = owner;
			m.addNotify();
			Toolkit.menuInsertItem( nativeData, m.nativeData, m.label, -1, m);
		}
	}
}

/**
 * @deprecated
 */
public int countMenus() {
	return menus.size();
}

public void deleteShortcut( MenuShortcut s) {
	for ( Enumeration e = shortcuts(); e.hasMoreElements(); ) {
		MenuItem mi = (MenuItem)e.nextElement();
		if ( s.equals( mi.shortcut))
			mi.deleteShortcut();
	}
}

public Menu getHelpMenu() {
	return helpMenu;
}

public Menu getMenu( int idx) {
	try { return (Menu)menus.elementAt( idx); }
	catch( Exception e) { return null; }
}

public int getMenuCount() {
	return (countMenus());
}

public MenuItem getShortcutMenuItem( MenuShortcut s) {
	int sz = menus.size();
	for ( int i=0; i<sz; i++) {
		Menu m = (Menu)menus.elementAt( i);
		MenuItem mi = m.getShortcutMenuItem( s);
		if ( mi != null)
			return mi;
	}
	return null;
}

protected void propagateOldEvents ( boolean isOldEventClient ) {
	super.propagateOldEvents( isOldEventClient);

	// hand this down to all menus attached to us
	for (int i = menus.size() - 1; i >= 0; i--) {
		((Menu)menus.elementAt(i)).propagateOldEvents( isOldEventClient);
	}
	if (helpMenu != null) {
		helpMenu.propagateOldEvents( isOldEventClient);
	}
}

public void remove( MenuComponent m) {
	if ( m instanceof Menu){
		if ( nativeData != null) {
			Toolkit.menuRemoveItem( nativeData, m);
		}
		m.removeNotify();
		menus.removeElement( m);
		updateView();
	}
}

public synchronized void remove( int idx) {
	try { remove( (MenuComponent)menus.elementAt( idx) ); }
	catch( Exception e) {}
}

public void removeNotify() {
	if ( nativeData != null) {
		int ms = menus.size();
		for ( int i=0; i<ms; i++) {
			Menu m = (Menu)menus.elementAt(i);
			m.removeNotify();
		}
		nativeData = null;
	}
}

public synchronized void setHelpMenu( Menu m) {
	helpMenu = m;
	updateView();
}

public synchronized Enumeration shortcuts() {
	Vector all = new Vector();
	int ms = menus.size();

	for ( int i=0; i<ms; i++) {
		Menu m = (Menu)menus.elementAt( i);
		m.addShortcuts( all);
	}

	return all.elements();
}

void updateView() {
}
}
