package java.awt;

import java.util.Vector;

/**
 * class Menu -
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author J.Mehlitz
 */
public class Menu
  extends MenuItem
  implements MenuContainer
{
	private static final long serialVersionUID = -8809584163345499784L;
	Vector items = new Vector( 5);
	boolean isTearOff;
	private static int counter;

public Menu() {
	this( null, false);
}

public Menu( String label) {
	this( label, false);
}

public Menu( String label, boolean tearOff) {
	super( label);
	isTearOff = tearOff;
	setName("menu" + counter++);
}

public synchronized MenuItem add( MenuItem mi) {
	if (mi.getLabel().equals("-")) {
		mi = MenuItem.separator;
	}
	insert( mi, -1);
	return mi;
}

public void add( String label) {
	if (label.equals("-")) {
		addSeparator();
	}
	else {
		insert( label, -1);
	}
}

void addAll( Menu mAdd) {
	int ms = mAdd.items.size();
	for ( int i=0; i<ms; i++) {
		add( (MenuItem)mAdd.items.elementAt( i));
	}
}

public void addNotify() {
	if ( (flags & IS_ADD_NOTIFIED) == 0 ) {
		super.addNotify();
	
		int is = items.size();
		for ( int i=0; i<is; i++) {
			MenuItem mi = (MenuItem)items.elementAt(i);
			mi.parent = this;
			mi.owner = owner;
			mi.addNotify();
		}
	}
}

public void addSeparator() {
	insertSeparator( -1);
}

Vector addShortcuts ( Vector v) {
	int sz = items.size();

	super.addShortcuts( v);
	for ( int i=0; i<sz; i++)
		((MenuItem)items.elementAt( i)).addShortcuts( v);

	return v;	
}

/**
 * @deprecated, use getItemCount()
 */
public int countItems() {
	return (items.size());
}

public MenuItem getItem( int idx) {
	try {
		return (MenuItem)items.elementAt( idx);
	}
	catch( Exception e) {
		return null;
	}
}

public int getItemCount() {
	return (countItems());
}

private MenuBar getMenuBar() {
	MenuContainer root = this;

	try {
		while (((Menu) root).getParent() != null) {
			root = ((Menu) root).getParent();
		}
	}
	catch (ClassCastException e) {
		if (root instanceof MenuBar) {
			return (MenuBar) root;
		}
	}

	return null;
}

MenuItem getShortcutMenuItem( MenuShortcut s) {
	int sz = items.size();

	MenuItem mi = super.getShortcutMenuItem(s);
	if ( mi != null)
		return mi;

	for ( int i=0; i<sz; i++) {
		mi = ((MenuItem)items.elementAt( i)).getShortcutMenuItem( s);
		if ( mi != null )
			return mi;
	}
	return null;
			
}

public synchronized void insert( MenuItem mi, int idx) {
	try { 
		items.insertElementAt( mi, idx > -1 ? idx : items.size());
		mi.parent = this;
	}
	catch( Exception e) {}
}

public void insert( String label, int idx) {
	insert( new MenuItem( label), idx);
}

public void insertSeparator( int idx) {
	insert( MenuItem.separator, idx);
}

private boolean isHelpMenu() {
	// find the menu bar, if it exists
	MenuBar bar = getMenuBar();

	// if there is a menu bar for this menu, and its help menu is
	// this menu, then return true.
	if (bar != null) {
		return bar.getHelpMenu() == this;
	}

	return false;
}

public boolean isTearOff() {
	return isTearOff;
}

public String paramString() {
	return super.paramString() + ",tearOff=" + isTearOff() + ",isHelpMenu=" + isHelpMenu();
}

protected void propagateOldEvents ( boolean isOldEventClient ) {
	super.propagateOldEvents( isOldEventClient);

	for (int i = getItemCount() - 1; i >= 0; i--) {
		MenuComponent comp = getItem(i);
		comp.propagateOldEvents( isOldEventClient);
	}
}

public synchronized void remove( MenuComponent m) {
	try { items.removeElement( m); }
	catch( Exception e) {}
}

public synchronized void remove( int idx) {
	try { items.removeElementAt( idx); }
	catch( Exception e) {}
}

public synchronized void removeAll() {
	try { items.removeAllElements(); }
	catch( Exception e) {}
}

public void removeNotify() {
	if ((flags & IS_ADD_NOTIFIED) > 0) {
		int is = items.size();
		for ( int i=0; i<is; i++) {
			MenuItem mi = (MenuItem)items.elementAt(i);
			mi.removeNotify();
		}
		super.removeNotify();
	}
}
}
