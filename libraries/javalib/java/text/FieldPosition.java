/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.text;

public class FieldPosition {

private int field;
private int begin;
private int end;

public FieldPosition(int field) {
	this.field = field;
	this.begin = 0;
	this.end = 0;
}

public boolean equals(Object object) {
    if (object == this) {
	return true;
    }
    else if (object == null || getClass() != object.getClass()) {
	return false;
    }
    else {
	FieldPosition other = (FieldPosition) object;
	return other.getField() == getField()
	    && other.getBeginIndex() == getBeginIndex()
	    && other.getEndIndex() == getEndIndex();
    }
}

public int getBeginIndex() {
	return (begin);
}

public int getEndIndex() {
	return (end);
}

public int getField() {
	return (field);
}

public int hashCode() {
    /* this is a rather bad hashCode method,
     * but it preserves equality. Sun didn't bother
     * to document the hashing algorithm anywhere.
     */
    return getField() + getBeginIndex() << 2 + getEndIndex() << 4;
}

public void setBeginIndex(int index) {
    begin = index;
}

public void setEndIndex(int index) {
    end = index;
}
}
