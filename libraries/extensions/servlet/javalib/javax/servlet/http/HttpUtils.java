/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.servlet.http;

import java.util.Hashtable;
import javax.servlet.ServletInputStream;
import java.io.IOException;

public class HttpUtils {

public HttpUtils() {
}

public static Hashtable parseQueryString(String s) {
	Hashtable h = new Hashtable();

	int spos = 0;
	for (;;) {
		int sep = s.indexOf('=', spos);
		if (sep == -1) {
			break;
		}
		int send = s.indexOf('&', sep);
		if (send == -1) {
			send = s.length();
		}
		add(h, s.substring(spos, sep), s.substring(sep+1, send));
		spos = send+1;
	}
	return (h);
}

private static void add(Hashtable table, String key, String value) {
	key = parse(key);
	value = parse(value);

	String[] vals = (String[])table.get(key);
	if (vals == null) {
		vals = new String[] { value };
	}
	else {
		String[] nvals = new String[vals.length + 1];
		System.arraycopy(vals, 0, nvals, 0, vals.length);
		vals = nvals;
	}
	table.put(key, vals);
}

private static String parse(String str) {
	StringBuffer nstr = new StringBuffer();
	for (int i = 0; i < str.length(); i++) {
		char ch = str.charAt(i);
		switch (ch) {
		case '%':
			try {
				nstr.append((char)Integer.parseInt(str.substring(i+1, i+3), 16));
			}
			catch (NumberFormatException _) {
				throw new IllegalArgumentException();
			}
			catch (StringIndexOutOfBoundsException _) {
				throw new IllegalArgumentException();
			}
			i += 2;
			break;
		case '+':
			nstr.append(' ');
			break;
		default:
			nstr.append(ch);
			break;
		}
	}
	return (nstr.toString());
}

public static Hashtable parsePostData(int len, ServletInputStream in) {
	byte[] buf = new byte[len];
	try {
		in.read(buf);
	}
	catch (IOException _) {
		throw new IllegalArgumentException("bad POST data");
	}
	return (parseQueryString(new String(buf)));
}

public static StringBuffer getRequestURL(HttpServletRequest req) {
	StringBuffer buf = new StringBuffer();
	buf.append(req.getScheme());
	buf.append("://");
	buf.append(req.getServerName());
	int port = req.getServerPort();
	if (port != 80) {
		buf.append(':');
		buf.append(port);
	}
	buf.append(req.getServletPath());
	return (buf);
}

}
