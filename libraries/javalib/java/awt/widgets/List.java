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
import java.util.BitSet;
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
	final private static long serialVersionUID = -3304312411574666869L;
	ActionListener aListener;
	ItemListener iListener;
	ItemPane ip = new ItemPane();
	Vector selections = new Vector( 1);
	boolean multipleMode;
	boolean selMouse;
	int sel = -1;
	BitSet multiSel;
	int nSel;

class ItemPane
  extends RowCanvas
  implements MouseListener, MouseMotionListener, KeyListener
{
	StringBuffer lnsBuf = new StringBuffer();
	int idxFlyOver = -1;

public ItemPane () {
	this.addKeyListener( this);
	this.addMouseListener( this);
	this.addMouseMotionListener( this);
	this.addFocusListener( this);
}

public void keyPressed( KeyEvent e) {
	int nIdx;
	int mods = e.getModifiers();

	if ( this.parent.keyListener != null ){
		// give our parent a chance to intercept keystrokes
		// check for keyListeners first, it's a rare case
		redirectKeyEvent( e);
	}

	//do not consume unused key for HotKeyHandler
	if ( mods != 0 )
		return;

	selMouse = false;

	switch ( e.getKeyCode() ) {
		case KeyEvent.VK_DOWN:
			nIdx = getSelectedIndex() + 1;
			this.makeVisible( nIdx);
			selectEvent( nIdx, true);
			break;
		case KeyEvent.VK_UP:
			nIdx = getSelectedIndex() - 1;
			this.makeVisible( nIdx);
			selectEvent( nIdx, true);
			break;
		case KeyEvent.VK_ENTER:
			notifyAction();
			break;
		case KeyEvent.VK_PAGE_UP:
			this.makeVisible( first - getVisibleRows());
			break;
		case KeyEvent.VK_PAGE_DOWN:
			this.makeVisible( first + 2 * getVisibleRows() - 1);
			break;
		case KeyEvent.VK_ESCAPE:
			clearSelections();
			this.repaint();
			break;
		default:
		  return;
	}

	e.consume();
}

public void keyReleased( KeyEvent e) {
	if ( this.parent.keyListener != null ){
		// check for keyListeners first, it's a rare case
		redirectKeyEvent( e);
		e.consume();
	}
}

public void keyTyped( KeyEvent e) {
	int mods = e.getModifiers();
	if ( (mods == 0) || (mods == KeyEvent.SHIFT_MASK ) ) {
		letterNav( e.getKeyChar(), e.isShiftDown() );
		e.consume();
	}

	if ( this.parent.keyListener != null ){
		// check for keyListeners first, it's a rare case
		redirectKeyEvent( e);
	}
}

void letterNav( char c, boolean acc) {
	int rs = rows.size();

	if ( !acc )
		lnsBuf.setLength( 0);
	lnsBuf.append( c);

	for ( int i=0; i<rs; i++) {
		String s = (String)rows.elementAt( i);
		if ( s.regionMatches( true, 0, lnsBuf.toString(), 0, lnsBuf.length() ) ){
			selectEvent( i, true);
			return;
		}
	}
}

int maxRowWidth() {
	int rs = rows.size();
	int iw, mw = 0;

	for ( int i=0; i<rs; i++ ) {
		iw = fm.stringWidth( (String)rows.elementAt( i) );
		if ( iw > mw )
			mw = iw;
	}

	return mw;
}

public void mouseClicked( MouseEvent e) {
	if ( e.getClickCount() == 1) {
		int idx = getRowIdx( e.getY() );
		if ( idx > -1 ) {
			selMouse = true;

			if ( isIndexSelected( idx) ) {
				if ( multipleMode )
					deselectElement( idx, true, true);
			}
			else {
				selectEvent( idx, true);
			}
		}
	}
	else
		notifyAction();

	redirectMouseEvent( e);
}

public void mouseDragged( MouseEvent e) {
	if ( this.parent.motionListener != null ){
		// unlikely, check listener first
		redirectMotionEvent( e);
	}
}

public void mouseEntered( MouseEvent e) {
	redirectMouseEvent( e);
}

public void mouseExited( MouseEvent e) {
	if ( rgr != null ) // otherwise we aren't visible anymore
		updateFlyOver( -1);

	redirectMouseEvent( e);
}

public void mouseMoved( MouseEvent e) {
	int row = getRowIdx( e.getY() );
	if ( row != idxFlyOver ) {
		updateFlyOver( row );
	}

	if ( this.parent.motionListener != null ){
		// unlikely, check listener first
		redirectMotionEvent( e);
	}
}

public void mousePressed( MouseEvent e) {
	if ( AWTEvent.keyTgt != ip )
		ip.requestFocus();

	if ( e.isPopupTrigger() )
		this.triggerPopup( e.getX(), e.getY());

	redirectMouseEvent( e);
}

public void mouseReleased( MouseEvent e) {
	redirectMouseEvent( e);
}

public void paint ( Graphics g ) {
	repaintRows( g, first, getVisibleRows() );
	this.kaffePaintBorder( g);
}

void repaintItem( Graphics g, int idx) {
	if ( g == null)
		return;

	int d = BORDER_WIDTH;
	int x0 = xOffs + d +2;
	int y0 = d + ( idx - first) * rowHeight;
	int y1 = y0 + rowHeight - (rowHeight - fm.getHeight())/2 - fm.getDescent();

	String s = (String)rows.elementAt( idx);

	if ( isIndexSelected( idx) ) {
		g.setColor( Defaults.ListSelBgClr);
		g.fill3DRect( d, y0, this.width-2*d, rowHeight, true);
		g.setColor( Defaults.ListSelTxtClr);
	}
	else if ( idx == idxFlyOver ) {
		g.setColor( Defaults.ListFlyOverBgClr);
		g.fill3DRect( d, y0, this.width-2*d, rowHeight, !Defaults.ListFlyOverInset);
		g.setColor( Defaults.ListFlyOverTxtClr);
	}
	else {
		g.setColor( this.bgClr );
		g.fillRect( d, y0, this.width-2*d, rowHeight);
		g.setColor( this.fgClr );
	}

	g.drawString( s, x0, y1);
}

void repaintItem( int idx) {
	if ( rgr != null )
		repaintItem( rgr, idx);
}

void repaintRow( Graphics g, int idx) {
	repaintItem( g, idx);
}

public void setFont( Font fnt) {
	fm = this.getFontMetrics( fnt);
	rowHeight = fm.getHeight() + 1;

	super.setFont( fnt);
}

void updateFlyOver( int newIdx) {
	int lov = idxFlyOver;
	idxFlyOver = newIdx;

	if ( lov == newIdx )
		return;

	int rs = rows.size();
	if ( (lov > -1) && (lov < rs) )
		repaintItem( lov);
	if ( (newIdx > -1) && (newIdx < rs) )
		repaintItem( newIdx);
}
}

public List () {
	this( 4, false);
}

public List ( int rows) {
	this( rows, false);
}

public List ( int rows, boolean multipleMode) {
	setMultipleMode( multipleMode);

	setLayout( null);

	setForeground( Defaults.ListTxtClr);
	setBackground( Defaults.ListBgClr);
	setFont( Defaults.ListFont);

	add( ip.vScroll = new Scrollbar( Scrollbar.VERTICAL,0,0,0,0));
	add( ip.hScroll = new Scrollbar( Scrollbar.HORIZONTAL,0,0,0,0));
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
}

void addElement ( String item, int index) {
	if ( index == -1 )
		ip.rows.addElement( item);
	else
		ip.rows.insertElementAt( item, index);

	ip.updateScrolls();

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
}

/**
 * @deprecated
 */
public boolean allowsMultipleSelections() {
	return multipleMode;
}

/**
 * @deprecated
 */
public synchronized void clear() {
	clearSelections();

	ip.rows.removeAllElements();
	ip.first = 0;
	ip.updateScrolls();
	ip.repaint();
}

void clearSelection ( int index ) {
	if ( sel == index )
		sel = -1;

	if ( multipleMode ) {
		if (multiSel.get(index)) {
			multiSel.clear( index);
			nSel--;
		}
	}
}

void clearSelections () {
	sel = -1;
	nSel = 0;

	if ( multipleMode ){
		nSel = 0;
		multiSel = new BitSet( ip.rows.size());
	}
}

/**
 * @deprecated
 */
public int countItems() {
	return ip.rows.size();
}

public synchronized void delItem ( int index) {
	removeElement( index);
}

/**
 * @deprecated
 */
public synchronized void delItems ( int start, int end ) {
	for (int i = end; i >= start; i--) {
		remove(i);
	}
}

public synchronized void deselect ( int index) {
	deselectElement( index, true, false);
}

void deselectElement ( int index, boolean repaint, boolean fire) {

	try {
		Object item = ip.rows.elementAt( index);
		clearSelection( index);

		if ( repaint)
			ip.repaintRows( index, 1);

		if (fire) {
			notifyItem( new Integer(index), ItemEvent.DESELECTED);
		}
	}
	catch ( Exception e) {}
}

public void doLayout () {
	ip.innerLayout();
}

public Color getBackground () {
	// some anomaly, we forward colors to our ip, so we should return its colors
	// for consistencies sake
	return ip.getBackground();
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeAll( getClass(), true);
}

public Color getForeground () {
	// some anomaly, we forward colors to our ip, so we should return its colors
	// for consistencies sake
	return ip.getForeground();
}

public String getItem ( int index) {
	return (String)ip.rows.elementAt( index);
}

public int getItemCount () {
	return (countItems());
}

public synchronized String[] getItems () {
	String[] si = new String[ ip.rows.size() ];
	for ( int i=0; i<si.length; i++) {
		si[i] = (String)ip.rows.elementAt( i);
	}
	return si;
}

public Dimension getMinimumSize(int rows) {
        return minimumSize(rows);
}

public int getRows () {
	return ip.rows.size();
}

public Dimension getPreferredSize(int rows) {
        return preferredSize(rows);
}

public synchronized int getSelectedIndex () {
	return sel;
}

public synchronized int[] getSelectedIndexes () {
	int[] r;
	int   i, j, n;

	if ( multipleMode ) {
		if ( nSel == 0 )
			return new int[0];
		else if ( (nSel == 1) && (sel >= 0) ){
			r = new int[1];
			r[0] = sel;
			return r;
		}
		else {
			r = new int[nSel];
			n = Math.min( ip.rows.size(), multiSel.size());

			for ( i=0, j=0; i<n; i++ ) {
				if ( multiSel.get( i) ){
					r[j++] = i;
					if ( j == nSel )
						break;
				}
			}
			return r;
		}
	}
	else {
		if ( sel >= 0 ) {
			r = new int[1];
			r[0] = sel;
			return r;
		}
		else {
			return new int[0];
		}
	}
}

public synchronized String getSelectedItem () {
	if ( sel >= 0 ) {
		return (String) ip.rows.elementAt( sel);
	}
	else if ( nSel > 0 ) {
		int  n = Math.min( multiSel.size(), ip.rows.size());
		int  i;

		for ( i=0; i<n; i++ ){
			if ( multiSel.get(i) )
				return (String) ip.rows.elementAt( i);
		}
	}

	return null;
}

public synchronized String[] getSelectedItems () {
	// Not very nice to copy this from getSelectedIndexes, but some people
	// don't care how many items they have in their lists, and creating this
	// from a temporary int[] might produce a lot of garbage. Moreover, some
	// people just don't care for if they are in multiMode and use this
	// as a default (i.e. also in single mode)

	String[] r;
	int   i, j, n;

	if ( multipleMode ) {
		if ( nSel == 0 )
			return new String[0];
		else if ( (nSel == 1) && (sel >= 0) ){
			r = new String[1];
			r[0] = (String) ip.rows.elementAt( sel);
			return r;
		}
		else {
			r = new String[nSel];
			n = Math.min( ip.rows.size(), multiSel.size());

			for ( i=0, j=0; i<n; i++ ) {
				if ( multiSel.get( i) ){
					r[j++] = (String) ip.rows.elementAt( i);
					if ( j == nSel )
						break;
				}
			}
			return r;
		}
	}
	else {
		if ( sel >= 0 ) {
			r = new String[1];
			r[0] = (String) ip.rows.elementAt( sel);
			return r;
		}
		else {
			return new String[0];
		}
	}
}

public Object[] getSelectedObjects () {
	// Now what is this - we can't put arbitrary Objects in there, but we can get
	// them out? Seems like somebody finally discovered the potential of Lists,
	// but didn't finish on it.
	// Since we don't consider this the default access method (unfortunately), we
	// don't do the same expensive processing like in getSelectedItems

	int      i;
	int[]    selIdx = getSelectedIndexes();
	Object[] selObj = new Object[selIdx.length];

	for ( i=0; i<selIdx.length; i++ ) {
		selObj[i] = ip.rows.elementAt( selIdx[i]);
	}

	return selObj;
}

public int getVisibleIndex () {
	return ip.first;
}

void hPosChange () {
	ip.repaint();
}

public boolean isIndexSelected ( int index) {
	return (isSelected(index));
}

public boolean isMultipleMode () {
	return (allowsMultipleSelections());
}

/**
 * @deprecated
 */
public boolean isSelected ( int index ) {
	if ( sel == index )
		return true;

	if ( multipleMode )
		return multiSel.get( index);

	return false;
}

public void makeVisible ( int idx ) {
	ip.makeVisible( idx);
}

public Dimension minimumSize () {
	return minimumSize(0);
}

public Dimension minimumSize (int rows) {
	return preferredSize(rows);
}

void notifyAction () {
	String s = (sel >= 0) ? (String) ip.rows.elementAt( sel) : null;

	if ( (s != null) &&
	     ((aListener != null) || (eventMask & AWTEvent.ACTION_EVENT_MASK) != 0) ) {
		Toolkit.eventQueue.postEvent( ActionEvt.getEvent( this, ActionEvent.ACTION_PERFORMED, s, 0));
	}
}

void notifyItem ( Object item, int op) {
	if ( (iListener != null) ||
	     (eventMask & AWTEvent.ITEM_EVENT_MASK) != 0 ||
	     (flags & IS_OLD_EVENT) != 0 ){
		ItemEvt e = ItemEvt.getEvent( this, ItemEvent.ITEM_STATE_CHANGED, item, op);
		Toolkit.eventQueue.postEvent( e);
	}
}

public void paint ( Graphics g ) {
	// we know about our childs, we don't have to blank the background,
	// so let's speed up things a little
	g.paintChild( ip, false);

	if ( (ip.hScroll != null) && ((ip.hScroll.flags & IS_VISIBLE) != 0) )
		g.paintChild( ip.hScroll, true);
	if ( (ip.vScroll != null) && ((ip.vScroll.flags & IS_VISIBLE) != 0) )
		g.paintChild( ip.vScroll, true);
}

protected String paramString() {
	return super.paramString();
}

public Dimension preferredSize () {
	int rows = ip.rows.size();
	if ( rows < 5 )
		rows = 5;
	else if ( rows > 10 )
		rows = 10;
	return preferredSize( rows);
}

public Dimension preferredSize (int rows) {
	int h = rows * ip.rowHeight;
	int w = 25 * ip.fm.charWidth( 'x');
	return new Dimension( w, h);
}

void process ( ActionEvent e ) {
	if ( (aListener != null) || ((eventMask & AWTEvent.ACTION_EVENT_MASK) != 0) ){
		processEvent( e);
	}
}

void process ( ItemEvent e ) {
	if ( (iListener != null) || ((eventMask & AWTEvent.ITEM_EVENT_MASK) != 0) ){
		processEvent( e);
	}

	if ((flags & IS_OLD_EVENT) != 0) {
		postEvent (Event.getEvent( e));
	}
}

protected void processActionEvent( ActionEvent e) {
	if ( aListener != null ){
		aListener.actionPerformed( e);
	}
}

protected void processItemEvent( ItemEvent e) {
	if (iListener != null) {
		iListener.itemStateChanged( e);
	}
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
	clear();
}

void removeElement ( int index) {
	try {
		deselectElement( index, false, true);
		ip.rows.removeElementAt( index);

		ip.updateScrolls();
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

public synchronized void repaintRow ( int idx) {
	ip.repaintItem( idx);
}

public synchronized void replaceItem ( String newValue, int index) {
	try {
		Object o = ip.rows.elementAt( index);
		ip.rows.setElementAt( newValue, index);
		ip.updateHScroll();
		ip.repaintRows( index, 1);
	}
	catch ( Exception e) {
	}
}

public void requestFocus () {
	ip.requestFocus();
}

public void reshape ( int x, int y, int w, int h ) {
	super.reshape( x, y, w, h);

	// there is no need for validation of compound IS_NATIVE_LIKES, they are no Containers
	// in JDK, so we automagically have to re-layout them
	ip.innerLayout();
	flags |= IS_VALID;
}

public void select ( int index ) {
	// It's unclear whether a user selection should fire an event of
	// not - but some code appears rely on event *NOT* being sent.
	selectEvent( index, false);
}

void selectEvent ( int index, boolean fire ) {
	if ( (index < 0) || (index > ip.rows.size() - 1) )
		return;

	if ( isIndexSelected( index) )
		return;

	if ( multipleMode ) {
		sel = index;
		multiSel.set( index);
		nSel++;
	}
	else if ( sel >= 0 ) {
		int oldSel = sel;
		sel = index;
		ip.repaintRows( oldSel, 1);
		if (fire) {
			notifyItem( new Integer( oldSel), ItemEvent.DESELECTED);
		}
	}
	else
		sel = index;

	ip.makeVisible( index);
	ip.repaintRows( index, 1);

	if (fire) {
		notifyItem( new Integer( index), ItemEvent.SELECTED);
	}
}

public void setBackground ( Color c) {
	// Ideally, we would just do a super.setBackground(), but that would give us
	// a strange look for the scrollbar backgrounds. If we just forward here, we also
	// have to resolve getBackground()

	//super.setBackground( c);
	ip.setBackground( c);
}

public void setEnabled ( boolean isEnabled ) {
	super.setEnabled( isEnabled);

	ip.setEnabled( isEnabled);
}

public void setFont ( Font fnt) {
	super.setFont( fnt);
	ip.setFont( fnt);
	ip.updateScrolls();
}

public void setForeground ( Color c) {
	//super.setForeground( c);
	ip.setForeground( c);
}

public synchronized void setMultipleMode ( boolean b) {
	setMultipleSelections(b);
}

/**
 * @deprecated
 */
public synchronized void setMultipleSelections ( boolean b ) {
	if ( multipleMode == b )
		return;

	if ( multipleMode ){
		multiSel = null;
		nSel = 0;

		multipleMode = false;
	}
	else {
		multiSel = new BitSet();
		nSel = 0;

		multipleMode = true;
	}
}

public void update ( Graphics g ) {
	paint( g); // no background blanking required
}

void vPosChange ( int dy) {
}
}
