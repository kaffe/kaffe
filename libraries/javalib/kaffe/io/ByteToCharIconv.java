/*
 * Java core library component.
 *
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *	Edouard G. Parmelan <egp@free.fr>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.io;

import java.io.UnsupportedEncodingException;

import kaffe.util.Ptr;

public class ByteToCharIconv extends ByteToCharConverter {
    private Ptr cd;

    static {
	initialize0();
    }
    
    public ByteToCharIconv (String enc)
	throws UnsupportedEncodingException
    {
	if (!open0(enc)) {
	    throw new UnsupportedEncodingException(enc);
	}
    }

    protected void finalize() throws Throwable {
	if (cd != null) {
	    close0(cd);
	}
	
	super.finalize();
    }

    native private static void initialize0();
    native private boolean open0(String enc);
    native private void close0(Ptr cd);

    native public int convert(byte[] from, int fpos, int flen, char[] to, int tpos, int tlen);

    public int getNumberOfChars(byte[] from, int fpos, int flen) {
	return flen;
    }
}
