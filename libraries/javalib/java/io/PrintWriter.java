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


public class PrintWriter extends Writer {

  private static final String newline = System.getProperty("line.separator");

  protected Writer out;
  private boolean flsh;
  private boolean error;

  public PrintWriter(Writer out)
  {
    this(out, false);
  }

  public PrintWriter(Writer out, boolean autoFlush)
  {
    super(out);
    this.out = out;
    flsh = autoFlush;
  }

  public PrintWriter(OutputStream out)
  {
    this(out, false);
  }

  public PrintWriter(OutputStream out, boolean autoFlush)
  {
    this(new OutputStreamWriter(out), autoFlush);
  }

  public void flush()
  {
    synchronized(lock) {
      if (isStillOpen()) {
	try {
	  out.flush();
	}
	catch (IOException _) {
	  error = true;
	}
      }
    }
  }

  public void close()
  {
    synchronized(lock) {
      if (isStillOpen()) {
	try {
	  out.close();
	}
	catch (IOException _) {
	  error = true;
	}
	out = null;
      }
    }
  }

  public boolean checkError()
  {
    synchronized(lock) {
      flush();
      return (error);
    }
  }

  /* returns true if the Writer is still open */
  private boolean isStillOpen() {
    return out != null;
  }

  protected void setError()
  {
    synchronized(lock) {
      error = true;
    }
  }

  public void write(int c)
  {
    try {
      super.write(c);
    }
    catch (IOException e) {
      /* can't happen */
    }
  }

  public void write(char buf[], int off, int len)
  {
    if (len < 0 || off < 0 || off + len > buf.length) {
      throw new IndexOutOfBoundsException();
    }

    synchronized(lock) {
      if (isStillOpen()) {
	try {
	  out.write(buf, off, len);
	}
	catch (IOException _) {
	  setError();
	}
      }
    }
  }

  public void write(char buf[])
  {
    write(buf, 0, buf.length);
  }
      
  public void write(String s, int off, int len)
  {
    write(s.toCharArray(), off, len);
  }

  public void write(String s)
  {
    write(s.toCharArray(), 0, s.length());
  }

  public void print(boolean b)
  {
    write(String.valueOf(b));
  }

  public void print(char c)
  {
    write(String.valueOf(c));
  }

  public void print(int i)
  {
    write(String.valueOf(i));
  }

  public void print(long l)
  {
    write(String.valueOf(l));
  }

  public void print(float f)
  {
    write(String.valueOf(f));
  }

  public void print(double d)
  {
    write(String.valueOf(d));
  }

  public void print(char s[])
  {
    write(String.valueOf(s));
  }

  public void print(String s)
  {
    /* use String.valueOf here to handle s == null in a single place */
    write(String.valueOf(s));
  }

  public void print(Object obj)
  {
    print(String.valueOf(obj));
  }

  public void println()
  {
    synchronized(lock) {
      write(newline);
      if (flsh) {
	flush();
      }
    }
  }

  public void println(boolean x)
  {
    println(String.valueOf(x));
  }

  public void println(char x)
  {
    println(String.valueOf(x));
  }

  public void println(int x)
  {
    println(String.valueOf(x));
  }

  public void println(long x)
  {
    println(String.valueOf(x));
  }

  public void println(float x)
  {
    println(String.valueOf(x));
  }

  public void println(double x)
  {
    println(String.valueOf(x));
  }

  public void println(char x[])
  {
    println(String.valueOf(x));
  }

  public void println(String x)
  {
    synchronized(lock) {
      print(x);
      println();
    }
  }

  public void println(Object x)
  {
    println(String.valueOf(x));
  }

}
