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

public interface ObjectInput extends DataInput {

  Object readObject() throws ClassNotFoundException, IOException;
  int read() throws IOException;
  int read(byte b[]) throws IOException;
  int read(byte b[], int off, int len) throws IOException;
  long skip(long n) throws IOException;
  int available() throws IOException;
  void close() throws IOException;

}
