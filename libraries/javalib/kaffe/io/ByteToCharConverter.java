/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.io;

import java.lang.String;
import java.io.UnsupportedEncodingException;
import kaffe.util.Assert;

abstract public class ByteToCharConverter
{
	private static String encodingRoot;
	private static String encodingDefault;
	protected byte[] buf;
	protected int blen;

static {
	encodingRoot = System.getProperty("file.encoding.pkg");
	encodingDefault = System.getProperty("file.encoding");
}

public ByteToCharConverter() {
}

void carry ( byte[] from, int fpos, int flen ) {
	int n;
	int m = blen + flen;

	Assert.that(blen == 0);

	if ( buf == null ){
		n = (flen < 128) ? 128 : flen;
		buf = new byte[n];
	}
	else {
		if ( m > buf.length ) {
			for ( n=buf.length*2; n<m; n *= 2);
			byte[] newBuf = new byte[n];
			System.arraycopy( buf, 0, newBuf, 0, blen);
			buf = newBuf;
		}
	}

	System.arraycopy( from, fpos, buf, blen, flen);
	blen = m;
}

abstract public int convert ( byte[] from, int fpos, int flen, char[] to, int tpos, int tlen );

public int flush ( char[] to, int tpos, int tlen ) {
	if ( blen == 0 ){
		return 0;
	}
	else {
		int oblen = blen;
		blen = 0;
		return (convert( buf, 0, oblen, to, tpos, tlen));
	}
}

public static ByteToCharConverter getConverter ( String enc )
			throws UnsupportedEncodingException
{
	try {
		return ((ByteToCharConverter)Class.forName(encodingRoot + ".ByteToChar" + enc).newInstance());
	}
	catch (ClassNotFoundException _) {
		throw new UnsupportedEncodingException(enc);
	}
	catch (ClassCastException _) {
		throw new UnsupportedEncodingException(enc);
	}
	catch (IllegalAccessException _) {
		throw new UnsupportedEncodingException(enc);
	}
	catch (InstantiationException _) {
		throw new UnsupportedEncodingException(enc);
	}
}

public static ByteToCharConverter getDefault() {
	String enc = encodingDefault;
	if (ClassLoader.getSystemResourceAsStream(encodingRoot + ".ByteToChar" + enc) == null) {
		enc = "Default";
	}
	try {
		return (getConverter(enc));
	}
	catch (UnsupportedEncodingException __) {
		return (null);
	}
}

abstract public int getNumberOfChars ( byte[] from, int fpos, int flen );
}
