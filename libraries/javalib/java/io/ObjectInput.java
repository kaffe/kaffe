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

  public abstract Object readObject() throws ClassNotFoundException, IOException;
  public abstract int read() throws IOException;
  public abstract int read(byte b[]) throws IOException;
  public abstract int read(byte b[], int off, int len) throws IOException;
  public abstract long skip(long n) throws IOException;
  public abstract int available() throws IOException;
  public abstract void close() throws IOException;

};
