/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.net;

import java.io.IOException;
import java.lang.String;
import java.util.StringTokenizer;

abstract public class URLStreamHandler {

public URLStreamHandler() {
}

abstract protected URLConnection openConnection(URL u) throws IOException;

//
// This algorithm works for most common Internet URLS, eg. http, ftp, etc.
// This is how we parse "spec" (between start and limit):
//
//	spec   ->   [//<hostname>[:<port>]][<file>]
//
// Where "port" is restricted to being between 0 and 65535.
//
// The JDK 1.2 docs say this can't throw a MalformedURLException.
// That seems wrong.
//
protected void parseURL(URL u, String spec0, int start, int limit)
		throws MalformedURLException {
	String spec = spec0.substring(start, limit);
	boolean gotHost = false;

	String host = u.getHost();
	int port = u.getPort();
	String file = u.getFile();
	int posn = 0;

	// Is a host specified?
	if (spec.length() > 2 && spec.startsWith("//")) {
		posn = 2;
		gotHost = true;
		int colon = spec.indexOf(':', posn);
		int slash = spec.indexOf('/', posn);

		// Is a port specified?
		if (colon != -1 && (slash == -1 || colon < slash)) {
			host = spec.substring(posn, colon);
			String portString = spec.substring(colon + 1,
			    slash != -1 ? slash : spec.length());
			try {
				port = Integer.parseInt(portString);
				if (port < 0 || port > 65535) {
					throw new NumberFormatException();
				}
			} catch (NumberFormatException e) {
				throw new MalformedURLException("bad port: "
				  + portString);
			}
		} else {
			if (slash != -1) {
				host = spec.substring(posn, slash);
			} else {
				host = spec.substring(posn);
			}
		}
		posn = slash;
	} else {
		if (spec.length() == 0) {
			posn = -1;
		}
	}

	// Is a file specified?
	if (posn != -1) {
		if (file.equals("") || spec.charAt(posn) == '/') {
			file = spec.substring(posn);
		} else {
			int lastSlash = file.lastIndexOf('/');
			if (lastSlash == -1) {
				file = spec.substring(posn);
			} else {
				file = file.substring(0, lastSlash + 1)
				    + spec.substring(posn);
			}
		}
	} else {
		if (gotHost) {
			file = "/";
		}
	}

	setURL(u, u.getProtocol(), host, port, compressFile(file), u.getRef());
}

//
// The JDK 1.2 docs say this can't throw a MalformedURLException.
// That seems wrong.
//
protected void setURL(URL u, String protocol, String host, int port,
		String file, String ref) throws MalformedURLException {
	u.set(protocol, host, port, file, ref);
}

protected String toExternalForm(URL u) {
	StringBuffer buf = new StringBuffer();
	buf.append(u.getProtocol());
	buf.append(":");
	String host = u.getHost();
	if (host != null && !u.getHost().equals("")) {
		buf.append("//");
		buf.append(u.getHost());
		if (u.getPort() != -1) {
			buf.append(":");
			buf.append(Integer.toString(u.getPort()));
		}
	}
	buf.append(u.getFile());
	if (u.getRef() != null && !u.getRef().equals("")) {
		buf.append('#');
		buf.append(u.getRef());
	}
	return (buf.toString());
}

private String compressFile(String file) {
	boolean isAbsolute = file.length() > 0 && file.charAt(0) == '/';
	StringTokenizer tok = new StringTokenizer(file, "/");
	int len = tok.countTokens();
	String[] array = new String[len+2];
	int j = 0;
	for (int i = 0; i < len; i++) {
		String str = tok.nextToken();
		if (str.equals("..")) {
			j--;
		}
		else if (!str.equals(".")) {
			if (i+1 < len || file.endsWith("/")) {
				str = str + '/';
			}
			array[j] = str;
			j++;
		}
	}

	// Build a string of the remaining elements.
	StringBuffer buf = new StringBuffer();
	if (isAbsolute) {
		buf.append('/');
	}
	for (int i = 0; i < j; i++) {
		buf.append(array[i]);
	}

	return (buf.toString());
}

}
