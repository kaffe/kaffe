package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;
import java.util.Vector;

/**
 * class Choice - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Choice
  extends Container
  implements ItemSelectable, ActionListener, ItemListener, MouseListener, KeyListener
{
	Vector items = new Vector();
	Object selection;
	ItemListener iListener;
	ChoiceWindow prompter;
	TextField entry = new TextField();
	boolean hilight;
	static int BTN_WIDTH = 15;

class ChoiceWindow
  extends Window
{
	List list = new List();

public ChoiceWindow ( Frame parent) {
	super( parent );
	setLayout( null);
	list.removeHScroll();
	this.add( list);
}

public void doLayout() {
	int db = 1;
	list.setBounds( db, db, width-2*db, height-2*db);
}

void popUpAt( int x, int y, int width, int height) {
	int is = items.size();

	setBounds( x, y, width, height);

	for ( int i=0; i<is; i++)
		list.add( (String)items.elementAt( i));

	setVisible( true);
	list.select( items.indexOf( selection));
	
	list.requestFocus();
}
}

public Choice () {
	setLayout( null);
	setBackground( Color.lightGray);
	add ( entry);
	
	entry.addActionListener( this);
	entry.addKeyListener( this);
	
	addMouseListener( this);
}

public void actionPerformed ( ActionEvent e) {
	select( e.getActionCommand() );
}

public synchronized void add ( String item) {
	addItem( item);
}

public synchronized void addItem ( String item) {
	insert( item, items.size() );
}

public synchronized void addItemListener ( ItemListener il) {
	iListener = AWTEventMulticaster.add( iListener, il);
	eventMask |= AWTEvent.ITEM_EVENT_MASK;
}

void closePrompt() {
	if ( prompter != null) {
		prompter.list.removeItemListener( this);
		prompter.list.removeActionListener( this);

		prompter.dispose();
		prompter = null;
		repaint();
		
		entry.requestFocus();
	}
}

public void doLayout () {
	int d = BORDER_WIDTH;
	entry.setBounds( 0, 0, width-BTN_WIDTH-2*d, height);
}

public String getItem ( int index) {
	return (String)items.elementAt( index);
}

public int getItemCount () {
	return items.size();
}

public Dimension getPreferredSize () {
	Dimension d = entry.getPreferredSize();
	d.width += BTN_WIDTH;
	return d;
}

public int getSelectedIndex () {
	return items.indexOf( selection);
}

public synchronized String getSelectedItem () {
	return (String)selection;
}

public synchronized Object[] getSelectedObjects () {
	Object[] oa;
	if ( selection != null) {
		oa = new Object[1];
		oa[0] = selection;
	}
	else
		oa = new Object[0];
		
	return oa;
}

public synchronized void insert ( String item, int index) {
	if ( items.size() == 0) {
		selection = item;
		entry.setText( item);
	}
	items.insertElementAt( item, index);
}

public void itemStateChanged ( ItemEvent e) {
	if ( e.getStateChange() == e.SELECTED ){
		if ( prompter.list.selMouse )
			select( prompter.list.getSelectedItem() );
	}
}

public void keyPressed( KeyEvent e) {
	int cc = e.getKeyCode();
	if ( cc == e.VK_DOWN )
		openPrompt();
}

public void keyTyped( KeyEvent e) {
}

public void keyReleased( KeyEvent e) {
}

public void mouseClicked ( MouseEvent e) {
}

public void mouseEntered ( MouseEvent e) {
	hilight = true;
	paintButton();
}

public void mouseExited ( MouseEvent e) {
	hilight = false;
	paintButton();
}

public void mousePressed ( MouseEvent e) {
	if ( prompter == null)
		openPrompt();
	else
		closePrompt();
}

public void mouseReleased ( MouseEvent e) {
}

void notifyItem() {
	if ( hasToNotify( AWTEvent.ITEM_EVENT_MASK, iListener) ) {
		ItemEvent ie = AWTEvent.getItemEvent( this, 0);
		ie.setItemEvent( selection, ItemEvent.SELECTED);
		Toolkit.eventQueue.postEvent( ie);
	}
}

void openPrompt() {
	if ( prompter != null)
		return;
	Frame fr = (Frame)entry.getToplevel();
	prompter = new ChoiceWindow( fr );
	Point p = getLocationOnScreen();
	prompter.popUpAt( p.x, p.y+height+1, width,
	                  Math.min( items.size() + 1, 8)*Defaults.WndFontMetrics.getHeight());
	prompter.list.addItemListener( this);
	prompter.list.addActionListener( this);
	repaint();
}

public void paint ( Graphics g) {
	super.paint( g);
	paintButton( g);
}

void paintButton() {
	Graphics g = getGraphics();
	if ( g != null ) {
		paintButton( g);
		g.dispose();
	}
}

void paintButton( Graphics g) {
	int x, y;
	int d = BORDER_WIDTH;
	int db = BTN_WIDTH;

	g.setColor( hilight ? Defaults.BtnPointClr : Defaults.BtnClr);
	g.fill3DRect ( width-db-d, d, db, height-2*d, prompter == null );
	
	//draw button marker
	x = width - d - db/2;
	y = height - (height-7)/2;
	g.setColor( Color.white);
	g.drawLine( x, y, x-3, y-7);
	g.drawLine( x, y, x+3, y-7);
	x--;
	g.setColor( hilight ? Defaults.FocusClr : Color.black);
	g.drawLine( x, y, x-3, y-7);
	g.drawLine( x, y, x+3, y-7);
	
	//draw non focus border around button
	g.setColor( Defaults.BtnClr);
	x = width - db - 2*d;
	g.draw3DRect( x, 0,  width-x-1, height-1, true);
	g.draw3DRect( x+1, 1, width-x-3, height-3, false);
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

public synchronized void remove ( String item) {
	items.removeElement( item);
}

public synchronized void removeAll () {
	items.removeAllElements();
}

public synchronized void removeItemListener ( ItemListener il) {
	iListener = AWTEventMulticaster.remove( iListener, il);
}

public void requestFocus () {
	entry.requestFocus();
}

public synchronized void select ( String item) {
	closePrompt();
	if ( items.contains( item) ) {
		selection = item;
		if ( ! entry.getText().equals( item) )
			entry.setText( item);
		notifyItem();
	}
}

public synchronized void select ( int index) {
	select( (String)items.elementAt( index));
}
}
