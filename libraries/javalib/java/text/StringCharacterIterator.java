
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
	if (begin < 0 || end > text.length() || begin > end
	    || pos < begin || pos > end) {
		throw new StringIndexOutOfBoundsException();
	}
	this.text = text;
	this.pos = pos;
	this.begin = begin;
	this.end = end;
}

void setText(String text) {
	this.text = text;
	this.pos = 0;
	this.begin = 0;
	this.end = text.length();
}

public Object clone() {
	return new StringCharacterIterator(text, begin, end, pos);
}

public char current() {
	if (pos < begin || pos >= end) {
		return (DONE);
	}
	return text.charAt(pos);
}

public boolean equals(Object obj) {
	if (obj instanceof StringCharacterIterator) {
		StringCharacterIterator other = (StringCharacterIterator)obj;
		if (text.equals(other.text)
		    && pos == other.pos && begin == other.begin
		    && end == other.end) {
			return (true);
		}
	}
	return (false);
}

public char first() {
	pos = begin;
	return (text.charAt(pos));
}

public char last() {
	pos = end - 1;
	return (text.charAt(pos));
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

public int hashCode() {
	return (super.hashCode());
}

public char next() {
	if ( pos < end - 1 ) {
		return (text.charAt(++pos));
	}
	pos = end;
	return (DONE);
}

public char previous() {
	if ( pos > begin ) {
		return (text.charAt(--pos));
	}
	pos = begin;
	return (DONE);
}

public char setIndex(int pos) {
        if ( pos < begin || pos > end ) {
	        throw new IllegalArgumentException("Invalid index: "+pos);
	}
	this.pos = pos;
	return (pos == end ? DONE : text.charAt(pos));
}
}
