package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.util.EventListener;
import java.util.Vector;
import kaffe.util.Ptr;

/**
 * class MenuItem -
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
public class MenuItem
  extends MenuComponent
  implements ShortcutConsumer
{
	Ptr nativeData;
	MenuShortcut shortcut;
	String label;
	ActionListener aListener;
	String aCmd;
	int eventMask;

public MenuItem() {
	this( null, null);
}

public MenuItem( String label) {
	this( label, null);
}

public MenuItem( String label, MenuShortcut s) {
	setShortcut( s);
	setLabel( label);
	setFont( defFnt);
}

public synchronized void addActionListener( ActionListener l) {
	if ( ! isSeparator() )
		aListener = AWTEventMulticaster.add( aListener, l);
}

public void addNotify() {
	if ( ( flags & IS_ADD_NOTIFIED) == 0) {
		if ( (parent != null) && ( parent instanceof Menu) ) {
			Menu m = (Menu)parent;
			Toolkit.menuInsertItem( m.nativeData, nativeData, label, -1, this);
			if ( ! isEnabled() ) {
				Toolkit.menuEnableItem( m.nativeData, this, false);
			}
		}
		if ( shortcut != null ) {
			ShortcutHandler.addShortcut( shortcut, owner, this);
		}
		flags |= IS_ADD_NOTIFIED;
	}
}

Vector addShortcuts( Vector v) {
	if ( shortcut != null)
		v.addElement( shortcut);
	return v;
}

public void deleteShortcut() {
	if ( (flags & IS_ADD_NOTIFIED) > 0 ) {
		if ( shortcut != null )
			ShortcutHandler.removeFromOwner( owner, shortcut);
	}
	shortcut = null;
}

/**
 * @deprecated
 */
public synchronized void disable() {
	setEnabled(false);
}

public void disableEvents ( long disableMask ) {
	eventMask &= ~disableMask;  
}

/**
 * @deprecated
 */
public synchronized void enable() {
	setEnabled(true);
}

/**
 * @deprecated
 */
public void enable(boolean on) {
	setEnabled(on);
}

public void enableEvents ( long enableMask ) {
	eventMask |= enableMask;
}

public String getActionCommand() {
	return (aCmd != null) ? aCmd : label;
}

public String getLabel() {
	return label;
}

public MenuShortcut getShortcut() {
	return shortcut;
}

MenuItem getShortcutMenuItem( MenuShortcut s) {
	if ( s.equals( shortcut))
		return this;
	return null;
}

public void handleShortcut ( MenuShortcut ms) {
	int mods = (ms != null) ? ms.mods : 0;
	if ( (aListener != null) ||
	     ((eventMask & (AWTEvent.ACTION_EVENT_MASK|AWTEvent.DISABLED_MASK))
		 == AWTEvent.ACTION_EVENT_MASK) ||
	     ((flags & IS_OLD_EVENT) > 0) ) {
		Toolkit.eventQueue.postEvent( ActionEvt.getEvent( this, ActionEvent.ACTION_PERFORMED,
		                                                  getActionCommand(), mods));
		return;
	}

	MenuContainer mp = parent;
	for (;;) {
		if (!(mp instanceof Menu)) {
			break;
		}
		Menu m = (Menu)mp;
		if ( (m.aListener != null) ||
		     ((m.eventMask & (AWTEvent.ACTION_EVENT_MASK|AWTEvent.DISABLED_MASK))
			 == AWTEvent.ACTION_EVENT_MASK) ) {
			Toolkit.eventQueue.postEvent( ActionEvt.getEvent( m, ActionEvent.ACTION_PERFORMED,
			                                                  getActionCommand(), mods));
			return;
		}
		mp = m.parent;
	}
}

public boolean isEnabled() {
	return (eventMask & AWTEvent.DISABLED_MASK) == 0;
}

public boolean isSeparator() {
	return ((label == null) || (label.startsWith( "-")));
}

public String paramString() {
	return getClass().getName() + "[Label: " + label + "]";
}

void process ( ActionEvent e ) {
	if ( (aListener != null) ||
	     ((eventMask & (AWTEvent.ACTION_EVENT_MASK|AWTEvent.DISABLED_MASK))
		 == AWTEvent.ACTION_EVENT_MASK) ){
		processEvent( e);
	}

	else if ( (flags & IS_OLD_EVENT) > 0 ) {
		postEvent( Event.getEvent( e));
	}

	else if ( (parent != null) && (parent instanceof Menu) ) {
		((Menu)parent).process( e);
	}

}

protected void processActionEvent ( ActionEvent e ) {
	if (aListener != null) {
		aListener.actionPerformed( e);
	}
}

protected void processEvent ( AWTEvent e ) {
	// same uglyness like in Component.processEvent - we already had it down
	// to the specific Event class and now have to go up to AWTEvent again because
	// this might be re-implemented by a derived class

	if ( e.id == ActionEvent.ACTION_PERFORMED ) {
		processActionEvent( (ActionEvent)e);
	}
}

public synchronized void removeActionListener( ActionListener l) {
	aListener = AWTEventMulticaster.remove( aListener, l);
}

public void removeNotify() {
	if ( (flags & IS_ADD_NOTIFIED) > 0 ) {
		if ( shortcut != null )
			ShortcutHandler.removeFromOwner( owner, shortcut);
		flags &= ~IS_ADD_NOTIFIED;
		owner = null;
		parent = null;
	}
}

public void setActionCommand( String cmd) {
	aCmd = cmd;
}

public synchronized void setEnabled ( boolean isEnabled ) {
	if ( isEnabled)
		eventMask &= ~AWTEvent.DISABLED_MASK;
	else
		eventMask |= AWTEvent.DISABLED_MASK;

	if ( (parent != null) && (parent instanceof Menu) ) {
		Ptr p = ((Menu)parent).nativeData;
		if ( p != null ) {
			Toolkit.menuEnableItem( p, this, isEnabled);
		}
	}

}

public synchronized void setLabel( String label) {
	this.label = label;
}

public void setShortcut( MenuShortcut s) {
	//has to be cloned due to existing links
	MenuShortcut ms = ( s != null) ? new MenuShortcut( s) : null;

	if ((flags & IS_ADD_NOTIFIED) > 0 ) {
		if ( shortcut != null ) {
			ShortcutHandler.removeFromOwner( owner, shortcut);
		}
		if ( ms != null )
			ShortcutHandler.addShortcut( ms, owner, this);
	}
	shortcut = ms;
}
}
