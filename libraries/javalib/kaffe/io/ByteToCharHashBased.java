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

import java.io.Serializable;

import kaffe.util.IntegerIntegerHashtable;

public class ByteToCharHashBased
	extends ByteToCharConverter implements Serializable {

private IntegerIntegerHashtable map;

public ByteToCharHashBased(IntegerIntegerHashtable map) {
	this.map = map;
}

public int convert ( byte[] from, int fpos, int flen, char[] to, int tpos, int tlen ) {
	int o = tpos;
	int oe = tpos + tlen;
	int i = fpos;
	int ie = fpos + flen;

	for (; i < ie && o < oe; i++) {
		to[o++] = (char)map.get(from[i] & 0xFF);
	}

	if (ie > i) {
		carry(from, i, ie - i);
	}

	return (o - tpos);
}

public int getNumberOfChars ( byte[] from, int fpos, int flen ) {
	return flen;
}

}
