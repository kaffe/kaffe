package java.awt.event;

import java.awt.AWTEvent;
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
public class ComponentEvent
  extends AWTEvent
{
	final public static int COMPONENT_FIRST = 100;
	final public static int COMPONENT_LAST = 103;
	final public static int COMPONENT_MOVED = COMPONENT_FIRST;
	final public static int COMPONENT_RESIZED = COMPONENT_FIRST + 1;
	final public static int COMPONENT_SHOWN = COMPONENT_FIRST + 2;
	final public static int COMPONENT_HIDDEN = COMPONENT_FIRST + 3;
	private static final long serialVersionUID = 8101406823902992965L;

public ComponentEvent ( Component source, int id ) {
	super( source, id);
}

public Component getComponent() {
	return (Component) source;
}

public String paramString () {
	String prefix = "COMPONENT_";

	switch ( id ) {
	case COMPONENT_SHOWN:		  return prefix + "SHOWN";
	case COMPONENT_HIDDEN:		return prefix + "HIDDEN";
	case COMPONENT_MOVED:			return prefix + "MOVED";
	case COMPONENT_RESIZED:		return prefix + "RESIZED";
	}
	
	return prefix + '?';
}
}
