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

  public abstract void writeObject(Object obj) throws IOException;
  public abstract void write(int b) throws IOException;
  public abstract void write(byte b[]) throws IOException;
  public abstract void write(byte b[], int off, int len) throws IOException;
  public abstract void flush() throws IOException;
  public abstract void close() throws IOException;

};
