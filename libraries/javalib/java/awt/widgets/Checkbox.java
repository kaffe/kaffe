package java.awt;

import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

/**
 * class Checkbox - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Checkbox
  extends Component
  implements ItemSelectable, MouseListener, FocusListener, KeyListener
{
	CheckboxGroup group;
	boolean state;
	String label;
	ItemListener iListener;
	boolean hilight;

public Checkbox () {
	this( "", false, null);
}

public Checkbox ( String label) {
	this( label, false, null);
}

public Checkbox ( String label, CheckboxGroup group, boolean state) {
	this( label, state, group);
}

public Checkbox ( String label, boolean state) {
	this( label, state, null);
}

public Checkbox ( String label, boolean state, CheckboxGroup group) {
	this.label = label;
	setCheckboxGroup( group);
	setState( state);
	
	setBackground( Color.lightGray);
	setForeground( Color.black);
	setFont( Defaults.TextFont);
	addMouseListener( this);
	addFocusListener( this);
	addKeyListener( this);
}

public synchronized void addItemListener ( ItemListener il) {
	iListener = AWTEventMulticaster.add( iListener, il);
	eventMask |= AWTEvent.ITEM_EVENT_MASK;
}

void drawButton( Graphics g, int ext, int x0, int y0 ) {
	g.setColor( hilight ? Defaults.BtnPointClr : Defaults.BtnClr);
	g.fill3DRect( x0, y0, ext, ext, true);

	if ( label.endsWith( " " ) )
		paintBorder( g);
	else {
		int d = BORDER_WIDTH;
		paintBorder( g, x0-d, y0-d, width-(x0+ext+d), height-(y0+ext+d) );
	}

}

void drawCheckMark( Graphics g, int ext, int x0, int y0 ) {
	g.setColor( Color.black);
	g.drawLine( x0+3, y0+4, x0+ext-5, y0+ext-4);
	g.drawLine( x0+2, y0+ext-4, x0+ext-5, y0+3);

	g.setColor( Color.white);
	g.drawLine( x0+3, y0+3, x0+ext-4, y0+ext-4);
	g.drawLine( x0+3, y0+ext-4, x0+ext-4, y0+3);
}

public void focusGained ( FocusEvent e) {
	hilight = true;
	repaint();
}

public void focusLost ( FocusEvent e) {
	hilight = false;
	repaint();
}

public CheckboxGroup getCheckboxGroup () {
	return group;
}

public String getLabel () {
	return label;
}

public Dimension getPreferredSize () {
	int cx = 50;
	int cy = 20;
	if ( font != null ){
		FontMetrics fm = getFontMetrics( font);
		cx = Math.max( cx, fm.stringWidth( label) + 2 * fm.getHeight() );
		cy = Math.max( cy, 3*fm.getHeight()/2 );
	}
	return new Dimension( cx, cy);
}

public Object[] getSelectedObjects () {
	Object[] oa;
	if ( state ) {
		oa = new Object[1];
		oa[0] = this;
	}
	else
		oa = new Object[0];
		
	return oa;
}

public boolean getState () {
	return state;
}

public void keyPressed ( KeyEvent e) {
}

public void keyReleased ( KeyEvent e) {
}

public void keyTyped ( KeyEvent e) {
	char c = e.getKeyChar();

	switch ( c) {
		case ' ':
		case 0xA:	//ENTER
			if ( state && (group != null) )
				return;
			setState ( ! state );
			break;
	}

}

public void mouseClicked ( MouseEvent e) {
}

public void mouseEntered ( MouseEvent e) {
	hilight = true;
	repaint();
}

public void mouseExited ( MouseEvent e) {
	hilight = false;
	repaint();
}

public void mousePressed ( MouseEvent e) {
	requestFocus();
	if ( state && (group != null) )
		return;
	setState( !state);
}

public void mouseReleased ( MouseEvent e) {
}

void notifyItem () {
	if ( hasToNotify( AWTEvent.ITEM_EVENT_MASK, iListener) ){
		ItemEvent ie = AWTEvent.getItemEvent( this, 0);
		ie.setItemEvent( label, state ? ItemEvent.SELECTED : ItemEvent.DESELECTED );
		Toolkit.eventQueue.postEvent( ie);
	}
}

public void paint ( Graphics g) {
	Color c1, c2;
	FontMetrics fm = getFontMetrics( font);
	int fh	= fm.getHeight();
	int ext = fh;                 // this controls the button size
	int dc  = fm.getDescent();
	int dx  = fm.charWidth( 'x');
	int x0  = dx;
	int y0  = (height - ext) / 2;
	
	g.setColor( bgClr);
	g.fillRect( 0, 0, width, height);
	
	drawButton( g, ext, x0, y0);
	
	if ( state)
		drawCheckMark( g, ext, x0, y0);

	x0 += ext + dx;
	y0 += ext/2 + fh/2 - dc;
	
	c1 = hilight ? Defaults.FocusClr : getForeground();
	c2 = Color.white;
	
	g.setColor( c2);
	g.drawString( label, x0+1, y0+1);
	g.setColor( c1);
	g.drawString( label, x0, y0);

}

protected String paramString() {
	return super.paramString();
}

protected void processEvent( AWTEvent e) {
	if ( e instanceof ItemEvent) 
		processItemEvent( (ItemEvent) e);
	else
		super.processEvent( e);
}

protected void processItemEvent( ItemEvent e) {
	iListener.itemStateChanged( e);
}

public synchronized void removeItemListener ( ItemListener il) {
	iListener = AWTEventMulticaster.remove( iListener, il);
}

public void setCheckboxGroup ( CheckboxGroup group) {
	if ( this.group == group )
		return;
		
	if ( this.group != null)
		this.group.boxes.removeElement( this);
		
	this.group = group;
	
	if ( this.group != null)
		this.group.boxes.addElement( this);
}

public synchronized void setLabel ( String label) {
	this.label = label;
	if ( isShowing() )
		repaint();
}

public void setState ( boolean state) {
	if ( this.state == state )
		return;
	if ( state && (group != null) )
		group.setSelectedCheckbox( this);
	else {
		this.state = state;
		if ( isShowing() )
			repaint();
		notifyItem();
	}
}
}
