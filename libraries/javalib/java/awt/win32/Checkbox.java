package java.awt;

import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

/**
 * class Checkbox - 
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Checkbox
  extends NativeComponent
  implements ItemSelectable
{
	ItemListener iListener;
	CheckboxGroup group;
	boolean state;
	String label;

public Checkbox() {
	this( "", false, null);
}

public Checkbox( String label) {
	this( label, false, null);
}

public Checkbox ( String label, boolean state) {
	this( label, state, null);
}

public Checkbox( String label, boolean state, CheckboxGroup group) {
	cursor = Cursor.getPredefinedCursor( Cursor.DEFAULT_CURSOR);

	this.label = label;
	this.state = state;
	setCheckboxGroup( group);
	setFont( Defaults.TextFont);
	setForeground( Color.black);
}

public void addItemListener( ItemListener l) {
	iListener = AWTEventMulticaster.add( iListener, l);
}

public void addNotify() {
	if ( nativeData == null ) {
		Toolkit.createNative(this);
		Toolkit.btnSetCheck( nativeData, state);
		super.addNotify();
	}
}

void createNative() {
	if ( group != null ) {
		nativeData = Toolkit.btnCreateRadiobutton( getParentData(), label );
	}
	else {
		nativeData = Toolkit.btnCreateCheckbox( getParentData(), label);
	}
}

public CheckboxGroup getCheckboxGroup() {
	return group;
}

public String getLabel() {
	return label;
}

public Dimension getPreferredSize() {
	FontMetrics fm = getFontMetrics( getFont() );
	return new Dimension( 50, fm.getHeight() + 8);
}

public Object[] getSelectedObjects() {
	Object[] so = { null };
	if ( state )
		so[0] = this;
	return so;
}

public boolean getState() {
	if ( nativeData != null ) {
		state = Toolkit.btnGetCheck( nativeData);
	}
	return state;
}

protected String paramString() {
	String s = ",label:" + label + ",state:";

	if ( state )    s += "checked";
	else            s += "unchecked";

	return super.paramString() + s;
}

void process( ItemEvent e) {
	switch ( e.getStateChange() ) {
	case ItemEvent.SELECTED:
		break;
	case ItemEvent.DESELECTED:
		if ( group != null )
			return;
		break;
	}

	setState(e.getStateChange() == ItemEvent.SELECTED);

	if ( (iListener != null) || (eventMask & AWTEvent.ITEM_EVENT_MASK) != 0){
		processEvent( e);
	}

	if ( (flags & IS_OLD_EVENT) != 0 ) postEvent( Event.getEvent( e));
}

protected void processItemEvent( ItemEvent e) {
	if ( iListener != null )
		iListener.itemStateChanged( e);
}

public void removeItemListener( ItemListener l) {
	iListener = AWTEventMulticaster.remove( iListener, l);
}

public void setCheckboxGroup( CheckboxGroup group) {
	if ( this.group == group )
		return;

	if ( this.group != null ) {
		this.group.boxes.removeElement( this);
		if ( (group == null) && (nativeData != null) )
			Toolkit.btnSetGrouped( nativeData, false);
	}
	else if ( (group != null) && (nativeData != null) ) {
		Toolkit.btnSetGrouped( nativeData, true);
	}

	this.group = group;

	if ( this.group != null )
		this.group.boxes.addElement( this);

}

public void setLabel( String label) {
	this.label = label;

	if ( nativeData != null ) {
		Toolkit.btnSetLabel( nativeData, label);
	}
}

public void setState( boolean state) {
	if ( this.state == state )
		return;
	if ( state && (group != null )) {
		group.setSelectedCheckbox( this);
	}
	else {
		this.state = state;
	}

	if ( nativeData != null ) {
		Toolkit.btnSetCheck( nativeData, state);
	}

}
}
