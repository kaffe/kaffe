package java.awt;

import java.awt.MenuComponent;
import java.awt.MenuContainer;
import java.awt.MenuItem;
import java.awt.MenuShortcut;
import java.awt.PopupMenu;
import java.awt.Toolkit;
import java.util.Vector;

/**
 * class Menu -
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
public class Menu
  extends MenuItem
  implements MenuContainer
{
	Vector items = new Vector( 5);
	boolean isTearOff;

public Menu() {
	this( null, false);
}

public Menu( String label) {
	this( label, false);
}

public Menu( String label, boolean tearOff) {
	super( label);
	isTearOff = tearOff;
}

public synchronized MenuItem add( MenuItem mi) {
	insert( mi, -1);
	return mi;
}

public void add( String label) {
	insert( new MenuItem( label), -1 );
}

void addAll( Menu mAdd) {
	int ms = mAdd.items.size();
	for ( int i=0; i<ms; i++) {
		add( (MenuItem)mAdd.items.elementAt( i));
	}
	updateView();
}

public void addNotify() {
	// PM are we in th the right thread??

	if ( nativeData == null) {
		nativeData = Toolkit.menuCreateMenu( true);
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
	insert( new MenuItem( "-"), -1);
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
		mi.owner = owner;
		if ( nativeData != null ) {
			mi.addNotify();
			Toolkit.menuInsertItem( nativeData, mi.nativeData, mi.label, idx, mi);
		}
		updateView();
	}
	catch( Exception e) {}
}

public void insert( String label, int idx) {
	insert( new MenuItem( label), idx);
}

public void insertSeparator( int idx) {
	insert( new MenuItem("-"), idx);
}

public boolean isTearOff() {
	return isTearOff;
}

public String paramString() {
	return super.paramString();
}

protected void propagateOldEvents ( boolean isOldEventClient ) {
	super.propagateOldEvents( isOldEventClient);

	for (int i = getItemCount() - 1; i >= 0; i--) {
		MenuComponent comp = getItem(i);
		comp.propagateOldEvents( isOldEventClient);
	}
}

public synchronized void remove( MenuComponent m) {
	items.removeElement( m);
	if ( nativeData != null ) {
		Toolkit.menuRemoveItem( nativeData, m);
	}
	updateView();
}

public synchronized void remove( int idx) {
	remove( (MenuComponent)items.elementAt( idx) );
}

public synchronized void removeAll() {
	try {
		items.removeAllElements();
		if ( nativeData != null ) {
			Toolkit.menuDeleteMenu( nativeData);
			nativeData = Toolkit.menuCreateMenu( this instanceof PopupMenu);
		}
		updateView();
	}
	catch( Exception e) {}
}

public void removeNotify() {
	if ( nativeData != null) {
		int is = items.size();
		for ( int i=0; i<is; i++) {
			MenuItem mi = (MenuItem)items.elementAt(i);
			mi.removeNotify();
		}
		nativeData = null;
	}
}

void updateView() {
}
}
