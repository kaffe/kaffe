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

public abstract class FilterWriter extends Writer {

  protected Writer out;

  protected FilterWriter(Writer o)
  {
    out = o;
  }

  public void write(int c) throws IOException
  {
    out.write(c);
  }

  public void write(char cbuf[], int off, int len) throws IOException
  {
    out.write(cbuf, off, len);
  }

  public void write(String str, int off, int len) throws IOException
  {
    out.write(str, off, len);
  }

  public void flush() throws IOException
  {
    out.flush();
  }

  public void close() throws IOException
  {
    out.close();
  }

}
