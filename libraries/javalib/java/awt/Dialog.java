/**
 * Dialog - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */

package java.awt;

import kaffe.util.Ptr;

public class Dialog
  extends Frame
{
	boolean isModal;
	static Insets dialogInsets;
	static Rectangle dialogDeco;

static {
	// these are just the guesses (to be overwritten by a subsequent
	// setDialogInsets() from the native layer)
	Insets guess = Defaults.DialogInsets;
	
	dialogInsets = new Insets( guess.top, guess.left, guess.bottom, guess.right);
	dialogDeco = new Rectangle( guess.left, guess.top,
	                           (guess.left+guess.right), (guess.top+guess.bottom));
	
	Toolkit.wndSetDialogInsets( guess.top, guess.left, guess.bottom, guess.right);
}

public Dialog ( Frame owner ) {
	this( owner, null, false);
}

public Dialog ( Frame owner, String title ) {
	this( owner, title, false);
}

public Dialog ( Frame owner, String title, boolean isModal) {
	super( owner, title);

	deco = dialogDeco;
	this.isModal = isModal;
}

public Dialog ( Frame owner, boolean isModal ) {
	this( owner, null, isModal);
}

Ptr createNativeWindow () {
	// this is the terminal class addNotify() part
	
	// insets seem to be set by the JDK during addNotify
	// (no need to create fresh objects since they are insets()-copied anyway <sigh>)
	insets = dialogInsets;

	return Toolkit.wndCreateDialog( owner.nativeData, title,
	                                x + deco.x, y + deco.y,
                                  width - deco.width,
                                  height - deco.height,
	                                cursor.type, bgClr.nativeValue, isResizable);
}

public boolean isModal() {
	return isModal;
}

public Dimension preferredSize () {
	Dimension d = super.preferredSize();
	
	d.width  += (Defaults.DialogInsets.left + Defaults.DialogInsets.right);
	d.height += (Defaults.DialogInsets.top + Defaults.DialogInsets.bottom);

	return (d);
}

static void setDecoInsets ( int top, int left, int bottom, int right ){
	// this is the native callBack to set exact (calculated) dialog deco extends

	dialogInsets.top    = top;
	dialogInsets.left   = left;
	dialogInsets.bottom = bottom;
	dialogInsets.right  = right;
	
	dialogDeco.x = left;
	dialogDeco.y = top;
	dialogDeco.width = left + right;
	dialogDeco.height = top + bottom;
}

public void setModal ( boolean isModal ) {
	this.isModal = isModal;
}

public void show () {
	super.show ();
	
	if ( isModal ) {
		Toolkit.eventThread.run( this);
	}
}
}
