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
{
	private char[] buffer;
	private int used;
	final private int SPARECAPACITY = 16;

public StringBuffer()
	{
	buffer=new char[SPARECAPACITY];
}

public StringBuffer(String str)
	{
	if ( str == null)
		str = String.valueOf( str);
	used   = str.length();
	buffer = new char[used+SPARECAPACITY];
	System.arraycopy(str.toCharArray(), 0, buffer, 0, used);
}

public StringBuffer(int length)
	{
	if (length<0) throw new NegativeArraySizeException();
	buffer=new char[length];
}

public synchronized StringBuffer append(Object obj)
	{
	return append(String.valueOf(obj));
}

public synchronized StringBuffer append ( String str ) {
	if ( str==null )
		str = String.valueOf( str);
		
	return append( str.value, str.offset, str.count);
}

public StringBuffer append(boolean b)
	{
	return append(String.valueOf(b));
}

public synchronized StringBuffer append(char c)
{
	if ( used + 1 > buffer.length ) {
		ensureCapacity(used+1);
	}
	buffer[used] = c;
	used += 1;

	return (this);
}

public synchronized StringBuffer append(char str[])
{
	return append(str, 0, str.length);
}

public synchronized StringBuffer append ( char str[], int offset, int len ) {

	if ( used + len > buffer.length )
		ensureCapacity(used+len);

	System.arraycopy( str, offset, buffer, used, len);
	used += len;

	return this;
}

public StringBuffer append(double d)
	{
	return append(String.valueOf(d));
}

public StringBuffer append(float f)
	{
	return append(String.valueOf(f));
}

public StringBuffer append(int i)
	{
	return append(String.valueOf(i));
}

public StringBuffer append(long l)
	{
	return append(String.valueOf(l));
}

public int capacity()
	{
	return buffer.length;
}

public synchronized char charAt(int index)
	{
	checkIndex(index);
	return buffer[index];
}

private synchronized void checkIndex(int index) throws StringIndexOutOfBoundsException
{
	if (index < 0 || index >= used)
		throw new StringIndexOutOfBoundsException("index = " + index + ", used = " + used);
}

public synchronized void ensureCapacity ( int minimumCapacity ) {
	int    n;
	char[] oldBuffer;

	if ( (minimumCapacity < 0) || (buffer.length > minimumCapacity) ) return; 

	n = buffer.length*2;
	if ( minimumCapacity > n ) n = minimumCapacity;

	oldBuffer = buffer;
	buffer = new char[n];

	System.arraycopy( oldBuffer, 0, buffer, 0, used);
}

public synchronized void getChars(int srcBegin, int srcEnd, char dst[], int dstBegin)
	{
	checkIndex(srcBegin);
	checkIndex(srcEnd-1);
	System.arraycopy(buffer, srcBegin, dst, dstBegin, srcEnd-srcBegin);
}

public synchronized StringBuffer insert(int offset, Object obj)
	{
	return insert(offset, String.valueOf(obj));
}

public synchronized StringBuffer insert(int offset, String str)
	{
	if ( str == null)
		str = String.valueOf( str);
	return insert(offset, str.toCharArray());
}

public StringBuffer insert(int offset, boolean b)
	{
	return insert(offset, String.valueOf(b));
}

public synchronized StringBuffer insert(int offset, char c)
	{
	return insert(offset, String.valueOf(c));
}

public synchronized StringBuffer insert(int offset, char str[])
	{
	if ((offset < 0) || (offset > used))
		throw new StringIndexOutOfBoundsException();

	ensureCapacity(used + str.length);

	// Copy buffer up to make space.
	System.arraycopy(buffer, offset, buffer, offset+str.length, used-offset);

	/* Now, copy insert into place */
	System.arraycopy(str, 0, buffer, offset, str.length);

	/* Update used count */
	used += str.length;

	return (this);
}

public StringBuffer insert(int offset, double d)
	{
	return insert(offset, String.valueOf(d));
}

public StringBuffer insert(int offset, float f)
	{
	return insert(offset, String.valueOf(f));
}

public StringBuffer insert(int offset, int i)
	{
	return insert(offset, String.valueOf(i));
}

public StringBuffer insert(int offset, long l)
	{
	return insert(offset, String.valueOf(l));
}

public int length()
	{
	return used;
}

public synchronized StringBuffer reverse()
	{
	for (int pos = used/2 - 1; pos >= 0; pos--) {
		char ch = buffer[pos];
		buffer[pos] = buffer[used-pos-1];
		buffer[used-pos-1] = ch;
	}
	return (this);
}

public synchronized void setCharAt(int index, char ch)
	{
	checkIndex(index);
	buffer[index]=ch;
}

public synchronized void setLength(int newLength)
	{
	char oldBuffer[] = buffer;
	buffer = new char[newLength];

	System.arraycopy(oldBuffer, 0, buffer, 0, newLength);

	/* Pad buffer */
	if (newLength > used) {
		for (int pos = used; pos < newLength; pos++) {
			buffer[pos]='\u0000';
		}
	}

	used = newLength;
}

public String toString()
	{
	char buf[] = new char[used];
	if (used > 0) {
		getChars(0, used, buf, 0);
	}
	return new String(buf);
}
}
