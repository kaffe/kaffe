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
public class PrintStream
  extends FilterOutputStream
{
	final private static char[] newline = System.getProperty("line.separator").toCharArray();
	final private boolean autoFlush;
	private boolean error = false;

public PrintStream(OutputStream out) {
	super(out);
	this.autoFlush = false;
	if (out == null) {
		throw new NullPointerException();
	}
}

public PrintStream(OutputStream out, boolean autoFlush) {
	super(out);
	this.autoFlush = autoFlush;
	if (out == null) {
		throw new NullPointerException();
	}
}

public boolean checkError() {
	flush();
	return error;
}

protected void setError() {
	error = true;
}

public void close() {
	try {
		flush();
		out.close();
	}
	catch (InterruptedIOException _) {
		Thread.currentThread().interrupt();
	}
	catch (IOException _) {
		error = true;
	}
}

public void flush() {
	try {
		out.flush();
	}
	catch (InterruptedIOException _) {
		Thread.currentThread().interrupt();
	}
	catch (IOException _) {
		error = true;
	}
}

public void print(Object x) {
	print(String.valueOf(x).getBytes());
}

public void print(String x) {
	print((x != null ? x : "null").getBytes());
}

public void print(boolean x) {
	print(String.valueOf(x).getBytes());
}

public void print(char x) {
	print(String.valueOf(x).getBytes());
}

public void print(char x[]) {
	print(String.valueOf(x).getBytes());
}

public void print(double x) {
	print(String.valueOf(x).getBytes());
}

public void print(float x) {
	print(String.valueOf(x).getBytes());
}

public void print(int x) {
	print(String.valueOf(x).getBytes());
}

public void print(long x) {
	print(String.valueOf(x).getBytes());
}

public void println() {
	print(newline);
	if (autoFlush) {
		flush();
	}
}

public void println(Object x) {
	synchronized(this) {
		print(x);
		println();
	}
}

public void println(String x) {
	synchronized(this) {
		print(x);
		println();
	}
}

public void println(boolean x) {
	synchronized(this) {
		print(x);
		println();
	}
}

public void println(char x) {
	synchronized(this) {
		print(x);
		println();
	}
}

public void println(char[] x) {
	synchronized(this) {
		print(x);
		println();
	}
}

public void println(double x) {
	synchronized(this) {
		print(x);
		println();
	}
}

public void println(float x) {
	synchronized(this) {
		print(x);
		println();
	}
}

public void println(int x) {
	synchronized(this) {
		print(x);
		println();
	}
}

public void println(long x) {
	synchronized(this) {
		print(x);
		println();
	}
}

public void write(byte b[], int off, int len) {
	try {
		out.write(b, off, len);
	}
	catch (InterruptedIOException _) {
		Thread.currentThread().interrupt();
	}
	catch (IOException _) {
		error = true;
	}
	if (autoFlush) {
		flush();
	}
}

public void write(int x) {
	try {
		out.write(x);
	}
	catch (InterruptedIOException _) {
		Thread.currentThread().interrupt();
	}
	catch (IOException _) {
		error = true;
	}
	if (x == '\n' && autoFlush) {
		flush();
	}
}

private void print(byte[] b) {
	write(b, 0, b.length);
	if (autoFlush) {	// This is not required by the spec,
	       flush();		//   but it appears to be what Sun does.
	}
}

}
