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

boolean readBoolean() throws IOException;
byte readByte() throws IOException;
char readChar() throws IOException;
double readDouble() throws IOException;
float readFloat() throws IOException;
void readFully(byte b[]) throws IOException;
void readFully(byte b[], int off, int len) throws IOException;
int readInt() throws IOException;
String readLine() throws IOException;
long readLong() throws IOException;
short readShort() throws IOException;
String readUTF() throws IOException;
int readUnsignedByte() throws IOException;
int readUnsignedShort() throws IOException;
int skipBytes(int n) throws IOException;

}
