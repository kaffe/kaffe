package java.awt;

import java.awt.AWTEvent;
import java.awt.AWTEventMulticaster;
import java.awt.Dimension;
import java.awt.ItemSelectable;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.Vector;

/**
 * class List - 
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class List
  extends NativeComponent
  implements ItemSelectable
{
	ActionListener aListener;
	ItemListener iListener;
	boolean multipleMode;
	Vector items = new Vector();
	int selection = -1;

public List () {
	this( 3, false);
}

public List ( int rows) {
	this( rows, false);
}

public List ( int rows, boolean multipleMode) {
	setMultipleMode( multipleMode);
	setForeground( Defaults.ListTxtClr);
	setBackground( Defaults.ListBgClr);
	setFont( Defaults.ListFont);
}

public void add ( String item) {
	addElement( item, -1);
}

public synchronized void add ( String item, int index) {
	addElement( item, index);
}

public synchronized void addActionListener ( ActionListener l) {
	aListener = AWTEventMulticaster.add( aListener, l);
}

void addElement ( String item, int index) {
	if ( index == -1 ) {
		items.addElement( item);
		if ( nativeData != null )
			Toolkit.lstAppendItem( nativeData, item);
  	}
  	else {
    		items.insertElementAt( item, index);
		if ( nativeData != null )
			Toolkit.lstInsertItem( nativeData, item, index);
	}
}

public void addItem ( String item) {
	addElement( item, -1);
}

public synchronized void addItem ( String item, int index) {
	addElement( item, index);
}

public synchronized void addItemListener ( ItemListener l) {
	iListener = AWTEventMulticaster.add( iListener, l);
}

public void addNotify() {
	if ( nativeData == null ) {
		Toolkit.createNative(this);
		super.addNotify();
		setItems();
		if ( selection > -1 ) {
			Toolkit.lstSelectItem( nativeData, selection);
		}
	}
}

void createNative() {
	nativeData = Toolkit.lstCreateList( getParentData(), multipleMode);
}

public synchronized void delItem ( int index) {
	removeElement( index);
}

public synchronized void deselect ( int index) {
	deselectElement( index);
}

void deselectElement ( int index ) {
	if ( nativeData != null ) {
		Toolkit.lstUnselectItem( nativeData, index);
	}
}

public String getItem ( int index) {
	return (String)items.elementAt( index);
}

public int getItemCount () {
	return items.size();
}

public synchronized String[] getItems () {
	String[] si = new String[ items.size() ];
	for ( int i=0; i<si.length; i++)
		si[i] = (String)items.elementAt( i);
	return si;
}

public Dimension getMinimumSize () {
	return new Dimension( 50, 50);
}

public Dimension getMinimumSize ( int rows) {
	return getMinimumSize();
}

public Dimension getPreferredSize () {
	return new Dimension( 100, 100);
}

public Dimension getPreferredSize ( int rows) {
	return getPreferredSize();
}

public int getRows () {
	return items.size();
}

public synchronized int getSelectedIndex () {
	return selection;
}

public synchronized int[] getSelectedIndexes () {
	if ( nativeData != null ) {
		return Toolkit.lstGetSelectionIdxs( nativeData);
	}
	return null;
}

public synchronized String getSelectedItem () {
	return (selection > -1 ) ? (String)items.elementAt( selection) : null;
}

public synchronized String[] getSelectedItems () {
	int[] idxs;
        String[] sels = null;

	if ( ! multipleMode ) {
		if ( selection == -1 ) {
                        sels = new String[0];
		}
		else {
			sels = new String[1];
			sels[0] = items.elementAt( selection).toString();
		}
	}
	else if ( nativeData != null) {
		idxs = Toolkit.lstGetSelectionIdxs( nativeData);
                if ( idxs == null ) {
                        sels = new String[0];
                }
                else {
                        sels = new String[ idxs.length];
                        for ( int i=0; i<idxs.length; i++) {
                                sels[i] = items.elementAt( idxs[i]).toString();
                        }
		}
	}

        return sels;
}

public Object[] getSelectedObjects () {
	int[] idxs;
        Object[] sels = null;

	if ( ! multipleMode ) {
		if ( selection == -1 ) {
                        sels = new Object[0];
		}
		else {
			sels = new Object[1];
			sels[0] = items.elementAt( selection);
		}
	}
	else if ( nativeData != null) {
		idxs = Toolkit.lstGetSelectionIdxs( nativeData);
                if ( idxs == null ) {
                        sels = new Object[0];
                }
                else {
                        sels = new Object[ idxs.length];
                        for ( int i=0; i<idxs.length; i++) {
                                sels[i] = items.elementAt( idxs[i]);
                        }
		}
	}

        return sels;
}

public int getVisibleIndex () {
	if ( nativeData != null ) {
		return Toolkit.lstGetVisibleIndex( nativeData);
	}
	else {
		return -1;
	}
}

public boolean isIndexSelected ( int index) {
	return ( selection == index);
}

public boolean isMultipleMode () {
	return multipleMode;
}

protected String paramString() {
	return super.paramString();
}

void process( ActionEvent e) {
	if ( (aListener != null) || (eventMask & AWTEvent.ACTION_EVENT_MASK) != 0){
		processEvent( e);
	}
}

void process( ItemEvent e) {
	int idx = ((Integer)e.getItem()).intValue();
	Object o = items.elementAt( idx);

        if ( ! multipleMode ) {
                selection = idx;
        }
        else {
                switch ( e.getStateChange() ) {
                case ItemEvent.SELECTED:
                        if ( selection == -1 )
                                selection = idx;
                        break;
                case ItemEvent.DESELECTED:
                        if ( selection == idx )
                                selection = -1;
                        break;
                }
	}

	if ( (iListener != null) || (eventMask & AWTEvent.ITEM_EVENT_MASK) != 0){
		processEvent( e);
	}
}

protected void processActionEvent( ActionEvent e) {
	if ( aListener != null )
		aListener.actionPerformed( e);
}

protected void processItemEvent( ItemEvent e) {
	if ( iListener != null )
		iListener.itemStateChanged( e);
}

public synchronized void remove ( String item) {
	removeElement( items.indexOf( item));
}

public synchronized void remove ( int index) {
	removeElement( index);
}

public synchronized void removeActionListener ( ActionListener l) {
	aListener = AWTEventMulticaster.remove( aListener, l);
}

public synchronized void removeAll () {
	items.removeAllElements();
	selection = -1;

	if ( nativeData != null ) {
		Toolkit.lstRemoveAll( nativeData);
	}
}

void removeElement ( int index) {
	items.removeElementAt( index);
	if ( index == selection )
		selection = -1;

	if ( nativeData != null ) {
		Toolkit.lstRemoveItem( nativeData, index);
        }
}

public synchronized void removeItemListener ( ItemListener l) {
	iListener = AWTEventMulticaster.remove( iListener, l);
}

public synchronized void replaceItem ( String newValue, int index) {
	items.setElementAt( newValue, index);

	if ( nativeData != null ) {
		Toolkit.lstReplaceItem( nativeData, newValue, index);
	}
}

public void select ( int index) {
	if ( (index < 0) || (index > items.size() - 1) )
		return;
	
	selection = index;
	if ( nativeData != null ) {
		Toolkit.lstSelectItem( nativeData, index);
	}
}

void setItems() {
	if ( nativeData != null ) {
		Toolkit.lstFreeze( nativeData);
		for ( int i=0; i<items.size(); i++)
			Toolkit.lstAppendItem( nativeData, (String)items.elementAt( i) );
		Toolkit.lstThaw( nativeData);
	}
}

public synchronized void setMultipleMode ( boolean b) {
	multipleMode = b;

	if ( nativeData != null ) {
		Toolkit.lstSetMultipleMode( nativeData, b);
	}
}
}
