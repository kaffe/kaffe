package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
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
  implements MouseListener, FocusListener, ShortcutConsumer
{
	private static final long serialVersionUID = -8774683716313001058L;
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
	
	setFont( Defaults.BtnFont);
	setBackground( Defaults.BtnClr);
	setForeground( Defaults.BtnTxtClr);
	setLabel( label);
	
	addMouseListener( this);
	addFocusListener( this);
}

public void addActionListener ( ActionListener a) {
	aListener = AWTEventMulticaster.add( aListener, a);
	eventMask |= AWTEvent.ACTION_EVENT_MASK;
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

	if ( (state & PUSHED) > 0 ){
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

public void focusGained( FocusEvent e) {
	repaint();
}

public void focusLost( FocusEvent e) {
	repaint();
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
		
	if ( hasToNotify( this, AWTEvent.ACTION_EVENT_MASK, aListener) ||
	     ((flags & IS_OLD_EVENT) != 0) ) {
		ActionEvt ae = ActionEvt.getEvent( this, ActionEvent.ACTION_PERFORMED,
		                                   getActionCommand(), mods);
		Toolkit.eventQueue.postEvent( ae);
	}
}

public void mouseClicked ( MouseEvent evt ) {
}

public void mouseEntered ( MouseEvent evt ) {
	state |= HILIGHTED;
	repaint();
}

public void mouseExited ( MouseEvent evt ) {
	state &= ~HILIGHTED;
	repaint();
}

public void mousePressed ( MouseEvent evt ) {
	state |= PUSHED;
	if ( AWTEvent.keyTgt != this )
		requestFocus();
	else
		repaint();
}

public void mouseReleased ( MouseEvent evt ) {
	if ( contains( evt.getX(), evt.getY()))
		handleShortcut( null);
	state &= ~PUSHED;
	repaint();
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
		paintBorder( g);
		g.setColor( ((state & HILIGHTED) > 0) ? Defaults.BtnPointClr : bgClr);
		g.fill3DRect( d, d, width-2*d, height-2*d, ((state & PUSHED) == 0));
		if ( imgs != null )
			drawImage( g);
		else if (label != null )
			drawText( g);
	}
}

protected String paramString() {
	return (super.paramString() + ",Label: " + label);
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

protected void processActionEvent( ActionEvent e) {
	if ( hasToNotify( this, AWTEvent.ACTION_EVENT_MASK, aListener) ) {
		aListener.actionPerformed( e);
	}

	if ( (flags & IS_OLD_EVENT) != 0 ) postEvent( Event.getEvent( e));
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
