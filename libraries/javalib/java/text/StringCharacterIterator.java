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

public final class StringCharacterIterator implements CharacterIterator {

private String text;
private int pos;
private int begin;
private int end;

public StringCharacterIterator(String text) {
	this(text, 0, text.length(), 0);
}

public StringCharacterIterator(String text, int pos) {
	this(text, 0, text.length(), pos);
}

public StringCharacterIterator(String text, int begin, int end, int pos) {
	this.text = text;
	this.pos = pos;
	this.begin = begin;
	this.end = end;
}

public char first() {
	pos = begin;
	return (current());
}

public char last() {
	pos = end - 1;
	return (current());
}

public char current() {
	if (pos < begin || pos >= end) {
		return (DONE);
	}
	else {
		return (text.charAt(pos));
	}
}

public char next() {
	pos++;
	return (current());
}

public char previous() {
	pos--;
	return (current());
}

public void setIndex(int pos) {
	this.pos = pos;
}

public int getBeginIndex() {
	return (begin);
}

public int getEndIndex() {
	return (end);
}

public int getIndex() {
	return (pos);
}

public Object clone() {
	StringCharacterIterator obj = new StringCharacterIterator(this.text, this.begin, this.end, this.pos);
	return (obj);
}

public boolean equals(Object obj) {
	try {
		StringCharacterIterator other = (StringCharacterIterator)obj;
		if (text.equals(other.text) && pos == other.pos && begin == other.begin && end == other.end) {
			return (true);
		}
	}
	catch (ClassCastException _) {
	}
	return (false);
}

public int hashCode() {
	return (super.hashCode());
}

}
