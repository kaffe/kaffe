package java.awt;

import java.awt.AWTError;
import java.awt.AWTEvent;
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Cursor;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Toolkit;
import java.awt.event.MouseEvent;

import kaffe.util.Ptr;

/**
 * NativeComponent - abstract root of all native widgets
 *
 * Copyright (c) 1999
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
abstract public class NativeComponent
  extends Component
{
	Ptr nativeData;

protected NativeComponent() {
	flags |= IS_ASYNC_UPDATED;
}

public void addNotify () {
	if ( nativeData == null ) {
		throw new AWTError( "native create failed: " + this);
	}

	// enable mapping of native events to Java Components
	AWTEvent.registerSource( this, nativeData);
	
	setNativeBounds( x, y, width, height);
	super.addNotify();

	//native components are created visible
	if ( (flags & IS_VISIBLE) == 0 )
		Toolkit.cmnSetVisible( nativeData, false);

	initAttribs();
		
	Toolkit.cmnSetBackground( nativeData, bgClr.getNativeValue());
	Toolkit.cmnSetForeground( nativeData, fgClr.getNativeValue());
	Toolkit.cmnSetFont( nativeData, font.nativeData);
	Toolkit.cmnSetCursor( nativeData, cursor.type);		
}

void cleanUpNative () {
	if ( nativeData != null ) {
		AWTEvent.unregisterSource( this, nativeData);
		nativeData = null;
	}
}

void createNative () {
}

void destroyNative () {
	if ( nativeData != null ) {
		Toolkit.cmnDestroyWindow( nativeData);
		cleanUpNative();
	}
}

ClassProperties getClassProperties() {
	return ClassAnalyzer.analyzeAll( getClass(), true);
}

public Graphics getGraphics () {
	if ( (flags & IS_ADD_NOTIFIED) != 0 )
		return NativeGraphics.getGraphics( null, nativeData, NativeGraphics.TGT_TYPE_WINDOW, 0,0,
	                                   0, 0, width, height, fgClr, bgClr, font, false);
	else
		return null;
}

Ptr getNativeData() {
	return nativeData;
}

Ptr getParentData() {
	for ( Container c=parent; c != null; c=c.parent) {
		if ( c instanceof NativeContainer ) {
			return ((NativeContainer)c).nativeData;
		}
	}
	return null;
}

public void hide ()  {
	if ( (flags & IS_VISIBLE) != 0 ) {
		super.hide();

		if ( nativeData != null ){
			Toolkit.cmnSetVisible( nativeData, false);
		}
	}
}

void initAttribs() {
	boolean pv;
	
	if ( bgClr == null ) {
		pv = ( (parent != null) && (parent.bgClr != null) );
		setBackground( pv ? parent.bgClr : Defaults.WndBackground);
	}
	if ( fgClr == null ) {
		pv = ( (parent != null) && (parent.fgClr != null) );
		setForeground( pv ? parent.fgClr : Defaults.WndForeground);
	}
	if ( font == null ) {
		pv = ( (parent != null) && (parent.font != null) );
		setFont( pv ? parent.font : Defaults.WndFont);
	}
}

void kaffePaintBorder ( Graphics g, int left, int top, int right, int bottom ) {
}

void processMouse( MouseEvent e) {
	if ( /*Defaults.AutoPopup &&*/ e.isPopupTrigger() ) {
		triggerPopup( e.getX(), e.getY() );
	}
	super.processMouse( e);
}

public void removeNotify () {
	if ( nativeData != null ) {
		// generally components are destroyed native
		// via parent destruction. To prevent further event
		// emmitting ( esp. Paint events ) for unregistered compoments
		// ensure native destruction for dedicated component removes
		super.removeNotify();
		Toolkit.destroyNative(this);
	}
}

public void requestFocus () {
	super.requestFocus();

	if ( nativeData != null ) {
		Toolkit.cmnSetFocus( nativeData);
	}
}

public void reshape ( int xNew, int yNew, int wNew, int hNew ) {
	super.reshape( xNew, yNew, wNew, hNew);
	setNativeBounds( xNew, yNew, wNew, hNew);
}

public void setBackground ( Color clr ) {
	if ( clr != null ) {
		super.setBackground( clr);

		if ( nativeData != null ) {
			Toolkit.cmnSetBackground( nativeData, bgClr.getNativeValue());
		}
	}
}

public void setCursor ( Cursor cursor ) {
	if ( cursor != null ) {
		this.cursor = cursor;
		setNativeCursor( cursor);
	}
}

public void setEnabled ( boolean isEnabled ) {
	super.setEnabled( isEnabled);

	if ( nativeData != null ) {
		Toolkit.cmnSetEnabled( nativeData, isEnabled);
	}
}

public void setFont ( Font newFont ) {
	if ( newFont != null ) {
		super.setFont( newFont);

		if ( nativeData != null ) {
			Toolkit.cmnSetFont( nativeData, font.nativeData);
		}
	}
}

public void setForeground ( Color clr ) {
	if ( clr != null ) {
		super.setForeground( clr);

		if ( nativeData != null ) {
			Toolkit.cmnSetForeground( nativeData, fgClr.getNativeValue());
		}
	}
}

void setNativeBounds ( int x, int y, int width, int height ) {
	if ( nativeData != null ) {
		for ( Component c=parent; c != null; c = c.parent ) {
			if ( c instanceof NativeContainer )
				break;
			x += c.x;
			y += c.y;
		}	
		Toolkit.cmnSetBounds( nativeData, x, y, width, height);
	}
}

void setNativeCursor ( Cursor cursor ) {
	if ( (nativeData != null) && ((flags & IS_ADD_NOTIFIED) != 0)) {
		Toolkit.cmnSetCursor( nativeData, cursor.type);
	}
}

public void show ()  {
	if ( (flags & IS_VISIBLE) == 0 ) {
		super.show();

		if ( nativeData != null ){
			Toolkit.cmnSetVisible( nativeData, true);
		}
	}
}

public void update ( Graphics g) {
	flags |= IS_IN_UPDATE;
	paint( g);
	flags &= ~IS_IN_UPDATE;
}
}
