package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
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
	int eventMask;
	static MenuItem separator = new MenuItem( "-");
	private static int counter;
	final private static long serialVersionUID = -21757335363267194L;

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
	setName("menuitem" + counter++);
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

int getHeight() {
	return ( isSeparator() ? 5 : fm.getHeight() + 5);
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

	if ( (eventMask & AWTEvent.DISABLED_MASK) != 0 )
		return;

	if ( (aListener != null) ||
	     ( ((eventMask & AWTEvent.ACTION_EVENT_MASK) != 0) || ((flags & IS_OLD_EVENT) != 0) ) ) {
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
		if ( (m.aListener != null) || ((m.eventMask & AWTEvent.ACTION_EVENT_MASK) != 0) ) {
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
	return (separator == this);
}

int paint ( Graphics g, int x, int xoff, int y, int width, int height,
            Color back, Color fore, boolean sel) {
	int ih = getHeight();
	int ya = (height > 0) ? y + (height - ih) / 2 : y;
	int xo = x + xoff;
	
	if ( separator == this) {
		g.setColor( back);
		g.draw3DRect( x + 2, y + ih/2, width - 4, 1, false);
	}
	else {
		ya += fm.getAscent() + 3;
		g.setFont( fnt);
		
		if ( sel && ((eventMask & AWTEvent.DISABLED_MASK) == 0) ) {
			g.setColor( Defaults.MenuSelBgClr);
			g.fill3DRect( x+1, y+1, width-2, ih-1, false);
		}
		else {
			g.setColor( back);
			g.fillRect( x+1, y+1, width-1, ih);
		}
		
		if ( (eventMask & AWTEvent.DISABLED_MASK) == 0 ) {
			if ( Defaults.MenuTxtCarved ) {
				g.setColor( Color.white);
				g.drawString( label, xo+1, ya+1);
			}
			g.setColor( sel ? Defaults.MenuSelTxtClr : fore);
		}
		else {
			g.setColor( back.darker() );
		}
		g.drawString( label, xo, ya);
	}
			
	return ih;
}

public String paramString() {
	return super.paramString() + ",label=" + label;
}

void process ( ActionEvent e ) {
	if ( (aListener != null) ||
	     ((eventMask & (AWTEvent.ACTION_EVENT_MASK|AWTEvent.DISABLED_MASK))
	                            == AWTEvent.ACTION_EVENT_MASK) ){
		processEvent( e);
	}

	if ( (flags & IS_OLD_EVENT) > 0 )
		postEvent( Event.getEvent( e));
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
