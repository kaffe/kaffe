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


public class StreamTokenizer {

public static final int TT_EOF = -1;
public static final int TT_EOL = '\n';
public static final int TT_NUMBER = -2;
public static final int TT_WORD = -3;
private static final int TT_INITIAL = -4;

public int ttype = TT_INITIAL;
public String sval;
public double nval;

private PushbackReader pushIn;
private BufferedReader bufferedIn;
private Reader rawIn;
private TableEntry lookup[] = new TableEntry[256];
private final TableEntry ordinary = new TableEntry();
private boolean pushBack;
private boolean EOLSignificant;
private boolean CComments;
private boolean CPlusPlusComments;
private boolean toLower;
private final StringBuffer buffer = new StringBuffer();
private boolean endOfFile;
private int lineNumber = 1;

/**
 * @deprecated
 */
public StreamTokenizer (InputStream i) {
	this(new InputStreamReader(i));
}

public StreamTokenizer(Reader r) {
	rawIn = r;
	bufferedIn = new BufferedReader(rawIn);
	pushIn = new PushbackReader(bufferedIn);
	for (int i = 0; i < lookup.length; i++) {
		lookup[i] = new TableEntry();
	}
	init();
}

private int chrRead() throws IOException {
	if (endOfFile) {
		return (-1);
	}
	else {
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

public void eolIsSignificant(boolean flag) {
	EOLSignificant = flag;
}

public int lineno() {
	return (lineNumber);
}

public void lowerCaseMode(boolean fl) {
	toLower = fl;
}

public int nextToken() throws IOException {
	if (pushBack) {
		/* Do nothing */
		pushBack = false;
	}
	else {
		/* pushBack is false,
		 * so get the next token type
		 */
		nextTokenType();
	}

	return (ttype);
}

private void nextTokenType() throws IOException {
	/* The rule for finding the next token was defined in JLS 1.ed
	   but this document is obsolete.  A simple testing shows 
	   that Sun's implementation no longer respects this old
	   document, but no official document for this can be found now.
	   The following rule, found by a try-and-error testing, seems
	   to match Sun's implementation.
	*/

        /* Sets ttype to the type of the next token */ 

	int chr = chrRead();

	TableEntry e = lookup(chr);

	if (e.isWhitespace) {
		/* Skip whitespace and return nextTokenType */
		parseWhitespaceChars(chr);
	}
	/* Contrary to the description in JLS 1.ed,
	   C & C++ comments seem to be checked
	   before other comments. That actually
	   make sense, since the default comment
	   character is '/'.
	*/
	else if (chr=='/' && CPlusPlusComments
	    && parseCPlusPlusCommentChars()) {
		/* Check for C++ comments */
	}
	else if (chr=='/' && CComments
	    && parseCCommentChars()) {
		/* Check for C comments */
	}
	else if (e.isComment) {
	        /* skip comment and return nextTokenType() */
	        parseCommentChars();
	}
	else if (e.isNumeric) {
		/* Parse the number and return */
		parseNumericChars(chr);
	}
	else if (e.isAlphabetic) {
		/* Parse the word and return */
		parseAlphabeticChars(chr);
	}
	else if (e.isStringQuote) {
	        /* Parse string and return word */
	        parseStringQuoteChars(chr);
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
}

private boolean EOLParsed(int chr) {
	/* Checks whether chr is an EOL character
	   like \r, \n.
	   Returns true if that's the case, false
	   otherwise.
	 */
	if (chr == '\r' || chr == '\n') {
		return true;
	}
	else {
		return false;
	}
}

private void skipEOL(int chr) throws IOException {
	/* Skips the \r in \r\n.
	 */
	if (chr == '\r') {
		chr = chrRead();
		if (chr != '\n') {
			unRead(chr);
		}
	}
}

private void parseWhitespaceChars(int chr) throws IOException {
	do {
		if (EOLParsed(chr)) {
			lineNumber ++;

			skipEOL(chr);

			if (EOLSignificant) {
				ttype = TT_EOL;
				return;
			}
		}
		
		chr = chrRead();
	} while (chr != -1 && lookup(chr).isWhitespace);
	
	/* For next time */
	unRead(chr);

	nextTokenType();
}

private void parseNumericChars(int chr) throws IOException {
	boolean dotParsed = false;

	buffer.setLength( 0);

	/* Parse characters until a non-numeric character, 
	 * or the first '-' after the first character, or
	 * the second decimal dot is parsed.
	 */
	do {
		if (chr == '.') {
			if (dotParsed) {
				/* Second decimal dot parsed,
				 * so the number is finished.
				 */
				break;
			}
			else {
				/* First decimal dot parsed */
				dotParsed = true;
			}
		}

		buffer.append((char)chr);
		chr = chrRead();

	} while (isNumericByDefault(chr)
		 && chr != '-'
		 && !(chr == '.' && dotParsed));


	/* For next time */
	unRead(chr);

	try {
		nval = Double.parseDouble(buffer.toString());
		ttype = TT_NUMBER;
	}
	catch ( NumberFormatException x) {
		if (buffer.toString().equals("-")) {
			/* if the first character was an '-'
			 * but no other numeric characters followed
			 */
			ttype = '-';
		}
		else if (buffer.toString().equals(".")) {
			/* A sole decimal dot is parsed as the 
			 * decimal number 0.0 according to what the
			 * JDK 1.1 does.
			 */
			ttype = TT_NUMBER;
			nval = 0.0;
		}
		else {
			/* A minus and a decimal dot are parsed as the 
			 * decimal number -0.0 according to what the
			 * JDK 1.1 does.
			 */
			ttype = TT_NUMBER;
			nval = -0.0;
		}		
	}
}
	
private void parseAlphabeticChars(int chr) throws IOException {
	buffer.setLength( 0);

	while (lookup(chr).isAlphabetic || lookup(chr).isNumeric) {
		buffer.append((char)chr);
		chr = chrRead();
	}

	/* For next time */
	unRead(chr);

	ttype = TT_WORD;
	sval = buffer.toString();
	if (toLower) {
		sval = sval.toLowerCase();
	}
}

private void parseCommentChars() throws IOException {
	skipLine();

	nextTokenType();
}

private void parseStringQuoteChars(int chr) throws IOException {
	int cq = chr;

	buffer.setLength(0);
	chr = chrRead();
	while ( chr != cq && !(EOLParsed(chr)) && chr != -1) {
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
			default:
				if ('0' <=  chr && chr <= '7') {
					/* it's an octal escape */
					chr = parseOctalEscape(chr);
				}
			}
		}
		buffer.append((char)chr);
		chr = chrRead();
	}
	if (EOLParsed(chr)) {
		unRead(chr);
	}

	/* JDK doc says:  When the nextToken method encounters a
	 * string constant, the ttype field is set to the string
	 * delimiter and the sval field is set to the body of the
	 * string.
	 */
	ttype = cq;
	sval = buffer.toString();
}

private boolean parseCPlusPlusCommentChars() throws IOException {
	int next = chrRead();
	if (next == '/') {
		/* C++ comment */
		skipLine();

		nextTokenType();
		return true;
	}
	else {
		unRead(next);
		return false;
	}
}

private boolean parseCCommentChars() throws IOException {
	int next = chrRead();
	if (next == '*') {
		/* C comment */
		skipCComment();

		nextTokenType();
		return true;
	}
	else {
		unRead(next);
		return false;
	}
}

private int parseOctalEscape(int chr) throws IOException {
	int value = 0;
	int digits = 1;
	boolean maybeThreeOctalDigits = false;

	/* There could be one, two, or three octal
	 * digits specifying a character's code.
	 * If it's three digits, the Java Language
	 * Specification says that the first one has
	 * to be in the range between '0' and '3'.
	 */
	if ('0' <= chr && chr <= '3') {
		maybeThreeOctalDigits = true;
	}

	do {
		value = value * 8 + Character.digit((char) chr, 8);
		chr = chrRead();
		digits++;

	} while (('0' <= chr && chr <= '7')
		 && (digits <= 2 || maybeThreeOctalDigits)
		 && (digits <= 3));

	unRead(chr);

	return (value);
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
	if (low < 0) {
		low = 0;
	}

	if (hi > 255) {
		hi = 255;
	}

	for (int letter=low; letter<=hi; letter++) {
		ordinaryChar(letter);
	}
}

public void parseNumbers() {
	for (int letter = '0'; letter <= '9'; letter++) {
		numericChar(letter);
	}
	numericChar('.');
	numericChar('-');
}

private boolean isNumericByDefault(int chr) {
	return ('0' <= chr && chr <= '9') || chr == '.' || chr == '-';
}

public void pushBack() {
	pushBack = true;
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

		if (EOLParsed(chr)) {
			lineNumber ++;
			skipEOL(chr);
		}
		else if (chr == '*') {
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
	/* Skip all characters to the end of line or EOF,
	 * whichever comes first.
	 */
	int chr = chrRead();

	while (!EOLParsed(chr) && chr != -1)
		chr = chrRead();

	if (EOLParsed(chr)) {
		unRead(chr);
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
		return ("Token[EOF], line "+lineno());
	}
	else if (ttype == TT_EOL) {
		return ("Token[EOL], line "+lineno());
	}
	else if (ttype == TT_NUMBER) {
		return ("Token[n="+nval+"], line "+lineno());
	}
	    else if (ttype == TT_WORD || lookup(ttype).isStringQuote) {
		return ("Token["+sval+"], line "+lineno());
	}
	else if (ttype == TT_INITIAL) {
		return("Token[NOTHING], line " + lineno());
	}
	else {
		return ("Token[\'"+ (char) ttype +"\'], line "+lineno());
	}
}

/*  While Sun's API document says "Each character can have zero or more
    of these attributes," it seems that Sun's implementation has
    some dependencies among these attributes.  The following rule
    is not clearly documented but found by a try-and-error approach.
*/

private void numericChar(int ch) {
	TableEntry e = lookup(ch);
	e.isNumeric = true;
	// e.isWhitespace = false;
	e.isStringQuote = false;
	e.isComment = false;
	e.isAlphabetic = false;
}

public void commentChar(int ch) {
	if (ch >= 0 && ch <= 255) {
		TableEntry e = lookup(ch);
		e.isComment = true;
		e.isStringQuote = false;
		e.isWhitespace = false;
		e.isAlphabetic = false;
		e.isNumeric = false;
	}
}

public void quoteChar(int ch) {
	if (ch >= 0 && ch <= 255) {
		TableEntry e = lookup(ch);
		e.isComment = false;
		e.isStringQuote = true;
		e.isWhitespace = false;
		e.isAlphabetic = false;
		e.isNumeric = false;
	}
}

public void whitespaceChars(int low, int hi) {
        if (low < 0) {
		low = 0;
	}

	if (hi > 255) {
		hi = 255;
	}

	for (int letter = low; letter <= hi; letter++) {
		TableEntry e = lookup(letter);
		e.isWhitespace = true;
		e.isComment = false;
		e.isStringQuote = false;
		e.isAlphabetic = false;
		e.isNumeric = false;
	}    
}

public void wordChars(int low, int hi) {
	if (low < 0) {
		low = 0;
	}

	if (hi > 255) {
		hi = 255;
	}

	for (int letter = low; letter <= hi; letter++) {
		TableEntry e = lookup(letter);
		e.isAlphabetic = true;
		e.isComment = false;
		e.isStringQuote = false;
		// e.isWhitespace = false;
		// e.isNumeric = false;
	}    
}

private TableEntry lookup(int letter) {
	if (letter < 0 || letter > 255) {
		return (ordinary);
	}
	return (lookup[letter]);
}

class TableEntry {

private boolean isNumeric;
private boolean isWhitespace;
private boolean isAlphabetic;
private boolean isStringQuote;
private boolean isComment;

}

}
