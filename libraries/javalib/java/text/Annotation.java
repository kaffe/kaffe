/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *      Dalibor Topic <robilad@yahoo.com>.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
package java.text;

public class Annotation {
    private final Object value;

    public Annotation(Object value) {
	this.value = value;
    }

    public Object getValue() {
	return value;
    }

    public String toString() {
	return getClass().getName() + "[value=" + getValue() + ']';
    }
}
