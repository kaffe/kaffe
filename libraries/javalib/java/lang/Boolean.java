/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

import java.io.Serializable;

final public class Boolean implements Serializable {

public static final Boolean TRUE = new Boolean(true);
public static final Boolean FALSE = new Boolean(false);
public static final Class TYPE = Class.getPrimitiveClass("boolean");

/* This is what Sun's JDK1.1 "serialver java.lang.Boolean" spits out */
private static final long serialVersionUID = -3665804199014368530L;

private final boolean value;

public Boolean(String s) {
	value = (s != null && s.toLowerCase().equals("true"));
}

public Boolean(boolean value) {
	this.value=value;
}

public boolean booleanValue() {
	return (value);
}

public boolean equals(Object obj) {
	return (obj instanceof Boolean)
	  && (((Boolean) obj).value == this.value);
}

public static boolean getBoolean(String name) {
	String value = System.getProperty(name);
	if (value == null) {
		return (false);
	}
	else {
		return (value.equalsIgnoreCase("true"));
	}
}

public int hashCode() {
	return (value ? 1231 : 1237);
}

public String toString() {
	return (value ? "true" : "false");
}

public static Boolean valueOf(String s) {
	return new Boolean(s);
}
}
