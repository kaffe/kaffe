package java.awt;


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
	final private static long serialVersionUID = 5920926903803293709L;
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

	if ( isModal )
		flags |= IS_MODAL;
}

public Dialog ( Frame owner, boolean isModal ) {
	this( owner, null, isModal);
}

void createNative () {
	// This is the terminal class addNotify() part. DANGER: ptr isn't a real object
	int u = x;
	int v = y;
	int w = width;
	int h = height;

	// insets seem to be set by the JDK during addNotify
	// (no need to create fresh objects since they are insets()-copied anyway <sigh>)
	insets = dialogInsets;

	if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ) {
		// we just pretend to own the deco space, subtract it before going native
		u += deco.x;
		v += deco.y;
		w -= deco.width;
		h -= deco.height;
	}

	nativeData = Toolkit.wndCreateDialog( owner.nativeData, title, u, v, w, h,
	                               cursor.type, bgClr.getNativeValue(), ((flags & IS_RESIZABLE) != 0));
}

public void hide () {
	if ( (flags & IS_MODAL) != 0 )
		dispose();
	else
		super.hide();
}

public boolean isModal() {
	return ((flags & IS_MODAL) != 0);
}

public Dimension preferredSize () {
	Dimension d = super.preferredSize();
	
	d.width  += (Defaults.DialogInsets.left + Defaults.DialogInsets.right);
	d.height += (Defaults.DialogInsets.top + Defaults.DialogInsets.bottom);

	return (d);
}

static void setDecoInsets ( int top, int left, int bottom, int right, int srcIdx  ){
	// this is the native callBack to set exact (calculated) dialog deco extends
	dialogInsets.top    = top;
	dialogInsets.left   = left;
	dialogInsets.bottom = bottom;
	dialogInsets.right  = right;
	
	dialogDeco.x = left;
	dialogDeco.y = top;
	dialogDeco.width = left + right;
	dialogDeco.height = top + bottom;

	// if we got the correction in the context of a initial Window positioning
	// we have to make sure a subsequent ComponentEvt.getEvent() invalidates
	// this instance (which wouldn't be the case if we let its (faked) dimension alone)
	if ( srcIdx != -1 ) {
		Component src = AWTEvent.sources[srcIdx];
		src.width = src.height = 0;
	}
}

public void setModal ( boolean isModal ) {
	if ( isModal ) 
		flags |= IS_MODAL;
	else
		flags &= ~IS_MODAL;
}

public void show () {
	// DEP - should be in setVisible()
	super.show ();
	
	if ( (flags & IS_MODAL) != 0 ) {
		if ( owner != null ) {
			// We don't use native modal dialogs, so let's at least temporarily
			// disable the owner
			owner.propagateTempEnabled( false);
			Toolkit.eventThread.run( this);
			owner.propagateTempEnabled( true);
		}
		else {
			Toolkit.eventThread.run( this);
		}
	}
}
}
