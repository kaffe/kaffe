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
	private boolean isChecked;
	private ItemListener iListener;
	private static int counter;
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
	setName("chkmenuitem" + counter++);
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
	if ( (eventMask & AWTEvent.DISABLED_MASK) != 0 )
		return;

	setState( !isChecked );

	if ( (iListener != null) ||
	     ( ((eventMask & AWTEvent.ITEM_EVENT_MASK) != 0) || ((flags & IS_OLD_EVENT) > 0) ) ) {
		Toolkit.eventQueue.postEvent( ItemEvt.getEvent( this, ItemEvent.ITEM_STATE_CHANGED, getLabel(),
                                      isChecked ? ItemEvent.SELECTED : ItemEvent.DESELECTED ));
	}
}

int paint ( Graphics g, int x, int xoff, int y, int width, int height, Color back, Color fore, boolean sel) {
	int ih = super.paint( g, x, xoff + 6, y, width, height, back, fore, sel);
	int y0 = y + (ih - 8) / 2 + 1;

	g.setColor( Defaults.BtnClr);
	g.fill3DRect( x+3, y0, 8, 8, !isChecked);

	return ih;
}

public String paramString() {
	return super.paramString() + ",state=" + getState();
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
