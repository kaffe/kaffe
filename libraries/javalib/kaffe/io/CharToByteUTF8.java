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

public class CharToByteUTF8 extends CharToByteConverter {

public CharToByteUTF8() {
}

public int convert ( char[] from, int fpos, int flen, byte[] to, int tpos, int tlen ) {
	int o = tpos;
	int oe = tpos + tlen;
	int i = fpos;
	int ie = fpos + flen;

	for (; i < ie; i++) {
                char chr = from[i];
                if (chr >= '\u0001' && chr <= '\u007F') {
			if (o >= oe) {
				break;
			}
			to[o++] = (byte)chr;
		}
                else if (chr <= '\u07FF') {
			if (o + 1 >= oe) {
				break;
			}
                        to[o++] = (byte)(0xC0 | (0x3F & (chr >> 6)));
                        to[o++] = (byte)(0x80 | (0x3F & chr));
                }
                else {
			if (o + 2 >= oe) {
				break;
			}
                        to[o++] = (byte)(0xE0 | (0x0F & (chr >> 12)));
                        to[o++] = (byte)(0x80 | (0x3F & (chr >>  6)));
                        to[o++] = (byte)(0x80 | (0x3F & chr));
                }
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
