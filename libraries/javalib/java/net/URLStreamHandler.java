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

import java.lang.String;
import java.io.IOException;
import java.util.StringTokenizer;

abstract public class URLStreamHandler {

public URLStreamHandler() {}

abstract protected URLConnection openConnection(URL u) throws IOException;

protected void parseURL(URL u, String spec, int start, int limit) {

	String host = u.getHost();
	int port = u.getPort();
	String file = u.getFile();
	String ref = u.getRef();

	/* spec -> [//<hostname>[:port]][/<file>] */

//System.out.println("Parsing URL " + spec);

	if (start+2 < limit && spec.substring(start, start+2).equals("//")) {
		start += 2;
		int hend = spec.indexOf(':', start);
		int mend = spec.indexOf('/', start);
		if (hend != -1 && (mend == -1 || hend < mend)) {
			host = spec.substring(start, hend);
			int postart = hend+1;
			int poend = spec.indexOf('/', postart);
			if (poend == -1) {
				poend = limit;
			}
			port = Integer.parseInt(spec.substring(postart, poend));
			start = spec.indexOf( '/', start);
		}
		else {
			hend = spec.indexOf('/', start);
			if (hend != -1) {
				host = spec.substring(start, hend);
				start = spec.indexOf( '/', start);
			}
			else {
				host = spec.substring(start);
				start = -1;
			}
		}
	}

	if (start != -1) {
		file = spec.substring(start);
		int rIdx = file.lastIndexOf( '#');
		if ( rIdx > -1 ) {
			ref = file.substring( rIdx+1);
			file = file.substring( 0, rIdx);
		}
	}

	file = compressFile(file);

	setURL(u, u.getProtocol(), host, port, file, ref);
}

protected void setURL(URL u, String protocol, String host, int port, String file, String ref) {
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
	return (buf.toString());
}

private String compressFile(String file) {
	StringTokenizer tok = new StringTokenizer(file, "/");
	int len = tok.countTokens();
	String[] array = new String[len+2];
	int j = 0;
	for (int i = 0; i < len; i++) {
		String str = tok.nextToken();
		if (str.equals("..")) {
			j--;
		}
		else {
			if (i+1 < len || file.endsWith("/")) {
				str = str + '/';
			}
			array[j] = str;
			j++;
		}
	}

	// Build a string of the remaining elements.
	StringBuffer buf = new StringBuffer();
	buf.append('/');
	for (int i = 0; i < j; i++) {
		buf.append(array[i]);
	}

	return (buf.toString());
}

}
