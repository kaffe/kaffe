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

public abstract class FilterReader extends Reader {

  protected Reader in;

  protected FilterReader(Reader i)
  {
    in = i;
  }

  public int read() throws IOException
  {
    return (in.read());
  }

  public int read(char cbuf[], int off, int len) throws IOException
  {
    return (in.read(cbuf, off, len));
  }

  public long skip(long n) throws IOException
  {
    return (in.skip(n));
  }

  public boolean ready() throws IOException
  {
    return (in.ready());
  }

  public boolean markSupported()
  {
    return (in.markSupported());
  }

  public void mark(int readAheadLimit) throws IOException
  {
    in.mark(readAheadLimit);
  }

  public void reset() throws IOException
  {
    in.reset();
  }

  public void close() throws IOException
  {
    in.close();
  }

}
