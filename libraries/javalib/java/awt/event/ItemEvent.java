package java.awt.event;

import java.awt.AWTEvent;
import java.awt.ItemSelectable;

/**
 * class ItemEvent - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class ItemEvent
  extends AWTEvent
{
	protected Object obj;
	protected int op;
	final public static int ITEM_FIRST = 701;
	final public static int ITEM_LAST = 701;
	final public static int ITEM_STATE_CHANGED = ITEM_FIRST;
	final public static int SELECTED = 1;
	final public static int DESELECTED = 2;

public ItemEvent ( ItemSelectable src, int evtId, Object item, int stateChange) {
	super( src, evtId);
	obj = item;
	op = stateChange;
}

public Object getItem () {
	return obj;
}

public ItemSelectable getItemSelectable () {
	return (ItemSelectable)source;
}

public int getStateChange () {
	return op;
}

public String paramString() {
	String s = (id == ITEM_STATE_CHANGED) ? "ITEM_STATE_CHANGED": "ITEM: " + id;
	
	s += ", item: " + obj + ' ';
	switch ( op ) {
	case SELECTED:    s += "SELECTED"; break;
	case DESELECTED:  s += "DESELECTED"; break;
	default:          s += "op: " + op;
	}
	
	return s;
}
}
