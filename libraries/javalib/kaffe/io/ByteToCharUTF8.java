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

public class ByteToCharUTF8 extends ByteToCharConverter {

public ByteToCharUTF8() {
}

public int convert ( byte[] from, int fpos, int flen, char[] to, int tpos, int tlen ) {
	int o = tpos;
	int oe = tpos + tlen;
	int i = fpos;
	int ie = fpos + flen;

	for (; i < ie && o < oe; i++) {
                int data = from[i] & 0xFF;
                if ((data & 0x80) == 0) {
			to[o++] = (char)data;
		}
		else {
                        /* Hi-bit set, multi byte char */
                        if ((data & 0xE0) == 0xC0) {
				// If we don't have enough data, bail.
				if (i + 1 >= ie) {
					break;
				}
				i++;
                                int data2 = from[i] & 0xFF;
                                if ((data2 & 0xC0) == 0x80) {
                                        /* Valid 2nd byte */
                                        to[o++] = (char)(((data & 0x1F) << 6) + (data2 & 0x3F));
                                }
                                else {
					to[o++] = 0; // Error
				}
                        }
			else if ((data & 0xF0) == 0xE0) {
				// If we don't have enough data, bail.
				if (i + 2 >= ie) {
					break;
				}
				i++;
                                int data2 = from[i] & 0xFF;
                                if ((data2 & 0xC0) == 0x80) {
					i++;
					int data3 = from[i] & 0xFF;
                                        if ((data3 & 0xC0) == 0x80) {
                                                to[o++] = (char)(((data & 0x0F) << 12) + ((data2 & 0x3F) << 6) + (data3 & 0x3F));
                                        }
                                        else {
						to[o++] = 0; // Error
					}
				}
                                else {
					to[o++] = 0; // Error
				}
                        }
                        else {
				to[o++] = 0; // Error
			}
                }
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
