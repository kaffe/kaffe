package java.awt;

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

static Component focusNext ( Container w ) {
	Component c = null, cur = AWTEvent.keyTgt;

	if ( (cur == null) || (cur == w) ) {
		c = focusNext( w, null);
	}
	else {
		for ( Component cc=cur; cc.parent != null; cc = cc.parent ) {
			if ( (c = focusNext( cc.parent, cc)) != null ){
				break;
			}
		}
		if ( c == null )
			c = focusNext( w, null);
	}

	return c;
}

static Component focusNext( Container co, Component cur ) {
	Component c;
	int i=-1;

	if ( cur != null ){
		for ( i=0; (i<co.nChildren) && (co.children[i] != cur); i++ );
	}
	
	for ( i++; i < co.nChildren; i++ ){
		c = co.children[i];
		if ( c instanceof Container ) {
			if ( ((c.flags & Component.IS_NATIVE_LIKE) == 0) || (c instanceof Panel) ){
				Component cc = focusNext( (Container)c, null);
				if ( cc != null )
					return cc;
			}
		}
		
		if ( c.isFocusTraversable() )
			return c;
	}
	
	return ( co.isFocusTraversable() && (cur == null) ? co : null);
}

static Component focusPrev ( Container w ) {
	Component c = null, cur = AWTEvent.keyTgt;

	if ( (cur == null) || (cur == w) ) {
		c = focusPrev( w, null);
	}
	else {
		for ( Component cc=cur; cc.parent != null; cc = cc.parent ) {
			if ( (c = focusPrev( cc.parent, cc)) != null ){
				break;
			}
		}
		if ( c == null )
			c = focusPrev( w, null);
	}

	return c;
}

static Component focusPrev( Container co, Component cur ) {
	Component c;
	int i=co.nChildren;

	if ( cur != null ){
		for ( i=co.nChildren-1; (i>=0) && (co.children[i] != cur); i-- );
	}
	
	for ( i--; i >= 0; i-- ){
		c = co.children[i];
		if ( c instanceof Container ) {
			if ( ((c.flags & Component.IS_NATIVE_LIKE) == 0) || (c instanceof Panel) ){
				Component cc = focusPrev( (Container)c, null);
				if ( cc != null )
					return cc;
			}
		}
		
		if ( c.isFocusTraversable() )
			return c;
	}
	
	return ( co.isFocusTraversable() && (cur == null) ? co : null);
}

static boolean handle( KeyEvent e) {
	Component c, cNext;
	Container co;	
	int em = e.getModifiers();
	int cc = e.getKeyCode();
	
	if ( (em & KeyEvent.CTRL_MASK) > 0 ) {
		if ( (cc >= 1) && (cc <= 26) )
			cc += 64;
	}
	
	if ( cc < 0 || cc >= codeTable.length )
		return false;

	MenuShortcut ms;		
	for ( ms = codeTable[cc]; ms != null; ms = ms.nextMod) {
		if ( ms.mods == em ) {
			ms.process();
			return true;
		}
	}
			
	c = (Component) e.getSource();
	co = (Container) c.getToplevel();
	
	// some day, we might add other focus-switch hotkeys (e.g. menubar, 1st, next-in-group etc.)
	if ( e.getKeyChar() == '\t' ) {
		cNext = e.isShiftDown() ? focusPrev( co) : focusNext( co);
		if ( cNext != c )	
			cNext.requestFocus();
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
