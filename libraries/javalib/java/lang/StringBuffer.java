package java.lang;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
final public class StringBuffer
  implements java.io.Serializable
{
	char[] buffer;
	int used;
	boolean isStringized;
	final private int SPARECAPACITY = 16;

	// This is what Sun's JDK1.1 "serialver java.lang.StringBuffer" says
	private static final long serialVersionUID = 3388685877147921107L; 

public StringBuffer() {
	buffer = new char[SPARECAPACITY];
}

public StringBuffer(String str) {
	if ( str == null)
		str = String.valueOf( str);
	used   = str.length();
	buffer = new char[used+SPARECAPACITY];
	System.arraycopy(str.toCharArray(), 0, buffer, 0, used);
}

public StringBuffer(int length) {
	if (length<0) throw new NegativeArraySizeException();
	buffer=new char[length];
}

public StringBuffer append(Object obj) {
	return append(String.valueOf(obj));
}

public StringBuffer append ( String str ) {
	if (str == null) {
		str = String.valueOf( str);
	}
	return (append( str.value, str.offset, str.count));
}

public StringBuffer append(boolean b) {
	return append(String.valueOf(b));
}

public synchronized StringBuffer append(char c) {
	if ( used + 1 > buffer.length ) {
		ensureCapacity(used + 1);
	}
	if ( isStringized ) {
		deStringize();
	}
	buffer[used++] = c;
	return (this);
}

public StringBuffer append(char str[]) {
	return append(str, 0, str.length);
}

public synchronized StringBuffer append ( char str[], int offset, int len ) {
	if ( used + len > buffer.length ) {
		ensureCapacity(used+len);
	}
	if ( isStringized ) {
		deStringize();
	}
	System.arraycopy( str, offset, buffer, used, len);
	used += len;
	return this;
}

public StringBuffer append(double d) {
	return append(String.valueOf(d));
}

public StringBuffer append(float f) {
	return append(String.valueOf(f));
}

public StringBuffer append(int i) {
	return append(String.valueOf(i));
}

public StringBuffer append(long l) {
	return append(String.valueOf(l));
}

public int capacity() {
	return buffer.length;
}

public synchronized char charAt(int index) {
	checkIndex(index);
	return buffer[index];
}

private synchronized void checkIndex(int index) throws StringIndexOutOfBoundsException {
	if (index < 0 || index >= used)
		throw new StringIndexOutOfBoundsException("index = " + index + ", used = " + used);
}

void deStringize () {
	char[] b = new char[buffer.length];
	System.arraycopy( buffer, 0, b, 0, buffer.length);

	buffer = b;
	isStringized = false;
}

public synchronized void ensureCapacity ( int minimumCapacity ) {
	int    n;
	char[] oldBuffer;

	if ( (minimumCapacity < 0) || (buffer.length > minimumCapacity) ) return; 

	n = buffer.length*2;
	if ( minimumCapacity > n ) n = minimumCapacity;

	oldBuffer = buffer;
	buffer = new char[n];
	isStringized = false;

	System.arraycopy( oldBuffer, 0, buffer, 0, used);
}

public synchronized void getChars(int srcBegin, int srcEnd, char dst[], int dstBegin) {
	checkIndex(srcBegin);
	checkIndex(srcEnd-1);
	System.arraycopy(buffer, srcBegin, dst, dstBegin, srcEnd-srcBegin);
}

public synchronized StringBuffer insert(int offset, Object obj) {
	return insert(offset, String.valueOf(obj));
}

public synchronized StringBuffer insert(int offset, String str) {
	if ( str == null) {
		str = String.valueOf( str);
	}
	return insert(offset, str.toCharArray());
}

public StringBuffer insert(int offset, boolean b) {
	return insert(offset, String.valueOf(b));
}

public synchronized StringBuffer insert ( int offset, char c ) {
	if ((offset < 0) || (offset > used)) {
		throw new StringIndexOutOfBoundsException();
	}
	ensureCapacity(used + 1);
	if ( isStringized ) {
		deStringize();
	}

	// Copy buffer up to make space.
	System.arraycopy(buffer, offset, buffer, offset+1, used-offset);

	/* Now, copy insert into place */
	buffer[offset] = c;

	/* Update used count */
	used ++;

	return (this);
}

public synchronized StringBuffer insert ( int offset, char str[] ) {
	if ((offset < 0) || (offset > used)) {
		throw new StringIndexOutOfBoundsException();
	}
	ensureCapacity(used + str.length);
	if ( isStringized ) {
		deStringize();
	}

	// Copy buffer up to make space.
	System.arraycopy(buffer, offset, buffer, offset+str.length, used-offset);

	/* Now, copy insert into place */
	System.arraycopy(str, 0, buffer, offset, str.length);

	/* Update used count */
	used += str.length;

	return (this);
}

public StringBuffer insert(int offset, double d) {
	return insert(offset, String.valueOf(d));
}

public StringBuffer insert(int offset, float f) {
	return insert(offset, String.valueOf(f));
}

public StringBuffer insert(int offset, int i) {
	return insert(offset, String.valueOf(i));
}

public StringBuffer insert(int offset, long l) {
	return insert(offset, String.valueOf(l));
}

public int length () {
	return used;
}

public synchronized StringBuffer reverse() {
	if ( isStringized ) {
		deStringize();
	}
	for (int pos = used/2 - 1; pos >= 0; pos--) {
		char ch = buffer[pos];
		buffer[pos] = buffer[used-pos-1];
		buffer[used-pos-1] = ch;
	}
	return (this);
}

public synchronized void setCharAt ( int index, char ch ) {
	if ( isStringized ) {
		deStringize();
	}
	if (index < 0 || index >= used)
		throw new StringIndexOutOfBoundsException("index = " + index + ", used = " + used);

	buffer[index]=ch;
}

public synchronized void setLength (int newLength) {
	if (newLength < 0) {
		throw new StringIndexOutOfBoundsException();
	}
	if (newLength > used) {
		/* buffer expands */
		if (newLength > buffer.length) {
			/* Need new buffer */
			char oldBuffer[] = buffer;
			buffer = new char[newLength];
			System.arraycopy(oldBuffer, 0, buffer, 0, used);
			isStringized = false;
		}
		else {
			if ( isStringized )
				deStringize();

			/* Pad buffer */
			for (int pos = used; pos < newLength; pos++) {
				buffer[pos] = (char) 0;
			}
		}
	}
	used = newLength;
}

public String toString() {
	// String ctor will be responsible for to set us isStringized
	return new String(this);
}
}
