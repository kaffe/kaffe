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

public class CharToByteHashBased
	extends CharToByteConverter implements Serializable {

private IntegerIntegerHashtable map;

public CharToByteHashBased(IntegerIntegerHashtable map) {
	this.map = map;
}

public int convert ( char[] from, int fpos, int flen, byte[] to, int tpos, int tlen ) {
	int o = tpos;
	int oe = tpos + tlen;
	int i = fpos;
	int ie = fpos + flen;

	for (; i < ie && o < oe; i++) {
		to[o++] = (byte)map.get((int)from[i]);
	}

	// Carry anything left.
	if (ie > i) {
		carry(from, i, ie - i);
	}

	return (o - tpos);
}

public int getNumberOfBytes ( char[] from, int fpos, int flen ) {
	return flen;
}

}
