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

	private int index;
	private int errorIndex;

	public ParsePosition(int index) {
		this.index = index;
		this.errorIndex = -1;
	}

	public int getIndex() {
		return index;
	}

	public void setIndex(int index) {
		this.index = index;
	}

	public int getErrorIndex() {
		return errorIndex;
	}

	public void setErrorIndex(int errorIndex) {
		this.errorIndex = errorIndex;
	}

	public boolean equals(Object obj) {
		if (obj == null || !getClass().equals(obj.getClass()))
			return false;
		ParsePosition that = (ParsePosition)obj;
		return this.index == that.index
		    && this.errorIndex == that.errorIndex;
	}

	public int hashCode() {
		return index + errorIndex;
	}

	public String toString() {
		return getClass().getName()
		    + "[index=" + index + ",errorIndex=" + errorIndex + ']';
	}
}

