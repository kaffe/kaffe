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

public class ByteToCharDefault extends ByteToCharConverter {

native public int convert ( byte[] from, int fpos, int flen, char[] to, int tpos, int tlen );

public int getNumberOfChars ( byte[] from, int fpos, int flen ) {
	return flen;
}

}
