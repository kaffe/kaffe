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

public class DataInputStream
  extends FilterInputStream
  implements DataInput
{
public DataInputStream(InputStream in) {
	super(in);
}

final public int read(byte b[]) throws IOException {
	return read(b, 0, b.length);
}

final public int read(byte b[], int off, int len) throws IOException {
	return in.read(b, off, len);
}

final public boolean readBoolean() throws IOException {
	return (readUnsignedByte() != 0);
}

final public byte readByte() throws IOException {
	int value = in.read();
	if (value == -1) {
		throw new EOFException();
	}
	return ((byte)value);
}

final public char readChar() throws IOException {
	int val = in.read() << 8;
	val |= in.read();
	if (val == -1) {
		throw new EOFException();
	}
	return ((char)val);
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
	int total = 0;
	while (total < len) {
		int got = read(b, off + total, len + total);
		if (got == -1) {
			throw new EOFException();
		}
		total += got;
	}
}

final public int readInt() throws IOException {
	int v1 = in.read() << 24;
	v1 |= in.read() << 16;
	v1 |= in.read() << 8;
	int v2 = in.read();
	if (v2 == -1) {
		throw new EOFException();
	}
	return (v1 | v2);
}

/**
 * @deprecated
 */
final public String readLine() throws IOException {
	final StringBuffer buffer = new StringBuffer();
	int nread = 0;

	while (true) {
		final int data = super.read();
		final char ch = (char) (data & 0xff);

		if (data == -1) {
			break;
		}
		nread++;

		if (ch == '\n') {
			break;
		}

		if (ch == '\r') {       // Check for '\r\n'
			int data2;

			// Note that we don't know whether the InputStream
			// implements mark() and reset(), but we're using
			// them anyway. If they don't, then characters
			// after a lone '\r' will be elided from the input
			// (ie, this is a bug). We could override mark()
			// and reset() to always provide at least a
			// one-character buffer, but then we'd violate
			// the spec, which says to inherit these from
			// FilterInputStream...

			super.mark(1);
			data2 = super.read();
			if (data2 != -1 && (char) (data & 0xff) != '\n') {
				try {
					super.reset();
				} catch (IOException e) { }
			}
			break;
		}
		buffer.append(ch);
	}
	return (nread == 0) ? null : buffer.toString();
}

final public long readLong() throws IOException {
	long v1 = (long)in.read() << 56;
	v1 |= (long)in.read() << 48;
	v1 |= (long)in.read() << 40;
	v1 |= (long)in.read() << 32;
	v1 |= (long)in.read() << 24;
	v1 |= (long)in.read() << 16;
	v1 |= (long)in.read() << 8;
	int v2 = in.read();
	if (v2 == -1) {
		throw new EOFException();
	}
	return (v1 | (long)v2);
}

final public short readShort() throws IOException {
	int val = in.read() << 8;
	val |= in.read();
	if (val == -1) {
		throw new EOFException();
	}
	return ((short)val);
}

final public String readUTF() throws IOException {
	return readUTF(this);
}

final public static synchronized String readUTF(DataInput in) throws IOException {
	int length=in.readUnsignedShort();
	StringBuffer buffer=new StringBuffer();

	int pos=0;
	while (pos<length) {
		int data=in.readUnsignedByte();

		if ((data & 0x80)==0x80) {
			/* Hi-bit set, multi byte char */
			if ((data & 0xE0)==0xC0) {
				/* Valid 2 byte string '110' */
				byte data2=in.readByte();

				if ((data2 & 0xC0) == 0x80) {
					/* Valid 2nd byte */
					char toAdd=(char )((((int )(data & 0x1F)) << 6) + (data2 & 0x3F));
					buffer.append(toAdd);
					pos=pos+2;
				}
				else throw new UTFDataFormatException();
			} else if ((data & 0xF0)==0xE0) {
				/* Valid 3 byte string '1110' */
				byte data2=in.readByte();

				if ((data2 & 0xC0) == 0x80) {
					/* Valid 2nd byte */
					byte data3=in.readByte();

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
	int value=in.read();

	if (value == -1) {
		throw new EOFException();
	}
	return (value);
}

final public int readUnsignedShort() throws IOException {
	int val = in.read() << 8;
	val |= in.read();
	if (val == -1) {
		throw new EOFException();
	}
	return (val);
}

final public int skipBytes(int n) throws IOException
{
	int skipped = (int)in.skip((long)n);
	if (skipped != n) {
		throw new EOFException();
	}
	return (skipped);
}
}
