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
	private static final long serialVersionUID = 7270714317450821763L;
	CheckboxGroup group;
	int state;
	String label;
	ItemListener iListener;

	public static int counter;
	static int CHECKED = 1;
	static int HILIGHTED = 2;

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
	this.label = (label == null) ? "" : label;
	setCheckboxGroup( group);
	setState( state);
	setName("checkbox" + counter++);

	setForeground( Color.black);
	setFont( Defaults.TextFont);
	addMouseListener( this);
	addFocusListener( this);
	addKeyListener( this);
}

public synchronized void addItemListener ( ItemListener il) {
	iListener = AWTEventMulticaster.add( iListener, il);
}

void drawButton( Graphics g, int ext, int x0, int y0 ) {
	g.setColor( ((state & HILIGHTED) > 0) ? Defaults.BtnPointClr : Defaults.BtnClr);
	g.fill3DRect( x0, y0, ext, ext, true);

	if ( label.endsWith( " " ) )
		kaffePaintBorder( g);
	else {
		int d = BORDER_WIDTH;
		kaffePaintBorder( g, x0-d, y0-d, width-(x0+ext+d), height-(y0+ext+d) );
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
	state |= HILIGHTED;
	repaint();
}

public void focusLost ( FocusEvent e) {
	state &= ~HILIGHTED;
	repaint();
}

public CheckboxGroup getCheckboxGroup () {
	return group;
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeAll( getClass(), true);
}

public String getLabel () {
	return label;
}

public Object[] getSelectedObjects () {
	Object[] oa;
	if ( (state & CHECKED) > 0 ) {
		oa = new Object[1];
		oa[0] = this;
	}
	else
		oa = new Object[0];
		
	return oa;
}

public boolean getState () {
	return ((state & CHECKED) > 0);
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
			if ( ((state & CHECKED) > 0) && (group != null) )
				return;
			setState ( (state & CHECKED) == 0 );
			break;
	}

}

public void mouseClicked ( MouseEvent e) {
}

public void mouseEntered ( MouseEvent e) {
	state |= HILIGHTED;
	repaint();
}

public void mouseExited ( MouseEvent e) {
	state &= ~HILIGHTED;
	repaint();
}

public void mousePressed ( MouseEvent e) {
	requestFocus();
	if ( ((state & CHECKED) > 0) && (group != null) )
		return;
	setState( (state & CHECKED) == 0);
}

public void mouseReleased ( MouseEvent e) {
}

void notifyItem () {
	if ( (iListener != null) ||
	     ((eventMask & AWTEvent.ITEM_EVENT_MASK) != 0) ||
	     ((flags & IS_OLD_EVENT) != 0) ) {
		int id = ((state & CHECKED) > 0) ? ItemEvent.SELECTED : ItemEvent.DESELECTED;
		Toolkit.eventQueue.postEvent( ItemEvt.getEvent( this, ItemEvent.ITEM_STATE_CHANGED, label, id));
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
	
	if ( (state & CHECKED) > 0)
		drawCheckMark( g, ext, x0, y0);

	x0 += ext + dx;
	y0 += ext/2 + fh/2 - dc;
	
	c1 = ((state & HILIGHTED) > 0) ? Defaults.FocusClr : getForeground();
	c2 = Color.white;
	
	g.setColor( c2);
	g.drawString( label, x0+1, y0+1);
	g.setColor( c1);
	g.drawString( label, x0, y0);

}

protected String paramString() {
	return super.paramString() + ",label=" + label + ",state=" + getState();
}

/** 
 * @deprecated
 */
public Dimension preferredSize () {
	int cx = 50;
	int cy = 20;
	if ( font != null ){
		FontMetrics fm = getFontMetrics( font);
		cx = Math.max( cx, fm.stringWidth( label) + 2 * fm.getHeight() );
		cy = Math.max( cy, 3*fm.getHeight()/2 );
	}
	return new Dimension( cx, cy);
}

void process ( ItemEvent e ) {
	if ( (iListener != null) || ((eventMask & AWTEvent.ITEM_EVENT_MASK) != 0) ){
		processEvent( e);
	}
	
	if ( (flags & IS_OLD_EVENT) > 0 ) {
		postEvent( Event.getEvent( e));
	}
}

protected void processItemEvent( ItemEvent e) {
	if (iListener != null) {
		iListener.itemStateChanged( e);
	}
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
	this.label = (label == null) ? "" : label;
	if ( isShowing() )
		repaint();
}

public void setState ( boolean state) {
	boolean curState = ((this.state & CHECKED) > 0);
	if ( curState == state )
		return;
	if ( state && (group != null) )
		group.setSelectedCheckbox( this);
	else {
		if ( state)	
			this.state |= CHECKED;
		else
			this.state &= ~CHECKED;
		if ( isShowing() )
			repaint();
		notifyItem();
	}
}
}
