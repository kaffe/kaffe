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

import java.util.NoSuchElementException;
import java.lang.String;

public class StringTokenizer
  implements Enumeration
{
	private char[] input;
	private String delims;
	private boolean retDelim;
	private int position;

public StringTokenizer(String str) {
	this (str, " \t\n\r");
}

public StringTokenizer(String str, String delim) {
	this (str, delim, false);
}

public StringTokenizer(String str, String delim, boolean ret) {
	input = str.toCharArray();
	delims = delim;
	retDelim = ret;
	position = 0;
}

public int countTokens() {

	int count;
	int oldPosition = position;

	for (count = 0; nextTokenInternal() != null; count++)
		;

	position = oldPosition;

	return (count);
}

public boolean hasMoreElements() {
	return (hasMoreTokens());
}


public boolean hasMoreTokens() {

	int oldPosition = position;
	String ret = nextTokenInternal();
	position = oldPosition;
	if (ret == null) {
		return (false);
	}
	return (true);
}

public Object nextElement() {
	return ((Object)nextToken());
}

public String nextToken(String delim) {
	delims = delim;
	return (nextToken());
}

public String nextToken() {
	String ret = nextTokenInternal();
	if (ret == null) {
		throw new NoSuchElementException("no more elements");
	}
	return (ret);
}

private String nextTokenInternal() {
	if (position >= input.length) {
		return (null);
	}

	// If we're on a delimiter, we must either return it or skip it.
	if (delims.indexOf(input[position]) != -1) {
		position++;
		// If we're returning them, do it now
		if (retDelim) {
			return (new String(input, position-1, 1));
		}
		// Otherwise step though stream until we've reached a
		// non-delimiter
		for (;;) {
			if (position >= input.length) {
				return (null);
			}
			if (delims.indexOf(input[position]) == -1) {
				break;
			}
			position++;
		}
	}

	int start = position;
	for (;;) {
		position++;
		if (position >= input.length || delims.indexOf(input[position]) != -1) {
			break;
		}
	}

	return (new String(input, start, position-start));
}

}
