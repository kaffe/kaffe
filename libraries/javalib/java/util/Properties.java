package java.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.Writer;

public class Properties
  extends Hashtable
{
	protected Properties defaults;

public Properties() {
	this(null);
}

public Properties(Properties defaults) {
	this.defaults=defaults;
}

private String bytesToString(byte array[]) {
	return new String(array, 0);
}

private String escape(String str) {
	/* Escape line separators */
	StringBuffer sb=new StringBuffer();
	for (int pos=0; pos<str.length(); pos++) {
		char ch=str.charAt(pos);
		switch (ch) {
		case '\\':
			sb.append("\\\\");
			break;
		case '\n':
			sb.append("\\n");
			break;
		case '\r':
			sb.append("\\r");
			break;
		default:
			sb.append(ch);
		}
	}

	return sb.toString();
}

public String getProperty(String key) {
	Object propSearch=get(key);

	if (propSearch==null) { /* Not found */
		if (defaults!=null) return defaults.getProperty(key); else return null;
	}
	else {
		return (String )propSearch;
	}
}

public String getProperty(String key, String defaultValue) {
	String result=getProperty(key);

	if (result==null) return defaultValue; else return result;
}

public void list(PrintStream out) {
	list(new PrintWriter(out));
}

public void list(PrintWriter out) {
	save(out, "Properties list");
}

public synchronized void load(InputStream in) throws IOException {

	for (;;) {
		int index;
		String line = readLine(in);

		if (line == null) {
			break;
		}
		int len = line.length();
		if ( (len == 0) || line.charAt(0) == '#') {
			// Skip empty and commented lines.
		}
		else if ((index = line.indexOf('=')) > 0) {
			String key = line.substring(0, index);
			// Strip off whitespace.
			for (index++; (index < len) && Character.isSpace(line.charAt(index)); index++);
			String value = line.substring(index);
			put(key, value);
		}
		else {
			// be more deliberate, don't ignore all subsequent entries
			System.err.println("malformed property: " + line);
		}
	}
}

public Enumeration propertyNames() {
	Vector result=new Vector();

	Enumeration mainKeys=keys();
	while (mainKeys.hasMoreElements()) {
		result.addElement(mainKeys.nextElement());
	}

	if (defaults!=null) {
		Enumeration defKeys=defaults.keys();    
		Object def;
		while (defKeys.hasMoreElements()) {
			def=defKeys.nextElement();
			if (!result.contains(def)) result.addElement(def);
		}
	}

	return new PropertyEnumeration(result);
}

private String readLine(InputStream in) throws IOException
{
	boolean EOF = false;
	boolean EOL = false;
	StringBuffer buffer = new StringBuffer();

	while (!EOL) {
		int chr = in.read();
		switch (chr) {
		case '\n':
		case '\r':
			EOL = true;
			break;
		case '\\':
			switch (chr=in.read()) {
			case 'n':
				buffer.append('\n');
				break;
			case 'r':
				buffer.append('\r');
				break;
			case '\\':
				buffer.append('\\');
				break;
			case '\n':
			case '\r': // Continuation
				break;
			default:
				buffer.append( '\\');
				buffer.append( chr);
			}
			break;
		case -1:
			EOL = true;
			EOF = true;
			break;
		default:
			buffer.append((char)chr);
		}
	}

	if ( (buffer.length() == 0) && EOF) {
		return (null);
	}
	else {
		return (buffer.toString().trim());
	}
}

public synchronized void save(OutputStream out, String header) {
	save(new PrintWriter(out), header);
}

private synchronized void save(Writer out, String header)
	{
	try {
		out.write("# ");
		out.write(escape(header));
		out.write("\n");

		Enumeration keys = propertyNames();

		while (keys.hasMoreElements()) {
			String key=(String)keys.nextElement();
			out.write(escape(key));
			out.write("=");
			out.write(escape(getProperty(key)));
			out.write("\n");
		}
	}
	catch (IOException e) {
		System.err.println("Unable to save properties: "+header);
	}
}

private byte[] stringToBytes(String string) {
	byte result[]=new byte[string.length()];

	string.getBytes(0, string.length(), result, 0);

	return result;
}
}

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
class PropertyEnumeration
  implements Enumeration
{
	private Vector vector;
	private int posn;

public PropertyEnumeration(Vector vectorToUse) {
	vector=vectorToUse;
	posn=0;
}

public boolean hasMoreElements() {
	return (posn<vector.size());
}

public Object nextElement() {
	if (posn==vector.size()) throw new NoSuchElementException();

	return vector.elementAt(posn++);
}
}
