/*
 * Java core library component.
 *
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Char to Byte converter for MS932 encoding.
 *
 * @author Ito Kazumitsu <kaz@maczuka.gcd.org>
 *
 */

/*
 * MS932 is similar to CP932, but slightly diffent.
 *
 * char --> Shift_JIS bytes
 * MS932:  301c --> 3f      CP932:  301c --> 8160
 * MS932:  ff5e --> 8160    CP932:  ff5e --> 
 * MS932:  2016 --> 3f      CP932:  2016 --> 8161
 * MS932:  2225 --> 8161    CP932:  2225 --> 
 * MS932:  2212 --> 3f      CP932:  2212 --> 817c
 * MS932:  ff0d --> 817c    CP932:  ff0d -->
 * MS932:  00a3 --> 8192    CP932:  00a3 --> 8192 
 * MS932:  ffe1 --> 8192    CP932:  ffe1 --> 
 * MS932:  00a2 --> 8191    CP932:  00a2 --> 8191
 * MS932:  ffe0 --> 8191    CP932:  ffe0 --> 
 * MS932:  00ac --> 81ca    CP932:  00ac --> 81ca
 * MS932:  ffe2 --> 81ca    CP932:  ffe2 --> fa54 (maybe iconv's bug)
 */ 
package kaffe.io;

public class CharToByteMS932 extends CharToByteIconv {

    public CharToByteMS932() throws java.io.UnsupportedEncodingException {
        super("CP932");
    }

    public int convert(char[] from, int fpos, int flen, byte[] to, int tpos, int tlen) {
        char[] buf = new char[flen];
        for(int i = 0, j = fpos; i < flen; i++, j++) {
            if      (from[j] == (char)'\uff5e') buf[i] = (char)'\u301c';
            else if (from[j] == (char)'\u2225') buf[i] = (char)'\u2016';
            else if (from[j] == (char)'\uff0d') buf[i] = (char)'\u2212';
            else if (from[j] == (char)'\uffe1') buf[i] = (char)'\u00a3';
            else if (from[j] == (char)'\uffe0') buf[i] = (char)'\u00a2';
            else if (from[j] == (char)'\uffe2') buf[i] = (char)'\u00ac';
            else buf[i] = from[j];
        }
        return super.convert(buf, 0, flen, to, tpos, tlen);
    }

}
