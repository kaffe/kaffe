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
}

Vector addShortcuts( Vector v) {
	if ( shortcut != null)
		v.addElement( shortcut);
	return v;
}

public void deleteShortcut() {
	shortcut = null;
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

boolean handleShortcut ( KeyEvent e) {
	if ( shortcut == null)
		return false;
  if ( shortcut.key != e.getKeyChar() + 96 )	//ctrl offset
		return false;

	process();
	return true;
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
			g.setColor( sel ? Defaults.FocusClr : fore);
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

void process () {
	if ( hasToNotify( aListener) ) {
		ActionEvent ae = AWTEvent.getActionEvent( this, ActionEvent.ACTION_PERFORMED);
		ae.setActionEvent( getActionCommand(), 0);
		Toolkit.eventQueue.postEvent( ae);
	}
	// Otherwise we send the ActionEvent to out parent, but only if it's
	// a Menu.
	else if (parent instanceof Menu) {
		for ( Menu mp = (Menu)parent; mp != null; mp = (Menu)mp.parent ) {
			if (mp.hasToNotify(mp.aListener)) {
				ActionEvent ae = AWTEvent.getActionEvent( mp, ActionEvent.ACTION_PERFORMED);
				ae.setActionEvent( getActionCommand(), 0);
				Toolkit.eventQueue.postEvent( ae);
				return;
			}
		}
	}
}

void processActionEvent ( ActionEvent e ) {
	aListener.actionPerformed( e);
}

void processActionEvent_X ( ActionEvent e ) {
	ActionListener al = aListener;
	
	if ( al == null) {
		for( MenuContainer mc = parent; al == null;) {
			if (! ( mc instanceof Menu))
				break;
			Menu m = (Menu)mc;
			al = m.aListener;
			mc = m.parent;
		}
	}
					
	if ( al != null )
		al.actionPerformed( e);
}

public synchronized void removeActionListener( ActionListener l) {
	aListener = AWTEventMulticaster.remove( aListener, l);
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
	if ( ! isSeparator() )
		shortcut = s;
}
}
