package java.util;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class StringTokenizer
  implements Enumeration
{
	private String input;
	private String delims;
	private boolean returnTokens;
	private int position = -1;
	private boolean onToken = true;

public StringTokenizer(String str) {
	this (str, " \t\n\r");
}

public StringTokenizer(String str, String delim) {
	this (str, delim, false);
}

public StringTokenizer(String str, String delim, boolean returnTokens) {
	input=str;
	delims=delim;
	this.returnTokens=returnTokens;
}

public int countTokens() {
	int count=0;
	int posn=position;

	boolean tempGlobal=onToken; /* Ick.. Yuck.. Bleurgghhhhh! Sorry Tim */
	while ((posn=nextTokenPosition(posn, delims))!=-1) { count++; }
	onToken=tempGlobal;

	return count;
}

public boolean hasMoreElements() {
	return hasMoreTokens();
}

public boolean hasMoreTokens() {
	boolean globalTemp=onToken; /* Sorry again, Tim */
	boolean result=((position>=input.length()) || (nextTokenPosition(position, delims)!=-1));
	onToken=globalTemp;

	return result;
}

private boolean isToken(int posn, String delim) {
	return (delim.indexOf(input.charAt(posn))!=-1);
}

public Object nextElement() {
	return (Object )nextToken();
}

public String nextToken() {
	return nextToken(delims);
}

public String nextToken(String delim) {
	position=nextTokenPosition(position, delim);

	if ((returnTokens) && (isToken(position, delim))) {
		return input.substring(position, position+1);
	}
	else {
		int end=position;
		try {
			while (!isToken(end, delim)) end++;
		}
		catch (StringIndexOutOfBoundsException e) {
			end=input.length();
		}

		int start=position;
		position=end-1;

		return input.substring(start, end);
	}
}

private int nextTokenPosition(int posn, String delim) {
	char chr;

	try {
		posn++;

		if ((!isToken(posn, delim)) && (onToken)) { onToken=false; return posn; }

		/* Skip non-tokens */
		while(!isToken(posn, delim)) posn++;

		/* Process tokens */
		if (returnTokens) {
			onToken=true;
			return posn;
		}
		else {
			/* Skip tokens */
			while (isToken(posn, delim)) posn++;

			return posn;
		}
	}
	catch (StringIndexOutOfBoundsException e) {
		/* Element not found */
		return -1;
	}
}
}
