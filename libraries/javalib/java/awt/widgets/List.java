package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.Vector;

/**
 * class List - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class List
  extends Container
  implements ItemSelectable
{
	ActionListener aListener;
	ItemListener iListener;
	ItemPane ip = new ItemPane();
	Vector selections = new Vector( 1);
	boolean multipleMode;
	boolean selMouse;

class ItemPane
  extends RowCanvas
  implements MouseListener, MouseMotionListener, KeyListener
{
	StringBuffer lnsBuf = new StringBuffer();
	int idxFlyOver = -1;

public ItemPane () {
	addKeyListener( this);
	addMouseListener( this);
	addMouseMotionListener( this);
}

public void keyPressed( KeyEvent e) {
	int nIdx;
	int mods = e.getModifiers();
	
	//do not consume unused key for HotKeyHandler
	if ( mods != 0 )
		return;

	selMouse = false;
	
	switch ( e.getKeyCode() ) {
		case e.VK_DOWN:
			nIdx = getSelectedIndex() + 1;
			makeVisible( nIdx);
			select( nIdx);
			break;
		case e.VK_UP:
			nIdx = getSelectedIndex() - 1;
			makeVisible( nIdx);
			select( nIdx);
			break;
		case e.VK_ENTER:
			notifyAction();
			break;
		case e.VK_PAGE_UP:
			makeVisible( first - getVisibleRows());
			break;
		case e.VK_PAGE_DOWN:
			makeVisible( first + 2 * getVisibleRows() - 1);
			break;
		case e.VK_ESCAPE:
			selections.removeAllElements();
			repaint();
			break;
		default:
		  return;
	}
	
	e.consume();
}

public void keyReleased( KeyEvent e) {
}

public void keyTyped( KeyEvent e) {
	int mods = e.getModifiers();
	if ( (mods == 0) || (mods == e.SHIFT_MASK ) )
		letterNav( e.getKeyChar(), e.isShiftDown() );
}

void letterNav( char c, boolean acc) {
	int rs = rows.size();
	
	if ( !acc )
		lnsBuf.setLength( 0);
	lnsBuf.append( c);
	
	for ( int i=0; i<rs; i++) {
		String s = (String)rows.elementAt( i);
		if ( s.regionMatches( true, 0, lnsBuf.toString(), 0, lnsBuf.length() ) ){
			select( i);
			return;
		}
	}
}

public void mouseClicked( MouseEvent e) {
	if ( e.getClickCount() == 1) {
		int idx = getRowIdx( e.getY() );
		if ( idx > -1 ) {
			selMouse = true;
			select( idx);
		}	
	}
	else
		notifyAction();
}

public void mouseDragged( MouseEvent e) {
}

public void mouseEntered( MouseEvent e) {
}

public void mouseExited( MouseEvent e) {
	updateFlyOver( -1);
}

public void mouseMoved( MouseEvent e) {
	int row = getRowIdx( e.getY() );
	if ( row != idxFlyOver ) {
		updateFlyOver( row );
	}
}

public void mousePressed( MouseEvent e) {
	if ( AWTEvent.keyTgt != this )
		ip.requestFocus();
}

public void mouseReleased( MouseEvent e) {
}

public void paint ( Graphics g ) {
	paintBorder( g);
	repaintRows( first, getVisibleRows() );
}

void repaintItem( Graphics g, int idx) {
	int d = BORDER_WIDTH;		
	int x0 = xOffs + d +2;
	int y0 = d + ( idx - first) * rowHeight;
	int y1 = y0 + rowHeight - (rowHeight - fm.getHeight())/2 - fm.getDescent();
		
	String s = (String)rows.elementAt( idx);

	if ( selections.contains( s) ) {
		g.setColor( Defaults.ListSelBgClr);
		g.fill3DRect( d, y0, width-2*d, rowHeight, true);
		g.setColor( Defaults.ListSelTxtClr);
	}
	else if ( idx == idxFlyOver ) {
		g.setColor( Defaults.ListFlyOverBgClr);
		g.fill3DRect( d, y0, width-2*d, rowHeight, !Defaults.ListFlyOverInset);
		g.setColor( Defaults.ListFlyOverTxtClr);
	}
	else {
		g.setColor( bgClr );
		g.fillRect( d, y0, width-2*d, rowHeight);
		g.setColor( fgClr );
	}
	
	g.drawString( s, x0, y1);
}

void repaintRow( Graphics g, int idx) {
	repaintItem( g, idx);
}

public void setFont( Font fnt) {
	fm = getFontMetrics( fnt);
	rowHeight = 3*fm.getHeight()/2;
	super.setFont( fnt);
}

void updateFlyOver( int newIdx) {
	int lov = idxFlyOver;
	idxFlyOver = newIdx;
	
	if ( lov == newIdx )
		return;
		
	Graphics g = getClippedGraphics();
	if ( g != null) {
		if ( (lov > -1) && (lov < rows.size() ) )
			repaintItem( g, lov);
		if ( newIdx > -1 )
			repaintItem( g, newIdx);
		g.dispose();
	}
}

void updateHScroll( String as) {
	int len = fm.stringWidth( as);
	if ( (hScroll != null) && (hScroll.getMaximum() < len) ) {
		hScroll.setMaximum( len);
	}
}
}

public List () {
	this( 3, false);
}

public List ( int rows) {
	this( rows, false);
}

public List ( int rows, boolean multipleMode) {
	setMultipleMode( multipleMode);

	setLayout( null);
	ip.setForeground( Defaults.ListTxtClr);
	ip.setBackground( Defaults.ListBgClr);
	setFont( Defaults.ListFont);

	add( ip.vScroll = new Scrollbar( Scrollbar.VERTICAL));
	add( ip.hScroll = new Scrollbar( Scrollbar.HORIZONTAL));
	add( ip);

	ip.setListeners();
}

public void add ( PopupMenu m) {
	ip.add( m);
}

public void add ( String item) {
	addElement( item, -1);
}

public synchronized void add ( String item, int index) {
	addElement( item, index);
}

public synchronized void addActionListener ( ActionListener l) {
	aListener = AWTEventMulticaster.add( aListener, l);
	eventMask |= AWTEvent.ACTION_EVENT_MASK;
}

void addElement ( String item, int index) {
	if ( index == -1 )
		ip.rows.addElement( item);
	else
		ip.rows.insertElementAt( item, index);

	ip.updateHScroll( item);
	ip.updateVScroll();
	
	if ( isShowing() )
		ip.repaint();
}

public void addItem ( String item) {
	addElement( item, -1);
}

public synchronized void addItem ( String item, int index) {
	addElement( item, index);
}

public synchronized void addItemListener ( ItemListener l) {
	iListener = AWTEventMulticaster.add( iListener, l);
	eventMask |= AWTEvent.ITEM_EVENT_MASK;
}

public synchronized void delItem ( int index) {
	removeElement( index);
}

public synchronized void deselect ( int index) {
	deselectElement( index, true);
}

void deselectElement ( int index, boolean repaint) {

	try {
		Object item = ip.rows.elementAt( index);
		selections.removeElement( item);

		if ( repaint)
			ip.repaintRows( index, 1);

		notifyItem( new Integer(index), ItemEvent.DESELECTED);
	}
	catch ( Exception e) {}
}

public void doLayout () {
	ip.innerLayout();
}

public String getItem ( int index) {
	return (String)ip.rows.elementAt( index);
}

public int getItemCount () {
	return ip.rows.size();
}

public synchronized String[] getItems () {
	String[] si = new String[ ip.rows.size() ];
	for ( int i=0; i<si.length; i++)
		si[i] = (String)ip.rows.elementAt( i);
	return si;
}

public Dimension getMinimumSize () {
	return new Dimension( 50, 50);
}

public Dimension getMinimumSize ( int rows) {
	return getMinimumSize();
}

public Dimension getPreferredSize () {
	return new Dimension( 100, 100);
}

public Dimension getPreferredSize ( int rows) {
	return getPreferredSize();
}

public int getRows () {
	return ip.rows.size();
}

public synchronized int getSelectedIndex () {
	if ( selections.size() == 0 )
		return -1;
	return ip.rows.indexOf( selections.firstElement() );
}

public synchronized int[] getSelectedIndexes () {
	int il = selections.size();
	int[] ia = new int[ il];

	for ( int i=0; i<il; i++)
		ia[i] = ip.rows.indexOf( selections.elementAt( i));

	return ia;
}

public synchronized String getSelectedItem () {
	return (selections.size() > 0 ) ? (String)selections.firstElement() : null;
}

public synchronized String[] getSelectedItems () {
	int il = selections.size();
	String[] sta = new String[ il];

	for ( int i=0; i<il; i++)
		sta[i] = (String)selections.elementAt( i);

	return sta;
}

public Object[] getSelectedObjects () {
	int il = selections.size();
	Object[] oa = new Object[ il];

	for ( int i=0; i<il; i++)
		oa[i] = selections.elementAt( i);

	return oa;
}

public int getVisibleIndex () {
	return ip.first;
}

void hPosChange () {
	ip.repaint();
}

public boolean isIndexSelected ( int index) {
	Object o = ip.rows.elementAt( index);
	return selections.contains( o);
}

public boolean isMultipleMode () {
	return multipleMode;
}

void notifyAction () {
	String s = selections.size() > 0 ? (String)selections.firstElement() : null;
	if ( ( s != null) && (hasToNotify( AWTEvent.ACTION_EVENT_MASK, aListener)) ){
		ActionEvent ae = AWTEvent.getActionEvent( this, ActionEvent.ACTION_PERFORMED);
		ae.setActionEvent( s, 0);
		Toolkit.eventQueue.postEvent( ae);
	}
}

void notifyItem ( Object item, int op) {
	if ( hasToNotify( AWTEvent.ITEM_EVENT_MASK, iListener) ){
		ItemEvent ie = AWTEvent.getItemEvent( this, 0);
		ie.setItemEvent( item, op);
		Toolkit.eventQueue.postEvent( ie);
	}
}

protected String paramString() {
	return super.paramString();
}

protected void processActionEvent( ActionEvent e) {
	aListener.actionPerformed( e);
}

protected void processEvent( AWTEvent e) {
	if ( e instanceof ActionEvent )
		processActionEvent( (ActionEvent) e);
	else if ( e instanceof ItemEvent )
		processItemEvent( (ItemEvent) e);
	else
		super.processEvent( e);
}

protected void processItemEvent( ItemEvent e) {
	iListener.itemStateChanged( e);
}

public synchronized void remove ( String item) {
	removeElement( ip.rows.indexOf( item));
}

public synchronized void remove ( int index) {
	removeElement( index);
}

public synchronized void removeActionListener ( ActionListener l) {
	aListener = AWTEventMulticaster.remove( aListener, l);
}

public synchronized void removeAll () {
	selections.removeAllElements();
	ip.rows.removeAllElements();
	ip.first = 0;
	ip.updateVScroll();
	ip.repaint();
}

void removeElement ( int index) {
	try {
		deselectElement( index, false);
		ip.rows.removeElementAt( index);
	
		ip.updateVScroll();
		ip.repaintRows( index, ip.getVisibleRows() );	
	}
	catch ( Exception e) {}
}

void removeHScroll () {
	//	remove( hScroll);
	ip.hScroll = null;
}

public synchronized void removeItemListener ( ItemListener l) {
	iListener = AWTEventMulticaster.remove( iListener, l);
}

void repaintRow ( Graphics g, int idx) {
	ip.repaintItem( g, idx);
}

public synchronized void replaceItem ( String newValue, int index) {
	try {
		Object o = ip.rows.elementAt( index);
		ip.rows.setElementAt( newValue, index);
		if ( selections.contains( o) ) {
			selections.setElementAt( newValue, selections.indexOf( o) );
		}
		ip.repaintRows( index, 1);
	}
	catch ( Exception e) {
	}
}

public void requestFocus () {
	ip.requestFocus();
}

public void select ( int index) {
	if ( (index < 0) || (index > ip.rows.size() - 1) )
		return;

	Object o = ip.rows.elementAt( index);
	if ( selections.contains( o) )
		return;

	if ( ! multipleMode && ( selections.size() > 0) ) {
		Object po = selections.firstElement();
		int pIdx = ip.rows.indexOf( po);
		selections.removeAllElements();
		ip.repaintRows( pIdx, 1);
		notifyItem( new Integer( pIdx), ItemEvent.DESELECTED);
	}

	selections.addElement( o);
	ip.makeVisible( index);
	ip.repaintRows( index, 1);
	notifyItem( new Integer( index), ItemEvent.SELECTED);
}

public void setBackground ( Color c) {
	super.setBackground( c);
	ip.setBackground( c);
}

public void setBounds ( int x, int y, int width, int height) {
	super.setBounds( x, y, width, height);
	ip.updateVScroll();
}

public void setFont ( Font fnt) {
	super.setFont( fnt);
	ip.setFont( fnt);
	ip.updateVScroll();
}

public void setForeground ( Color c) {
	super.setForeground( c);
	ip.setForeground( c);
}

public synchronized void setMultipleMode ( boolean b) {
	multipleMode = b;
}

void vPosChange ( int dy) {
}
}
