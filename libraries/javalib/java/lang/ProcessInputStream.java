package java.lang;

import java.io.DataInput;
import java.io.EOFException;
import java.io.FilterInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UTFDataFormatException;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class ProcessInputStream
  extends FilterInputStream
  implements DataInput
{
public ProcessInputStream(InputStream in) {
	super(in);
}

final public int read(byte b[]) throws IOException {
	return read(b, 0, b.length);
}

final public int read(byte b[], int off, int len) throws IOException {
	return super.read(b, off, len);
}

final public boolean readBoolean() throws IOException {
	return (readByte()!=0);
}

final public byte readByte() throws IOException {
	int value=super.read();

	if (value==-1) throw new EOFException();

	return (byte )value;
}

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
	int b1=readUnsignedByte();
	int b2=readUnsignedByte();
	int b3=readUnsignedByte();
	int b4=readUnsignedByte();

	int temp=(b1 << 24) | (b2 << 16) | (b3 << 8) | b4;

	return temp;
}

final public String readLine() throws IOException {
	boolean EOL=false;
	StringBuffer buffer=new StringBuffer();

	while (!EOL) {
		int data=super.read();

		if (data==-1) {
			EOL=true;
		}
		else {
			buffer.append((byte )data);
			if ((char )data=='\n') EOL=true;
			if ((char )data=='\r') {
				/* Check for "\r\n" */
				super.mark(1);
				int nextByte=super.read();

				if (nextByte==-1) EOL=true;
				else if ((char )nextByte=='\n') {
					buffer.append((byte )nextByte);
					EOL=true;
				}
				else {
					/* Jump back to mark */
					reset();
				}
			}
		}
	}

	return buffer.toString();
}

final public long readLong() throws IOException {
	int i1=readInt(); /* b1-4 */
	int i2=readInt(); /* b5-8 */

	return (((long)i1) << 32) | (((long)i2) & 0xFFFFFFFF);
}

final public short readShort() throws IOException {
	int b1=readUnsignedByte();
	int b2=readUnsignedByte();

	return (short)((b1 << 8)|b2);		
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

				if ((data2 & 0xC0) == 0xF0) {
					/* Valid 2nd byte */
					char toAdd=(char )((((int )(data & 0x1F)) << 6) + (data2 & 0x3F));
					buffer.append(toAdd);
					pos=pos+2;
				}
				else throw new UTFDataFormatException();
			} else if ((data & 0xF0)==0xE0) {
				/* Valid 3 byte string '1110' */
				byte data2=in.readByte();

				if ((data2 & 0xC0) == 0xF0) {
					/* Valid 2nd byte */
					byte data3=in.readByte();

					if ((data3 & 0xC0) == 0xF0) {
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
	int value=super.read();

	if (value==-1) throw new EOFException();
	return (value & 0xFF);
}

final public int readUnsignedShort() throws IOException {
	int b1=readUnsignedByte();
	int b2=readUnsignedByte();

	return (int )(b1 << 8) | b2;
}

final public int skipBytes(int n) throws IOException {
	long temp = super.skip((long)n);
	int skipped = (int)temp;

	if (skipped != n) {
		throw new EOFException();
	}

	return skipped;
}
}
