/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.io.PrintStream;
import java.io.OutputStream;

public class LogStream
	extends PrintStream {

public static final int SILENT = 0;
public static final int BRIEF = 1;
public static final int VERBOSE = 2;

private static PrintStream defStream;

private LogStream(OutputStream s) {
	super(s);
}

public static LogStream log(String name) {
	throw new kaffe.util.NotImplemented();
}

public static PrintStream getDefaultStream() {
	return (defStream);
}

public static void setDefaultStream(PrintStream s) {
	defStream = s;
}

public OutputStream getOutputStream() {
	return (out);
}

public void setOutputStream(OutputStream s) {
	out = s;
}

public void write(int b) {
	super.write(b);
}

public void write(byte[] b, int off, int len) {
	super.write(b, off, len);
}

public String toString() {
	throw new kaffe.util.NotImplemented();
}

public static int parseLevel(String s) {
	if (s.equalsIgnoreCase("silent")) {
		return (SILENT);
	}
	if (s.equalsIgnoreCase("brief")) {
		return (BRIEF);
	}
	if (s.equalsIgnoreCase("verbose")) {
		return (VERBOSE);
	}
	return (SILENT);
}

}
