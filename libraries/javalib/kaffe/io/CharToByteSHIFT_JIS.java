/*
 * Java core library component.
 *
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Char to Byte converter for SHIFT_JIS encoding.
 *
 * @author Ito Kazumitsu <kaz@maczuka.gcd.org>
 *
 */

/*
 * Without this class,  raw libiconv will be used for SHIFT_JIS encoding,
 * in which case libiconv uses JIS X 0201 for characters below 0x80
 * instead of US-ASCII.  That may be pedantically correct, but
 * practically annoying.  In fact, Sun's implementation seems to
 * use US-ASCII for characters below 0x80 even if the cocoding is
 * SHIFT_THIS.
 */ 
package kaffe.io;

public class CharToByteSHIFT_JIS extends CharToByteIconv {

    public CharToByteSHIFT_JIS() throws java.io.UnsupportedEncodingException {
        super("SHIFT_JIS");
    }

    public int convert(char[] from, int fpos, int flen, byte[] to, int tpos, int tlen) {
        char[] buf = new char[flen];
        for(int i = 0, j = fpos; i < flen; i++, j++) {
            if (from[j] == (char)'\\') buf[i] = (char)0x00a5;
            else if (from[j] == (char)'~') buf[i] = (char)0x203e;
            else buf[i] = from[j];
        }
        return super.convert(buf, 0, flen, to, tpos, tlen);
    }

}
