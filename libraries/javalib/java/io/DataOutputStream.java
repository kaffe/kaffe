package java.io;

import java.lang.String;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class DataOutputStream
  extends FilterOutputStream
  implements DataOutput
{
	protected int written;

public DataOutputStream(OutputStream out) {
	super(out);
}

public void flush() throws IOException {
	super.flush();
}

final public int size() {
	return written;
}

public synchronized void write(byte b[], int off, int len) throws IOException {
	super.write(b, off, len);
	written += len;
}

public synchronized void write(int b) throws IOException {
	super.write(b);
	written++;		
}

final public void writeBoolean(boolean v) throws IOException {
	if (v==true) writeByte(1); else writeByte(0);
}

final public void writeByte(int v) throws IOException {
	write(v);
}

final public void writeBytes(String s) throws IOException {
	char[] c = s.toCharArray();
	byte[] b = new byte[c.length];
	for (int pos = 0; pos < c.length; pos++)
		b[pos] = (byte)(c[pos] & 0xFF);
	write(b, 0, b.length);
}

final public void writeChar(int v) throws IOException {
	writeByte((v & 0xFF00) >> 8);
	writeByte((v & 0x00FF));		
}

final public void writeChars(String s) throws IOException {
	for (int pos=0; pos<s.length(); pos++) {
		writeChar(s.charAt(pos));
	}
}

final public void writeDouble(double v) throws IOException {
	writeLong(Double.doubleToLongBits(v));
}

final public void writeFloat(float v) throws IOException {
	writeInt(Float.floatToIntBits(v));
}

final public void writeInt(int v) throws IOException {
	byte b[] = new byte[4];
	int i, shift;

	for (i = 0, shift = 24; i < 4; i++, shift -= 8)
		b[i] = (byte)(0xFF & (v >> shift));
	write(b, 0, 4);
}

final public void writeLong(long v) throws IOException {
	int hiInt=(int )(v >> 32);
	int loInt=(int )(v & 0xFFFFFFFF);

	writeInt(hiInt);
	writeInt(loInt);
}

final public void writeShort(int v) throws IOException {
	writeChar(v);
}

final public void writeUTF(String str) throws IOException {
	char c[] = str.toCharArray();
	ByteArrayOutputStream b = new ByteArrayOutputStream(c.length);
	for (int i = 0; i < c.length; i++) {
		char chr = c[i];

		if (chr >= '\u0001' && chr <= '\u007F')
			b.write(chr);
		else if (chr <= '\u07FF') {
			b.write(0xC0 | (0x3F & (chr >> 6)));
			b.write(0x80 | (0x3F & chr));
		}
		else {
			b.write(0xE0 | (0x0F & (chr >> 12)));
			b.write(0x80 | (0x3F & (chr >>  6)));
			b.write(0x80 | (0x3F & chr));
		}
	}
	c = null;

	int len = b.size();
	if (len > 65535) {
		throw new UTFDataFormatException("String too long");
	}
	synchronized(this) {
		writeShort(len);
		b.writeTo(this);
	}
}
}
