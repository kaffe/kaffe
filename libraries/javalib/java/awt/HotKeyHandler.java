package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.lang.String;

class HotKeyEntry
{
	Component tgt;
	int keyCode;
	int mods;
	HotKeyEntry next;
	String sel;
	MenuItem mi;

HotKeyEntry ( Component tgt, MenuItem mi, int keyCode, int mods, String sel) {
	this.tgt = tgt;
	this.mi = mi;
	this.keyCode = keyCode;
	this.mods = mods;
	this.sel = sel;
}

boolean process( KeyEvent e) {

	if ( e.getModifiers() != mods )
		return false;

	if ( ! AWTEvent.activeWindow.isAncestorOf( tgt) )
		return false;

	int cc = e.getKeyCode();

	if (cc == keyCode ) {
		ActionEvt ae;
		if ( mi != null ) {
			ae = ActionEvt.getEvent( mi, ActionEvent.ACTION_PERFORMED, sel, mods );
			mi.processActionEvent( ae);
		}
		else {
			ae = ActionEvt.getEvent( tgt, ActionEvent.ACTION_PERFORMED, sel, mods );
			tgt.processEvent( ae);
		}
		return true;
	}
	
	return false;
}
}

/**
 * HotKeyHandler - 
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
class HotKeyHandler
{
	static HotKeyEntry entry;

static void addHotKey( Component tgt, MenuItem mi, int keyCode, int mods, String sel) {
	HotKeyEntry hk, e;
	
	if ( (mods & KeyEvent.CTRL_MASK) > 0 )
		keyCode = mapCtrlCode( keyCode, mods);

	hk = new HotKeyEntry( tgt, mi, keyCode, mods, sel);
	
	if ( entry == null )
		entry = hk;
	else {
		for ( e = entry; e.next != null; e = e.next);
		e.next = hk;
	}
}

static void addHotKey( Component tgt, int keyCode, int mods, String sel) {
	addHotKey( tgt, null, keyCode, mods, sel);
}

static void focusNext( Container co, Component entry) {
	boolean useNext = (entry == null);
	
	for ( int i=0; i<co.nChildren; i++) {
		Component c = co.children[i];
		if ( useNext && c.isFocusTraversable() ) {
			c.requestFocus();
			return;
		}
		if ( entry == c )
			useNext = true;
	}
	
	if ( co.parent != null )
		focusNext( co.parent, co);
	else
		focusNext( co, null);
}

static void focusPrev( Container co, Component entry) {
	boolean useNext = (entry == null);
	
	for ( int i=co.nChildren-1; i>=0; i--) {
		Component c = co.children[i];
		if ( useNext && c.isFocusTraversable() ) {
			c.requestFocus();
			return;
		}
		if ( entry == c )
			useNext = true;
	}
	
	if ( co.parent != null )
		focusPrev( co.parent, co);
	else
		focusPrev( co, null);
}

static boolean handle( KeyEvent e) {
	if ( (e.getID() != e.KEY_PRESSED) || e.consumed )
		return false;

	for ( HotKeyEntry hk = entry; hk != null; hk = hk.next ) {
		if ( hk.process( e) )
			return true;
	}
		
	char kc = e.getKeyChar();
	Component c = (Component) e.getSource();
	boolean back = e.isShiftDown();

	Container co = (c.parent != null ) ? c.parent : (Container)c;
	
	switch( kc) {
		case '\t':
			if ( back)
			    focusPrev( co, AWTEvent.keyTgt);
			else			
			    focusNext( co, AWTEvent.keyTgt);
			return true;
	}

	return false;
}

static int mapCtrlCode( int keyCode, int mods) {

	if ( (mods & KeyEvent.CTRL_MASK) > 0 ) {
		if ( (keyCode >= 1) && (keyCode <= 26) )
			return keyCode + 64;
	}
	
	return keyCode;
}

static void removeHotKey( Component tgt, int keyCode, int mods) {

	if ( entry == null )
		return;

	if ( (mods & KeyEvent.CTRL_MASK) > 0 )
		keyCode = mapCtrlCode( keyCode, mods);

	if ( (entry.tgt == tgt) && (entry.keyCode == keyCode) && (entry.mods == mods) ) {
		entry = entry.next;
		return;
	}
		
	for ( HotKeyEntry hk = entry; hk.next != null; hk = hk.next) {
		HotKeyEntry hkn = hk.next;
		if ( (hkn.tgt == tgt) && (hkn.keyCode == keyCode) && (hkn.mods == mods) ) {
			hk.next = hkn.next;
			break;
		}
	}
}

static void removeHotKeys( Component tgt) {

	if ( entry == null )
		return;

	for ( HotKeyEntry hk = entry; hk != null; hk = hk.next) {
		HotKeyEntry hkn = hk.next;
		if (( hk.tgt == tgt) && ( hk == entry ))
				entry = hk.next;
		else if ( (hkn != null) && (hkn.tgt == tgt) )
			hk.next = hkn.next;
	}
		
}

static int size() {
	int s = 0;
	for ( HotKeyEntry hk = entry; hk != null; hk = hk.next)
		s++;
		
	return s;
}
}
