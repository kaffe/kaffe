package java.awt;

import kaffe.util.Ptr;

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
public class Dialog
  extends Frame
{
	boolean isModal;

public Dialog ( Frame owner ) {
	this( owner, "", false);
}

public Dialog ( Frame owner, String title ) {
	this( owner, title, false);
}

public Dialog ( Frame owner, String title, boolean isModal) {
	super( owner, title);
	this.isModal = isModal;
}

public Dialog ( Frame owner, boolean isModal ) {
	this( owner, "", isModal);
}

Ptr createNativeWindow () {
	return Toolkit.wndCreateDialog( owner.nativeData, title, x, y, width, height,
	                                cursor.type, bgClr.nativeValue, isResizable);
}

public boolean isModal() {
	return isModal;
}

public void setModal ( boolean isModal ) {
	this.isModal = isModal;
}

public void setVisible ( boolean isVisible ) {
	super.setVisible( isVisible);
	
	if ( isVisible && isModal ) {
		Toolkit.eventThread.run( this);
	}
}
}
