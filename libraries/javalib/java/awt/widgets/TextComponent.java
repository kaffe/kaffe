package java.awt;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import java.awt.event.KeyEvent;
import java.awt.event.TextEvent;
import java.awt.event.TextListener;

/**
 * class TextComponent - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
abstract public class TextComponent
  extends Container
{
	protected transient TextListener textListener;
	boolean isEditable = true;
	protected static TextEvent tEvt = new TextEvent( null, TextEvent.TEXT_VALUE_CHANGED);

public void addTextListener( TextListener l) {
	textListener = AWTEventMulticaster.add( textListener, l);
}

void copyToClipboard () {
	Clipboard cb = Toolkit.getDefaultToolkit().getSystemClipboard();
	String sel = getSelectedText();
	if ( (sel != null) && (sel.length() > 0) ) {
		StringSelection ss = new StringSelection( sel);
		cb.setContents( ss, ss);
	}
}

abstract public int getCaretPosition();

abstract public String getSelectedText();

abstract public int getSelectionEnd();

abstract public int getSelectionStart();

abstract public String getText();

boolean handleClipboard( KeyEvent e) {
	int code = e.getKeyCode();
	int mods = e.getModifiers();
	
	if ( mods == e.CTRL_MASK ) {
		if ( code == 15 ){ //ctrl-o
			copyToClipboard();
			return true;
		}
		if ( code == 1 ) { //ctrl-a
			pasteFromClipboard();
			return true;
		}
	}

	return false;			
}

public boolean isEditable() {
	return isEditable;
}

boolean isPrintableTyped( KeyEvent e) {
	int mods = e.getModifiers();
	int chr  = e.getKeyChar();
	
	if ( (mods != 0) && (mods != e.SHIFT_MASK) )
		return false;
		
	switch( chr ) {
		case 8:		//BACKSPACE
		case 9:		//TAB
		case 10:	//ENTER
		case 27:	//ESC
		case 127:	//DEL
			return false;
	}
	
	return true;		
}

protected String paramString() {
	return (super.paramString() );
}

void pasteFromClipboard () {
	Clipboard cb = Toolkit.getDefaultToolkit().getSystemClipboard();
	Transferable tf = cb.getContents( this);
	
	if ( tf != null ){
		try {
			String s = (String) tf.getTransferData( DataFlavor.stringFlavor);
			replaceSelectionWith( s);
		}
		catch ( Exception x ) {}
	}
}

protected void processEvent( AWTEvent e) {
	if ( e instanceof TextEvent)
		processTextEvent( (TextEvent) e);
	else
		super.processEvent( e);
}

protected void processTextEvent( TextEvent e) {
	if ( hasToNotify( AWTEvent.TEXT_EVENT_MASK, textListener))
		textListener.textValueChanged( e);
}

public void removeTextListener( TextListener l) {
	textListener = AWTEventMulticaster.remove( textListener, l);
}

void replaceSelectionWith ( String s ) {
}

abstract public void select( int start, int end);

abstract public void selectAll();

abstract public void setCaretPosition( int pos);

public void setEditable( boolean edit) {
	isEditable = edit;
}

abstract public void setSelectionEnd( int end);

abstract public void setSelectionStart( int start);

abstract public void setText( String text);
}
