/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;

import java.lang.String;

public class StreamTokenizer {

final public static int TT_EOF = -1;
final public static int TT_EOL = '\n';
final public static int TT_NUMBER = -2;
final public static int TT_WORD = -3;

public int ttype;
public String sval;
public double nval;

private PushbackReader pushIn;
private LineNumberReader lineIn;
private Reader rawIn;
private TableEntry lookup[] = new TableEntry[256];
private TableEntry ordinary = new TableEntry();
private boolean pushBack = false;
private boolean EOLSignificant;
private boolean CComments;
private boolean CPlusPlusComments;
private boolean toLower;
private StringBuffer buffer = new StringBuffer();
private boolean endOfFile;

/**
 * @deprecated
 */
public StreamTokenizer (InputStream i) {
	this(new InputStreamReader(i));
}

public StreamTokenizer(Reader r) {
	rawIn = r;
	lineIn = new LineNumberReader(rawIn);
	pushIn = new PushbackReader(lineIn);
	for (int i = 0; i < lookup.length; i++) {
		lookup[i] = new TableEntry();
	}
	init();
}

private int chrRead() throws IOException {
	if (endOfFile) {
		return (-1);
	} else {
		return (pushIn.read());
	}
}

private void unRead(int c) throws IOException {
	/* do not push EOF back --- it would show up as 65535 the next time */
	if (c == -1) {
		endOfFile = true;
	} else {
		pushIn.unread(c);
	}
}

public void commentChar(int ch) {
	if (ch >= 0 && ch <= 255) {
		lookup(ch).isComment = true;
	}
}

public void eolIsSignificant(boolean flag) {
	EOLSignificant = flag;
}

public int lineno() {
	return (lineIn.getLineNumber());
}

public void lowerCaseMode(boolean fl) {
	toLower = fl;
}

public int nextToken() throws IOException {
	if (pushBack == true) {
		/* Do nothing */
		pushBack = false;
		return (ttype);
	}
	else {
		return (nextTokenType());
	}
}

private int nextTokenType() throws IOException {
	int chr = chrRead();
	if (chr=='/' && (CComments || CPlusPlusComments)) {
		/* Check for C/C++ comments */
		int next = chrRead();
		if (next == '/' && (CPlusPlusComments)) {
			/* C++ comment */
			skipLine();

			nextTokenType();
			return (ttype);
		}
		else if (next == '*' && (CComments)) {
			/* C comments */
			skipCComment();

			nextTokenType();
			return (ttype);
		}
		else {
			unRead(next);
		}
	}

	if (chr=='\n' && EOLSignificant) {
		ttype = TT_EOL;
		return (ttype);
	}

	TableEntry e = lookup(chr);

	if (e.isWhitespace) {
		/* Skip whitespace and return nextTokenType */
		while (lookup(chr).isWhitespace) {
			chr = chrRead();
			if (chr=='\n' && EOLSignificant) {
				ttype = TT_EOL;
				return (ttype);
			}
		}

		/* For next time */
		unRead(chr);
		ttype = nextTokenType();
	}
	else if (e.isNumeric) {
		/* Parse the number and return */
		buffer.setLength( 0);
		while (lookup(chr).isNumeric) {
			buffer.append((char)chr);
			chr = chrRead();
			if (chr == '\n' && EOLSignificant)
				break;
		}

		/* For next time */
		unRead(chr);

		try {
			nval = new Double(buffer.toString()).doubleValue();
			ttype = TT_NUMBER;
		}
		catch ( NumberFormatException x) {
			ttype = TT_WORD;
			sval = buffer.toString();
			if (toLower) {
				sval = sval.toLowerCase();
			}
		}
	}
	else if (e.isAlphabetic) {
		/* Parse the word and return */
		buffer.setLength( 0);
		while (lookup(chr).isAlphabetic || lookup(chr).isNumeric) {
			buffer.append((char)chr);
			chr = chrRead();
			if (chr == '\n' && EOLSignificant)
				break;
		}

		/* For next time */
		unRead(chr);

		ttype = TT_WORD;
		sval = buffer.toString();
		if (toLower) {
			sval = sval.toLowerCase();
		}
	}
	else if (e.isStringQuote) {
		/* Parse string and return word */
		int cq = chr;

		buffer.setLength( 0);
		chr = chrRead();
		while ( chr != cq) {
			if ( chr == '\\' ) {
				chr = chrRead();
				switch (chr) {
				case 'a':
					chr = 0x7;
					break;
				case 'b':
					chr = '\b';
					break;
				case 'f':
					chr = 0xC;
					break;
				case 'n':
					chr = '\n';
					break;
				case 'r':
					chr = '\r';
					break;
				case 't':
					chr = '\t';
					break;
				case 'v':
					chr = 0xB;
					break;
				}
			}
			buffer.append((char)chr);
			chr = chrRead();
			if ( chr == -1 ) {
				break;
			}
		}

		/* JDK doc says:  When the nextToken method encounters a
		 * string constant, the ttype field is set to the string
		 * delimiter and the sval field is set to the body of the
		 * string.
		 */
		ttype = cq;
		sval = buffer.toString();      
	}
	else if (e.isComment) {
		/* skip comment and return nextTokenType() */
		skipLine();

		ttype = nextTokenType();    
	}
	else {
		/* Just return it as a token */
		sval = null;
		if (chr == -1) {
			ttype = TT_EOF;
		}
		else {
			ttype = chr;
		}
	}

	return (ttype);
}

public void ordinaryChar(int c) {
	if (c >= 0 && c <= 255) {
		TableEntry e = lookup(c);
		e.isAlphabetic = false;
		e.isStringQuote = false;
		e.isNumeric = false;
		e.isComment = false;
		e.isWhitespace = false;
	}
}

public void ordinaryChars(int low, int hi) {
	for (int letter=low; letter<=hi; letter++) {
		ordinaryChar(letter);
	}
}

public void parseNumbers() {
	for (int letter = '0'; letter <= '9'; letter++) {
		lookup(letter).isNumeric = true;
	}
	lookup('.').isNumeric = true;
	lookup('-').isNumeric = true;
}

public void pushBack() {
	pushBack = true;
}

public void quoteChar(int ch) {
	if (ch >= 0 && ch <= 255) {
		lookup(ch).isStringQuote = true;
	}
}

private void init() {
	wordChars('A', 'Z');
	wordChars('a', 'z');
	wordChars('\u00A0', '\u00FF');
	whitespaceChars('\u0000', '\u0020');
	parseNumbers();

	commentChar('/');
	quoteChar('\'');
	quoteChar('"');

	EOLSignificant    = false;
	CComments         = false;
	CPlusPlusComments = false;
	toLower           = false;    
}

public void resetSyntax() {
	ordinaryChars('\u0000', '\u00FF');
}

private void skipCComment() throws IOException {
	for (;;) {
		int chr = chrRead();
		if (chr == '*') {
			int next = chrRead();
			if (next=='/') {
				break;
			}
			else {
				unRead(next);
			}
		}
		else if (chr == -1) {
			break;
		}
	}
}

private void skipLine() throws IOException {
	while (chrRead() != '\n')
		;
	if (EOLSignificant) {
		unRead('\n');
	}
}

public void slashSlashComments(boolean flag) {
	CPlusPlusComments = flag;
}

public void slashStarComments(boolean flag) {
	CComments = flag;
}

public String toString() {
	if (ttype == TT_EOF) {
		return ("EOF");
	}
	else if (ttype == TT_EOL) {
		return ("EOL");
	}
	else if (ttype == TT_NUMBER) {
		return ("Token[n="+nval+"], line "+lineno());
	}
	else {
		return ("Token[s="+sval+"], line "+lineno());
	}
}

public void whitespaceChars(int low, int hi) {
	for (int letter = low; letter <= hi; letter++) {
		TableEntry e = lookup(letter);
		e.isWhitespace = true;
		e.isAlphabetic = false;
		e.isNumeric = false;
	}    
}

public void wordChars(int low, int hi) {
	for (int letter = low; letter <= hi; letter++) {
		lookup(letter).isAlphabetic = true;
	}    
}

private TableEntry lookup(int letter) {
	if (letter < 0 || letter > 255) {
		return (ordinary);
	}
	return (lookup[letter]);
}

class TableEntry {

private boolean isNumeric = false;
private boolean isWhitespace = false;
private boolean isAlphabetic = false;
private boolean isStringQuote = false;
private boolean isComment = false;

}

}
