package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;

import kaffe.awt.ImageDict;
import kaffe.awt.ImageSpec;

/**
 * class Button -
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Button
  extends Component
  implements ShortcutConsumer
{
	final private static long serialVersionUID = -8774683716313001058L;
	private static int counter;

	String label;
	ActionListener aListener;
	String aCmd;
	FontMetrics fm;
	ImageSpec imgs;
	MenuShortcut shortcut;
	int state;
	static int PUSHED = 1;
	static int HILIGHTED = 2;

public Button () {
	this( "");
}

public Button ( String label) {
	cursor = Cursor.getPredefinedCursor( Cursor.HAND_CURSOR);

	flags |= IS_MOUSE_AWARE;

	setFont( Defaults.BtnFont);
	setBackground( Defaults.BtnClr);
	setForeground( Defaults.BtnTxtClr);
	setLabel( label);
	setName("button" + counter++);
}

public void addActionListener ( ActionListener a) {
	aListener = AWTEventMulticaster.add( aListener, a);
}

public void addNotify () {
	super.addNotify();

	if ( shortcut != null )
		ShortcutHandler.addShortcut( shortcut, getToplevel(), this);

	MenuShortcut s1 = new MenuShortcut( this, KeyEvent.VK_ENTER, 0);
	MenuShortcut s2 = new MenuShortcut( this, KeyEvent.VK_SPACE, 0);
	ShortcutHandler.addShortcut( s1, this, this);
	ShortcutHandler.addShortcut( s2, this, this);
}

void animate() {
	Graphics g = getGraphics();

	state |= PUSHED;
	paint( g);
	Toolkit.tlkSync();
	try { Thread.sleep( 100); }
	catch ( Exception _x) {}
	state &= ~PUSHED;
	paint( g);
	Toolkit.tlkSync();

	g.dispose();
}

void checkMouseAware () {
	// we always want our processMouse to be called
}

void drawImage ( Graphics g) {
	Image img = imgs.getImage();
	int iw = img.getWidth( this);
	int ih = img.getHeight( this);
	int di = ((state & PUSHED) > 0) ? 1 : 0;
	int x = (width - iw) / 2 + di;
	int y = (height - ih) / 2 + di;

	g.drawImage( img, x, y, this);
}

void drawText ( Graphics g) {
	Color c1 = null, c2;
	int x = (width - fm.stringWidth( label)) / 2;
	int y = height - (height - fm.getHeight()) / 2 - fm.getDescent();

	// If button is disabled then the text as such.
	if (!isEnabled()) {
		c1 = null;
		c2 = bgClr.darker();
	}
	else if ( (state & PUSHED) > 0 ){
		x--; y--;
		c1 = Color.yellow;
		c2 = Color.red;
	}
	else {
		if ( (state & HILIGHTED) > 0 ) {
			c1 = Defaults.BtnTxtCarved ? bgClr.brighter() : null;
			c2 = Defaults.BtnPointTxtClr;
		}
		else {
			c1 = Defaults.BtnTxtCarved ? bgClr.brighter() : null;
			c2 = fgClr;
		}
	}

	if ( c1 != null ){
		g.setColor( c1);
		g.drawString( label, x+1, y+1);
	}

	g.setColor( c2);
	g.drawString( label, x, y);
}

public String getActionCommand () {
	return ( (aCmd != null) ? aCmd : label);
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeAll( getClass(), true);
}

public String getLabel() {
	return label;
}

public void handleShortcut( MenuShortcut ms) {
	int mods = (ms != null) ? ms.mods : 0;

	if ( (state & PUSHED) == 0 )
		animate();

	if ( (aListener != null) ||
	     ((eventMask & AWTEvent.ACTION_EVENT_MASK) != 0) ||
	     ((flags & IS_OLD_EVENT) != 0) ) {
		Toolkit.eventQueue.postEvent( ActionEvt.getEvent( this, ActionEvent.ACTION_PERFORMED,
		                                                  getActionCommand(), mods));
	}
}

public void paint ( Graphics g) {
	int d = BORDER_WIDTH;

	if ( (imgs != null) && imgs.isPlain() ) {
		g.setColor( parent.getBackground() );
		g.fillRect( 0, 0, width, height);
		if ( (state & HILIGHTED) > 0 ) {
			g.setColor( Defaults.BtnPointClr);
			g.draw3DRect( 0, 0, width-1, height-1, ((state & PUSHED) == 0));
		}
		drawImage( g);
	}
	else {
		kaffePaintBorder( g);
		g.setColor( ((state & HILIGHTED) > 0) ? Defaults.BtnPointClr : bgClr);
		g.fill3DRect( d, d, width-2*d, height-2*d, ((state & PUSHED) == 0));
		if ( imgs != null )
			drawImage( g);
		else if (label != null )
			drawText( g);
	}
}

protected String paramString() {
	return (super.paramString()
		+ ",label=" + getLabel());
}

public Dimension preferredSize () {
	int cx = 40;
	int cy = 20;
	if ( imgs != null ) {
		cx = imgs.getImage().getWidth( this) + 2*BORDER_WIDTH;
		cy = imgs.getImage().getHeight( this) + 2*BORDER_WIDTH;
	}
	else if ( fm != null ){
		cx = Math.max( cx, 4*fm.stringWidth( label)/3);
		cy = Math.max( cy, 2*fm.getHeight() );
	}
	return new Dimension( cx, cy);
}

void process ( ActionEvent e ) {
	if ( (aListener != null) || ((eventMask & AWTEvent.ACTION_EVENT_MASK) != 0) ){
		processEvent( e);
	}

	if ( (flags & IS_OLD_EVENT) != 0 ) postEvent( Event.getEvent( e));
}

void process ( FocusEvent e ) {
	repaint();

	super.process( e);
}

protected void processActionEvent( ActionEvent e) {
	if ( aListener != null ) {
		aListener.actionPerformed( e);
	}
}

void processMouse ( MouseEvent e ) {
	switch ( e.id ) {
		case MouseEvent.MOUSE_ENTERED:
			state |= HILIGHTED;
			repaint();
			break;
		case MouseEvent.MOUSE_EXITED:
			state &= ~HILIGHTED;
			repaint();
			break;
		case MouseEvent.MOUSE_PRESSED:
			state |= PUSHED;
			if ( AWTEvent.keyTgt != Button.this )
				requestFocus();
			else
				repaint();
			break;
		case MouseEvent.MOUSE_RELEASED:
			if ( contains( e.getX(), e.getY()))
				handleShortcut( null);
			state &= ~PUSHED;
			repaint();
			break;
	}

	super.processMouse( e);
}

public void removeActionListener ( ActionListener a) {
	aListener = AWTEventMulticaster.remove( aListener, a);
}

public void removeNotify () {
	if ( shortcut != null )
		ShortcutHandler.removeFromOwner( getToplevel(), shortcut);
	ShortcutHandler.removeShortcuts( this);
	super.removeNotify();
}

public void setActionCommand ( String aCmd) {
	this.aCmd = aCmd;
}

public void setFont ( Font f) {
	fm = getFontMetrics( f);
	super.setFont( f);
}

public void setLabel ( String label) {
	int ti = label.indexOf( '~');

	if ( ti > -1){
		this.label = label.substring( 0, ti) + label.substring( ti+1);
		shortcut = new MenuShortcut( this, Character.toUpperCase( label.charAt(ti+1)), KeyEvent.ALT_MASK);
	}
	else if ( label.startsWith( " ") && label.endsWith( " ") ) {
		imgs = ImageDict.getDefaultDict().getSpec( label.substring( 1, label.length() - 1), null, this );
		this.label = (imgs != null) ? null : label;
	}
	else
		this.label = label;

	if ( isShowing() )
		repaint();
}
}
