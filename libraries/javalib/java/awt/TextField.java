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
	final private static long serialVersionUID = -2966288784432217853L;
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
	NativeGraphics rgr;

public TextField() {
	this( "", 0);
}

public TextField( String text) {
	this( text, (text != null) ? text.length() : 0);
}

public TextField( String text, int cols) {
	this.cols = cols;

	cursor = Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR);
	
	setFont( Defaults.TextFieldFont);
	setBackground( Defaults.TextFieldBgClr);
	setForeground( Defaults.TextFieldTxtClr);
	setText( text);
	
	buildMenu();
	
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
}

public void addNotify () {
	super.addNotify();
	
	if ( rgr == null ) {
		setResGraphics();
	}
}

void blankSelection() {
	if ( selStart != selEnd)
		setTextCursor( selStart, true, false);
}

protected void buildMenu() {
	PopupMenu p = new PopupMenu();
	p.add( new MenuItem("Cut")).setShortcut( new MenuShortcut( KeyEvent.VK_U, false) );
	p.add( new MenuItem("Copy")).setShortcut( new MenuShortcut( KeyEvent.VK_O, false) );
	p.add( new MenuItem("Paste")).setShortcut( new MenuShortcut( KeyEvent.VK_A, false) );
	p.addSeparator();
	p.add( new MenuItem("Select All")).setShortcut( new MenuShortcut( KeyEvent.VK_S, false) );
	p.addActionListener( this);
	
	add( p);
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
	kaffePaintBorder();
	if ( Defaults.ShowInactiveCursor ) {
		paintInactiveCursor();
	}
	else {
		repaintFrom( 0);
	}
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

public Dimension getMinimumSize() {
	return (minimumSize());
}

public Dimension getMinimumSize(int cols) {
	return (minimumSize(cols));
}

public Dimension getPreferredSize(int cols) {
	return (preferredSize(cols));
}

public String getSelectedText() {
	if ( selStart != selEnd) {
		return textBuf.getString( selStart, selEnd-selStart);
	}
	return (null);
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
        boolean changed = false;
	int mods = e.getModifiers();
	boolean shift = e.isShiftDown();
	int code = e.getKeyCode();

	// do not consume unused keys for ShortcutHandler
	if ( (mods != 0) && (mods != e.SHIFT_MASK) ) {
		return;
	}

	switch( code ){
	case KeyEvent.VK_ENTER:
		notifyAction();
		break;
	case KeyEvent.VK_ESCAPE:
		if (isEditable) {
			setText( "");
			changed = true;
		}
		break;
	case KeyEvent.VK_HOME:
		if ( shift) {
			extendSelectionTo( 0, false);
		}
		else {
			setTextCursor( 0, true, false);
		}
		break;
	case KeyEvent.VK_END:
		if ( shift) {
			extendSelectionTo( textBuf.len, true);
		}
		else {
			setTextCursor( textBuf.len, true, false);
		}
		break;
	case KeyEvent.VK_LEFT:
		if ( shift) {
			extendSelectionTo( tCursor.index-1, false);
		}
		else {
			shiftTextCursor( -1, true);
		}
		break;
	case KeyEvent.VK_RIGHT:
		if ( shift) {
			extendSelectionTo( tCursor.index+1, true);
		}
		else {
			shiftTextCursor( 1, true);
		}
		break;
	case KeyEvent.VK_BACK_SPACE:
		if (isEditable) {
			if ( selStart != selEnd) {
				deleteSelection();
				changed = true;
			}
			else if ( tCursor.index > 0) {
				textBuf.remove( tCursor.index-1, 1);
				shiftTextCursor( -1, true);
				changed = true;
			}
		}
		break;
	case KeyEvent.VK_DELETE:
		if (isEditable) {
			if ( selStart != selEnd) {
				deleteSelection();
				changed = true;
			}
			else if ( tCursor.index < textBuf.len) {
				textBuf.remove( tCursor.index, 1);
				repaintTrailing();
				changed = true;
			}
		}
		break;
	default:
		return;
	}
	
	if (changed && ((textListener != null) || (eventMask & AWTEvent.TEXT_EVENT_MASK) != 0))
	{
	  Toolkit.eventQueue.postEvent( TextEvt.getEvent( this, TextEvt.TEXT_VALUE_CHANGED));		
	}

	e.consume();
}

public void keyReleased ( KeyEvent evt ) {
}

public void keyTyped( KeyEvent e) {
	if ( ! isEditable || e.isConsumed() || ! isPrintableTyped( e) )
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

	if ( (textListener != null) || (eventMask & AWTEvent.TEXT_EVENT_MASK) != 0 ) {
		Toolkit.eventQueue.postEvent( TextEvt.getEvent( this, TextEvt.TEXT_VALUE_CHANGED));		
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
			int mp = width - 2*db - 2*xOffs - tCursor.width;
			for (; mp < xp - tb.getPos( first); first++ );
		}
	}
		
	if ( first != lf )
		repaint();
}

/**
 * @deprecated
 */
public Dimension minimumSize() {
	return (minimumSize(cols));
}

/**
 * @deprecated
 */
public Dimension minimumSize(int cols) {
	FontMetrics  fm = getFontMetrics( font);
	int          nc = cols;
	int	     h = fm.getHeight() + 3*BORDER_WIDTH;
	int	     w = 0;

	if ( nc <= 0 ) {
		if ( textBuf.len > 0 )
			w = textBuf.getWidth( 0, textBuf.len);
		else
			nc = 5;
	}
	if ( w == 0 )
		w = nc * fm.charWidth( 'X');


	w += 2*(BORDER_WIDTH + xOffs);
	return new Dimension( w, h);
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
	if ( e.isPopupTrigger() ){
		if ( (triggerPopup( e.getX(), e.getY())) != null )
			return;
	}

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
	if ( (aListener != null) || ((eventMask & AWTEvent.ACTION_EVENT_MASK) != 0) ){
		Toolkit.eventQueue.postEvent( ActionEvt.getEvent( this, ActionEvent.ACTION_PERFORMED,
		                                                  getText(), 0));
	}
}

public void paint( Graphics g) {
	int d = BORDER_WIDTH;
	kaffePaintBorder( g);
	
	g.setColor( bgClr);
	g.fillRect( d, d, width-2*d, height-2*d);

	repaintFrom( 0);
}

public void paintInactiveCursor() {
	rgr.setColor( Defaults.TextCursorInactiveClr);
	tCursor.blank( rgr);
}

protected String paramString() {
	return super.paramString();
}

/**
 * @deprecated
 */
public Dimension preferredSize() {
	return (preferredSize(cols));
}

/**
 * @deprecated
 */
public Dimension preferredSize(int cols) {
	return (getMinimumSize(cols));
}

void process ( ActionEvent e ) {
	if ( (aListener != null) || ((eventMask & (AWTEvent.ACTION_EVENT_MASK|AWTEvent.DISABLED_MASK)) == AWTEvent.ACTION_EVENT_MASK) ){
		processEvent( e);
	}

	if ( (flags & IS_OLD_EVENT) > 0 ) {
		postEvent( Event.getEvent( e));
	}
}

protected void processActionEvent ( ActionEvent e) {
	if ( aListener != null )
		aListener.actionPerformed( e);
}

void propagateReshape () {
	if ( rgr != null ){
		updateLinkedGraphics();

		// we have our own clipping, which we have to take care of
		rgr.setClip( BORDER_WIDTH, BORDER_WIDTH,
		             width - 2*BORDER_WIDTH, height - 2*BORDER_WIDTH);
	}
}

public void removeActionListener( ActionListener al) {
	aListener = AWTEventMulticaster.remove( aListener, al);
}

public void removeNotify () {
	super.removeNotify();

	if ( rgr != null ) {
		rgr.dispose();
		rgr = null;
	}
}

void repaintFrom( int start){
	if ( (flags & IS_SHOWING) != IS_SHOWING ) return;

	TextBuffer tb = getBuffer();

	if ( start > 0 )
		start = start -1;

	if ( (selStart == selEnd) || (start > selEnd) )
		repaintRange( tb, start, tb.len, false);
	else {
		if ( selStart > start) 
			repaintRange( tb, start, selStart, false);
		if ( selEnd > start)
			repaintRange( tb, Math.max( selStart, start), selEnd, true);
		if ( textBuf.len > selEnd)
			repaintRange( tb, selEnd, textBuf.len, false);
	}

	if ( AWTEvent.keyTgt == this)
		tCursor.paint( rgr);
	else if ( Defaults.ShowInactiveCursor )
		paintInactiveCursor();

}

void repaintRange( TextBuffer tb, int start, int end, boolean invert) {
	int x0, x1;
	int db = BORDER_WIDTH;
	
	if ( rgr == null )
		return;
		
	if ( start < first )
		start = first;
		
	x0 = db + xOffs + tb.getWidth( first, start);
	x1 = x0 + tb.getWidth ( start, end);

	if ( invert) {
		rgr.setColor( Defaults.TextFieldSelBgClr );
		if ( end == tb.len )
			x1 += tCursor.width;
		rgr.fill3DRect( x0, tCursor.y, x1-x0, tCursor.height+1, true);
		rgr.setColor( Defaults.TextFieldSelTxtClr);
	}
	else {
		rgr.setColor( bgClr );
		if ( end == tb.len )
			x1 = width - db;
		rgr.fillRect( x0, tCursor.y, x1-x0, tCursor.height+1);
		rgr.setColor( fgClr);
	}

	tb.paintFrom( rgr, db + xOffs, 0, height, first, start, end-start);
}

void repaintTrailing() {
	repaintFrom( tCursor.index);
}

void replaceSelectionWith ( String s ) {
	deleteSelection();
	if ( s != null ) {
		textBuf.insert( selEnd, s);
		selStart = selEnd + s.length();
		selEnd = selStart;
		setCaretPosition( selStart);
		repaint();
	}
}

void resetSelIdxs( int idx) {
	selStart = idx;
	selEnd = idx;
}

public void reshape( int x, int y, int w, int h) {
	super.reshape( x, y, w, h);

	int fh = fm.getHeight();
	tCursor.setPos( (height - fh)/2, fh);
}

public void select( int start, int end) {
	selStart = start;
	selEnd = end;
	setTextCursor( selStart, false, true);
}

public void selectAll() {
	select( 0, textBuf.len);
}

public void setCaretPosition( int pos) {
	setTextCursor( pos, true, false);
}

public void setColumns( int cols) {
	if ( this.cols != cols)
		this.cols = cols;
}

public void setEchoChar( char echoChar) {
	setEchoCharacter(echoChar);
}

/**
 * @deprecated
 */
public void setEchoCharacter(char echoChar) {
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
	fm = getFontMetrics( f);
	textBuf.setMetrics( fm, 20);
	if ( hiddenBuf != null )
		hiddenBuf.setMetrics( fm, 20);

	int h = fm.getHeight();
	TextBuffer tb = (hiddenBuf != null ) ? hiddenBuf : textBuf;
	tCursor.setPos( (height - h)/2, h);
	tCursor.setIndex( tCursor.index, getCursorPos( tb, tCursor.index) );

	if ( rgr != null )
		rgr.setFont( f);
	
	super.setFont( f);
}

void setResGraphics () {
	rgr = NativeGraphics.getClippedGraphics( rgr, this, 0, 0,
					                                 BORDER_WIDTH, BORDER_WIDTH,
		                                       width - 2*BORDER_WIDTH, height - 2*BORDER_WIDTH, false);
	if ( rgr != null )
		linkGraphics( rgr);
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

	repaintFrom( repIdx);
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
