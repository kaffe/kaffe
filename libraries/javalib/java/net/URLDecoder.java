
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.net;

public class URLDecoder {

	public URLDecoder() {
	}

	public static String decode(String s) {
		StringBuffer buf = new StringBuffer();

		for (int pos = 0; pos < s.length(); pos++) {
			char ch = s.charAt(pos);
			if (ch == '+') {
				buf.append(' ');
			} else if (ch == '%') {
				if (pos + 3 <= s.length()) {
					buf.append((char)Integer.parseInt(
					    s.substring(pos + 1, pos + 3), 16));
				} else {
					throw new IllegalArgumentException("invalid input");
				}
				pos += 2;
			} else {
				buf.append(ch);
			}
		}
		return buf.toString();
	}
}

