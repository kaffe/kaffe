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

public class CharToByteDefault extends CharToByteConverter {

native public int convert ( char[] from, int fpos, int flen, byte[] to, int tpos, int tlen );

public int getNumberOfBytes ( char[] from, int fpos, int flen ) {
	return flen;
}

}
