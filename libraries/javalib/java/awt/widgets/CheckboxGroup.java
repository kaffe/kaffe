/**
 * class CheckboxGroup - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.awt;

import java.util.Vector;

public class CheckboxGroup implements java.io.Serializable {

private static final long serialVersionUID = 3729780091441768983L;

/** @serial */
Checkbox selectedCheckbox;
transient Vector boxes = new Vector();

public CheckboxGroup () {
}

/**
 * @deprecated
 */
public Checkbox getCurrent() {
	return selectedCheckbox;
}

public Checkbox getSelectedCheckbox () {
	return (getCurrent());
}

public synchronized void setSelectedCheckbox ( Checkbox box) {
	setCurrent(box);
}

/**
 * @deprecated
 */
public synchronized void setCurrent(Checkbox box) {
	if ( selectedCheckbox == box) {
		return;
	}
	selectedCheckbox = box;
	selectedCheckbox.group = null;
	for ( int i=0; i<boxes.size(); i++) {
		Checkbox cb = (Checkbox)boxes.elementAt( i);
		cb.setState( cb == selectedCheckbox);
	}
	selectedCheckbox.group = this;
}

public String toString () {
	return ( "CheckboxGroup: " + boxes.size() + " items");
}
}
