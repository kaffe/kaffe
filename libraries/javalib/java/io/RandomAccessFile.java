/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;

import java.lang.String;

public class RandomAccessFile
  implements DataOutput, DataInput
{
	private FileDescriptor fd = new FileDescriptor();

static {
	System.loadLibrary("io");
}

public RandomAccessFile(File file, String mode) throws IOException {
	this(file.getPath(), mode);
}

public RandomAccessFile(String name, String mode) throws IOException {
	boolean writable;
	if (mode.equalsIgnoreCase("r")) writable=false;
	else if (mode.equalsIgnoreCase("rw")) writable=true;
	else throw new IllegalArgumentException();

	System.getSecurityManager().checkRead(name);
	if (writable) {
		System.getSecurityManager().checkWrite(name);
	}

	open(name, writable);
}

native public void close() throws IOException;

final public FileDescriptor getFD() throws IOException {
	return fd;
}

native public long getFilePointer() throws IOException;

native public long length() throws IOException;

native private void open(String name, boolean rw);

native public int read() throws IOException;

public int read(byte b[]) throws IOException {
	return read(b, 0, b.length);
}

public int read(byte b[], int off, int len) throws IOException {
	int total = 0;
	while (total < len) {
		int got = readBytes(b, off + total, len - total);
		if (got == -1) {
			break;
		}
		total += got;
	}
	if (len > 0 && total == 0) {
		return -1;
	}
	return total;
}

final public boolean readBoolean() throws IOException {
	return (readByte()==0);
}

final public byte readByte() throws IOException {
	int value = read();
	if (value == -1) {
		throw new EOFException();
	}
	return (byte)value;
}

native private int readBytes(byte bytes[], int off, int len);

final public char readChar() throws IOException {
	int b1=readUnsignedByte();
	int b2=readUnsignedByte();

	return (char )((b1 << 8) | b2);
}

final public double readDouble() throws IOException {
	return Double.longBitsToDouble(readLong());
}

final public float readFloat() throws IOException {
	return Float.intBitsToFloat(readInt());
}

final public void readFully(byte b[]) throws IOException {
	readFully(b, 0, b.length);
}

final public void readFully(byte b[], int off, int len) throws IOException {
	int got = read(b, off, len);
	if (got != len) {
		throw new EOFException();
	}
}

final public int readInt() throws IOException {
	int b1=readUnsignedByte();
	int b2=readUnsignedByte();
	int b3=readUnsignedByte();
	int b4=readUnsignedByte();

	return (b1 << 24) | (b2 << 16) + (b3 << 8) + b4;
}

final public String readLine() throws IOException {
	final StringBuffer buffer = new StringBuffer();
	int nread = 0;

	while (true) {
		final int data = read();
		final char ch = (char) (data & 0xff);

		if (data == -1)
			break;
		nread++;
		if (ch == '\n')
			break;
		if (ch == '\r') {       // Check for '\r\n'
			final int data2 = read();
			final char ch2 = (char) (data & 0xff);

			if (data2 != -1 && ch2 != '\n')
				seek(getFilePointer() - 1);
			break;
		}
		buffer.append(ch);
	}
	return (nread == 0) ? null : buffer.toString();
}

final public long readLong() throws IOException {    
	int i1=readInt(); /* b1-4 */
	int i2=readInt(); /* b5-8 */

	return ((long)i1 << 32) + (((long)i2) & 0xFFFFFFFF);
}

final public short readShort() throws IOException {
	int b1=readUnsignedByte();
	int b2=readUnsignedByte();

	return (short)((b1 << 8)|b2);		
}

final public String readUTF() throws IOException {
	int length=readUnsignedShort();
	StringBuffer buffer=new StringBuffer();

	int pos=0;
	while (pos<length) {
		byte data=readByte();

		if ((data & 0x80)==0x80) {
			/* Hi-bit set, multi byte char */
			if ((data & 0xE0)==0xC0) {
				/* Valid 2 byte string '110' */
				byte data2=readByte();

				if ((data2 & 0xC0) == 0x80) {
					/* Valid 2nd byte */
					char toAdd=(char )((((int )(data & 0x1F)) << 6) + (data2 & 0x3F));
					buffer.append(toAdd);
					pos=pos+2;
				}
				else throw new UTFDataFormatException();
			} else if ((data & 0xF0)==0xE0) {
				/* Valid 3 byte string '1110' */
				byte data2=readByte();

				if ((data2 & 0xC0) == 0x80) {
					/* Valid 2nd byte */
					byte data3=readByte();

					if ((data3 & 0xC0) == 0x80) {
						/* Valid 3rd byte */
						char toAdd=(char )((((int )(data & 0x0F)) << 12) + (((int )(data2 & 0x3F)) << 6)+ (data3 & 0x3F));
						buffer.append(toAdd);
						pos=pos+3;
					}
					else throw new UTFDataFormatException();
				}
				else throw new UTFDataFormatException();
			}
			else throw new UTFDataFormatException();
		}
		else {
			buffer.append((char )data);
			pos++;
		}
	}
	if (pos>length) throw new UTFDataFormatException();

	return buffer.toString();
}

final public int readUnsignedByte() throws IOException {
	return (int)readByte() & 0xFF;
}

final public int readUnsignedShort() throws IOException {
	int b1=readUnsignedByte();
	int b2=readUnsignedByte();

	return (b1 << 8) | b2;
}

native public void seek(long pos) throws IOException;

public int skipBytes(int n) throws IOException {
	long pos = getFilePointer();
	seek(pos+(long)n);
	return n;
}

public void write(byte b[]) throws IOException {
	writeBytes(b, 0, b.length);
}

public void write(byte b[], int off, int len) throws IOException {
	writeBytes(b, off, len);
}

native public void write(int b) throws IOException;

final public void writeBoolean(boolean v) throws IOException {
	if (v==true) writeByte(1); else writeByte(0);
}

final public void writeByte(int v) throws IOException {
	write(v);
}

final public void writeBytes(String s) throws IOException {
	char[] c = s.toCharArray();
	byte[] b = new byte[c.length];
	for (int pos = 0; pos < c.length; pos++) {
		b[pos] = (byte)(c[pos] & 0xFF);
	}
	write(b, 0, b.length);
}

native private void writeBytes(byte bytes[], int off, int len);

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
	byte result[] = b.toByteArray();

	if (result.length > 65535)
		throw new UTFDataFormatException("String too long");

	synchronized(this) {
		writeShort(result.length);
		write(result, 0, result.length);
	}
}
}
