package kaffe.io;

import java.io.UnsupportedEncodingException;
import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.util.Hashtable;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
abstract public class CharToByteConverter
{
	protected char[] buf;
	protected int blen;
	private static String encodingRoot;
	private static String encodingDefault;
	private static Hashtable cache = new Hashtable();
	private static Class noConverter = Object.class;

static {
	// see explanation in ByteToCharConverter
	encodingRoot = "kaffe.io";
	encodingDefault = System.getProperty("file.encoding");
}

public CharToByteConverter() {
}

public Object clone() {
	try {
		return (super.clone());
	}
	catch (CloneNotSupportedException _) {
		return (null);
	}
}

public void carry ( char[] from, int fpos, int flen ) {
	int n;
	int m = blen + flen;

	if ( (from == buf) && (fpos == 0) ) // avoid recursive carry by flush()
		return;

	if ( buf == null ){
		n = (flen < 128) ? 128 : flen;
		buf = new char[n];
	}
	else {
		if ( m > buf.length ) {
			for ( n=buf.length*2; n<m; n *= 2);
			char[] newBuf = new char[n];
			System.arraycopy( buf, 0, newBuf, 0, blen);
			buf = newBuf;
		}
	}

	System.arraycopy( from, fpos, buf, blen, flen);
	blen = m;
}

abstract public int convert(char[] from, int fpos, int flen, byte[] to, int tpos, int tlen);

public int flush ( byte[] to, int tpos, int tlen ) {
	if ( blen == 0 ){
		return 0;
	}
	else {
		int oblen = blen;
		blen = 0;
		return (convert( buf, 0, oblen, to, tpos, tlen));
	}
}

private static CharToByteConverter getConverterInternal(String enc)
{
	Class cls = (Class)cache.get(enc);
	if (cls == noConverter) {
		return (null);
	}
	try {
		if (cls == null) {
			String realenc = ConverterAlias.alias(enc);
			// Check whether realenc has a character that
			// cannot be used for class names but may appear
			// in encoding names.  As far as I know, such
			// characters include: '-'.
			for (int i = 0; i < realenc.length(); i++) {
				if (realenc.charAt(i) == '-') {
					try {
						return (new CharToByteIconv (realenc);
					}
					catch (UnsupportedEncodingException _) {
						cache.put(enc, noConverter);
						return (null);
					}
				}
			}
			realenc = encodingRoot + ".CharToByte" + realenc;
			cls = Class.forName(realenc);
			cache.put(enc, cls);
		}
		return (CharToByteConverter)cls.newInstance();
	}
	catch (ClassNotFoundException _) {
		try {
			String realenc = encodingRoot + ".CharToByte" + ConverterAlias.alias(enc);
			InputStream in = ClassLoader.getSystemResourceAsStream(realenc.replace('.', '/') + ".ser");
			if (in != null) {
				ObjectInputStream oin = new ObjectInputStream(in);
				Object obj = oin.readObject();
				oin.close();
				return (CharToByteConverter)obj;
			}
		}
		catch (IOException __) {
		}
		catch (ClassNotFoundException __) {
		}
	}
	catch (ClassCastException _) {
	}
	catch (InstantiationException _) {
	}
	catch (IllegalAccessException _) {
	}
	cache.put(enc, noConverter);
	return (null);
}

public static CharToByteConverter getConverter(String enc) throws UnsupportedEncodingException {
	CharToByteConverter conv = getConverterInternal(enc);
	if (conv != null) {
		return (conv);
	}
	return new CharToByteIconv (enc);
}

public static CharToByteConverter getDefault() {
	try {
	    return getConverter(encodingDefault);
	}
	catch (UnsupportedEncodingException __) {
	    return new CharToByteDefault();
	}
}

abstract public int getNumberOfBytes ( char[] from, int fpos, int flen );
}
