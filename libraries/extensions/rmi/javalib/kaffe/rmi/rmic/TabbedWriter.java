/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package kaffe.rmi.rmic;

import java.io.FilterWriter;
import java.io.Writer;
import java.io.IOException;

public class TabbedWriter
	extends FilterWriter {

private static final String defaultTabstring = "    ";
private char[] tabstring; 
private int tabs;

public TabbedWriter(Writer o) {
	this(o, defaultTabstring);
}

public TabbedWriter(Writer o, String str) {
	super(o);
	tabs = 0;
	tabstring = str.toCharArray();
}

public void write(int c) throws IOException {
	out.write(c);
	if (c == '\n') {
		for (int i = 0; i < tabs; i++) {
			out.write(tabstring, 0, tabstring.length);
		}
	}
}

public void write(char cbuf[], int off, int len) throws IOException {
	for (int i = 0; i < len; i++) {
		write((int)cbuf[i+off]);
	}
}

public void write(String str, int off, int len) throws IOException {
	write(str.toCharArray(), off, len);
}

public void unindent() throws IOException {
	unindent(1);
}

public void unindent(int nr) throws IOException {
	indent(-nr);
}

public void indent() throws IOException {
	indent(1);
}

public void indent(int nr) throws IOException {
	tabs += nr;
	if (tabs < 0) {
		tabs = 0;
	}
	write((int)'\n');
}

}
