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


public interface DataInput {

abstract public boolean readBoolean() throws IOException;
abstract public byte readByte() throws IOException;
abstract public char readChar() throws IOException;
abstract public double readDouble() throws IOException;
abstract public float readFloat() throws IOException;
abstract public void readFully(byte b[]) throws IOException;
abstract public void readFully(byte b[], int off, int len) throws IOException;
abstract public int readInt() throws IOException;
abstract public String readLine() throws IOException;
abstract public long readLong() throws IOException;
abstract public short readShort() throws IOException;
abstract public String readUTF() throws IOException;
abstract public int readUnsignedByte() throws IOException;
abstract public int readUnsignedShort() throws IOException;
abstract public int skipBytes(int n) throws IOException;

}
