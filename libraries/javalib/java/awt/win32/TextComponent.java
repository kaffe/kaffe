package java.awt;

import java.awt.AWTEvent;
import java.awt.AWTEventMulticaster;
import java.awt.Cursor;
import java.awt.Toolkit;
import java.awt.event.TextEvent;
import java.awt.event.TextListener;

/**
 * class TextComponent -
 *
 * Copyright (c) 1999
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
public class TextComponent
  extends NativeComponent
{
	String text;
	boolean isEditable = true;
	TextListener tListener;

TextComponent () {
	cursor = Cursor.getPredefinedCursor( Cursor.TEXT_CURSOR);
}

public void addNotify() {
	if ( nativeData != null ) {
		Toolkit.editSetEditable( nativeData, isEditable);
		super.addNotify();
	}
}

public void addTextListener( TextListener l) {
	tListener = AWTEventMulticaster.add( tListener, l);
}

public int getCaretPosition() {
	if ( nativeData != null )
		return Toolkit.editGetCaretPosition( nativeData);
	return 0;
}

public String getSelectedText() {
	if ( nativeData != null )
		return Toolkit.editGetSelectedText( nativeData);
	return null;
}

public int getSelectionEnd() {
	if ( nativeData != null )
		return Toolkit.editGetSelectionEnd( nativeData);
	return 0;
}

public int getSelectionStart() {
	if ( nativeData != null )
		return Toolkit.editGetSelectionStart( nativeData);
	return 0;
}

public String getText() {
	if ( nativeData != null )
		text = Toolkit.editGetText( nativeData);

	return text;
}

public boolean isEditable() {
	return isEditable;
}

void process ( TextEvent e ) {
	if ( (tListener != null) || (eventMask & AWTEvent.TEXT_EVENT_MASK) != 0 )
		processEvent( e);
}

protected void processTextEvent( TextEvent e) {
	if ( tListener != null ) {
		tListener.textValueChanged( e);
	}
}

public void removeTextListener( TextListener l) {
	tListener = AWTEventMulticaster.remove( tListener, l);
}

public void select( int start, int end) {
	if ( nativeData != null )
		Toolkit.editSelect( nativeData, start, end);
}

public void selectAll() {
	if ( nativeData != null )
		Toolkit.editSelectAll( nativeData);
}

public void setCaretPosition( int pos) {
	if ( nativeData != null )
		Toolkit.editSetCaretPosition( nativeData, pos);
}

public void setEditable( boolean edit ) {
	isEditable = edit;
	if ( nativeData != null )
		Toolkit.editSetEditable( nativeData, edit);
}

public void setSelectionEnd( int end) {
	if ( nativeData != null )
		Toolkit.editSetSelectionEnd( nativeData, end);
}

public void setSelectionStart( int start) {
	if ( nativeData != null )
		Toolkit.editSetSelectionStart( nativeData, start);
}

public void setText( String text) {
	this.text = text;
	if ( nativeData != null )
		Toolkit.editSetText( nativeData, text);
}
}
