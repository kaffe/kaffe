package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowEvent;
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
  implements ItemSelectable, ActionListener, ItemListener, MouseListener, KeyListener, FocusListener
{
	final private static long serialVersionUID = -4075310674757313071L;
	Vector items = new Vector();
	Object selection;
	ItemListener iListener;
	ChoiceWindow prompter;
	TextField entry = new TextField();
	int state;
	static int BTN_WIDTH = 15;
	static int HILIGHTED = 1;

class ChoiceWindow
  extends Window
{
	List list = new List();

public ChoiceWindow ( Frame owner ) {
	super( owner);

	this.setLayout( null);
	list.removeHScroll();
	this.add( list);
}

public void doLayout() {
	int db = 1;
	list.setBounds( db, db, this.width-2*db, this.height-2*db);
}

void popUpAt( int x, int y, int width, int height) {
	int is = items.size();

	this.setBounds( x, y, width, height);
	list.setFont( Choice.this.font);

	for ( int i=0; i<is; i++)
		list.add( (String)items.elementAt( i));

	this.setVisible( true);
	list.select( items.indexOf( selection));
	
	prompter.list.requestFocus();
}

void process ( WindowEvent e ) {
	super.process( e);

	if ( e.id == WindowEvent.WINDOW_DEACTIVATED )
		closePrompt( false);
}
}

public Choice () {
	setLayout( null);
	setBackground( Color.lightGray);
	add ( entry);
	
	entry.setEditable(false);
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
}

void closePrompt( boolean resetFocus) {
	if ( prompter != null) {
		prompter.list.removeItemListener( this);
		prompter.list.removeActionListener( this);
		prompter.list.removeFocusListener( this);

		prompter.dispose();
		prompter = null;
		paintButton();
		
		if ( resetFocus)
			entry.requestFocus();
	}
}

public int countItems() {
        return getItemCount();
}

public void doLayout () {
	int d = BORDER_WIDTH;
	entry.setBounds( 0, 0, width-BTN_WIDTH-2*d, height);
}

public void focusGained ( FocusEvent e) {
}

public void focusLost ( FocusEvent e) {
	if ( (prompter != null) && (e.getSource() == prompter.list) ) {
		closePrompt( false);
	}
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeAll( getClass(), true);
}

public String getItem ( int index) {
	return (String)items.elementAt( index);
}

public int getItemCount () {
	return (getItems());
}

/**
 * @deprecated
 */
public int getItems() {
	return items.size();
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

public void keyPressed ( KeyEvent e ) {
	Object src = e.getSource();
	int cc = e.getKeyCode();
	
	if ( (prompter != null) && (src == prompter.list) ){
		if ( cc == e.VK_ESCAPE )
			closePrompt( true);
	}
	else {	
		if ( cc == e.VK_DOWN )
			openPrompt();
	}
}

public void keyReleased( KeyEvent e) {
}

public void keyTyped( KeyEvent e) {
}

public void mouseClicked ( MouseEvent e) {
}

public void mouseEntered ( MouseEvent e) {
	state |= HILIGHTED;
	paintButton();
}

public void mouseExited ( MouseEvent e) {
	state &= ~HILIGHTED;
	paintButton();
}

public void mousePressed ( MouseEvent e) {
	if ( prompter == null)
		openPrompt();
	else
		closePrompt( true);
}

public void mouseReleased ( MouseEvent e) {
}

void notifyItem() {
	if ( (iListener != null) || (eventMask & AWTEvent.ITEM_EVENT_MASK) != 0 ){
		Toolkit.eventQueue.postEvent( ItemEvt.getEvent( this, ItemEvent.ITEM_STATE_CHANGED,
		                                                selection, ItemEvent.SELECTED));
	}
}

void openPrompt() {
	if ( prompter == null) {
		Component top = getToplevel();
		prompter = new ChoiceWindow( (top instanceof Frame) ? (Frame)top : null );
		
		Point p = getLocationOnScreen();
		prompter.popUpAt( p.x, p.y+height+1, width,
		                  Math.min( items.size() + 1, 8)*Defaults.WndFontMetrics.getHeight());
		prompter.list.addItemListener( this);
		prompter.list.addActionListener( this);
		prompter.list.addKeyListener( this);
//		prompter.list.addFocusListener( this);
		
		repaint();
	}
}

public void paint ( Graphics g) {
	super.paint( g);
	paintButton( g);
}

void paintButton() {
	if ( isShowing() ) {
		Graphics g = getGraphics();
		paintButton( g);
		g.dispose();
	}
}

void paintButton( Graphics g) {
	int x, y;
	int d = BORDER_WIDTH;
	int db = BTN_WIDTH;

	g.setColor( ((state & HILIGHTED) > 0) ? Defaults.BtnPointClr : Defaults.BtnClr);
	g.fill3DRect ( width-db-d, d, db, height-2*d, prompter == null );
	
	//draw button marker
	x = width - d - db/2;
	y = height - (height-7)/2;
	g.setColor( Color.white);
	g.drawLine( x, y, x-3, y-7);
	g.drawLine( x, y, x+3, y-7);
	x--;
	g.setColor( ((state & HILIGHTED) > 0) ? Defaults.FocusClr : Color.black);
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

/**
 * @deprecated
 */
public Dimension preferredSize () {
    /* Instead of simply asking the current selection's size,
     * we better find the maximum...
     */
    //Dimension d = entry.getPreferredSize();
    Dimension d = new Dimension();
    for (int i = items.size(); --i >= 0; ) {
       Dimension d2 = entry.getPreferredSize(
          ((String) items.elementAt(i)).length());
       if (d2.width > d.width)
           d.width = d2.width;
       if (d2.height > d.height)
           d.height = d2.height;
    }
    d.width += BTN_WIDTH + 2*BORDER_WIDTH;
    return d;
}

void process ( ItemEvent e ) {
	if ( (iListener != null) || ((eventMask & AWTEvent.ITEM_EVENT_MASK) != 0) ){
		processEvent( e);
	}
}

protected void processItemEvent( ItemEvent e) {
	if ( iListener != null ){
		iListener.itemStateChanged( e);
	}
}

public synchronized void remove ( String item) {
	items.removeElement( item);
}

public synchronized void remove ( int index ) {
	if (index >= 0 && index < items.size()) {
		items.removeElementAt( index);
	}
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

public void reshape ( int x, int y, int w, int h ) {
	super.reshape( x, y, w, h);
	
	// there is no need for validation of compound IS_NATIVE_LIKES, they are no Containers
	// in JDK, so we automagically have to re-layout them
	doLayout();
	flags |= IS_VALID;
}

public synchronized void select ( String item) {
	closePrompt( true);
	if ( items.contains( item) ) {
		selection = item;
		if ( ! entry.getText().equals( item) )
			entry.setText( item);
		notifyItem();
	}
}

public synchronized void select ( int index) {
	if (index >= 0 && index < items.size()) {
		select((String)items.elementAt(index));
	}
}

public void setEnabled ( boolean isEnabled ) {
	super.setEnabled( isEnabled);

	entry.setEnabled( isEnabled);
}

public void setFont ( Font fnt) {
	super.setFont( fnt);
	entry.setFont( fnt);
}
}
