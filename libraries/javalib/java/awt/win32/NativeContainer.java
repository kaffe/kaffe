package java.awt;

import java.awt.event.*;
import kaffe.util.Ptr;

/**
 * NativeContainer - abstract root of all native containers
 *
 * Copyright (c) 1999
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
abstract public class NativeContainer
  extends Container
{
	Ptr nativeData;

protected NativeContainer() {
	flags |= IS_ASYNC_UPDATED;
}

public void addNotify () {
	if ( nativeData != null ) {
		super.addNotify();

		setNativeBounds( x, y, width, height);
		AWTEvent.registerSource( this, nativeData);

		//
		// THIS IS HACKED UP TO DEAL WITH MISSING VALUES!!!
		// FIX PROPERLY - TIM
		//
		Color clr = getBackground();
		if (clr == null) {
			clr = Defaults.WndBackground;
		}
		Toolkit.cmnSetBackground( nativeData, clr.nativeValue);
		clr = getForeground();
		if (clr == null) {
			clr = Defaults.WndForeground;
		}
		Toolkit.cmnSetForeground( nativeData, clr.nativeValue);
		Font fnt = getFont();
		if (fnt == null) {
			fnt = Defaults.WndFont;
		}
		Toolkit.cmnSetFont( nativeData, fnt.nativeData);
		Cursor cur = cursor;
		if (cur == null) {
			cur = Cursor.defaultCursor;
		}
		Toolkit.cmnSetCursor( nativeData, cur.type);
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

Ptr getParentData() {
	for ( Container c=parent; c != null; c=c.parent) {
		if ( c instanceof NativeContainer ) {
			NativeContainer nc = (NativeContainer)c;
			return nc.nativeData;
		}
	}
	return null;
}

void paintBorder ( Graphics g, int left, int top, int right, int bottom ) {
}

void processMouse( MouseEvent e) {
	if ( Defaults.AutoPopup && e.isPopupTrigger() ) {
		triggerPopup( e.getX(), e.getY() );
	}
	super.processMouse( e);
}

void processPaintEvent ( int id, int ux, int uy, int uw, int uh ) {
	// used by native paints
	Graphics g = NativeGraphics.getGraphics( null, nativeData, NativeGraphics.TGT_TYPE_WINDOW, 0,0,
	                                         ux, uy, uw, uh, fgClr, bgClr, font, false);

	if ( g != null ){
		if ( id == PaintEvent.UPDATE )
			update( g);
		else
			paint( g);
		g.dispose();
	}        
}

public void removeNotify () {
	if ( nativeData != null ) {
		AWTEvent.unregisterSource( this, nativeData);

		// generally components are destroyed native
		// via parent destruction. To prevent further event
		// emmitting ( esp. Paint events ) for unregistered compoments
		// ensure native destruction for dedicated component removes
		Toolkit.cmnDestroyWindow( nativeData);

		nativeData = null;
		super.removeNotify();
	}
}

public void requestFocus() {
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
			Toolkit.cmnSetBackground( nativeData, bgClr.nativeValue);
		}
	}
}

public void setCursor ( Cursor cursor ) {
	this.cursor = cursor;
	setNativeCursor( cursor);
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
			Toolkit.cmnSetForeground( nativeData, fgClr.nativeValue);
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
	if ( nativeData != null ) {
		Toolkit.cmnSetCursor( nativeData, cursor.type);
	}
}

public void setVisible ( boolean showIt ) {
	if ( showIt != isVisible() ) {
		super.setVisible( showIt);
		if ( nativeData != null )
			Toolkit.cmnSetVisible( nativeData, showIt);
	}
}

public void update ( Graphics g) {
	paint( g);
}
}
