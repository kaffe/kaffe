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

public class URLEncoder
{
public static String encode(String s) {
	StringBuffer result = new StringBuffer();

	for (int pos = 0; pos <s.length(); pos++) {
		char chr = s.charAt(pos);
		if ((chr>='A') && (chr<='Z')) {
			result.append(chr);
		}
		else if ((chr>='a') && (chr<='z')) {
			result.append(chr);
		}
		else if ((chr>='0') && (chr<='9')) {
			result.append(chr);
		}
		else if (chr==' ') {
			result.append('+');
		}
		else {
			result.append("%"+Integer.toString((int )chr, 16));
		}
	}

	return result.toString();
}
}
