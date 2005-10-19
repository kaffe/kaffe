package java.awt;

import java.awt.AWTEvent;
import java.awt.AWTEventMulticaster;
import java.awt.ItemSelectable;
import java.awt.Menu;
import java.awt.MenuItem;
import java.awt.MenuShortcut;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import org.kaffe.util.Ptr;

/**
 * class CheckboxMenuItem -
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
public class CheckboxMenuItem
  extends MenuItem
  implements ItemSelectable
{
	boolean isChecked;
	ItemListener iListener;
	final private static long serialVersionUID = 6190621106981774043L;

public CheckboxMenuItem () {
	this( "", false);
}

public CheckboxMenuItem ( String label ) {
	this( label, false);
}

public CheckboxMenuItem ( String label, boolean isChecked ) {
	super( label);
	this.isChecked = isChecked;
}

public void addItemListener( ItemListener newListener) {
	iListener = AWTEventMulticaster.add( iListener, newListener);
}

public void addNotify() {
	super.addNotify();
	if ( isChecked )
		setState( isChecked );
}

public Object[] getSelectedObjects () {
	// ItemSelectable interface

	if ( isChecked ) {
		Object[] selItems = new Object[1];
		selItems[0] = getLabel();
		return selItems;
	}
	else
		return null;
}

public boolean getState () {
	return isChecked;
}

public void handleShortcut( MenuShortcut ms) {
	if ( (eventMask & AWTEvent.DISABLED_MASK) != 0 )
		return;

	setState( !isChecked );

	if ( (iListener != null) ||
	     ( ((eventMask & AWTEvent.ITEM_EVENT_MASK) != 0) || ((flags & IS_OLD_EVENT) > 0) ) ) {
		Toolkit.eventQueue.postEvent( ItemEvt.getEvent( this, ItemEvent.ITEM_STATE_CHANGED, getLabel(),
		                                                isChecked ? ItemEvent.SELECTED : ItemEvent.DESELECTED ));
	}
}

public String paramString() {
	return super.paramString() + ", " + (isChecked ? "checked" : "unchecked");
}

void process ( ActionEvent ae ) {
	setState( !isChecked );
	ItemEvt ie = ItemEvt.getEvent( this, ItemEvt.ITEM_STATE_CHANGED, 
					label, isChecked ? ItemEvt.SELECTED : ItemEvt.DESELECTED );
	Toolkit.eventQueue.postEvent( ie);
	super.process( ae);
}

void process ( ItemEvent ie ) {
	if ( (iListener != null) ||
	     ((eventMask & (AWTEvent.ITEM_EVENT_MASK|AWTEvent.DISABLED_MASK))
		 == AWTEvent.ITEM_EVENT_MASK) ){
		processEvent( ie);
	}
}

protected void processEvent ( AWTEvent e ) {
	// same uglyness like in Component.processEvent - we already had it down
	// to the specific Event class and now have to go up to AWTEvent again because
	// this might be re-implemented by a derived class

	if ( e.id ==  ItemEvent.ITEM_STATE_CHANGED ){
		processItemEvent( (ItemEvent) e);
	}
	else {
		super.processEvent( e);
	}
}

protected void processItemEvent ( ItemEvent ie ) {
	if ( iListener != null ){
		iListener.itemStateChanged( ie);
	}
}

public void removeItemListener ( ItemListener listener ) {
	iListener = AWTEventMulticaster.remove( iListener, listener);
}

public synchronized void setState ( boolean isChecked ) {
	this.isChecked = isChecked;

	if ( (parent != null) && (parent instanceof Menu) ) {
		Ptr p = ((Menu)parent).nativeData;
		if ( p != null ) {
			Toolkit.menuCheckItem( p, this, isChecked);
		}
	}

}
}
