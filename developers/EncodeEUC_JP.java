/*
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Create encoding tables for EUC-JP used by classes
 * kaffe.io.ByteToCharEUC_JP and kaffe.io.CharToByteEUC_JP.
 *
 * Initialy written by Ito Kazumitsu <kaz@maczuka.gcd.org> with two
 * classes MakeUnicodeToJISTable and CharToByteEUC_JPTableMaker.
 *
 * Rewrite in one class by Edouard G. Parmelan <egp@free.fr> with
 * addition of ByteToChar support.
 */

import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.ObjectOutputStream;
import java.util.Arrays;
import java.util.Comparator;
import java.util.StringTokenizer;
import java.util.Vector;

public class EncodeEUC_JP 
{
    private static class Char {
	public int set;	// G0, G1, G2 or G3
	public char unicode;
	public int jiscode;

	public Char (char unicode, int jiscode, int set) {
	    this.set = set;
	    this.unicode = unicode;
	    this.jiscode = jiscode;
	}
    };


    private static class CmpUnicode implements Comparator {
	public int compare (Object o1, Object o2) {
	    char uni1 = ((Char)o1).unicode;
	    char uni2 = ((Char)o2).unicode;
	    if (uni1 < uni2)
		return -1;
	    else if (uni1 > uni2)
		return 1;
	    else
		return 0;
	}
    };

    private static class CmpSetJIS implements Comparator {
	public int compare (Object o1, Object o2) {
	    int set1 = ((Char)o1).set;
	    int set2 = ((Char)o2).set;
	    if (set1 < set2)
		return -1;
	    else if (set1 > set2)
		return 1;
	    int jis1 = ((Char)o1).jiscode;
	    int jis2 = ((Char)o2).jiscode;
	    if (jis1 < jis2)
		return -1;
	    else if (jis1 > jis2)
		return 1;
	    else
		return 0;
	}
    };

    
    private static Char[] load_JIS_file()
	throws Exception
    {
        // The following table illustrates the Japanese representation of
	// EUC packed format:
	//
	//  EUC Code Sets                                 Encoding Range
	//  ^^^^^^^^^^^^^                                 ^^^^^^^^^^^^^^
	//  Code set 0 (ASCII or JIS X 0201-1976 Roman):  0x21-0x7E
	//  Code set 1 (JIS X 0208):                      0xA1A1-0xFEFE
	//  Code set 2 (half-width katakana):             0x8EA1-0x8EDF
	//  Code set 3 (JIS X 0212-1990):                 0x8FA1A1-0x8FFEFE

	Vector v = new Vector (4096, 4096);
	String s;

	BufferedReader in = new BufferedReader(new FileReader("JIS0201.TXT"));
	while ((s = in.readLine()) != null) {
	    if (s.startsWith("#")) {
		continue;
	    }
	    StringTokenizer st = new StringTokenizer(s);
	    String jis = (st.hasMoreTokens() ? st.nextToken() : null);
	    String uni = (st.hasMoreTokens() ? st.nextToken() : null);
	    if ((jis == null) || (uni == null)) {
		continue;
	    }
	    int unicode = Integer.parseInt(uni.substring(2, uni.length()), 16);
	    int jiscode = Integer.parseInt(jis.substring(2, jis.length()), 16);
	    if (jiscode < 0x80) {
 		// Code set 0 (ASCII or JIS X 0201-1976 Roman): 0x21-0x7E
		// Classes CharToByteEUC_JP and ByteToCharEUC_JP will
		// handle exceptions if US_ASCII is false.
		// 0x5C -> U+A5 YEN SIGN
		// 0x7E -> U+203E OVERLINE
	    }
	    else {
		v.addElement (new Char ((char)unicode, jiscode, 2));
	    }
	}
	in.close();
	
	in = new BufferedReader (new FileReader("JIS0208.TXT"));
	while ((s = in.readLine()) != null) {
	    if (s.startsWith ("#")) {
		continue;
	    }
	    StringTokenizer st = new StringTokenizer(s);
	    String sjis = (st.hasMoreTokens() ? st.nextToken() : null);
	    String jis = (st.hasMoreTokens() ? st.nextToken() : null);
	    String uni = (st.hasMoreTokens() ? st.nextToken() : null);
	    if ((jis == null) || (uni == null)) {
		continue;
	    }
	    int unicode = Integer.parseInt(uni.substring(2, uni.length()), 16);
	    int jiscode = Integer.parseInt(jis.substring(2, jis.length()), 16);
	    v.addElement (new Char ((char)unicode, jiscode, 1));
	}
	in.close();

	in = new BufferedReader(new FileReader("JIS0212.TXT"));
	while ((s = in.readLine()) != null) {
	    if (s.startsWith("#")) {
		continue;
	    }
	    StringTokenizer st = new StringTokenizer(s);
	    String jis = (st.hasMoreTokens() ? st.nextToken() : null);
	    String uni = (st.hasMoreTokens() ? st.nextToken() : null);
	    if ((jis == null) || (uni == null)) {
		continue;
	    }
	    int unicode = Integer.parseInt(uni.substring(2, uni.length()), 16);
	    int jiscode = Integer.parseInt(jis.substring(2, jis.length()), 16);
	    v.addElement (new Char ((char)unicode, jiscode, 3));
	}
	in.close();

	return (Char[]) v.toArray (new Char[0]);
    }


    private static void Build_CharToByte(Char[] map)
	throws Exception
    {
	// First, sort map by unicode
	Arrays.sort (map, new CmpUnicode());

	char[] utable = new char[map.length];
	byte[] e1table = new byte[map.length];
	byte[] e2table = new byte[map.length];

	int nr = 0;
	for (int i = 0; i < map.length; i++) {
	    Char c = map[i];
	    int jiscode = c.jiscode;
	    int eucode;

	    switch (c.set) {
	    case 0:
 		// Code set 0 (ASCII or JIS X 0201-1976 Roman): 0x21-0x7E
		// Don't save exceptions to US-ASCII, there are handled in
		// class CharToByteEUC_JP.
		continue;
		
 	    case 1:
		// Code set 1 (JIS X 0208): 0xA1A1-0xFEFE
		if ((c.jiscode & 0x8080) != 0) {
		    throw new Exception ("Bad JIS value for code set 1");
		}
		eucode = 0x8080 | c.jiscode;
		break;

	    case 2:
		// Code set 2 (half-width katakana): 0x8EA1-0x8EDF
		if (c.jiscode > 0xFF) {
		    throw new Exception ("Bad JIS value for code set 2");
		}
		eucode = 0x8e00 + c.jiscode;
		break;

	    case 3:
		// Code set 3 (JIS X 0212-1990): 0x8FA1A1-0x8FFEFE
		if ((c.jiscode & 0x8080) != 0) {
		    throw new Exception ("Bad JIS value for code set 3");
		}
		eucode = c.jiscode;
		// as MSB of e1 is 0, CharToByteEUC_JP will emit 0x8F
		// before e1 and e2.
		break;

	    default:
		throw new Exception ("Bad Code set");
	    }
	    utable[nr] = c.unicode;
	    e1table[nr] = (byte)(eucode >> 8);
	    e2table[nr] = (byte)eucode;
	    nr++;
	}
	char[] utable1 = new char[nr];
	byte[] e1table1 = new byte[nr];
	byte[] e2table1 = new byte[nr];
	System.arraycopy(utable, 0, utable1, 0, nr);
	System.arraycopy(e1table, 0, e1table1, 0, nr);
	System.arraycopy(e2table, 0, e2table1, 0, nr);
	ObjectOutputStream out = new ObjectOutputStream
	    (new FileOutputStream("CharToByteEUC_JP.tbl"));
	out.writeObject(utable1);
	out.writeObject(e1table1);
	out.writeObject(e2table1);
	out.flush();
    }
    
    private static void Build_ByteToChar(Char[] map)
	throws Exception
    {
	// First, sort map by set and JIS Code
	Arrays.sort (map, new CmpSetJIS());
	ObjectOutputStream out = new ObjectOutputStream
	    (new FileOutputStream("ByteToCharEUC_JP.tbl"));
	int i = 0;

	// Code set 0 (ASCII or JIS X 0201-1976 Roman): 0x21-0x7E
	// Don't save exceptions to US-ASCII, there are handled in
	// class ByteToCharEUC_JP.
	while ((i < map.length) && (map[i].set == 0)) {
	    i++;
	}

	char[] wtable = new char[map.length];
	char[] utable = new char[map.length];
	byte[] btable = new byte[map.length];

	// Code set 1 (JIS X 0208): 0xA1A1-0xFEFE
	int nr = 0;
	while ((i < map.length) && (map[i].set == 1)) {
	    wtable[nr] = (char)(map[i].jiscode | 0x8080);
	    utable[nr] = map[i].unicode;
	    nr++;
	    i++;
	}
	char[] wtable1 = new char[nr];
	char[] utable1 = new char[nr];
	System.arraycopy(wtable, 0, wtable1, 0, nr);
	System.arraycopy(utable, 0, utable1, 0, nr);
	out.writeObject(wtable1);
	out.writeObject(utable1);

	// Code set 2 (half-width katakana): 0x8EA1-0x8EDF
	nr = 0;
	while ((i < map.length) && (map[i].set == 2)) {
	    btable[nr] = (byte)map[i].jiscode;
	    utable[nr] = map[i].unicode;
	    nr++;
	    i++;
	}
	byte[] btable1 = new byte[nr];
	utable1 = new char[nr];
	System.arraycopy(btable, 0, btable1, 0, nr);
	System.arraycopy(utable, 0, utable1, 0, nr);
	out.writeObject(btable1);
	out.writeObject(utable1);
	
	// Code set 3 (JIS X 0212-1990): 0x8FA1A1-0x8FFEFE
	nr = 0;
	while ((i < map.length) && (map[i].set == 3)) {
	    utable[nr] = map[i].unicode;
	    wtable[nr] = (char)(map[i].jiscode | 0x8080);
	    nr++;
	    i++;
	}
	wtable1 = new char[nr];
	utable1 = new char[nr];
	System.arraycopy(wtable, 0, wtable1, 0, nr);
	System.arraycopy(utable, 0, utable1, 0, nr);
	out.writeObject(wtable1);
	out.writeObject(utable1);

	out.flush();
    }

    public static void main(String[] args) {
	try {
	    Char[] map = load_JIS_file();
	  
	    Build_ByteToChar(map);
	    Build_CharToByte(map);
	  
	} catch (Exception e) {
	    e.printStackTrace();
	}
    }
}
