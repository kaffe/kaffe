
/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.PushbackInputStream;
import java.io.Writer;
import java.lang.String;
import java.lang.System;

public class Properties extends Hashtable {
	private static final long serialVersionUID = 4112578634029874840L;
	protected Properties defaults;
	private StringBuffer key, value;

public Properties() {
	this(null);
}

public Properties(Properties defaults) {
	this.defaults=defaults;
}

private static String escape(String str, boolean isKey) {
	StringBuffer sb = new StringBuffer();
	for (int pos = 0; pos < str.length(); pos++) {
		char ch = str.charAt(pos);
		switch (ch) {
		case '!':
		case '#':
			if (pos == 0)
				sb.append("\\");
			sb.append(ch);
			break;
		case ':':
		case '=':
		case ' ':
			if (!isKey) {
				sb.append(ch);
				break;
			}			// fall through
		case '\\':
			sb.append("\\" + ch);
			break;
		case '\n':
			sb.append("\\n");
			break;
		case '\t':
			sb.append("\\t");
			break;
		case '\r':
			sb.append("\\r");
			break;
		default:
			if (ch > 0x7e) {
				sb.append("\\\\u");
				sb.append(Character.forDigit((ch >> 12) & 0xf, 16));
				sb.append(Character.forDigit((ch >> 8) & 0xf, 16));
				sb.append(Character.forDigit((ch >> 4) & 0xf, 16));
				sb.append(Character.forDigit(ch & 0xf, 16));
			} else
				sb.append(ch);
			break;
		}
	}

	return sb.toString();
}

public String getProperty(String key) {
	// Apparently we should use the superclass get method rather than
	// our own because it may be overridden
	// Software: HotJava
  // if (System.out != null) System.out.println("getProperty: " + key);
	Object propSearch = super.get(key);
	if (propSearch != null) {
		return ((String)propSearch);
	}
	else if (defaults != null) {
		return (defaults.getProperty(key));
	}
	else {
		return (null);
	}
}

public String getProperty(String key, String defaultValue) {
	String result=getProperty(key);

	if (result==null) return defaultValue; else return result;
}

public Object setProperty(String key, String value) {
	return super.put(key, value);
}

public void list(PrintStream out) {
	list(new PrintWriter(out, true));
}

public void list(PrintWriter out) {
	try {
		save(out, "Properties list");
	} catch (IOException _) { 
		System.err.println("unable to list properties");
	}
}

public Enumeration propertyNames() {
	final Vector result = new Vector();

	// Add main properties
	for (Enumeration e = keys(); e.hasMoreElements(); ) {
		result.addElement(e.nextElement());
	}

	// Add non-overridden default properties
	if (defaults != null) {
		for (Enumeration e = defaults.keys(); e.hasMoreElements(); ) {
			Object def = e.nextElement();
			if (!result.contains(def))
				result.addElement(def);
		}
	}

	// Return enumeration of vector
	return new Enumeration() {
		private int posn = 0;
		public boolean hasMoreElements() {
			return (posn < result.size());
		}
		public Object nextElement() {
			if (posn == result.size())
				throw new NoSuchElementException();
			return result.elementAt(posn++);
		}
	} // not unreachable
	;
}

public synchronized void load(InputStream in) throws IOException {
	PushbackInputStream pin = new PushbackInputStream(in, 16);

	while (readKeyAndValue(pin)) {
		put(key.toString(), value.toString());
	}
	key = null;
	value = null;
	//pin.close(); ??
}

private boolean readKeyAndValue(PushbackInputStream in) throws IOException {
	int ch;

	while (true) {
		// Eat initial white space
		while ((ch = in.read()) != -1 && ch <= ' ');

		// Skip comments
		switch (ch) {
			case '#':
			case '!':
				while ((ch = in.read()) != '\n');
				continue;
			case -1:
				return false;
		}

		// Initialize
		this.key = new StringBuffer();
		this.value = new StringBuffer();

		// Read in key
		boolean eatSeparator = false;
getKey:		while (true) {
			switch (ch) {
			case '=':
			case ':':
				break getKey;
			case '\r':
				switch ((ch = in.read())) {
				case '\n':
					break;
				case -1:
					return true;
				default:
					in.unread(ch);
					break getKey;
				}				// fall through
			case -1:
			case '\n':
				return true;
			default:
				if (ch <= ' ') {
					eatSeparator = true;
					break getKey;
				}
				in.unread(ch);
				key.append((char) getEscapedChar(in));
			}
			ch = in.read();
		}

		// Eat white space (and separator, if expecting) before value
		while (true) {
			while ((ch = in.read()) <= ' ') {
				if (ch == -1 || ch == '\n')
					return true;
			}
			if (eatSeparator && (ch == '=' || ch == ':')) {
				eatSeparator = false;
			} else {
				break;
			}
		}

		// Read in value
		while (true) {
			switch (ch) {
			case '\r':
				switch ((ch = in.read())) {
				case -1:
					value.append('\r');	// fall through
				case '\n':
					return true;
				default:
					in.unread(ch);
					value.append('\r');
					break;
				}
				break;
			case -1:
			case '\n':
				return true;
			default:
				in.unread(ch);
				value.append((char) getEscapedChar(in));
				break;
			}
			ch = in.read();
		}
	}
}

// Get next char, respecting backslash escape codes and end-of-line stuff
private static int getEscapedChar(PushbackInputStream in) throws IOException {
	int ch;
	switch ((ch = in.read())) {
	case '\\':
		switch ((ch = in.read())) {
		case '\r':
			switch ((ch = in.read())) {
			default:
				in.unread(ch);		// fall through
			case -1:
				in.unread('\r');
				return '\\';
			case '\n':
				break;
			}				// fall through
		case '\n':
			while ((ch = in.read()) != -1 && ch <= ' ');
			return ch;
		case 'n':
			return '\n';
		case 'r':
			return '\r';
		case 't':
			return '\t';
		case '\\':
			switch ((ch = in.read())) {
			case 'u':
				int[] dig = new int[4];
				int n;
			    getUnicode:
				{
					int dval, cval = 0;
					for (n = 0; n < 4; ) {
						if ((dig[n] = in.read()) == -1)
							break getUnicode;
						if ((dval = Character.digit(
						    (char) dig[n++], 16)) == -1)
							break getUnicode;
						cval = (cval << 4) | dval;
					}
					return cval;
				}
				// not unreachable, break getUnicode go here
				while (n > 0)
				    in.unread(dig[--n]);	// fall through
			default:
				in.unread(ch);
				return '\\';
			}

		// not fall through, previous switch always return
		case -1:
			return '\\';
		default:
			return ch;
		}
	// not fall through, previous switch always return
	case '\r':
		switch ((ch = in.read())) {
		default:
			in.unread(ch);		// fall through
		case -1:
			return '\r';
		case '\n':
			break;
		}				// fall through
	default:
		return ch;
	}
}

public synchronized void save(OutputStream out, String header) {
	try {
		store(out, header);
	}
	catch (IOException e) {
		System.err.println("Unable to save properties: "+header);
	}
}

public synchronized void store(OutputStream out, String header) throws IOException {
	save(new PrintWriter(out, true), header);
}

// NB: use a PrintWriter here to get platform-specific line separator
private synchronized void save(PrintWriter out, String header) throws IOException {
	if (header != null) {
		out.println("# " + escape(header, false));
	}

	Enumeration keys = propertyNames();

	while (keys.hasMoreElements()) {
		String key=(String)keys.nextElement();
		out.println(escape(key, true)
			+ "=" + escape(getProperty(key), false));
	}
	out.flush();	// shouldn't be necessary
}
}

