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

public class CheckboxGroup
{
	Checkbox selection;
	Vector boxes = new Vector();

public CheckboxGroup () {
}

public Checkbox getSelectedCheckbox () {
	return selection;
}

public synchronized void setSelectedCheckbox ( Checkbox box) {
	if ( selection == box)
		return;
	selection = box;
	selection.group = null;
	for ( int i=0; i<boxes.size(); i++) {
		Checkbox cb = (Checkbox)boxes.elementAt( i);
		cb.setState( cb == selection);
	}
	selection.group = this;
}

public String toString () {
	return ( "CheckboxGroup: " + boxes.size() + " items");
}
}
