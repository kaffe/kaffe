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

public final class StringBuffer implements java.io.Serializable {
	private static final int SPARECAPACITY = 16;

	char[] buffer;				// character buffer
	int used;				// # chars used in buffer
	boolean isStringized;			// buffer also part of String
						//  and therefore unmodifiable

	// This is what Sun's JDK1.1 "serialver java.lang.StringBuffer" says
	private static final long serialVersionUID = 3388685877147921107L;

public StringBuffer() {
	buffer = new char[SPARECAPACITY];
}

public StringBuffer(String str) {
	used = str.count;
	buffer = new char[used + SPARECAPACITY];
	System.arraycopy(str.value, str.offset, buffer, 0, used);
}

public StringBuffer(int length) {
	buffer = new char[length];
}

public StringBuffer append(Object obj) {
	return append(String.valueOf(obj));
}

public StringBuffer append ( String str ) {
	if (str == null) {
		str = "null";
	}
	return append(str.value, str.offset, str.count);
}

public StringBuffer append(boolean b) {
	return append(String.valueOf(b));
}

public synchronized StringBuffer append(char c) {
	if (used + 1 > buffer.length || isStringized)		// optimization
		ensureCapacity(used + 1, isStringized);
	buffer[used++] = c;
	return this;
}

public StringBuffer append(char str[]) {
	return append(str, 0, str.length);
}

public synchronized StringBuffer append(char str[], int offset, int len) {
	if (offset < 0 || len < 0 || offset + len > str.length)
		throw new StringIndexOutOfBoundsException();
	if (used + len > buffer.length || isStringized)		// optimization
		ensureCapacity(used + len, isStringized);
	System.arraycopy(str, offset, buffer, used, len);
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
	if (index >= used)
		throw new StringIndexOutOfBoundsException();
	return buffer[index];
}

private void checkIndex(int index) {
	if (index < 0 || index > length()) {
		throw new StringIndexOutOfBoundsException();
	}
}

private void checkExistingIndex(int index) {
	if (index < 0 || index >= length()) {
		throw new StringIndexOutOfBoundsException();
	}
}

public synchronized StringBuffer deleteCharAt(int index) {
	checkExistingIndex(index);
	System.arraycopy(buffer, index + 1, buffer, index, --used - index);
	return this;
}

public synchronized StringBuffer delete(int start, int end) {
	if (end > used)
		end = used;
	if (start < 0 || start > end)
		throw new StringIndexOutOfBoundsException();
	System.arraycopy(buffer, end, buffer, start, used - end);
	used -= end - start;
	return this;
}

public synchronized StringBuffer replace(int start, int end, String str) {
	return delete(start, end).insert(start, str);
}

public synchronized String substring(int start) {
	return substring(start, used);
}

public synchronized String substring(int start, int end) {
	int length = end - start;

	if (start < 0 || end > used || length < 0)
		throw new StringIndexOutOfBoundsException();
	if (buffer.length - length > String.STRINGBUFFER_SLOP)
		return new String(buffer, start, length);
	else {
		isStringized = true;
		return new String(start, end, buffer);
	}
}

public void ensureCapacity(int minCapacity) {
	if (minCapacity <= 0)
		return;
	synchronized (this) {
		ensureCapacity(minCapacity, isStringized);
	}
}

// This method assumes synchronization
private boolean ensureCapacity(int minCapacity, boolean forceNew) {

	// Do we really need to create a new buffer?
	if (!forceNew && minCapacity <= buffer.length) {
		return false;
	}

	// Increase buffer size in powers of two to avoid O(n^2) behavior
	if (minCapacity < used) {
		minCapacity = used;
	} else if (minCapacity < buffer.length * 2 + 2) {
		minCapacity = buffer.length * 2 + 2;
	}

	// Allocate a new buffer and copy data over
	char[] newBuffer = new char[minCapacity];
	System.arraycopy(buffer, 0, newBuffer, 0, used);
	buffer = newBuffer;
	isStringized = false;
	return true;
}

public synchronized void getChars(int srcBegin, int srcEnd,
		char dst[], int dstBegin) {
	if (srcEnd > used)
		throw new StringIndexOutOfBoundsException();
	System.arraycopy(buffer, srcBegin, dst, dstBegin, srcEnd - srcBegin);
}

public synchronized StringBuffer insert(int offset, Object obj) {
	return insert(offset, String.valueOf(obj));
}

public synchronized StringBuffer insert(int offset, String str) {
	if (str == null) {
		str = "null";
	}
	return insert(offset, str.value, str.offset, str.count);
}

public StringBuffer insert(int offset, boolean b) {
	return insert(offset, String.valueOf(b));
}

public synchronized StringBuffer insert(int offset, char c) {
	return insert(offset, new char[] { c }, 0, 1);
}

public StringBuffer insert(int offset, char[] str) {
	checkIndex(offset);
	return insert(offset, str, 0, str.length);
}

public synchronized StringBuffer insert(int index, char[] str,
		int offset, int len) {
	checkIndex(index);
	if (offset < 0 || len < 0 || offset + len > str.length) {
		throw new StringIndexOutOfBoundsException();
	}
	if (used + len > buffer.length || isStringized)		// optimization
		ensureCapacity(used + len, isStringized);

	// Shift buffer rightward to make space
	System.arraycopy(buffer, index, buffer, index + len, used - index);

	// Copy inserted chars into place
	System.arraycopy(str, offset, buffer, index, len);

	// Update used count
	used += len;
	return this;
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
	if (isStringized)				// optimization
		ensureCapacity(used, true);
	for (int pos = used/2 - 1; pos >= 0; pos--) {
		char ch = buffer[pos];
		buffer[pos] = buffer[used - pos - 1];
		buffer[used - pos - 1] = ch;
	}
	return this;
}

public synchronized void setCharAt(int index, char ch) {
	checkIndex(index);
	if (isStringized)				// optimization
		ensureCapacity(used, true);
	buffer[index] = ch;
}

public synchronized void setLength(int newLength) {
	if (newLength < 0) {
		throw new StringIndexOutOfBoundsException();
	}
	boolean newBuf = ensureCapacity(newLength, isStringized);
	if (!newBuf && newLength > used) {
		for (int index = used; index < newLength; index++)
			buffer[index] = '\0';
	}
	used = newLength;
}

public String toString() {
	// String constructor will be responsible for setting isStringized
	return new String(this);
}
}
