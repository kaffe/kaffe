package java.awt.event;

import java.awt.Component;
import java.awt.Container;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
public class ContainerEvent
  extends ComponentEvent
{
	protected Component child;
	final public static int CONTAINER_FIRST = 300;
	final public static int CONTAINER_LAST = 301;
	final public static int COMPONENT_ADDED = CONTAINER_FIRST;
	final public static int COMPONENT_REMOVED = CONTAINER_FIRST + 1;
	private static final long serialVersionUID = -4114942250539772041L;

public ContainerEvent ( Component src, int evtId, Component child ) {
	super( src, evtId);
	
	this.child = child;
}

public Component getChild() {
	return child;
}

public Container getContainer() {
	return (Container) source;
}

public String paramString () {
	String s = "COMPONENT_";

	switch ( id ) {
	case COMPONENT_ADDED:  s += "ADDED"; break;
	case COMPONENT_REMOVED: s += "REMOVED"; break;
	default: s += "?";
	}

	s += " [" + child + ']';
	
	return s;
}
}
