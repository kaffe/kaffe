package java.awt.event;

import java.awt.Component;

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
	Component child;
	final public static int CONTAINER_FIRST = 300;
	final public static int CONTAINER_LAST = 301;
	final public static int COMPONENT_ADDED = CONTAINER_FIRST;
	final public static int COMPONENT_REMOVED = CONTAINER_FIRST + 1;

public ContainerEvent ( Component src, int evtId, Component child ) {
	super( src, evtId);
	
	this.child = child;
}

protected void dispatch () {
	processContainerEvent( this);
}

public Component getChild() {
	return child;
}

public Component getContainer() {
	return (Component) source;
}

public String paramString () {
	String s = "COMPONENT_";

	switch ( id ) {
	case COMPONENT_ADDED:  s += "ADDED";
	case COMPONENT_REMOVED: s += "REMOVED";
	default: s += "?";
	}

	s += " [" + child + ']';
	
	return s;
}
}
