package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

/**
 * class TextField - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class TextField
  extends TextComponent
  implements MouseMotionListener, KeyListener, FocusListener, MouseListener
{
	char echoChar;
	ActionListener aListener;
	int cols;
	FontMetrics fm;
	TextCursor tCursor = new TextCursor();
	static int xOffs = 2;
	int selStart;
	int selEnd;
	TextBuffer textBuf = new TextBuffer();
	TextBuffer hiddenBuf;
	int first;

public TextField() {
	this( "", 1);
}

public TextField( String text) {
	this( text, (text != null) ? text.length() : 1);
}

public TextField( String text, int cols) {
	this.cols = cols;

	cursor = Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR);
	
	setFont( Defaults.TextFieldFont);
	setBackground( Defaults.TextFieldBgClr);
	setForeground( Defaults.TextFieldTxtClr);
	setText( text);
	
	addKeyListener( this);
	addFocusListener( this);
	addMouseListener( this);
	addMouseMotionListener( this);
}

public TextField( int cols) {
	this( "", cols);
}

public void addActionListener ( ActionListener al) {
	aListener = AWTEventMulticaster.add( aListener, al);
	eventMask |= AWTEvent.ACTION_EVENT_MASK;
}

void blankSelection() {
	if ( selStart != selEnd)
		setTextCursor( selStart, true, false);
}

public void deleteSelection() {
	if ( selStart != selEnd) {
		textBuf.remove( selStart, selEnd-selStart);
		if ( hiddenBuf != null )
			hiddenBuf.remove( selStart, selEnd-selStart);
		setTextCursor( selStart, true, false);
	}
}

public boolean echoCharIsSet() {
	return echoChar != 0;
}

void extendSelectionTo( int idx, boolean fromLeft) {
	if ( idx < 0)
		idx = 0;
	else if ( idx > textBuf.len)
		idx = textBuf.len;
	if ( idx == tCursor.index)
		return;

	if ( idx < selStart)
		selStart = idx;
	else if ( idx > selEnd)
		selEnd = idx;
	else if ( fromLeft) {
		//force repaint from start
		tCursor.index = selStart;
		selStart = idx;
	}
	else
		selEnd = idx;

	setTextCursor( idx, false, true);
}

public void focusGained( FocusEvent e) {
	repaint();
}

public void focusLost( FocusEvent e) {
	Graphics g = getGraphics();
	if ( g != null ) {
		paintBorder( g);
		if ( Defaults.ShowInactiveCursor ) {
			paintInactiveCursor( g);
		}
		else {
			repaint( g, 0);
		}
		g.dispose();
	}
	
	if ( selStart < selEnd )
		copyToClipboard();
}

TextBuffer getBuffer() {
	return ( (hiddenBuf != null ) ? hiddenBuf : textBuf );
}

public int getCaretPosition() {
	return tCursor.index;
}

public int getColumns() {
	return cols;
}

int getCursorPos( TextBuffer tb, int idx) {
	if ( idx < 0 )
		idx = 0;
	else if ( idx > textBuf.len )
		idx = textBuf.len;
	return ( BORDER_WIDTH + xOffs + tb.getWidth( first, idx) );
}

public char getEchoChar() {
	return echoChar;
}

public Dimension getMinimumSize( int cols) {
	FontMetrics fm = getFontMetrics( font);
	int w = cols * fm.charWidth( 'x');
	int h = fm.getHeight() * 3/2;
	return new Dimension( w, h);
}

public Dimension getPreferredSize() {
	int cx = 50;
	int cy = 20;
	if ( font != null ){
		FontMetrics fm = getFontMetrics( font);
//		cx = Math.max( cx, fm.stringWidth( label));
		cy = Math.max( cy, 3*fm.getHeight()/2 );
	}
	return new Dimension( cx, cy);
}

public String getSelectedText() {
	if ( selStart != selEnd)
		return textBuf.getString( selStart, selEnd-selStart);
	return null;
}

public int getSelectionEnd() {
	return selEnd;
}

public int getSelectionStart() {
	return selStart;
}

public String getText() {
	return textBuf.toString();
}

public void keyPressed( KeyEvent e) {
	int mods = e.getModifiers();
	boolean shift = e.isShiftDown();
	int code = e.getKeyCode();

	if ( handleClipboard( e) ) {
		e.consume();
	  return;
	}

	// do not consume unused keys for HotKeyHandler
	if ( (mods != 0) && (mods != e.SHIFT_MASK) )
		return;
		
	switch( code ){
		case KeyEvent.VK_ENTER:
			notifyAction();
			break;
		case KeyEvent.VK_ESCAPE:
			setText( "");
			break;
		case KeyEvent.VK_HOME:
			if ( shift) extendSelectionTo( 0, false);
			else        setTextCursor( 0, true, false);
			break;
		case KeyEvent.VK_END:
			if ( shift) extendSelectionTo( textBuf.len, true);
			else        setTextCursor( textBuf.len, true, false);
			break;
		case KeyEvent.VK_LEFT:
			if ( shift) extendSelectionTo( tCursor.index-1, false);
			else        shiftTextCursor( -1, true);
			break;
		case KeyEvent.VK_RIGHT:
			if ( shift) extendSelectionTo( tCursor.index+1, true);
			else        shiftTextCursor( 1, true);
			break;
		case KeyEvent.VK_BACK_SPACE:
			if ( selStart != selEnd)
				deleteSelection();
			else if ( tCursor.index > 0) {
				textBuf.remove( tCursor.index-1, 1);
				shiftTextCursor( -1, true);
			}
			break;
		case KeyEvent.VK_DELETE:
			if ( selStart != selEnd)
				deleteSelection();
			else if ( tCursor.index < textBuf.len){
				textBuf.remove( tCursor.index, 1);
				repaintTrailing();
			}
			break;
		default:
		  return;
	}
	
	e.consume();
}

public void keyReleased ( KeyEvent evt ) {
}

public void keyTyped( KeyEvent e) {

	if ( ! isEditable || ! isPrintableTyped( e) )
		return;

	char c = e.getKeyChar();

	if ( selStart != selEnd){
		textBuf.replace( selStart, selEnd - selStart, c);
		if ( hiddenBuf != null )
			hiddenBuf.replace( selStart, selEnd - selStart, echoChar);
		setTextCursor( selStart+1, true, false);
	}
	else {
		textBuf.insert( tCursor.index, c );
		if ( hiddenBuf != null )
			hiddenBuf.insert( tCursor.index, echoChar);
		shiftTextCursor( 1, true);
	}
	
	if ( textListener != null ) {
		tEvt.setSource( this);
		processTextEvent( tEvt);
	}
}

void makeVisible( int idx) {
	int db = BORDER_WIDTH;
	TextBuffer tb = getBuffer();
	int lf = first;
	
	if ( idx < 0 )
		idx = 0;
	else if ( idx > tb.len )
		idx = tb.len;
		
	if ( idx < first ) {
		first = idx;
	}
	else {
		int xp = tb.getPos( idx);
		if ( width > 0 ) {
			int mp = width - 2*db - 2*xOffs - tCursor.width;;
			for (; mp < xp - tb.getPos( first); first++ );
		}
	}
		
	if ( first != lf )
		repaint();
}

public void mouseClicked ( MouseEvent evt ) {
}

public void mouseDragged( MouseEvent e) {
	TextBuffer tb = getBuffer();
	int cIdx = tb.getIdxFrom( first, e.getX() - xOffs - BORDER_WIDTH);
	extendSelectionTo( cIdx, cIdx > tCursor.index);
}

public void mouseEntered ( MouseEvent evt ) {
}

public void mouseExited ( MouseEvent evt ) {
}

public void mouseMoved( MouseEvent e) {
}

public void mousePressed( MouseEvent e) {
	TextBuffer tb = getBuffer();
	int cIdx = tb.getIdxFrom( first, e.getX() - xOffs - BORDER_WIDTH);
	blankSelection();
	setTextCursor( cIdx, true, false);
	requestFocus();
	
	if ( e.getModifiers() == InputEvent.BUTTON2_MASK )
		pasteFromClipboard();
}

public void mouseReleased( MouseEvent e) {
}

void notifyAction(){
	if ( hasToNotify( AWTEvent.ACTION_EVENT_MASK, aListener)) {
		ActionEvent ae = AWTEvent.getActionEvent( this, ActionEvent.ACTION_PERFORMED);
		ae.setActionEvent( getText(), 0 );
		Toolkit.eventQueue.postEvent( ae);
	}
}

public void paint( Graphics g) {
	int d = BORDER_WIDTH;
	paintBorder( g);
	
	g.setColor( bgClr);
	g.fillRect( d, d, width-2*d, height-2*d);

	repaint( g, 0);
	
}

void paintInactiveCursor( Graphics g) {
	g.setColor( Defaults.TextCursorInactiveClr);
	tCursor.blank( g);
}

protected String paramString() {
	return super.paramString();
}

protected void processActionEvent( ActionEvent e) {
	aListener.actionPerformed( e);
}

protected void processEvent( AWTEvent e) {
	if ( hasToNotify( AWTEvent.ACTION_EVENT_MASK, aListener))
		processActionEvent( (ActionEvent) e);
	else
		super.processEvent( e);
}

public void removeActionListener( ActionListener al) {
	aListener = AWTEventMulticaster.remove( aListener, al);
}

void repaint( Graphics g, TextBuffer tb, int start, int end, boolean invert) {
	int x0, x1;
	int db = BORDER_WIDTH;
	
	if ( start < first )
		start = first;
		
	x0 = db + xOffs + tb.getWidth( first, start);
	x1 = x0 + tb.getWidth ( start, end);

	if ( invert) {
		g.setColor( Defaults.TextFieldSelBgClr );
		if ( end == tb.len )
			x1 += tCursor.width;
		g.fill3DRect( x0, tCursor.y, x1-x0, tCursor.height+1, true);
		g.setColor( Defaults.TextFieldSelTxtClr);
	}
	else {
		g.setColor( bgClr );
		if ( end == tb.len )
			x1 = width - db - xOffs;
		g.fillRect( x0, tCursor.y, x1-x0, tCursor.height+1);
		g.setColor( fgClr);
	}

	tb.paintFrom( g, db + xOffs, 0, height, first, start, end-start);
}

void repaint( Graphics g, int start){
	int db = BORDER_WIDTH;
	TextBuffer tb = getBuffer();

	g.clipRect( db + xOffs, db, width - 2*db - 2*xOffs, height - 2*db);
	g.setFont( font);

	if ( start > 0 )
		start = start -1;
		
	if ( (selStart == selEnd) || (start > selEnd) )
		repaint( g, tb, start, tb.len, false);
	else {
		if ( selStart > start)
			repaint( g, tb, start, selStart, false);
		if ( selEnd > start)
			repaint( g, tb, Math.max( selStart, start), selEnd, true);
		if ( textBuf.len > selEnd)
			repaint( g, tb, selEnd, textBuf.len, false);
	}

	if ( AWTEvent.keyTgt == this)
		tCursor.paint( g);
	else if ( Defaults.ShowInactiveCursor )
		paintInactiveCursor( g);

}

void repaint( int start){
	Graphics g = getGraphics();
	if ( g != null) {
		repaint( g, start);
		g.dispose();
	}
}

void repaintTrailing() {
	repaint( tCursor.index);
}

void replaceSelectionWith ( String s ) {
	deleteSelection();
	textBuf.insert( selEnd, s);
	selStart = selEnd + s.length();
	selEnd = selStart;
	setCaretPosition( selStart);
	repaint();
}

void resetSelIdxs( int idx) {
	selStart = idx;
	selEnd = idx;
}

public void select( int start, int end) {
	selStart = start;
	selEnd = end;
	if ( isShowing() )
		repaint();
}

public void selectAll() {
	select( 0, textBuf.len);
}

public void setBounds( int x, int y, int width, int height) {
	int h = fm.getHeight();
	tCursor.setPos( (height - h)/2, h);
	super.setBounds( x, y, width, height);
}

public void setCaretPosition( int pos) {
	setTextCursor( pos, true, false);
}

public void setColumns( int cols) {
	if ( this.cols != cols)
		this.cols = cols;
}

public void setEchoChar( char echoChar) {
	this.echoChar = echoChar;
	if ( echoChar != 0 ) {
		if ( hiddenBuf == null ) {
			hiddenBuf = new TextBuffer();
			hiddenBuf.setMetrics( fm, 20);
		}
		hiddenBuf.set( echoChar, textBuf.len);
	}
	else
		hiddenBuf = null;

	setTextCursor( tCursor.index, false, true);
}

public void setFont( Font f) {
	super.setFont( f);
	fm = getFontMetrics( f);
	textBuf.setMetrics( fm, 20);
	if ( hiddenBuf != null )
		hiddenBuf.setMetrics( fm, 20);

	int h = fm.getHeight();
	TextBuffer tb = (hiddenBuf != null ) ? hiddenBuf : textBuf;
	tCursor.setPos( (height - h)/2, h);
	tCursor.setIndex( tCursor.index, getCursorPos( tb, tCursor.index) );
	
	if ( isShowing() )
		repaint();
}

public void setSelectionEnd( int end) {
	select( selStart, end);
}

public void setSelectionStart( int start) {
	select( start, selEnd);
}

public void setText( String text) {
	textBuf.set( text);
	if ( hiddenBuf != null )
		hiddenBuf.set( echoChar, text.length() );
	setTextCursor( 0, true, false);
}

protected void setTextCursor( int xIdx, boolean resetSel, boolean repaintAll) {
	int repIdx = 0;
	int db = BORDER_WIDTH;
	TextBuffer tb = getBuffer();

	if ( xIdx > tb.len)
		xIdx = tb.len;
	else if ( xIdx < 0)
		xIdx = 0;

	makeVisible( xIdx);

	if ( resetSel) {
		repIdx = Math.min( selStart, xIdx);
		resetSelIdxs( xIdx);
	}
	else if ( ! repaintAll )
		repIdx = Math.min( xIdx, tCursor.index);

	tCursor.setIndex( xIdx, getCursorPos( tb, xIdx) );

	repaint( repIdx);
}

protected void shiftTextCursor( int d, boolean resetSel) {
	if ( tCursor.index + d < 0 )
		setTextCursor( 0, resetSel, false);
	else if ( tCursor.index + d > textBuf.len )
		setTextCursor( textBuf.len, resetSel, false);
	else
		setTextCursor( tCursor.index + d, resetSel, false);
}
}
