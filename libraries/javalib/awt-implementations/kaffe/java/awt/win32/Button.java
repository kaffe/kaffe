package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import org.kaffe.util.Ptr;

/**
 * class Button - 
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Button
  extends NativeComponent
{
	String label;
	ActionListener aListener;
	String aCmd;

public Button () {
	this( "");
}

public Button ( String label) {
	cursor = Cursor.getPredefinedCursor( Cursor.DEFAULT_CURSOR);
	
	setFont( Defaults.BtnFont);
	setBackground( Defaults.BtnClr);
	setForeground( Defaults.BtnTxtClr);
	setLabel( label);
}

public void addActionListener ( ActionListener a) {
	aListener = AWTEventMulticaster.add( aListener, a);
}

public void addNotify() {
	if ( nativeData == null ) {
		Toolkit.createNative(this);
		super.addNotify();
	}			
}

void createNative() {
	nativeData = Toolkit.btnCreatePushbutton( getParentData(), label);
}

public String getActionCommand () {
	return ( (aCmd != null) ? aCmd : label);
}

public String getLabel() {
	return label;
}

public Dimension getMinimumSize() {
    return getPreferredSize();
}

public Dimension getPreferredSize () {
	FontMetrics fm = getFontMetrics( getFont() );
	int ax = fm.stringWidth( label) + 10;
	int ay = fm.getHeight() + 8;
	
        	return new Dimension( ax, ay);
}

protected String paramString() {
	return (super.paramString() + ",Label: " + label);
}

void process( ActionEvent e) {
	if ( (aListener != null) || (eventMask & AWTEvent.ACTION_EVENT_MASK) != 0 )
                	processEvent( e);

        	if ( (flags & IS_OLD_EVENT) != 0 )
                	postEvent( Event.getEvent( e));
}

protected void processActionEvent( ActionEvent e) {
        	if ( aListener != null )
                	aListener.actionPerformed( e);
}

public void removeActionListener ( ActionListener a) {
	aListener = AWTEventMulticaster.remove( aListener, a);
}

public void setActionCommand ( String aCmd) {
	this.aCmd = aCmd;
}

public void setLabel ( String label) {
	this.label = label;

	if ( nativeData != null ) {
		Toolkit.btnSetLabel( nativeData, label);
	}
}
}
