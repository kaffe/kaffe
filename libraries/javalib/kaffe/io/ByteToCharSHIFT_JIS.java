/*
 * Java core library component.
 *
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Byte to Char converter for SHIFT_JIS encoding.
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

public class ByteToCharSHIFT_JIS extends ByteToCharIconv {

    public ByteToCharSHIFT_JIS() throws java.io.UnsupportedEncodingException {
        super("SHIFT_JIS");
    }

    public int convert(byte[] from, int fpos, int flen, char[] to, int tpos, int tlen) {
        int l = super.convert(from, fpos, flen, to, tpos, tlen);
        int m = tpos + l;
        for(int i = tpos; i < m; i++) {
            if (to[i] == (char)0x00a5) to[i] = (char)'\\';
            else if (to[i] == (char)0x203e) to[i] = (char)'~';
        }
        return l;
    }

}
