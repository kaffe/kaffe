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

public interface ObjectOutput extends DataOutput {

void writeObject(Object obj) throws IOException;
void write(int b) throws IOException;
void write(byte b[]) throws IOException;
void write(byte b[], int off, int len) throws IOException;
void flush() throws IOException;
void close() throws IOException;

}
