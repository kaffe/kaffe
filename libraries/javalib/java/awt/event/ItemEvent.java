package java.awt.event;

import java.awt.AWTEvent;
import java.awt.Checkbox;
import java.awt.Event;
import java.awt.ItemSelectable;
import java.awt.List;

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
	private static final long serialVersionUID = -608708132447206933L;

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

/**
 * Create an old style event from an Item event.  Note that we
 * create different kinds of events if the source is a Checkbox or
 * a List.
 */
protected Event initOldEvent ( Event e ) {
	if (source instanceof Checkbox) {
		e.target = source;
		e.id = Event.ACTION_EVENT;
		if (op == SELECTED) {
			e.arg = Boolean.TRUE;
		}
		else {
			e.arg = Boolean.FALSE;
		}
		return e;
	}
	else if (source instanceof List) {
		e.target = source;
		if (op == SELECTED) {
			e.id = Event.LIST_SELECT;
		}
		else {
			e.id = Event.LIST_DESELECT;
		}
		e.arg = obj;
		return e;
	}
	return null;
}

}
