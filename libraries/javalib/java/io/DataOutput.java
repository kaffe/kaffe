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
public interface DataOutput
{
abstract public void write(byte b[]) throws IOException;

abstract public void write(byte b[], int off, int len) throws IOException;

abstract public void write(int b) throws IOException;

abstract public void writeBoolean(boolean v) throws IOException;

abstract public void writeByte(int v) throws IOException;

abstract public void writeBytes(String s) throws IOException;

abstract public void writeChar(int v) throws IOException;

abstract public void writeChars(String s) throws IOException;

abstract public void writeDouble(double v) throws IOException;

abstract public void writeFloat(float v) throws IOException;

abstract public void writeInt(int v) throws IOException;

abstract public void writeLong(long v) throws IOException;

abstract public void writeShort(int v) throws IOException;

abstract public void writeUTF(String str) throws IOException;
}
