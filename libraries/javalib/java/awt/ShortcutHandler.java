package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.util.Hashtable;

/**
 * ShortcutHandler - 
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
class ShortcutHandler
{
	static MenuShortcut[] codeTable = new MenuShortcut[160];
	static Hashtable tgtTable = new Hashtable();

static void addShortcut( MenuShortcut ms, Component owner, ShortcutConsumer consumer ) {

	ms.consumer = consumer;
	ms.next = null;
		
	MenuShortcut msr = (MenuShortcut)tgtTable.get( owner);
	
	if ( msr == null)
		tgtTable.put( owner, ms);
	else {
		for ( ; msr.next != null; msr = msr.next);
		msr.next = ms;
	}
}

static void buildCodeTable( Component tgt) {
	int i, ctl = codeTable.length;
	MenuShortcut me, mt;

	for ( i=0; i<ctl; i++)
		codeTable[i] = null;

	if ( tgt == null )
		return;
		
	//collect codes from all window components
	for ( Component c = tgt; c != null; c = c.parent) {
		for ( me = (MenuShortcut)tgtTable.get( c); me != null; me = me.next) {
			me.nextMod = null;
			mt = codeTable[ me.keyCode];
			if ( mt == null) {
				codeTable[me.keyCode] = me;
			}
			else {
				for (; mt.nextMod != null; mt = mt.nextMod);
				mt.nextMod = me;
			}
		}
	}
}

static void focusNext( Container co, Component entry) {
	boolean useNext = (entry == null);
	
	for ( int i=0; i<co.nChildren; i++) {
		Component c = co.children[i];
		if ( useNext && c.isFocusTraversable() ) {
/*
			if ( c instanceof Container )
				focusNext( (Container)c, null);
			else
*/
				c.requestFocus();
			return;
		}
		if ( entry == c )
			useNext = true;
	}
	
	if ( co.parent != null )
		focusNext( co.parent, co);
	else if ( (entry != null) && useNext ) // from start
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
	else if ( (entry != null) && useNext ) // from start
		focusPrev( co, null);
}

static boolean handle( KeyEvent e) {
	int em = e.getModifiers();
	int cc = e.getKeyCode();
	
	if ( (em & KeyEvent.CTRL_MASK) > 0 ) {
		if ( (cc >= 1) && (cc <= 26) )
			cc += 64;
	}
	
	if ( cc >= codeTable.length )
		return false;

	MenuShortcut ms;		
	for ( ms = codeTable[cc]; ms != null; ms = ms.nextMod) {
		if ( ms.mods == em ) {
			ms.process();
			return true;
		}
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

static void removeFromOwner( Component owner, MenuShortcut msr) {
	MenuShortcut ms, msp = null;
	
	ms = (MenuShortcut)tgtTable.get( owner);
	for (; ms != null; ms = ms.next) {
		if ( msr == ms ) {
			if ( msp == null ) {
				if (msr.next == null ) {
					tgtTable.remove( owner);
				}
				else {
					tgtTable.put( owner, msr.next);
				}
			}
			else {
				msp.next = msr.next;
			}
			return;
		}
		msp = ms;
	}
}

static void removeFromOwner( Component owner, ShortcutConsumer consumer) {
	MenuShortcut ms, msp = null;
	
	ms = (MenuShortcut)tgtTable.get( owner);
	for (; ms != null; ms = ms.next) {
		if ( ms.consumer == consumer ) {
			if ( msp == null ) {
				if (ms.next == null ) {
					tgtTable.remove( owner);
					return;
				}
				else {
					tgtTable.put( owner, ms.next);
				}
			}
			else {
				msp.next = ms.next;
			}
		}
		msp = ms;
	}
}

static void removeShortcuts( Component owner) {
	tgtTable.remove( owner);
}

static int size() {
	return tgtTable.size();
}
}
