package java.awt;

import java.awt.AWTEvent;
import java.awt.AWTEventMulticaster;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.FontMetrics;
import java.awt.TextComponent;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * class TextField -
 *
 * Copyright (c) 1999
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
public class TextField
  extends TextComponent
{
	char echoChar;
	int cols;
	ActionListener aListener;

public TextField() {
	this( "", 0);
}

public TextField( String text) {
	this( text, text.length() );
}

public TextField( String text, int cols) {
	this.cols = cols;

	cursor = Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR);
	setBackground( Defaults.TextFieldBgClr);
	setForeground( Defaults.TextFieldTxtClr);
	setFont( Defaults.TextFieldFont);
	setText( text);
}

public TextField( int cols) {
	this( "", cols);
}

public void addActionListener( ActionListener l) {
	aListener = AWTEventMulticaster.add( aListener, l);
}

public void addNotify() {
	if ( nativeData == null ) {
		Toolkit.createNative(this);
		Toolkit.editSetEchoChar( nativeData, echoChar);
		super.addNotify();
	}
}

void createNative() {
	nativeData = Toolkit.editCreateField( getParentData(), text);
}

public boolean echoCharIsSet() {
	return echoChar != 0;
}

public int getColumns() {
	return cols;
}

public char getEchoChar() {
	return echoChar;
}

public Dimension getMinimumSize() {
	return getMinimumSize( (cols > 0) ? cols : 2);
}

public Dimension getMinimumSize( int cols) {
	return getPreferredSize( cols);
}

public Dimension getPreferredSize() {
	return getPreferredSize( (cols > 0) ? cols+1 : 2);
}

public Dimension getPreferredSize( int cols) {
	FontMetrics fm = getFontMetrics( getFont() );
	return new Dimension( cols * fm.charWidth( 'X') + 8, fm.getHeight() + 8);
}

protected String paramString() {
	return super.paramString();
}

void process( ActionEvent e) {
	if ( (aListener != null) || (eventMask & AWTEvent.ACTION_EVENT_MASK) != 0 )
		processEvent( e);
}

protected void processActionEvent( ActionEvent e) {
	if ( aListener != null )
		aListener.actionPerformed( e);
}

public void removeActionListener( ActionListener l) {
	aListener = AWTEventMulticaster.remove( aListener, l);
}

public void setColumns( int cols) {
	this.cols = cols;
}

public void setEchoChar( char c) {
	echoChar = c;
	if ( nativeData != null )
		Toolkit.editSetEchoChar( nativeData, echoChar);
}
}
