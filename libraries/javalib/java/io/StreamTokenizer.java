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


public class StreamTokenizer
{
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
	private TableEntry[] lookup = new TableEntry[257];
	private boolean pushBack = false;
	private boolean EOLSignificant;
	private boolean CComments;
	private boolean CPlusPlusComments;
	private boolean toLower;

public StreamTokenizer (InputStream i)
	{
	this(new InputStreamReader(i));
}

public StreamTokenizer(Reader r)
	{
	rawIn = r;
	lineIn = new LineNumberReader(rawIn);
	pushIn = new PushbackReader(lineIn);

	for (int pos = 0; pos < lookup.length; pos++) {
		lookup[pos] = new TableEntry();
	}

	reset();
}

private int chrRead() throws IOException {
	int chr=pushIn.read();
	if (chr==-1) chr=256; /* EOF as ordinary char */

	return chr;    
}

public void commentChar(int ch) {
	lookup[ch].isComment=true;
}

public void eolIsSignificant(boolean flag) {
	EOLSignificant=flag;
}

public int lineno() {
	return lineIn.getLineNumber();
}

public void lowerCaseMode(boolean fl) {
	toLower=fl;
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

	if (EOLSignificant && chr=='\n') {
		ttype = TT_EOL;
	}
	else if (chr=='/' && (CComments || CPlusPlusComments)) {
		/* Check for C/C++ comments */
		int next = chrRead();
		if (next == '/' && (CPlusPlusComments)) {
			/* C++ comment */
			skipLine();

			ttype = nextTokenType();
		}
		else if (next == '*' && (CComments)) {
			/* C comments */
			skipCComment();

			ttype = nextTokenType();
		}
		else {
			pushIn.unread(next);
		}
	}
	else if (lookup[chr].isWhitespace) {
		/* Skip whitespace and return nextTokenType */
		while (lookup[chr].isWhitespace) {
			chr = chrRead();
		}

		/* For next time */
		pushIn.unread(chr);
		ttype = nextTokenType();
	}
	else if (lookup[chr].isNumeric) {
		/* Parse the number and return */
		StringBuffer buffer = new StringBuffer();
		while (lookup[chr].isNumeric) {
			buffer.append((char)(chr & 0xFF));
			chr = chrRead();
		}

		/* For next time */
		pushIn.unread(chr);

		ttype = TT_NUMBER;
		try {
			nval = new Double(buffer.toString()).doubleValue();
		}
		catch (NumberFormatException _) {
			nval = 0.0;
		}
	}
	else if (lookup[chr].isAlphabetic) {
		/* Parse the word and return */
		StringBuffer buffer = new StringBuffer();
		while (lookup[chr].isAlphabetic) {
			buffer.append((char)(chr & 0xFF));
			chr = chrRead();
		}

		/* For next time */
		pushIn.unread(chr);

		ttype = TT_WORD;
		sval = buffer.toString();

		if (toLower) sval = sval.toLowerCase();
	}
	else if (lookup[chr].isStringQuote) {
		/* Parse string and return word */
		StringBuffer buffer = new StringBuffer();
		chr = chrRead();
		while (!lookup[chr].isStringQuote) {
			buffer.append((char)(chr & 0xFF));
			chr = chrRead();
		}

		ttype = TT_WORD;
		sval = buffer.toString();      
	}
	else if (lookup[chr].isComment) {
		/* skip comment and return nextTokenType() */
		skipLine();

		ttype = nextTokenType();    
	}
	else {
		/* Just return it as a token */
		if (chr == 256) ttype = TT_EOF; else ttype = chr & 0xFF;
	}

	return (ttype);
}

public void ordinaryChar(int ch) {
	lookup[ch].isAlphabetic=false;
	lookup[ch].isStringQuote=false;
	lookup[ch].isNumeric=false;
	lookup[ch].isComment=false;
	lookup[ch].isWhitespace=false;
}

public void ordinaryChars(int low, int hi) {
	for (int letter=low; letter<=hi; letter++) {
		ordinaryChar(letter);
	}
}

public void parseNumbers() {
	for (int letter='0'; letter<='9'; letter++) {
		lookup[letter].isNumeric=true;
	}

	lookup['.'].isNumeric=true;
	lookup['-'].isNumeric=true;
}

public void pushBack() {
	pushBack=true;
}

public void quoteChar(int ch) {
	lookup[ch].isStringQuote = true;
}

public void reset() {
	wordChars('A', 'Z');
	wordChars('a', 'z');
	whitespaceChars('\u0000', '\u0020');
	ordinaryChar(256); /* EOF */
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
	boolean exit=false;
	int chr;

	while (!exit) {
		chr=chrRead();
		if (chr=='*') {
			int next=chrRead();
			if (next=='/') exit=true; else pushIn.unread(next);
		}
		else if (chr==256) exit=true;
	}
}

private void skipLine() throws IOException {
	int chr;

	do {
		chr=chrRead();
	}
	while (chr!='\n');
}

public void slashSlashComments(boolean flag) {
	CPlusPlusComments=flag;
}

public void slashStarComments(boolean flag) {
	CComments=flag;
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
	for (int letter=low; letter<=hi; letter++) {
		lookup[letter].isWhitespace=true;
	}    
}

public void wordChars(int low, int hi) {
	for (int letter=low; letter<=hi; letter++) {
		lookup[letter].isAlphabetic=true;
	}    
}
}

class TableEntry
{
	public boolean isNumeric = false;
	public boolean isWhitespace = false;
	public boolean isAlphabetic = false;
	public boolean isStringQuote = false;
	public boolean isComment = false;
}
