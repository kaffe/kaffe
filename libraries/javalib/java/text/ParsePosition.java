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

public class ParsePosition {

int idx;

public ParsePosition(int idx) {
	this.idx = idx;
}

public int getIndex() {
	return (idx);
}

public void setIndex(int idx) {
	this.idx = idx;
}

}
