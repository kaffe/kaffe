/*
 * Java core library component.
 *
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Byte to Char converter for EUC-JP encoding.
 * See also class EncodeEUC_JP in developers directory.
 *
 * @author Ito Kazumitsu <kaz@maczuka.gcd.org>
 */

/*
 * This is only a wrapper of ByteToCharIconv for EUC-JP.
 * The name "EUC-JP" cannot be used within a Java class name.
 * So we use "EUC_JP" for "EUC-JP".
 */ 

package kaffe.io;

public class ByteToCharEUC_JP extends ByteToCharIconv {

    public ByteToCharEUC_JP() throws java.io.UnsupportedEncodingException {
        super("EUC-JP");
    }

}
