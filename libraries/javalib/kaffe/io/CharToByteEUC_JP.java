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
 * See also class EncodeEUC_JP in developers directory.
 *
 * @author Ito Kazumitsu <kaz@maczuka.gcd.org>
 *
 * Modified by Edouard G. Parmelan <egp@free.fr> to add comments and
 * to handle exceptions in Code set 0.
 */

package kaffe.io;

import java.io.InputStream;
import java.io.ObjectInputStream;
import java.util.Arrays;

public class CharToByteEUC_JP extends CharToByteConverter {
    // Set this to false if you want handle exceptions defined in
    // JIS-X-201 for US-ASCII range (0x5C '~' map to overline and 0x7E
    // '\' to yen).  You must use the same value in classes
    // kaffe.io.ByteToCharEUC_JP and kaffe.io.CharToByteEUC_JP.
    private final static boolean US_ASCII = true;

    static boolean tableLoaded;
    static char[] utable;
    static byte[] e1table;
    static byte[] e2table;
    static String encodingRoot;

    static {
	tableLoaded = false;
	encodingRoot = "kaffe.io";
    }

    public CharToByteEUC_JP() {
    }

    public int convert (char[] from, int fpos, int flen, byte[] to, int tpos, int tlen ) {
	if (!tableLoaded) {
	    loadTable();
	}
	int o = tpos;
	int oe = tpos + tlen;
	int i = fpos;
	int ie = fpos + flen;

	for (; i < ie; i++) {
	    char chr = from[i];
	    if (chr < 0x80) {
		// Code set 0 (ASCII or JIS X 0201-1976 Roman): 0x21-0x7E
		if (o >= oe) {
		    break;
		}
		if (!US_ASCII) {
		    // handled exceptions
		    // 0x5C -> U+A5 YEN SIGN
		    // 0x7E -> U+203E OVERLINE
		    if (chr == 0xA5) {
			chr = 0x5C;
		    }
		}
		    
		if (o >= oe) {
		    break;
		}
		to[o++] = (byte)chr;
		continue;
	    }
	    if (!US_ASCII && (chr == 0x203E)) {
		if (o >= oe) {
		    break;
		}
		to[o++] = (byte)0x7E;
		continue;
	    }
	    
	    int m = Arrays.binarySearch(utable, chr);
	    if (m >= 0) {
		byte e1 = e1table[m];
		byte e2 = e2table[m];
		if (e1 > 0) {
		    // Code set 3 (JIS X 0212-1990): 0x8FA1A1-0x8FFEFE
		    if (o + 2 >= oe) {
			break;
		    }
		    to[o++] = (byte)0x8f;
		    to[o++] = (byte)((int)e1 | 0x80);
		    to[o++] = (byte)((int)e2 | 0x80);
		}
		else {
		    //  Code set 1 (JIS X 0208): 0xA1A1-0xFEFE
		    //  Code set 2 (half-width katakana): 0x8EA1-0x8EDF
		    if (o + 1 >= oe) {
			break;
		    }
		    to[o++] = e1;
		    to[o++] = e2;
		}
	    }
	    else {
		if (o >= oe) {
		    break;
		}
		to[o++] = (byte)'?'; // Error
	    }
	}

	// Carry anything left.
	if (ie > i) {
	    carry(from, i, ie - i);
	}

	return (o - tpos);
    }

    public int getNumberOfBytes ( char[] from, int fpos, int flen ) {
	return flen; // This may return something wrong, but I do not care.
    }

    private static void loadTable() {
	try {
	    String tablename = encodingRoot + ".CharToByteEUC_JP";
	    InputStream in = ClassLoader.getSystemResourceAsStream(tablename.replace('.', '/') + ".tbl");
	    if (in != null) {
		ObjectInputStream oin = new ObjectInputStream(in);
		utable = (char[])(oin.readObject());
		e1table = (byte[])(oin.readObject());
		e2table = (byte[])(oin.readObject());
		tableLoaded = true;
	    }
	} catch (Exception e) {
	    System.err.println("CharToByteEUC_JP.tbl could not be loaded" + e);
	}
	if(!tableLoaded) {
	    utable = new char[0];
	    e1table = new byte[0];
	    e2table = new byte[0];
	    tableLoaded = true;
	    System.err.println("CharToByteEUC_JP.tbl could not be loaded");
	}
    }
}
