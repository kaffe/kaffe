/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.servlet;

import java.io.OutputStream;
import java.io.IOException;

public abstract class ServletOutputStream
  extends OutputStream {

private static final String newline = System.getProperty("line.separator");

protected ServletOutputStream() {
}

public void print(String s) throws IOException {
	write(s.getBytes());
}

public void print(boolean b) throws IOException {
	print(String.valueOf(b));
}

public void print(char c) throws IOException {
	print(String.valueOf(c));
}

public void print(int i) throws IOException {
	print(String.valueOf(i));
}

public void print(long l) throws IOException {
	print(String.valueOf(l));
}

public void print(float f) throws IOException {
	print(String.valueOf(f));
}

public void print(double d) throws IOException {
	print(String.valueOf(d));
}

public void println() throws IOException {
	print(newline);
	flush();
}

public void println(String s) throws IOException {
	print(s);
	println();
}

public void println(boolean b) throws IOException {
	print(b);
	println();
}

public void println(char c) throws IOException {
	print(c);
	println();
}

public void println(int i) throws IOException {
	print(i);
	println();
}

public void println(long l) throws IOException {
	print(l);
	println();
}

public void println(float f) throws IOException {
	print(f);
	println();
}

public void println(double d) throws IOException {
	print(d);
	println();
}

}
