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

import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.UnsupportedEncodingException;
import java.util.Hashtable;

import kaffe.util.Assert;

abstract public class ByteToCharConverter
{
	private static String encodingRoot;
	private static String encodingDefault;
	private static Hashtable cache = new Hashtable();
	private static Class noConverter = Object.class;
	private static Class useIconv = ByteToCharIconv.class;
	protected byte[] buf;
	protected int blen;

static {
	/* NB.: encodingRoot = System.getProperty("file.encoding.pkg");
	 * would be pointless.  Either file.encoding.pkg == kaffe.io, in
	 * which it does not matter, or if file.encoding.pkg is set to
	 * something else, then these other implementations would have to be
	 * subclasses of kaffe.io.ByteToCharConverter.  However, if someone
	 * subclasses kaffe.io.ByteToCharConverter, he might as well put it
	 * in this package.  There's no documented way to add converters
	 * to Java, since Sun implements this internally in sun.io.*;
	 *
	 * Secondly, by ignoring the file.encoding.pkg property, we're
	 * free to redefine it.  This helps not 100% pure Java apps such
	 * as HotJava that access sun.io.* directly.
	 */
	encodingRoot = "kaffe.io";
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

public boolean havePending() {
	return blen != 0;
}

public int withdraw ( byte[] to, int tpos, int tlen ) {
	int n = (tlen < blen ? tlen : blen);

	if ( n == 0 ){
		return 0;
	}

	System.arraycopy( buf, 0, to, 0, n);
	if ( n < blen ) {
		System.arraycopy( buf, n, buf, 0, blen - n);
	}
	blen -= n;
	return n;
}

private static ByteToCharConverter getConverterInternal ( String enc ) {
	Class cls = (Class)cache.get(enc);
	if (cls == noConverter) {
		return (null);
	}
	String realenc = ConverterAlias.alias(enc);
	if (cls == useIconv) {
		return (getByteToCharIconv(ConverterAlias.iconvAlias(realenc)));
	}
	try {
		if (cls == null) {
			if (ConverterAlias.shouldUseIconv(realenc)) {
				ByteToCharConverter conv = getByteToCharIconv(ConverterAlias.iconvAlias(realenc));
				cache.put(enc, (conv != null ? useIconv : noConverter));
				return (conv);
			}
			String enccls = encodingRoot + ".ByteToChar" + realenc;
			cls = Class.forName(enccls);
			cache.put(enc, cls);
		}
		return ((ByteToCharConverter)cls.newInstance());
	}
	catch (ClassNotFoundException _) {
		try {
			String enccls = encodingRoot + ".ByteToChar" + realenc;
			InputStream in = ClassLoader.getSystemResourceAsStream(enccls.replace('.', '/') + ".ser");
			if (in != null) {
				ObjectInputStream oin = new ObjectInputStream(in);
				Object obj = oin.readObject();
				oin.close();
				return ((ByteToCharConverter)obj);
			}
		}
		catch (IOException __) {
		}
		catch (ClassNotFoundException __) {
		}
	}
	catch (ClassCastException _) {
	}
	catch (IllegalAccessException _) {
	}
	catch (InstantiationException _) {
	}
	// Finally, try iconv.
        ByteToCharConverter conv = getByteToCharIconv(realenc);
	cache.put(enc, (conv != null ? useIconv : noConverter));
	return (conv);
}

private static ByteToCharConverter getByteToCharIconv ( String enc ) {
	try {
		return (new ByteToCharIconv (enc));
	}
	catch (UnsupportedEncodingException _) {
		return (null);
	}
}

public static ByteToCharConverter getConverter ( String enc ) throws UnsupportedEncodingException {
	ByteToCharConverter conv = getConverterInternal(enc);
	if (conv != null) {
		return (conv);
	}
	throw new UnsupportedEncodingException(enc);
}
public static ByteToCharConverter getDefault() {
	try {
		return getConverter (encodingDefault);
	}
	catch (UnsupportedEncodingException __) {
		return new ByteToCharDefault();
	}
}

abstract public int getNumberOfChars ( byte[] from, int fpos, int flen );
}
