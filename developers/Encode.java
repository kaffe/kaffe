/*
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Encode a i18n Character Map into a resource we can load.
 */

//package kaffe.tools.charset;

import java.io.*;
import java.util.*;
import kaffe.util.*;
import kaffe.io.*;

public class Encode {

private StreamTokenizer toks;
private String codeSetName;

private IntegerIntegerHashtable unicode2natural = new IntegerIntegerHashtable();
private IntegerIntegerHashtable natural2unicode = new IntegerIntegerHashtable();

public static void main(String[] args) throws Throwable {
	if (args.length == 0) {
		System.err.println("Usage: encode <charset> ....");
		return;
	}
	for (int i = 0; i < args.length; i++) {
		(new Encode(args[i])).process();
	}
}

public Encode(String fname) throws Exception {
	toks = new StreamTokenizer(new FileReader(fname));
	toks.eolIsSignificant(true);
	toks.ordinaryChars(33, 127);
	toks.wordChars(33, 127);
}

private void process() throws Exception {
	// Read header
	readString();
	codeSetName = readString();
	eol();

	// Skip comment
	eol();

	// Skip escape
	eol();

	// Skip until we find the character map begins
	for (;;) {
		String start = readString();
		eol();
		if (start.equals("CHARMAP")) {
			break;
		}
	}
	

	for (;;) {
		String natural;
		String unicode;
		try {
			String key = readString();
			if (key.equals("END")) {
				eol();
				break;
			}
			natural = readString();
			unicode = readString();
			eol();
		}
		catch (EOFException _) {
			break;
		}

		int nval;
		int uval;
		nval = Integer.parseInt(natural.substring(2), 16);
		uval = Integer.parseInt(unicode.substring(2, unicode.length() - 1), 16);
// System.out.println(natural + " -> " + unicode);
// System.out.println(Integer.toHexString(nval) + " -> " + Integer.toHexString(uval));

		// Add to hash tables.
		unicode2natural.put(uval, nval);
		natural2unicode.put(nval, uval);
	}

	// Okay, serialize the results

	ObjectOutputStream out;

	out = new ObjectOutputStream(new FileOutputStream("ByteToChar" + codeSetName + ".ser"));
	out.writeObject(new ByteToCharHashBased(natural2unicode));
	out.close();

	out = new ObjectOutputStream(new FileOutputStream("CharToByte" + codeSetName + ".ser"));
	out.writeObject(new CharToByteHashBased(unicode2natural));
	out.close();
}

private String readString() throws Exception {
	int val = toks.nextToken();
	switch (val) {
	case StreamTokenizer.TT_EOL:
		toks.pushBack();
		return ("");

	case StreamTokenizer.TT_WORD:
		return (toks.sval);

	case StreamTokenizer.TT_EOF:
		throw new EOFException();

	default:
		throw new Exception("missing TT_WORD - got " + val);
	}
}

private void eol() throws Exception {
	while (toks.nextToken() != StreamTokenizer.TT_EOL)
		;
}

}
