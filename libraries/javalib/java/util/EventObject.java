/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;

import java.io.Serializable;

public class EventObject extends Object
  implements Serializable
{
	protected transient Object source;

public EventObject(Object src) {
	source = src;
}

public Object getSource() {
	return (source);
}

public String toString() {
	return (source.toString());
}
}
