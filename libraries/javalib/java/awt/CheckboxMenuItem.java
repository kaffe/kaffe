package java.awt;

import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

/**
 * CheckboxMenuItem
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc., BISS GmbH.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author J.H.Mehlitz
 */
public class CheckboxMenuItem
  extends MenuItem
  implements ItemSelectable
{
	boolean isChecked;
	ItemListener iListener;
	private static final long serialVersionUID = 6190621106981774043L;

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

int getWidth() {
	int sw = super.getWidth();
	sw += 3*getHeight()/4;
	return sw;
}

public void handleShortcut( MenuShortcut ms) {
	setState( !isChecked );

	if ( (iListener != null) ||
	     ((eventMask & (AWTEvent.ITEM_EVENT_MASK|AWTEvent.DISABLED_MASK))
	                        == AWTEvent.ITEM_EVENT_MASK) ) {
		Toolkit.eventQueue.postEvent( ItemEvt.getEvent( this, ItemEvent.ITEM_STATE_CHANGED, getLabel(),
                                      isChecked ? ItemEvent.SELECTED : ItemEvent.DESELECTED ));
	}
}

int paint ( Graphics g, int xoff, int y, int width, Color back, Color fore, boolean sel) {
	int cm = getHeight();
//	g.setColor( isChecked ? Defaults.FocusClr : Defaults.BtnClr);
	g.setColor( Defaults.BtnClr);
	g.fill3DRect( xoff, y + cm/5, cm/2, cm/2, !isChecked);
	xoff += 3*cm/4;

	return super.paint( g, xoff, y, width, back, fore, sel);
}

public String paramString() {
	return super.paramString() + ", " + (isChecked ? "checked" : "unchecked");
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
}
}
