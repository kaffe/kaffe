package java.awt;

import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.Vector;

/**
 * class Choice - 
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Choice
  extends NativeComponent
  implements ItemSelectable
{
	ItemListener iListener;
	Vector items = new Vector();
	int selection = -1;

public Choice() {
}

public void add( String item) {
	insert( item, -1);
}

public void addItem( String item) {
	insert( item, -1);
}

public void addItemListener( ItemListener l) {
	iListener = AWTEventMulticaster.add( iListener, l);
}

public void addNotify() {
	if ( nativeData == null ) {
		Toolkit.createNative(this);
		super.addNotify();
		updateItems();
	}
}

void createNative() {
	nativeData = Toolkit.choiceCreateChoice(getParentData());
}

public String getItem( int idx) {
	return (String)items.elementAt( idx);
}

public int getItemCount() {
	return items.size();
}

public Dimension getPreferredSize() {
	FontMetrics fm = getFontMetrics( getFont() );
	return new Dimension( 100, fm.getHeight() + 6);
}

public int getSelectedIndex() {
	return selection;
}

public String getSelectedItem() {
	if ( selection == -1 )
		return null;
	return (String)items.elementAt( selection);
}

public Object[] getSelectedObjects() {
	Object[] so = { null };
	if ( selection > -1 )
		so[0] = items.elementAt( selection);
	return so;
}

public void insert( String item, int idx) {
	if ( idx == -1 ) {
		items.addElement( item);
	}
	else {
		items.insertElementAt( item, idx);
	}
	updateItems();
}

protected String paramString() {
	return super.paramString();
}

void process ( ItemEvent e ) {
	int idx = ((Integer)e.getItem()).intValue();
	Object o = items.elementAt( idx);

	switch ( e.getStateChange() ) {
	case ItemEvent.SELECTED:
		selection = idx;
		break;
    	}

	if ( (iListener != null) || (eventMask & AWTEvent.ITEM_EVENT_MASK) != 0){
		processEvent( e);
    	}
	
    	if ( (flags & IS_OLD_EVENT) != 0 ) postEvent( Event.getEvent( e));
}

protected void processItemEvent( ItemEvent e) {
	if ( iListener != null )
		iListener.itemStateChanged( e);
}

public void remove( String item) {
	remove( items.indexOf( item));
}

public void remove( int idx) {
	items.removeElementAt( idx);
	if ( selection == idx )
		selection = -1;
	updateItems();
}

public void removeAll() {
	items.removeAllElements();
	selection = -1;
	updateItems();
}

public void removeItemListener( ItemListener l) {
	iListener = AWTEventMulticaster.remove( iListener, l);
}

public void select( String item) {
	select( items.indexOf( item));
}

public void select( int idx) {
	selection = idx;
	if ( nativeData != null )
		Toolkit.choiceSelectItem( nativeData, idx);
}

void updateItems() {
	if ( nativeData != null ) {
		Toolkit.choiceRemoveAll( nativeData);
		for ( int i=0; i<items.size(); i++)
			Toolkit.choiceAppendItem( nativeData, (String)items.elementAt( i));
		select( 0);
	}
}
}
