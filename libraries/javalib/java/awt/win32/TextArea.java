package java.awt;

import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.FontMetrics;
import java.awt.TextComponent;
import java.awt.Toolkit;
import java.awt.event.KeyEvent;

/**
 * class TextArea -
 *
 * Copyright (c) 1999
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
public class TextArea
  extends TextComponent
{
	int rows;
	int cols;
	final public static int SCROLLBARS_BOTH = 0;
	final public static int SCROLLBARS_VERTICAL_ONLY = 1;
	final public static int SCROLLBARS_HORIZONTAL_ONLY = 2;
	final public static int SCROLLBARS_NONE = 3;
	int scrolls;

public TextArea() {
	this( "", 0, 0, SCROLLBARS_BOTH);
}

public TextArea( String text) {
	this( text, 0, 0, SCROLLBARS_BOTH);
}

public TextArea( String text, int rows, int cols) {
	this( text, rows, cols, SCROLLBARS_BOTH);
}

public TextArea( String text, int rows, int cols, int scrolls) {
	this.rows = rows;
	this.cols = cols;
	this.scrolls = scrolls;

	cursor = Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR);
	setForeground( Defaults.TextAreaTxtClr);
	setBackground( Defaults.TextAreaBgClr);
	setFont( Defaults.TextAreaFont);
	setText( text);
}

public TextArea( int rows, int cols) {
	this( "", rows, cols, SCROLLBARS_BOTH);
}

public void addNotify() {
	if ( nativeData == null ) {
		Toolkit.createNative(this);
		super.addNotify();
	}
}

public void append( String text) {
	if ( nativeData != null )
		Toolkit.editAppend( nativeData, text);
}

void createNative() {
	nativeData = Toolkit.editCreateArea( getParentData(), text, scrolls);
}

public int getColumns() {
	return cols;
}

public Dimension getMinimumSize() {
	return getMinimumSize( Math.max( cols, 5), Math.max( rows, 5));
}

public Dimension getMinimumSize( int cols, int rows) {
	return getPreferredSize( cols, rows);
}

public Dimension getPreferredSize() {
	return getPreferredSize( Math.max( cols, 5), Math.max( rows, 5));
}

public Dimension getPreferredSize( int cols, int rows) {
	FontMetrics fm = getFontMetrics( getFont() );
	int cx = cols * fm.charWidth('x');
	int cy = rows * fm.getHeight();
	return new Dimension( cx, cy);
}

public int getRows() {
	return rows;
}

public int getScrollbarVisibility() {
	return scrolls;
}

public void insert( String text, int pos) {
	if ( nativeData != null )
		Toolkit.editInsert( nativeData, text, pos);
}

protected String paramString() {
	return "";
}

protected void process( KeyEvent e) {
	//consume simple tabs from ShortcutHandler
	if ( (e.getModifiers() == 0) && (e.getKeyChar() == '\t' ) )
		e.consumed = true;
	super.process( e);
}

public void replaceRange( String text, int start, int end) {
	if ( nativeData != null )
		Toolkit.editReplaceRange( nativeData, text, start, end);
}

public void setColumns( int cols) {
	this.cols = cols;
}

public void setRows( int rows) {
	this.rows = rows;
}
}
