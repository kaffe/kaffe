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
	private PrintWriter strm;
	private boolean error;

public PrintStream(OutputStream out)
	{
	this(out, false);
}

public PrintStream(OutputStream out, boolean autoflush)
	{
	super(out);
	strm = new PrintWriter(out, autoflush);
	error = false;
}

public boolean checkError() {
	return (error || strm.checkError());
}

public void close() {
	strm.close();
}

public void flush() {
	strm.flush();
}

public void print(Object obj) {
	strm.print(obj);
}

public void print(String s) {
	strm.print(s);
}

public void print(boolean b) {
	strm.print(b);
}

public void print(char c) {
	strm.print(c);
}

public void print(char s[]) {
	strm.print(s);
}

public void print(double d) {
	strm.print(d);
}

public void print(float f) {
	strm.print(f);
}

public void print(int i) {
	strm.print(i);
}

public void print(long l) {
	strm.print(l);
}

public void println() {
	strm.println();
}

public void println(Object obj) {
	strm.println(obj);
}

public void println(String s) {
	strm.println(s);
}

public void println(boolean b) {
	strm.println(b);
}

public void println(char c) {
	strm.println(c);
}

public void println(char s[]) {
	strm.println(s);
}

public void println(double d) {
	strm.println(d);
}

public void println(float f) {
	strm.println(f);
}

public void println(int i) {
	strm.println(i);
}

public void println(long l) {
	strm.println(l);
}

public void setError()
	{
	error = true;
}

public void write(byte b[], int off, int len) {
	for (int i = 0; i < len; i++) {
		write((int)b[i+off]);
	}
}

public void write(int b) {
	strm.write(b);
}
}
