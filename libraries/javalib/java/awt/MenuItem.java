package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.util.EventListener;
import java.util.Vector;

/**
 * class MenuItem -
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author J.Mehlitz
 */
public class MenuItem
  extends MenuComponent
  implements ShortcutConsumer
{
	MenuShortcut shortcut;
	String label;
	ActionListener aListener;
	String aCmd;
	boolean isEnabled = true;
	static MenuItem separator = new MenuItem( "-");

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
	if ( (flags & IS_ADD_NOTIFIED) == 0 ) {
		if ( shortcut != null )
			ShortcutHandler.addShortcut( shortcut, owner, this);
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

public String getActionCommand() {
	return (aCmd != null) ? aCmd : label;
}

int getHeight() {
	return ( isSeparator() ? 4 : fm.getHeight() + 4);
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

int getWidth() {
	return fm.stringWidth( label);
}

public void handleShortcut ( MenuShortcut ms) {
	int mods = (ms != null) ? ms.mods : 0;
	
	if ( hasToNotify( aListener) || ((flags & IS_OLD_EVENT) > 0) ) {
		ActionEvt ae = ActionEvt.getEvent( this, ActionEvent.ACTION_PERFORMED,
		                                   getActionCommand(), mods);
		Toolkit.eventQueue.postEvent( ae);
		return;
	}

	MenuContainer mp = parent;
	for (;;) {
		if (!(mp instanceof Menu)) {
			break;
		}
		Menu m = (Menu)mp;
		if (m.hasToNotify(m.aListener)) {
			ActionEvt ae = ActionEvt.getEvent( m, ActionEvent.ACTION_PERFORMED,
			                                   getActionCommand(), mods);
			Toolkit.eventQueue.postEvent( ae);
			return;
		}
		mp = m.parent;
	}
}

boolean hasToNotify ( EventListener listener ) {
	return ( isEnabled && (listener != null));
}

public boolean isEnabled() {
	return isEnabled;
}

public boolean isSeparator() {
	return (separator == this);
}

int paint ( Graphics g, int xoff, int y, int width, Color back, Color fore, boolean sel) {
	int ih = getHeight();
	int as = fm.getAscent();
	
	if ( separator == this) {
		int ys = y+ih/2;
		g.setColor( back);
		g.draw3DRect( 5, ys-1, width -10, 1, false);
	}
	else {
		g.setFont( fnt);
		if ( isEnabled ) {
			if ( Defaults.MenuTxtCarved ) {
				g.setColor( Color.white);
				g.drawString( label, xoff+1, y+as+1);
			}
			g.setColor( sel ? Defaults.MenuSelTxtClr : fore);
		}
		else {
			g.setColor( back.darker() );
		}
		g.drawString( label, xoff, y+as);
	}
			
	return ih;
}

public String paramString() {
	return getClass().getName() + "[Label: " + label + "]";
}

void processActionEvent ( ActionEvent e ) {
	if (aListener != null) {
		aListener.actionPerformed( e);
	}

	if ( (flags & IS_OLD_EVENT) > 0 )
		postEvent( Event.getEvent( e));
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

public synchronized void setEnabled( boolean b) {
	isEnabled = b;
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
