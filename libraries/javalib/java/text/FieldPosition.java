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

int field;
int begin;
int end;

public FieldPosition(int field) {
	this.field = field;
	this.begin = 0;
	this.end = 0;
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

}
