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
 * @author Edouard G. Parmelan <egp@free.fr>
 */

package kaffe.io;

import java.io.InputStream;
import java.io.ObjectInputStream;
import java.util.Arrays;

public class ByteToCharEUC_JP extends ByteToCharConverter {
    // Set this to false if you want handle exceptions defined in
    // JIS-X-201 for US-ASCII range (0x5C '~' map to overline and 0x7E
    // '\' to yen).  You must use the same value in classes
    // kaffe.io.ByteToCharEUC_JP and kaffe.io.CharToByteEUC_JP.
    private final static boolean US_ASCII = true;
    
    static boolean tableLoaded;
    static String encodingRoot;

    // Code set 1 (JIS X 0208): 0xA1A1-0xFEFE
    static char[] cs1table;
    static char[] uni1table;

    // Code set 2 (half-width katakana): 0x8EA1-0x8EDF
    static byte[] cs2table;
    static char[] uni2table;

    // Code set 3 (JIS X 0212-1990): 0x8FA1A1-0x8FFEFE
    static char[] cs3table;
    static char[] uni3table;

    static {
	tableLoaded = false;
	encodingRoot = "kaffe.io";
    }


    public ByteToCharEUC_JP() {
    }

    public int convert(byte[] from, int fpos, int flen, char[] to, int tpos, int tlen ) {
	if (!tableLoaded) {
	    loadTable();
	}
	int o = tpos;
	int oe = tpos + tlen;
	int i = fpos;
	int ie = fpos + flen;

	for (; i < ie && o < oe; i++) {
	    int data = from[i] & 0xFF;
	    if ((data & 0x80) == 0) {
		// Code set 0 (ASCII or JIS X 0201-1976 Roman): 0x21-0x7E
		if (!US_ASCII) {
		    // handle exceptions
		    // 0x5C -> U+A5 YEN SIGN
		    // 0x7E -> U+203E OVERLINE
		    if (data == 0x5C) {
			data = 0xA5;
		    }
		    else if (data == 0x7E) {
			data = 0x203E;
		    }
		}
		to[o++] = (char)data;
	    }
	    else if (data == 0x8E) {
		// Code set 2 (half-width katakana): 0x8EA1-0x8EDF
		// If we don't have enough data, bail.
		if (i + 1 >= ie) {
		    break;
		}
		i++;
		data = from[i] & 0xFF;
		int m = Arrays.binarySearch (cs2table, (byte)data);
		if (m >= 0) {
		    to[o++] = uni2table[m];
		}
		else {
		    to[o++] = 0; // Error
		}
	    }
	    else if (data == 0x8F) {
		// Code set 3 (JIS X 0212-1990): 0x8FA1A1-0x8FFEFE
		// If we don't have enough data, bail.
		if (i + 2 >= ie) {
		    break;
		}
		data = ((from[i + 1] & 0xFF) << 8) + (from[i + 2] & 0xFF);
		i += 2;
		int m = Arrays.binarySearch (cs3table, (char)data);
		if (m >= 0) {
		    to[o++] = uni3table[m];
		}
		else {
		    to[o++] = 0; // Error
		}
	    }
	    else {
		// Code set 1 (JIS X 0208): 0xA1A1-0xFEFE
		// If we don't have enough data, bail.
		if (i + 1 >= ie) {
		    break;
		}
		data = ((from[i] & 0xFF) << 8) + (from[i + 1] & 0xFF);
		i += 1;
		int m = Arrays.binarySearch (cs1table, (char)data);
		if (m >= 0) {
		    to[o++] = uni1table[m];
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

    public int getNumberOfChars (byte[] from, int fpos, int flen ) {
	return flen;
    }

    private static void loadTable() {
	try {
	    String tablename = encodingRoot + ".ByteToCharEUC_JP";
	    InputStream in = ClassLoader.getSystemResourceAsStream(tablename.replace('.', '/') + ".tbl");
	    if (in != null) {
		ObjectInputStream oin = new ObjectInputStream(in);
		// Code set 1 (JIS X 0208): 0xA1A1-0xFEFE
		cs1table = (char[])(oin.readObject());
		uni1table = (char[])(oin.readObject());

		// Code set 2 (half-width katakana): 0x8EA1-0x8EDF
		cs2table = (byte[])(oin.readObject());
		uni2table = (char[])(oin.readObject());

		// Code set 3 (JIS X 0212-1990): 0x8FA1A1-0x8FFEFE
		cs3table = (char[])(oin.readObject());
		uni3table = (char[])(oin.readObject());

		tableLoaded = true;
	    }
	} catch (Exception e) {
	    System.err.println("ByteToCharEUC_JP.tbl could not be loaded" + e);
	}
	if(!tableLoaded) {
	    cs1table = new char[0];
	    uni1table = new char[0];
	    cs2table = new byte[0];
	    uni2table = new char[0];
	    cs3table = new char[0];
	    uni3table = new char[0];
	    tableLoaded = true;
	    System.err.println("CharToByteEUC_JP.tbl could not be loaded");
	}
    }
}
