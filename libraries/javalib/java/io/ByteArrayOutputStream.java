package java.io;

import kaffe.io.ByteToCharConverter;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class ByteArrayOutputStream
  extends OutputStream
{
	protected byte[] buf;
	protected int count;

public ByteArrayOutputStream()
	{
	this(32);
}

public ByteArrayOutputStream(int size)
	{
	buf = new byte[size];
	count = 0;
}

private void ensureCapacity ( int size ) {
	if ( size > buf.length ) {
		byte oldBuf[] = buf;
		buf = new byte[size + 32];
		System.arraycopy( oldBuf, 0, buf, 0, oldBuf.length);
	}
}

public synchronized void reset()
	{
	count = 0;
}

public int size()
	{
	return (count);
}

public synchronized byte[] toByteArray()
	{
	byte result[] = new byte[count];
	System.arraycopy(buf, 0, result, 0, count);
	return (result);
}

public String toString () {
	return (toString(ByteToCharConverter.getDefault()));
}

private String toString ( ByteToCharConverter encoding ) {
	// this is still bad - preferrably, we should be able to create a 
	// String (just internally) without any temporary buffer. Otherwise,
	// the buffer should be a least static

	int     n = encoding.getNumberOfChars( buf, 0, count);
	char[]  cBuf = new char[n];
	
	encoding.convert( buf, 0, count, cBuf, 0, cBuf.length);
	return new String( cBuf);
}

public String toString(String enc) throws UnsupportedEncodingException
{
	return (toString(ByteToCharConverter.getConverter(enc)));
}

/**
 * @deprecated
 */
public String toString(int hibyte) {
	return new String(buf, hibyte, 0, count);
}

public synchronized void write ( byte b[], int off, int len ) {
	ensureCapacity(count + len);
	System.arraycopy(b, off, buf, count, len);
	count += len;
}

public synchronized void write(int b)
	{
	ensureCapacity(count+1);
	buf[count++]=(byte)b;
}

public synchronized void writeTo(OutputStream out) throws IOException
{
	out.write(buf, 0, count);
}
}
