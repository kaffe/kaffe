package java.awt;

import java.awt.event.FocusEvent;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.Vector;

/**
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author J.Mehlitz
 */
public class TextArea
  extends TextComponent
{
	final private static long serialVersionUID = 3692302836626095722L;
	final public static int SCROLLBARS_BOTH = 0;
	final public static int SCROLLBARS_VERTICAL_ONLY = 1;
	final public static int SCROLLBARS_HORIZONTAL_ONLY = 2;
	final public static int SCROLLBARS_NONE = 3;
	int tabWidth;
	int crows;
	int ccols;
	TextPane tp = new TextPane();

class TextPane
  extends RowCanvas
  implements KeyListener, MouseListener, MouseMotionListener
{
	TextCursor tCursor = new TextCursor();
	Point sSel = new Point();
	Point eSel = new Point();

public TextPane () {
	xOffsInit = 4;
	xOffs = 4;

	this.setCursor( Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR));
	tCursor.setPos( xOffs, rowHeight + BORDER_WIDTH);
	insertLine( "", 0);

	this.addKeyListener( this);
	this.addMouseListener( this);
	this.addMouseMotionListener( this);
	this.addFocusListener( this);
}

synchronized void append ( String s ) {
	int         i, nOld, nNew;
	TextBuffer  tb;
	String[]    lines;

	blankCursor();
	lines = breakLines( s);

	nOld = rows.size() - 1;
	tb = (TextBuffer)rows.elementAt( nOld);
	tb.append( lines[0]);

	for ( i=1; i<lines.length; i++ ){
		tb = new TextBuffer( lines[i]);
		tb.setMetrics( fm, tabWidth);
		rows.addElement( tb);
	}

	nNew = rows.size() - 1;
	makeVisible( nNew);
	updateScrolls();

	repaintRows( nOld, nNew - nOld);

	setCursorPos( tb.len, nNew, true, true);
}

void backspace() {
	TextBuffer tb = getCursorLine();
	if ( tCursor.index >= 1 ) {
		tb.remove( tCursor.index-1, 1);
		setCursorPos( tCursor.index-1, tCursor.yindex, false, true);
		repaintLine( tCursor.yindex, tCursor.index, tb);
	}
	else if ( cursorLeft(1, false) ){
		TextBuffer tb1 = getCursorLine();
		tb1.append( tb);
		rows.removeElement( tb);
		this.repaint();
		updateScrolls();
	}
}

void blankCursor() {
	TextBuffer tb;

	if ( rgr != null ) {
		rgr.setColor( this.bgClr );
		tCursor.blank( rgr, xOffs, getRowYPos( tCursor.yindex) );
		rgr.setColor( this.fgClr );
		tb = (TextBuffer)rows.elementAt( tCursor.yindex );
		tb.paint( rgr, xOffs, tCursor.y, rowHeight, tCursor.index, 1);
	}
}

String[] breakLines( String str) {
	int       i, i0 = 0, n = str.length();
	char      c;
	Vector    v;
	String[]  sbuf;
	char[]    cbuf;

	if ( (str == null) || (n == 0) )
		return new String[1];

	v = new Vector( n / 20);
	cbuf = str.toCharArray();

	// sometimes I wonder how long we will suffer from old DOS habits: some
	// editors still use obscure '\r\r\n' or '\r' "weak" linefeeds to mark
	// automatically wrapped lines

	for ( i=0; i<n; i++ ) {
		if ( (c=cbuf[i]) == '\n' ) {
			v.addElement( new String( cbuf, i0, i-i0));
			i0 = i+1;
		}
		else if ( c == '\r' ) {
			v.addElement( new String( cbuf, i0, i-i0));
			// skip all subsequent '\r'
			for ( i++; (i < n) && (cbuf[i] == '\r'); i++ );
			i0 = (cbuf[i] == '\n') ? i+1 : i;
		}
	}

	if ( i0 <= n ){
		v.addElement( new String( cbuf, i0, i-i0));
	}

	sbuf = new String[v.size()];
	v.copyInto( sbuf);

	return sbuf;
}

void cursorDown( int steps, boolean extend) {
	int newY = Math.min( tCursor.yindex + steps, rows.size() -1);
	if ( newY != tCursor.yindex) {
		if ( extend )
			updateSel( tCursor.index, newY, true);
		else
			setCursorPos( tCursor.index, newY, true, true);
	}
}

void cursorEnd( boolean extend) {
	TextBuffer tb = getCursorLine();
	if ( extend)
		updateSel( tb.len, tCursor.yindex, true);
	else
		setCursorPos( tb.len, tCursor.yindex, true, true);
}

void cursorHome( boolean extend) {
	if ( extend)
		updateSel( 0, tCursor.yindex, true);
	else
		setCursorPos( 0, tCursor.yindex, true, true);
}

boolean cursorLeft( int steps, boolean extend) {
	int nx = -1, ny = -1;

	if ( tCursor.index >= steps) {
		nx = tCursor.index - steps;
		ny = tCursor.yindex;
	}
	else if ( tCursor.yindex > 0 ) {
		nx = ((TextBuffer)rows.elementAt( tCursor.yindex-1)).len;
		ny = tCursor.yindex-1;
	}

	if ( ny > -1 ) {
		if ( extend )
			updateSel( nx, ny, true);
		else
			setCursorPos( nx, ny, true, true);
		return true;
	}

	return false;
}

void cursorRight( int steps, boolean extend) {
	int nx = -1, ny = -1;
	TextBuffer tb = (TextBuffer)rows.elementAt( tCursor.yindex);

	if ( tb.len >= tCursor.index + steps) {
		nx = tCursor.index + steps;
		ny = tCursor.yindex;
	}
	else if ( tCursor.yindex < rows.size() - 1 ) {
		nx = 0;
		ny = tCursor.yindex+1;
	}

	if ( ny > -1 ) {
		if ( extend)
			updateSel( nx, ny, true);
		else
			setCursorPos( nx, ny, true, true);
	}
}

void cursorTextEnd( boolean extend) {
	int yIdx = rows.size() - 1;
	TextBuffer tb = (TextBuffer)rows.elementAt( yIdx);
	if ( extend)
		updateSel( tb.len, yIdx, true);
	else
		setCursorPos( tb.len, yIdx, true, true);
}

void cursorTextHome( boolean extend) {
	if ( extend)
		updateSel( 0, 0, true);
	else
		setCursorPos( 0, 0, true, true);
}

void cursorUp( int steps, boolean extend) {
	int newY = Math.max( tCursor.yindex - steps, 0);
	if ( newY != tCursor.yindex) {
		if ( extend )
			updateSel( tCursor.index, newY, true);
		else
			setCursorPos( tCursor.index, newY, true, true);
	}
}

void del() {
	TextBuffer tb = getCursorLine();
	if ( tb.len > tCursor.index ) {
		tb.remove( tCursor.index, 1);
		repaintLine( tCursor.yindex, tCursor.index, tb);
	}
	else if ( tCursor.yindex < rows.size() - 1 ){
		TextBuffer tb1 = (TextBuffer)rows.elementAt( tCursor.yindex+1);
		tb.append( tb1);
		rows.removeElement( tb1);
		updateScrolls();
		this.repaint();
	}
}

void deleteSel() {
	Point ps = getSelStart();
	Point pe = getSelEnd();
	TextBuffer tb = (TextBuffer)rows.elementAt( ps.y);
	int af = 1;

	if ( (ps.x == pe.x) && (ps.y == pe.y) )
		return;

	if ( ps.y == pe.y ) {
		tb.remove( ps.x, pe.x - ps.x);
		setCursorPos( ps.x, ps.y, true, true);
		if ( updateHScroll() )
			rearrange();
		return;
	}

	if ( ps.x > 0 )
		tb.remove( ps.x, tb.len - ps.x);
	else
		af = 0;
	tb = (TextBuffer)rows.elementAt( pe.y);
	tb.remove( 0, pe.x);

	for ( int i = ps.y + af; i < pe.y; i++)
		rows.removeElementAt( ps.y + af);

	setCursorPos( ps.x, ps.y, false, true);
	updateScrolls();
	this.repaint();
}

public void focusGained( FocusEvent e) {
	repaintCursor();

	super.focusGained( e);
}

public void focusLost( FocusEvent e) {
	paintInactiveCursor();

	//would confuse replacement with clipboard contents via popup
  //	if ( hasSel() )
  //		copyToClipboard();

	super.focusLost( e);
}

int get1D( Point p) {
	int xt = 0;

	for ( int i=0; i<p.y; i++) {
		TextBuffer tb = (TextBuffer)rows.elementAt( i);
		xt += tb.len;
		xt ++;  // hjb 01-27-1999: count the virtual newline
	}
	xt += p.x;

	return xt;
}

Point get2D( int pos) {
	int rs = rows.size();
	int xt = 0;
	TextBuffer tb = null;

	if ( rs == 0 )
		return new Point( -1, -1);

	for ( int i=0; i<rs; i++) {
		tb = (TextBuffer)rows.elementAt( i);
		xt += tb.len;
		if (xt >= pos)
			return new Point( tb.len - xt + pos, i);
		pos --;  // hjb 01-27-1999: substract 1 for the virtual newline
	}

	return new Point( tb.len, rs-1);
}

int getCol( int row, int x) {
	TextBuffer tb = (TextBuffer) rows.elementAt( row);
	return tb.getIdx( x - xOffs);
}

TextBuffer getCursorLine() {
	return (TextBuffer)rows.elementAt( tCursor.yindex);
}

Point getSelEnd() {
	if ( sSel.y > eSel.y )
		return sSel;
	if ( sSel.y == eSel.y ) {
		if ( sSel.x > eSel.x )
			return sSel;
		return eSel;
	}
	return eSel;
}

Point getSelStart() {
	if ( sSel.y < eSel.y )
		return sSel;
	if ( sSel.y == eSel.y ) {
		if ( sSel.x < eSel.x )
			return sSel;
		return eSel;
	}
	return eSel;
}

boolean hasSel() {
	return ( (sSel.x != eSel.x) || (sSel.y != eSel.y) );
}

boolean hasSel( int row) {
	if ( hasSel() )
		return ( (sSel.y <= row) || (eSel.y >= row) );
	return false;
}

void insert( String s, boolean keepCursor) {
	if ( (s == null) || (s.length() == 0) )
		return;

	String[] lns = breakLines( s);
	int lnsi = 0;
	TextBuffer tb = (TextBuffer)rows.elementAt( tCursor.yindex);

	if ( lns.length == 0 )
		return;

	if ( lns.length == 1 ) {
		int ncp = keepCursor ? tCursor.index : tCursor.index + lns[0].length();
		tb.insert( tCursor.index, lns[0] );
		setCursorPos( ncp, tCursor.yindex, false, false);
		repaintLine( tCursor.yindex, 0, tb);
		return;
	}

	String le = tb.getString( tCursor.index, tb.len - tCursor.index);
	tb.remove( tCursor.index, tb.len - tCursor.index);
	tb.append( lns[lnsi++] );

	while ( lnsi < lns.length)
		tb = insertLine( lns[lnsi++], tCursor.yindex + lnsi - 1 );

	if ( ! keepCursor)
		setCursorPos( tb.len, tCursor.yindex + lns.length - 1, false, false);
	tb.append( le);

	updateScrolls();
	this.repaint();
}

void insert( String s, int pos) {
	insertText(s, pos);
}

void insertChar( char c) {
	boolean app;

	TextBuffer tb = (TextBuffer)rows.elementAt( tCursor.yindex);
	app = ( tCursor.index == tb.len);
	tb.insert( tCursor.index, c);

	if ( !app){
		//leading problem ( repaint left cursor char also)
		int sIdx = (tCursor.index > 0 ) ? tCursor.index-1 : tCursor.index;
		repaintLine( tCursor.yindex, sIdx, tb);
	}

	cursorRight( 1, false);
}

TextBuffer insertLine( String str, int lIdx) {
	TextBuffer tb = new TextBuffer( str);
	tb.setMetrics( fm, tabWidth);
	rows.insertElementAt( tb, lIdx);
	return tb;
}

/**
 * @deprecated
 */
void insertText(String s, int pos) {
	Point p = get2D( pos);
	setCursorPos( p.x, p.y, true, true);

	insert( s, false);
}

public void keyPressed( KeyEvent e) {

	if ( this.parent.keyListener != null ){
		// give our parent a chance to intercept keystrokes
		// check for keyListeners first, it's a rare case
		redirectKeyEvent( e);
	}
	int code = e.getKeyCode();
	int mods = e.getModifiers();
	boolean sh = e.isShiftDown();

	//do not consume unused key for ShortcutHandler
	if ( (mods != 0) && (mods != KeyEvent.SHIFT_MASK) )
		return;

	switch( code) {
		case KeyEvent.VK_LEFT:
			cursorLeft( 1, sh);
			break;
		case KeyEvent.VK_RIGHT:
			cursorRight( 1, sh);
			break;
		case KeyEvent.VK_UP:
			cursorUp( 1, sh);
			break;
		case KeyEvent.VK_DOWN:
			cursorDown( 1, sh);
			break;
		case KeyEvent.VK_TAB:
			if (sh) {
				return;	//do not consume event for HotKeyHandler
			}
			if (!isEditable) { // hjb 01-27-1999: do nothing for
				break;     // TAB, ENTER, DEL and BS if not editable
			}
			insertChar( '\t' );
			break;
		case KeyEvent.VK_ENTER:
		        if ( ! isEditable)
			    break;
			newline();
			break;
		case KeyEvent.VK_BACK_SPACE:
		        if ( ! isEditable)
			    break;
			if ( hasSel() )
				replaceSelectionWith("");
			else
				backspace();
			break;
		case KeyEvent.VK_DELETE:
		        if ( ! isEditable)
		            break;
			if ( hasSel() )
				replaceSelectionWith("");
			else
				del();
			break;
		case KeyEvent.VK_HOME:
			cursorHome( sh);
			break;
		case KeyEvent.VK_END:
			cursorEnd( sh);
			break;
		case KeyEvent.VK_PAGE_UP:
			pageUp( sh);
			break;
		case KeyEvent.VK_PAGE_DOWN:
			pageDown( sh);
			break;
		case KeyEvent.VK_ESCAPE:
			resetSel( true);
			break;
		default:
		  return;
	}

  e.consumed = true;
}

public void keyReleased( KeyEvent e) {
	redirectKeyEvent( e);

	e.consumed = true;
}

public void keyTyped( KeyEvent e) {
	if ( ! isEditable || ! isPrintableTyped( e) )
		return;

	char c = e.getKeyChar();

	if ( hasSel() ) {
		Character cc = new Character( c);
		replaceSelectionWith( cc.toString() );
	}
	else
		insertChar( c );

  if ( this.parent.keyListener != null ) {
		// check for keyListeners first, it's a rare case
		redirectKeyEvent( e);
	}

	if ( (textListener != null) || (this.eventMask & AWTEvent.TEXT_EVENT_MASK) != 0 ) {
		Toolkit.eventQueue.postEvent( TextEvt.getEvent( TextArea.this,
		                                                TextEvt.TEXT_VALUE_CHANGED));
	}

	e.consumed = true;
}

int maxRowWidth() {
	int rs = rows.size();
	int iw, mw = 0;

	for ( int i=0; i<rs; i++ ) {
		iw = ((TextBuffer)rows.elementAt( i)).getWidth();
		if ( iw > mw )
			mw = iw;
	}

	return mw;
}

public void mouseClicked( MouseEvent e) {
	if ( this.parent.mouseListener != null ){
		// no need to retarget, already done by mousePressed
		this.parent.process( e);
	}
}

public void mouseDragged( MouseEvent e) {
	int y = getRowIdx( e.getY() );
	int x = getCol( y, e.getX() );
	updateSel( x, y, true);

	if ( this.parent.motionListener != null ){
		// unlikely, check motionListener first
		redirectMotionEvent( e);
	}
}

public void mouseEntered( MouseEvent e) {
	redirectMouseEvent( e);
}

public void mouseExited( MouseEvent e) {
	redirectMouseEvent( e);
}

public void mouseMoved( MouseEvent e) {
	if ( this.parent.motionListener != null ){
		// unlikely, check listener first
		redirectMotionEvent( e);
	}
}

public void mousePressed( MouseEvent e) {
	int mods = e.getModifiers();

	if ( e.isPopupTrigger() ){
		if ( (this.triggerPopup( e.getX(), e.getY())) != null )
			return;
	}

	switch ( mods) {
		case InputEvent.BUTTON1_MASK:
			tp.requestFocus();
			resetSel( true);
			int y = getRowIdx( e.getY() );
			int x = getCol( y, e.getX() );
			setCursorPos( x, y, true, true);
			break;
		case InputEvent.BUTTON2_MASK:
			pasteFromClipboard();
			break;
	}

	redirectMouseEvent( e);
}

public void mouseReleased( MouseEvent e) {
	redirectMouseEvent( e);
}

void newline() {
	String s = "";

	TextBuffer tb = getCursorLine();
	if ( tb.len > tCursor.index ) {
		int rl = tb.len - tCursor.index;
		s = tb.getString( tCursor.index, rl);
		tb.remove( tCursor.index, rl);
	}

	TextBuffer tbNew = insertLine( s, tCursor.yindex+1);
	tbNew.copyLevelFrom( tb);

	updateScrolls();

	setCursorPos( tbNew.getLevel(), tCursor.yindex+1, true, true);
	repaintRows( tCursor.yindex-1, getVisibleRows() );
}

void pageDown( boolean extend) {
	int vr = getVisibleRows();
	int newY = Math.min( tCursor.yindex + vr, rows.size() - 1);
	if ( extend)
		updateSel( tCursor.index, newY, true);
	else
		setCursorPos( tCursor.index, newY, true, true);
}

void pageUp( boolean extend) {
	int vr = getVisibleRows();
	int newY = Math.max( tCursor.yindex - vr, 0);
	if ( extend)
		updateSel( tCursor.index, newY, true);
	else
		setCursorPos( tCursor.index, newY, true, true);
}

public void paint( Graphics g) {
	repaintRows( g, first, rows.size()-first );
	this.kaffePaintBorder( g);
}

void paintInactiveCursor() {
	if ( rgr != null)
		paintInactiveCursor( rgr);
}

void paintInactiveCursor( Graphics g) {
	g.setColor( Defaults.TextCursorInactiveClr );
	tCursor.blank( g, xOffs, getRowYPos( tCursor.yindex) );
}

void repaintCursor() {
	repaintCursor( rgr);
}

void repaintCursor( Graphics g) {
	if ( g != null ) {
		if ( AWTEvent.keyTgt == this)
			tCursor.paint( g, xOffs, getRowYPos( tCursor.yindex) );
		else
			paintInactiveCursor( g);
	}
}

void repaintLine( Graphics g, int row, int startX, TextBuffer tb) {
	int x0, w;
	int d = BORDER_WIDTH;

	if ( g == null )
		return;

	if ( tb == null )
		tb = (TextBuffer)rows.elementAt( row);

	int ss = selXStart( row);
	int se = selXEnd( row, tb);
	int y0 = d + (row - first) * rowHeight;

	if ( ss == se ) {
		x0 = (startX == 0) ? 0 : tb.getPos( startX) + xOffs;
		w = this.width - x0;
		g.setColor( this.bgClr );
		g.fillRect( x0, y0, w-d, rowHeight);
		g.setColor( this.fgClr );
		tb.paint( g, xOffs, y0, rowHeight, startX);
	}
	else {
		if ( ss > startX ) {
			x0 = tb.getPos( startX) + xOffs;
			w = tb.getWidth( startX, ss);
			g.setColor( this.bgClr );
			g.fillRect( x0, y0, w, rowHeight);
			g.setColor( this.fgClr );
			tb.paint( g, xOffs, y0, rowHeight, startX, ss-startX);
		}
		if ( se > startX ) {
			x0 = tb.getPos( ss) + xOffs;
			w = tb.getWidth( ss, se);
			g.setColor( Defaults.TextAreaSelBgClr );
			g.fill3DRect( x0, y0, w, rowHeight, true);
			g.setColor( Defaults.TextAreaSelTxtClr );
			tb.paint( g, xOffs, y0, rowHeight, ss, se-ss);
		}
		x0 = tb.getPos( se) + xOffs;
		w = this.width - x0;
		g.setColor( this.bgClr );
		g.fillRect( x0, y0, w, rowHeight);
		if ( se < tb.len ) {
			g.setColor( this.fgClr );
			tb.paint( g, xOffs, y0, rowHeight, se);
		}
	}

	if ( tCursor.yindex == row )
		repaintCursor( g);

}

void repaintLine( int row, int startX, TextBuffer tb) {
	repaintLine( rgr, row, startX, tb);
}

void repaintRow( Graphics g, int idx) {
	repaintLine( g, idx, 0, null);
}

void replaceRange( String s, int start, int end) {
	sSel = get2D( start);
	eSel = get2D( end);

	if ( (sSel.y > -1) && (eSel.y > -1) )
		deleteSel();
	else
		resetSel( false);

	insert( s, true);
}

void replaceSelectionWith( String s) {
	deleteSel();
	insert( s, false);
}

void resetSel( boolean repaint) {
	boolean se = hasSel();

	int y0 = Math.min( sSel.y, eSel.y);
	int y1 = Math.max( sSel.y, eSel.y);

	sSel.x = tCursor.index;
	sSel.y = tCursor.yindex;
	eSel.x = sSel.x;
	eSel.y = sSel.y;

	if ( se && repaint)
		repaintRows( y0, y1-y0);
}

int selXEnd( int row, TextBuffer tb) {
	Point ps = getSelStart();
	Point pe = getSelEnd();

	if ( row > pe.y )
		return -1;
	if ( row < ps.y )
		return -1;
	if ( row == pe.y )
		return pe.x;

	return tb.len;
}

int selXStart( int row) {
	Point ps = getSelStart();
	Point pe = getSelEnd();

	if ( row > pe.y )
		return -1;
	if ( row < ps.y )
		return -1;
	if ( row == ps.y )
		return ps.x;

	return 0;
}

synchronized void setContents( String s) {
	String[] sa = breakLines( s);
	rows.removeAllElements();
	for ( int i=0; i<sa.length; i++)
		insertLine( sa[i], i);
	setCursorPos( 0, 0, false, true);
	updateScrolls();
	this.repaint();
}

void setCursorPos( int x, int y, boolean repaint, boolean resetSel) {
	TextBuffer tb;
	int xPos;
	int lastX = tCursor.index;

	if ( resetSel )
		resetSel( repaint);

	if ( repaint) {
		blankCursor();
	}

	makeVisible( y);

	tb = (TextBuffer)rows.elementAt( y);
	tCursor.setYIndex( y, getRowYPos( y) );
	if ( x > tb.len)
		x = tb.len;
	xPos = tb.getPos( x);
	tCursor.setIndex( x, xPos );

	repaintCursor();

	if ( resetSel)
		resetSel( false);

	if ( this.width > 0) {
		int dx = 10;
		if ( (x > lastX) && (xPos - xOffs > this.width -dx) ){
			xOffs = this.width - xPos - dx;
			if (hScroll != null) {
				if ( updateHScroll() )
					rearrange();
				hScroll.setValue( -xOffs);
			}
			this.repaint();
		}
		else if ( xPos + xOffs < xOffsInit ) {
			xOffs = -xPos + xOffsInit;
			if (hScroll != null) {
				if ( updateHScroll() )
					rearrange();
				hScroll.setValue( -xOffs);
			}
			this.repaint();
		}
	}
}

public void setFont( Font f) {
	TextBuffer tb;
	int s = rows.size();

	super.setFont( f);
	fm = this.getFontMetrics( f);

	if ( rgr != null )
		rgr.setFont( f);

	tabWidth = 3*fm.charWidth( 'x');
	rowHeight = fm.getHeight() + 2;

	for ( int i=0; i<s; i++) {
		tb = (TextBuffer)rows.elementAt( i);
		tb.setMetrics( fm, tabWidth);
	}

	tb = (TextBuffer)rows.elementAt( tCursor.yindex);
	tCursor.setHeight( rowHeight-1 );
	tCursor.setYIndex( tCursor.yindex, getRowYPos( tCursor.yindex) );
	tCursor.setIndex( tCursor.index, tb.getPos( tCursor.index) );

	if ( this.isShowing() )
		this.repaint();
}

boolean updateSel( int x, int y, boolean repaint) {
	int y0, y1;

	if ( (x == eSel.x) && (y == eSel.y) )
		return false;

	eSel.x = x;
	eSel.y = y;
	y0 = Math.min( sSel.y, eSel.y );
	y1 = Math.max( sSel.y, eSel.y );

	setCursorPos( x, y, false, false);
	if ( repaint) {
		repaintRows( y0, y1 - y0 + 1);
	}

	return true;
}

void vPosChange( int dy) {
	tCursor.setYIndex( tCursor.yindex, getRowYPos( tCursor.yindex) );
}
}

public TextArea() {
	this( null, 10, 20, SCROLLBARS_BOTH);
}

public TextArea( String text) {
	this( text, 10, 20, SCROLLBARS_BOTH);
}

public TextArea( String text, int rows, int cols) {
	this( text, rows, cols, SCROLLBARS_BOTH);
}

public TextArea( String text, int rows, int cols, int scrolls) {
	crows = rows;
	ccols = cols;

	setLayout( null);
	setFont( Defaults.TextAreaFont);

	if (scrolls == SCROLLBARS_VERTICAL_ONLY || scrolls == SCROLLBARS_BOTH) {
		tp.vScroll = new Scrollbar( Scrollbar.VERTICAL,0,0,0,0);
		add( tp.vScroll);
	}
	if (scrolls == SCROLLBARS_HORIZONTAL_ONLY || scrolls == SCROLLBARS_BOTH) {
		tp.hScroll = new Scrollbar( Scrollbar.HORIZONTAL,0,0,0,0);
//		tp.hScroll.setValues( 0, 5 * tabWidth, 0,  100 * tp.fm.charWidth( 'x'));
		tp.hScroll.setUnitIncrement( tp.fm.charWidth( 'x'));
		add( tp.hScroll);
	}

	buildMenu();
	add( tp);
	tp.setListeners();

	setBackground( Defaults.TextAreaBgClr);
	setForeground( Defaults.TextAreaTxtClr);

	if ( text != null )
		append( text);
}

public TextArea( int rows, int cols) {
	this( null, rows, cols, SCROLLBARS_BOTH);
}

public void add( PopupMenu m) {
	tp.add( m);
}

public void append( String str) {
	tp.append( str);
}

/**
 * @deprecated as of JDK 1.1, replaced by append(String s)
 */
public void appendText(String s) {
	append(s);
}

protected void buildMenu() {
	PopupMenu p = new PopupMenu();
	p.add( new MenuItem("Cut")).setShortcut( new MenuShortcut( KeyEvent.VK_U, false) );
	p.add( new MenuItem("Copy")).setShortcut( new MenuShortcut( KeyEvent.VK_O, false) );
	p.add( new MenuItem("Paste")).setShortcut( new MenuShortcut( KeyEvent.VK_A, false) );
	p.addSeparator();
	p.add( new MenuItem("Select All")).setShortcut( new MenuShortcut( KeyEvent.VK_S, false) );
	p.addActionListener( this);

	tp.add( p);
}

public void doLayout() {
	tp.innerLayout();
}

public Color getBackground () {
	// some anomaly, we forward colors to our tp, so we should return its colors
	// for consistencies sake
	return tp.getBackground();
}

public int getCaretPosition() {
	Point p = new Point( tp.tCursor.index, tp.tCursor.yindex);
	return tp.get1D( p);
}

public int getColumns() {
	return ccols;
}

public Color getForeground () {
	// some anomaly, we forward colors to our tp, so we should return its colors
	// for consistencies sake
	return tp.getForeground();
}

public Dimension getMinimumSize() {
	return (minimumSize());
}

public Dimension getMinimumSize( int rows, int cols) {
	return (minimumSize(rows, cols));
}

public Dimension getPreferredSize() {
	return preferredSize();
}

public Dimension getPreferredSize( int rows, int cols) {
	return preferredSize(rows, cols);
}

public int getRows() {
	return crows;
}

public int getScrollbarVisibility() {
	if ( (tp.hScroll == null) && (tp.vScroll == null) )
		return SCROLLBARS_NONE;
	if ( tp.hScroll == null )
		return SCROLLBARS_VERTICAL_ONLY;
	if ( tp.vScroll == null )
		return SCROLLBARS_HORIZONTAL_ONLY;

	return SCROLLBARS_BOTH;
}

public String getSelectedText() {
	StringBuffer sb = new StringBuffer();

	int y0 = Math.min( tp.sSel.y, tp.eSel.y);
	int yMax = Math.max( tp.sSel.y, tp.eSel.y);

	for ( int i=y0; i<=yMax; i++) {
		TextBuffer tb = (TextBuffer) tp.rows.elementAt( i);
		int x0 = tp.selXStart( i);
		int x1 = tp.selXEnd( i, tb);
		sb.append( tb.buf, x0, x1-x0 );
		if ( i < yMax )
			sb.append( " ");
	}

	return sb.toString();
}

public int getSelectionEnd() {
	int i1 = tp.get1D( tp.sSel);
	int i2 = tp.get1D( tp.eSel);

	return Math.max( i1, i2);
}

public int getSelectionStart() {
	int i1 = tp.get1D( tp.sSel);
	int i2 = tp.get1D( tp.eSel);

	return Math.min( i1, i2);
}

public String getText() {
	int i, imax = tp.rows.size()-1;
	StringBuffer sb = new StringBuffer( (imax+1) * 80);

	for ( i = 0; i <= imax; ) {
		TextBuffer tb = (TextBuffer) tp.rows.elementAt(i);
		sb.append( tb.buf, 0, tb.len);
		if ( i++ < imax )
			sb.append( '\n');
	}

	return sb.toString();
}

void hPosChange() {
	tp.hPosChange();
}

public synchronized void insert( String str, int pos) {
	tp.insert( str, pos);
}

/**
 * @deprecated
 */
public Dimension minimumSize() {
	return (minimumSize(crows, ccols));
}

/**
 * @deprecated
 */
public Dimension minimumSize( int rows, int cols) {
	return new Dimension( cols*tp.fm.charWidth( 'x'), rows*tp.fm.getHeight() );
}

public void paint ( Graphics g ) {
	// we know about our childs, we don't have to blank the background,
	// so let's speed up things a little
	g.paintChild( tp, false);

	if ( (tp.hScroll != null) && ((tp.hScroll.flags & IS_VISIBLE) != 0) )
		g.paintChild( tp.hScroll, true);
	if ( (tp.vScroll != null) && ((tp.vScroll.flags & IS_VISIBLE) != 0) )
		g.paintChild( tp.vScroll, true);
}

protected String paramString() {
	return super.paramString();
}

/**
 * @deprecated
 */
public Dimension preferredSize() {
	return preferredSize( crows, ccols);
}

/**
 * @deprecated
 */
public Dimension preferredSize( int rows, int cols) {
	return new Dimension( cols*tp.fm.charWidth( 'x'), rows*tp.fm.getHeight() );
}

public void repaintRow( int row) {
	tp.repaintLine( row, 0, null);
}

public synchronized void replaceRange( String str, int start, int end) {
	replaceText(str, start, end);
}

void replaceSelectionWith ( String s ) {
	tp.replaceSelectionWith( s);
}

/**
 * @deprecated
 */
public synchronized void replaceText( String str, int start, int end) {
	tp.replaceRange( str, start, end);
}

public void requestFocus() {
	tp.requestFocus();
}

public void reshape ( int x, int y, int w, int h ) {
	super.reshape( x, y, w, h);

	// there is no need for validation of compound IS_NATIVE_LIKES, they are no Containers
	// in JDK, so we automagically have to re-layout them
	tp.innerLayout();
	flags |= IS_VALID;
}

public void select( int start, int end) {
	Point p = tp.get2D( end);

	tp.sSel = tp.get2D( start);
	tp.updateSel( p.x, p.y, true);
}

public void selectAll() {
	TextBuffer tb = (TextBuffer) tp.rows.lastElement();
	tp.sSel.x = 0;
	tp.sSel.y = 0;
	tp.eSel.x = 0;
	tp.eSel.y = 0;
	tp.updateSel( tb.len, tp.rows.size()-1, true);
}

public void setBackground( Color clr) {
	// we don't have a color of our own, forward this to tp
	tp.setBackground( clr);
}

public void setCaretPosition( int pos) {
	Point p = tp.get2D( pos);
	tp.setCursorPos( p.x, p.y, true, true);
}

public void setColumns( int cols) {
	ccols = cols;
}

public void setEnabled ( boolean isEnabled ) {
	super.setEnabled( isEnabled);

	tp.setEnabled( isEnabled);
}

public void setFont( Font f) {
	super.setFont( f);
	tp.setFont( f);
}

public void setForeground( Color clr) {
	// we don't have a color of our own, forward thisto tp
	tp.setForeground( clr);
}

public void setRows( int rows) {
}

public void setSelectionEnd( int end) {
	Point p = tp.get2D( end);
	tp.updateSel( p.x, p.y, true);
}

public void setSelectionStart( int start) {
	Point p = tp.get2D( start);
	tp.updateSel( p.x, p.y, true);
}

public void setText( String text) {
	tp.setContents( text);
}

public void update ( Graphics g ) {
	paint( g); // no background blanking required
}

void vPosChange( int dy) {
	tp.vPosChange( dy);
}
}
