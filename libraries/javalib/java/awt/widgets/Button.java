/**
 * class Button - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import kaffe.awt.ImageDict;
import kaffe.awt.ImageSpec;

public class Button
  extends Component
  implements MouseListener, KeyListener, FocusListener
{
	String label;
	boolean pushed;
	ActionListener aListener;
	String aCmd;
	boolean active;
	FontMetrics fm;
	ImageSpec imgs;

public Button () {
	this( "");
}

public Button ( String label) {
	bgClr = Defaults.BtnClr;
	cursor = Cursor.getPredefinedCursor( Cursor.HAND_CURSOR);
	
	setFont( Defaults.BtnFont);
	setBackground( Defaults.BtnClr);
	setForeground( Defaults.BtnTxtClr);
	setLabel( label);
	
	addMouseListener( this);
	addKeyListener( this);
	addFocusListener( this);
}

void activate() {
	pushed = true;
	repaint();
	Toolkit.tlkSync();
	try { Thread.sleep( 100); }
	catch ( Exception e) {}
	pushed = false;
	repaint();
}

public void addActionListener ( ActionListener a) {
	aListener = AWTEventMulticaster.add( aListener, a);
	eventMask |= AWTEvent.ACTION_EVENT_MASK;
}

void drawImage ( Graphics g) {
	Image img = imgs.getImage();
	int iw = img.getWidth( this);
	int ih = img.getHeight( this);
	int di = pushed ? 1 : 0;
	int x = (width - iw) / 2 + di;
	int y = (height - ih) / 2 + di;
	
	g.drawImage( img, x, y, this);
}

void drawText ( Graphics g) {
	Color c1, c2;
	int x = (width - fm.stringWidth( label)) / 2;
	int y = height - (height - fm.getHeight()) / 2 - fm.getDescent();

	if ( pushed ){
		x--; y--;
		c1 = Color.yellow;
		c2 = Color.red;
	}
	else {
		if ( active ) {
			c1 = bgClr.brighter();
			c2 = Defaults.FocusClr;
		}
		else {
			c1 = bgClr.brighter();
			c2 = fgClr;
		}
	}

	g.setColor( c1);
	g.drawString( label, x+1, y+1);
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

public void keyPressed ( KeyEvent evt ) {
	int mods = evt.getModifiers();
	
	//do not consume unused key for HotKeyHandler
	if ( mods != 0 )
		return;
		
	switch( evt.getKeyCode() ) {
		case KeyEvent.VK_ENTER:
			activate();
			notifyAction();
			break;
		default:
		  return;
	}
	
	evt.consume();
}

public void keyReleased ( KeyEvent evt ) {
}

public void keyTyped ( KeyEvent evt ) {	
	switch( evt.getKeyChar() ) {
		case ' ':
			activate();
			notifyAction();
			break;
	}
}

public void mouseClicked ( MouseEvent evt ) {
}

public void mouseEntered ( MouseEvent evt ) {
	active = true;
	repaint();
}

public void mouseExited ( MouseEvent evt ) {
	active = false;
	repaint();
}

public void mousePressed ( MouseEvent evt ) {
	pushed = true;
	if ( AWTEvent.keyTgt != this )
		requestFocus();
	else
		repaint();
}

public void mouseReleased ( MouseEvent evt ) {
	pushed = false;
	if ( contains( evt.getX(), evt.getY()))
		notifyAction();
	repaint();
}

void notifyAction() {
	if ( hasToNotify( this, AWTEvent.ACTION_EVENT_MASK, aListener) || oldEvents ) {
		ActionEvt ae = ActionEvt.getEvent( this, ActionEvent.ACTION_PERFORMED,
		                                   getActionCommand(), 0);
		Toolkit.eventQueue.postEvent( ae);
	}
}

public void paint ( Graphics g) {
	int d = BORDER_WIDTH;
	
	if ( (imgs != null) && imgs.isPlain() ) {
		g.setColor( parent.getBackground() );
		g.fillRect( 0, 0, width, height);
		if ( active ) {
			g.setColor( Defaults.BtnPointClr);
			g.draw3DRect( 0, 0, width-1, height-1, !pushed);
		}
		drawImage( g);
	}
	else {
		paintBorder( g);
		g.setColor( active ? Defaults.BtnPointClr : bgClr);
		g.fill3DRect( d, d, width-2*d, height-2*d, !pushed);
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
	if ( AWTEvent.keyTgt != this )
		activate();
		
	if ( hasToNotify( this, AWTEvent.ACTION_EVENT_MASK, aListener) ) {
		aListener.actionPerformed( e);
	}

	if ( oldEvents ) postEvent( Event.getEvent( e));
}

public void removeActionListener ( ActionListener a) {
	aListener = AWTEventMulticaster.remove( aListener, a);
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
		int code = (int)label.charAt( ti+1);
		HotKeyHandler.addHotKey( this, code, KeyEvent.ALT_MASK, this.label );
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
