/*
 * Java core library component.
 *
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Char to Byte converter for EUC-JP encoding.
 *
 * @author Ito Kazumitsu <kaz@maczuka.gcd.org>
 *
 */

package kaffe.io;

public class CharToByteEUC_JP extends CharToByteIconv {

    public CharToByteEUC_JP() throws java.io.UnsupportedEncodingException {
        super("EUC-JP");
    }

}
