
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.util.jar;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.EOFException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.PushbackReader;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class Manifest implements Cloneable {
	private static final Attributes.Name NAME = new Attributes.Name("Name");
	private static final String MANIFEST_ENCODING = "8859_1";

	private Attributes main;	// main manifest attributes
	private HashMap ents;		// per-entry attributes

	public Manifest() {
		main = new Attributes();
		ents = new HashMap();
	}

	public Manifest(InputStream in) throws IOException {
		main = new Attributes();
		ents = new HashMap();
		read(in, false);
	}

	public Manifest(Manifest man) {
		main = (Attributes)man.main.clone();
		ents = (HashMap)man.ents.clone();
	}

	public Attributes getMainAttributes() {
		return main;
	}

	public Map getEntries() {
		return ents;
	}

	public Attributes getAttributes(String name) {
		return (Attributes)getEntries().get(name);
	}

	public void clear() {
		main.clear();
		ents.clear();
	}

	public void write(OutputStream outstream) throws IOException {
		PrintWriter out = new PrintWriter(new BufferedWriter(
		    new OutputStreamWriter(outstream, MANIFEST_ENCODING)));

		// Manifest-Version is mandatory and should always be first
		String mver = main.getValue(Attributes.Name.MANIFEST_VERSION);
		if (mver == null) {
			mver = "1.0";
		}
		out.println(Attributes.Name.MANIFEST_VERSION + ": " + mver);

		// Do remaining manifest attributes
		writeAttributes(out, main, Attributes.Name.MANIFEST_VERSION);
		out.println();

		// Do per-entry attributes, ``Name: '' header first
		for (Iterator i = ents.entrySet().iterator(); i.hasNext(); ) {
			Map.Entry e = (Map.Entry)i.next();
			String name = (String)e.getKey();
			Attributes attr = (Attributes)e.getValue();

			out.println("Name: " + attr.getValue(NAME));
			writeAttributes(out, attr, NAME);
			out.println();
		}
		out.flush();
	}

	private void writeAttributes(PrintWriter out,
			Attributes attr, Attributes.Name avoid) {
		for (Iterator i = attr.entrySet().iterator(); i.hasNext(); ) {
			Map.Entry e = (Map.Entry)i.next();
			Attributes.Name name = (Attributes.Name)e.getKey();
			String value = (String)e.getValue();

			if (name.equals(avoid)) {
				continue;
			}
			out.println(name + ": " + value);
		}
	}

	public void read(InputStream in) throws IOException {
		read(in, true);
	}

	private void read(InputStream in, boolean merge) throws IOException {

		// Parsing state
		final int ST_START = 0;		// start of line
		final int ST_NAME = 1;		// reading name
		final int ST_VALUE = 2;		// reading value
		final int ST_EOL = 3;		// after newine, middle of block
		int state = ST_START;		// parsing state
		int numChunks = 0;		// number of chunks read
		int lineno = 1;			// input line number

		// Input
		PushbackReader p = new PushbackReader(new BufferedReader(
		    new InputStreamReader(in, MANIFEST_ENCODING)));

		// Parse input until we reach EOF
		try {

		    // Parse each chunk of headers (ie, attributes block)
		    while (true) {
			Attributes attr = new Attributes();
			StringBuffer nbuf = null;
			StringBuffer vbuf = null;
getAttr:
			// Read and parse one character at a time
			while (true) {

			    // Get next character
			    int c = p.read();
			    if (c == -1) {
				if (state != ST_EOL) {
				    throw new EOFException();
				}
			    }

			    // Map all EOL possibilities to '\n'
			    if (c == '\r') {
				c = p.read();
				if (c != -1 && c != '\n') {
				    p.unread(c);
				}
				c = '\n';
			    }
			    if (c == '\n') {
				lineno++;
			    }

			    // Update state
			    switch (state) {
			    case ST_START:
				if (c == '\n') {
				    if (attr.size() == 0) {
					continue;	// initial blank line
				    } else {
					break getAttr;	// end of block
				    }
				}
				if (!((c >= '0' && c <= '9') ||
				      (c >= 'A' && c <= 'Z') ||
				      (c >= 'a' && c <= 'z'))) {
				    throw new IOException(
					"invalid manifest, line " + lineno);
				}
				nbuf = new StringBuffer();
				nbuf.append((char)c);
				state = ST_NAME;
				break;

			    case ST_NAME:
				if (c == ':') {
				    c = p.read();
				    if (c != ' ') {
					throw new IOException(
					    "invalid manifest, line " + lineno);
				    }
				    vbuf = new StringBuffer();
				    state = ST_VALUE;
				    break;
				}
				if (!((c >= '0' && c <= '9') ||
				      (c >= 'A' && c <= 'Z') ||
				      (c >= 'a' && c <= 'z') ||
				       c == '_' || c == '-')) {
				    throw new IOException(
					"invalid manifest, line " + lineno);
				}
				nbuf.append((char)c);
				break;

			    case ST_VALUE:
				if (c == '\n') {
				    state = ST_EOL;
				    break;
				}
				vbuf.append((char)c);
				break;

			    case ST_EOL:
				if (c == ' ' || c == '\t') {	// continuation
				    state = ST_VALUE;
				    break;
				}
				attr.putValue(nbuf.toString(), vbuf.toString());
				state = ST_START;
				if (c == -1) {
				    // process last attributes
				    break getAttr;
				}
				else {
				    p.unread(c);
				}
				break;
			    }
			}

			// Now set/merge in the attributes we just read
			if (numChunks++ == 0) {
			    if (merge) {
			    	main.putAll(attr);
			    } else {
				main = attr;
			    }
			} else {
			    String name = attr.getValue(NAME);
			    if (name == null) {
				throw new IOException("invalid manifest, line "
				  + lineno + ": missing ``Name'' header");
			    }
			    Attributes attr0 = (Attributes)ents.get(name);
			    if (attr0 == null) {
				ents.put(name, attr);
			    } else if (merge) {
				attr0.putAll(attr);
			    }
			}
		    }

		// Jump here when we get EOF
		} catch (EOFException e) {
		    if (numChunks == 0 || state != ST_START) {
			throw new IOException("premature EOF, line " + lineno);
		    }
		}
	}

	public boolean equals(Object o) {
		return (o instanceof Manifest)
		    && main.equals(((Manifest)o).main)
		    && ents.equals(((Manifest)o).ents);
	}

	public int hashCode() {
		return main.hashCode() ^ ents.hashCode();
	}

	public Object clone() {
		return new Manifest(this);
	}
}

