package java.io;


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

private byte[] charToUTF(char chr) {
	byte result[];

	if (chr >= '\u0001' && chr <= '\u007F') {
		/* Single byte */
		result = new byte[1];
		result[0] = (byte)chr;
	}
	else if ((chr >= '\u0080' && chr <= '\u07FF') | chr == '\u0000') {
		result = new byte[2];
		result[0] = (byte)(((chr & 0x07C0) >> 6) | 0xC0);
		result[1] = (byte)((chr & 0x003F) | 0x80);
	}
	else {
		result = new byte[3];
		result[0] = (byte)(((chr & 0xF000) >> 12) | 0xE0);
		result[1] = (byte)(((chr & 0x0FC0) >> 6) | 0x80);
		result[2] = (byte)((chr & 0x003F) | 0x80);
	}

	return (result);
}

public void flush() throws IOException {
	super.flush();
}

final public int size() {
	return written;
}

public synchronized void write(byte b[], int off, int len) throws IOException {
	super.write(b, off, len);

	written = written+len;
}

public synchronized void write(int b) throws IOException {
	super.write(b);
	written++;		
}

final public void writeBoolean(boolean v) throws IOException {
	if (v==true) writeByte(1); else writeByte(0);
}

final public void writeByte(int v) throws IOException {
	super.write(v);
	written++;
}

final public void writeBytes(String s) throws IOException {
	for (int pos=0; pos<s.length(); pos++) {
		writeByte(s.charAt(pos) & 0xFF);
	}
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
	int mask=0xFF000000;

	for (int pos=3; pos>=0; pos--) {
		writeByte((v & mask) >> (pos*8));
		mask = mask >> 8;
	}
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

final public synchronized void writeUTF(String str) throws IOException {
	int len=0;

	/* Calculate length first, yes inefficient I know */
	for (int pos=0; pos<str.length(); pos++) {
		len=len+charToUTF(str.charAt(pos)).length;
	}

	/* Write it out */
	writeShort(len);

	/* Now write the same arrays out for real */
	for (int pos=0; pos<str.length(); pos++) {
		byte conv[]=charToUTF(str.charAt(pos));
		write(conv, 0, conv.length);
	}
}
}
