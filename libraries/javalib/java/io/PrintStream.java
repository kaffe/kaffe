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
public class PrintStream extends FilterOutputStream
{
	private static final char[] newline =
			System.getProperty("line.separator").toCharArray();
	private final boolean autoFlush;
	private boolean error = false;

public PrintStream(OutputStream out) {
	this(out, false);
}

public PrintStream(OutputStream out, boolean autoFlush) {
	if (out == null) {
		throw new NullPointerException();
	}
	super(out);
	this.autoFlush = autoFlush;
}

public boolean checkError() {
	flush();
	return error;
}

protected void setError()
{
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
	if (x == '\n' && autoFlush)
		flush();
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
	if (autoFlush)
		flush();
}

public void print(boolean x) {
	print(String.valueOf(x).toCharArray());
}

public void print(char x) {
	print(new char[] { x });
}

public void print(int x) {
	print(String.valueOf(x).toCharArray());
}

public void print(long x) {
	print(String.valueOf(x).toCharArray());
}

public void print(float x) {
	print(String.valueOf(x).toCharArray());
}

public void print(double x) {
	print(String.valueOf(x).toCharArray());
}

public void print(char x[]) {
	for (int k = 0; k < x.length; k++)
		write((int) x[k]);	// this is our default char encoding
	// technically not to spec, but it appears to be what Sun does
	if (autoFlush) {
		flush();
	}
}

public void print(String x) {
	print((x != null ? x : "null").toCharArray());
}

public void print(Object x) {
	print(String.valueOf(x).toCharArray());
}

public void println()
{
	print(newline);
	if (autoFlush) {
		flush();
	}
}

public void println(boolean x)
{
	print(x);
	println();
}

public void println(char x)
{
	print(x);
	println();
}

public void println(int x)
{
	print(x);
	println();
}

public void println(long x)
{
	print(x);
	println();
}

public void println(float x)
{
	print(x);
	println();
}

public void println(double x)
{
	print(x);
	println();
}

public void println(char[] x)
{
	print(x);
	println();
}

public void println(String x)
{
	print(x);
	println();
}

public void println(Object x)
{
	print(x);
	println();
}

}
