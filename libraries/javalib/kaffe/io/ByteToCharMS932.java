/*
 * Java core library component.
 *
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Byte to Char converter for MS932 encoding.
 *
 * @author Ito Kazumitsu <kaz@maczuka.gcd.org>
 *
 */

/*
 * MS932 is similar to CP932, but slightly different.
 *
 * Shift_JIS bytes --> char
 * MS932:  8160 --> ff5e    CP932:  8160 --> 301c
 * MS932:  8161 --> 2225    CP932:  8161 --> 2016
 * MS932:  817c --> ff0d    CP932:  817c --> 2212
 * MS932:  8192 --> ffe1    CP932:  8192 --> a3
 * MS932:  81ca --> ffe2    CP932:  81ca --> ac
 */ 
package kaffe.io;

public class ByteToCharMS932 extends ByteToCharIconv {

    public ByteToCharMS932() throws java.io.UnsupportedEncodingException {
        super("CP932");
    }

    public int convert(byte[] from, int fpos, int flen, char[] to, int tpos, int tlen) {
        int l = super.convert(from, fpos, flen, to, tpos, tlen);
        int m = tpos + l;
        for(int i = tpos; i < m; i++) {
            if      (to[i] == (char)'\u301c') to[i] = (char)'\uff5e';
            else if (to[i] == (char)'\u2016') to[i] = (char)'\u2225';
            else if (to[i] == (char)'\u2212') to[i] = (char)'\uff0d';
            else if (to[i] == (char)'\u00a3') to[i] = (char)'\uffe1';
            else if (to[i] == (char)'\u00ac') to[i] = (char)'\uffe2';
        }
        return l;
    }

}
