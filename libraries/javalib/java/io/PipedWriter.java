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

public class PipedWriter extends Writer {

  PipedReader rd = null;

  public PipedWriter()
  {
  }

  public PipedWriter(PipedReader sink) throws IOException
  {
    connect(sink);
  }

  public void connect(PipedReader sink) throws IOException
  {
    sink.connect(this);
  }

  public void write(char cbuf[], int off, int len) throws IOException
  {
    PipedReader reader = rd;
    if (reader == null) {
      throw new IOException("not connected");
    }
    reader.write(cbuf, off, len);
  }

  public void flush() throws IOException
  {
    // Does nothing.
  }

  public void close() throws IOException
  {
    PipedReader reader = rd;
    if (reader != null) {
      reader.close();
      rd = null;
    }
  }

}
