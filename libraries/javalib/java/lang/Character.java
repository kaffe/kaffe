/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.Serializable;
import kaffe.util.IntegerHashtable;

public final class Character implements Serializable, Comparable {

  public static final int MIN_RADIX = 2;
  public static final int MAX_RADIX = 36;
  public static final char MIN_VALUE = 0x0000;
  public static final char MAX_VALUE = 0xffff;
  public static final byte UNASSIGNED = 0;		// Cn
  public static final byte UPPERCASE_LETTER = 1;	// Lu
  public static final byte LOWERCASE_LETTER = 2;	// Ll
  public static final byte TITLECASE_LETTER = 3;	// Lt
  public static final byte MODIFIER_LETTER = 4;		// Lm
  public static final byte OTHER_LETTER = 5;		// Lo
  public static final byte NON_SPACING_MARK = 6;	// Mn
  public static final byte ENCLOSING_MARK = 7;		// Me
  public static final byte COMBINING_SPACING_MARK = 8;	// Mc
  public static final byte DECIMAL_DIGIT_NUMBER = 9;	// Nd
  public static final byte LETTER_NUMBER = 10;		// Nl
  public static final byte OTHER_NUMBER = 11;		// No
  public static final byte SPACE_SEPARATOR = 12;	// Zs
  public static final byte LINE_SEPARATOR = 13;		// Zl
  public static final byte PARAGRAPH_SEPARATOR = 14;	// Zp
  public static final byte CONTROL = 15;		// Cc
  public static final byte FORMAT = 16;			// Cf
  public static final byte PRIVATE_USE = 18;		// Co
  public static final byte SURROGATE = 19;		// Cs
  public static final byte DASH_PUNCTUATION = 20;	// Pd
  public static final byte START_PUNCTUATION = 21;	// Ps
  public static final byte END_PUNCTUATION = 22;	// Pe
  public static final byte CONNECTOR_PUNCTUATION = 23;	// Pc
  public static final byte OTHER_PUNCTUATION = 24;	// Po
  public static final byte MATH_SYMBOL = 25;		// Sm
  public static final byte CURRENCY_SYMBOL = 26;	// Sc
  public static final byte MODIFIER_SYMBOL = 27;	// Sk
  public static final byte OTHER_SYMBOL = 28;		// So

  public static final Class TYPE = Class.getPrimitiveClass("char");

  private final char value;

  /* This is what Sun's JDK1.1 "serialver java.lang.Character" spits out */
  private static final long serialVersionUID = 3786198910865385080L;

  public Character(char value)
  {
    this.value = value;
  }

  public char charValue()
  {
    return (value);
  }

  public int hashCode()
  {
    return ((int)value);
  }

  public boolean equals(Object obj)
  {
    return (obj instanceof Character)
      && (((Character) obj).value == this.value);
  }

  public String toString()
  {
    return (String.valueOf(value));
  }

  public int compareTo(Object o)
  {
    return (int)value - (int)((Character)o).value;
  }

  /**
   * @deprecated Replaced by isWhitespace(char).
   */
  public static boolean isSpace(char ch)
  {
    switch ((int)ch) {
    case 0x0009:	// HORIZONTAL TABULATION
    case 0x000A:	// NEW LINE
    case 0x000C:	// FORM FEED
    case 0x000D:	// CARRIAGE RETURN
    case 0x0020:	// SPACE
      return (true);
    default:
      return (false);
    }
  }

  public static boolean isLetterOrDigit(char ch)
  {
    return (isLetter(ch) || isDigit(ch));
  }
  
  /**
   * @deprecated Replaced by isJavaIdentifierStart(char).
   */
  public static boolean isJavaLetter(char ch)
  {
    return ((ch == '$') || (ch == '_') || isLetter(ch));
  }
  
  /**
   * @deprecated Replaced by isJavaIdentifierPart(char).
   */
  public static boolean isJavaLetterOrDigit(char ch)
  {
    return ((ch == '$') || (ch == '_') || isLetterOrDigit(ch));
  }

  public static boolean isTitleCase(char ch)
  {
      switch (getType(ch)) {
      case TITLECASE_LETTER:
	  return true;
      }
      return false;
  }
  
  /**
   * Determines if a character has a defined meaning in Unicode. 
   * A character is defined if at least one of the following is true:  <br>
   *   It has an entry in the Unicode attribute table.  <br>
   *   Its value is in the range 0x3040 <= ch <= 0x9FA5.  <br>
   *   Its value is in the range 0xF900 <= ch <= 0xFA2D.  <br>
   */
  public static boolean isDefined(char ch) {
    // FIXME: check compatibility: U+3040 is not defined in Unicode 2.1.8
    if (getType(ch) == UNASSIGNED) {
      return (false);
    }
    return (true);
  }

  public static boolean isIdentifierIgnorable(char ch)
  {
    if ((ch >= 0x0000 && ch <= 0x0008) ||	// ISO control characters that
        (ch >= 0x000E && ch <= 0x001B) ||	// are not whitespace
        (ch >= 0x007F && ch <= 0x009F) ||	// and this range
        (ch >= 0x200C && ch <= 0x200F) ||	// join controls
        (ch >= 0x202A && ch <= 0x202E) ||	// bidirectional controls
        (ch >= 0x206A && ch <= 0x206F) ||	// format controls
        (ch == 0xFEFF)) {			// zero-width no-break space
      return (true);
    }
    else {
      return (false);
    }
  }

  public static boolean isLowerCase(char ch)
  {
    switch (getType(ch)) {
    case LOWERCASE_LETTER:
      return (true);
    }
    return (false);
  }

  public static boolean isUpperCase(char ch)
  {
    switch (getType(ch)) {
    case UPPERCASE_LETTER:
      return (true);
    }
    return (false);
  }

  public static boolean isDigit(char ch)
  {
    if (getType(ch) == DECIMAL_DIGIT_NUMBER) {
      return (true);
    }
    return (false);
  }

  public static boolean isLetter(char ch)
  {
    switch (getType(ch)) {
    case UPPERCASE_LETTER:
    case LOWERCASE_LETTER:
    case TITLECASE_LETTER:
    case MODIFIER_LETTER:
    case OTHER_LETTER:
      return (true);
    default:
      return (false);
    }
  }

  /**
   * Converts the caracter argument to lowercase.
   *
   * @param ch the character to be converted.
   * @return the lowercase equivalent defined by the Unicode database
   * 	or the character itself.
   */
  public static char toLowerCase(char ch)
  {
      CharacterProperties chProp = getCharProp(ch);
      if (chProp.lower != 0x0000) {
	  return chProp.lower;
      }
      else {
	  return ch;
      }
  }

  /**
   * Converts the caracter argument to uppercase.
   *
   * @param ch the character to be converted.
   * @return the uppercase equivalent defined by the Unicode database
   * 	or the character itself.
   */
  public static char toUpperCase(char ch)
  {
      CharacterProperties chProp = getCharProp(ch);
      if (chProp.upper != 0x0000) {
	  return chProp.upper;
      }
      else {
	  return ch;
      }
  }

  /**
   * Converts the caracter argument to titlecase.
   *
   * @param ch the character to be converted.
   * @return the titlecase equivalent defined by the Unicode database
   * 	or the character itself.
   */
  public static char toTitleCase(char ch)
  {
      CharacterProperties chProp = getCharProp(ch);
      if (chProp.title != 0x0000) {
	  return chProp.title;
      }
      else {
	  return ch;
      }
  }

  /**
   * Returns the numeric value of the character ch in the specified
   * radix.
   *
   * @param ch the character to be converted.
   * @param radix the radix.
   * @return the numeric value or -1.
   */
  public static int digit(char ch, int radix)
  {
    if (radix < MIN_RADIX || radix > MAX_RADIX) {
      return -1;
    }

    int val = radix;
    if (isDigit(ch)) {
	// FIXME: true as long `decimal digit value' == `numeric value'
	// FIXME: in the Unicode Database.  Add a check in unicode.pl
	val = getNumericValue(ch);
	if (val < 0) {
	    return -1;
	}
    }
    else if (('A' <= ch) && (ch <= 'Z')) {
	// Help the compiler, group constant values !
	val = (int)ch - ('A' - 10);
    }
    else if (('a' <= ch) && (ch <= 'z')) {
	// Help the compiler, group constant values !
	val = (int)ch - ('a' - 10);
    }

    return (val < radix) ? val : -1;
  }

  public static char forDigit(int digit, int radix)
  {
    if (radix < MIN_RADIX || radix > MAX_RADIX) {
      return 0x0000;
    }
    if (digit < 0 || digit >= radix) {
      return 0x0000;
    }
    if (digit < 10) {
      return (char)(((int)'0')+digit);
    }
    else {
      return (char)(((int)'a')+(digit-10));
    }
  }

  /**
   * Returns the Unicode numeric value of the character as a nonnegative
   * integer.
   *
   * @param ch the character to be converted.
   * @return the numeric value equivalent defined in the Unicode database
   * 	or -2 if it is negative or not integer, else -1.
   */
  public static int getNumericValue(char ch)
  {
      return getCharProp(ch).numeric;
  }

  /**
   * Returns a value indicating a character category.
   *
   * @param ch the character to be tested.
   * @return the Unicode category of the character as an integer.
   */
  public static int getType(char ch)
  {
      return getCharProp(ch).category;
  }

  public static boolean isISOControl(char ch)
  {
    if (ch < 32) {	// U+0000 - U+001F
      return (true);
    }
    if (ch < 127) {
      return (false);
    }
    if (ch < 160) {	// U+007F - U+009F
      return (true);
    }
    return (false);
  }

  public static boolean isJavaIdentifierPart(char ch)
  {
    switch (getType(ch)) {
    case UPPERCASE_LETTER:
    case LOWERCASE_LETTER:
    case TITLECASE_LETTER:
    case MODIFIER_LETTER:
    case OTHER_LETTER:
    case CURRENCY_SYMBOL:
    case CONNECTOR_PUNCTUATION:
    case DECIMAL_DIGIT_NUMBER:
    case LETTER_NUMBER:
    case COMBINING_SPACING_MARK:
    case NON_SPACING_MARK:
      return (true);
    default:
      return isIdentifierIgnorable(ch);
    }
  }

  public static boolean isJavaIdentifierStart(char ch)
  {
    switch (getType(ch)) {
    case UPPERCASE_LETTER:
    case LOWERCASE_LETTER:
    case TITLECASE_LETTER:
    case MODIFIER_LETTER:
    case OTHER_LETTER:
    case CURRENCY_SYMBOL:
    case CONNECTOR_PUNCTUATION:
      return (true);
    default:
      return (false);
    }
  }

  public static boolean isSpaceChar(char ch)
  {
    switch (getType(ch)) {
    case SPACE_SEPARATOR:
    case LINE_SEPARATOR:
    case PARAGRAPH_SEPARATOR:
      return (true);
    default:
      return (false);
    }
  }

  public static boolean isUnicodeIdentifierPart(char ch)
  {
    switch (getType(ch)) {
    case UPPERCASE_LETTER:
    case LOWERCASE_LETTER:
    case TITLECASE_LETTER:
    case MODIFIER_LETTER:
    case OTHER_LETTER:
    case CONNECTOR_PUNCTUATION:
    case DECIMAL_DIGIT_NUMBER:
    case LETTER_NUMBER:
    case COMBINING_SPACING_MARK:
    case NON_SPACING_MARK:
      return (true);
    default:
      return isIdentifierIgnorable(ch);
    }
  }

  public static boolean isUnicodeIdentifierStart(char ch)
  {
      return (isLetter(ch));
  }

  public static boolean isWhitespace(char ch)
  {
    switch ((int)ch) {
    case 0x0009:	// HORIZONTAL TABULATION.
    case 0x000A:	// LINE FEED.
    case 0x000B:	// VERTICAL TABULATION.
    case 0x000C:	// FORM FEED.
    case 0x000D:	// CARRIAGE RETURN.
    case 0x001C:	// FILE SEPARATOR.
    case 0x001D:	// GROUP SEPARATOR.
    case 0x001E:	// RECORD SEPARATOR.
    case 0x001F:	// UNIT SEPARATOR.
	return (true);
    }

    switch (getType(ch)) {
    case SPACE_SEPARATOR:
	// but not a no-break separator
        return (!getCharProp(ch).noBreak);
    case LINE_SEPARATOR:
    case PARAGRAPH_SEPARATOR:
	return (true);
    default:
	return (false);
    }
  }


    private static class CharacterProperties {
	char unicode;
	int category;
	boolean noBreak;
	short numeric;
	char upper;
	char lower;
	char title;

	static IntegerHashtable cache = new IntegerHashtable();

	CharacterProperties(char unicode, int category, boolean noBreak,
		short numeric, char upper, char lower, char title) {
	    this.unicode = unicode;
	    this.category = category;
	    this.noBreak = noBreak;
	    this.numeric = numeric;
	    this.upper = upper;
	    this.lower = lower;
	    this.title = title;

	    cache.put((int)unicode, this);
	}


	private static byte propTable[];

	// three basic type tables of N entries are
	// better than one tablee of N objects
	private static char rangeStart[];
	private static char rangeEnd[];
	private static int rangeOffset[];


	static {
	    try {
	    	propTable = getResource("/kaffe/lang/unicode.tbl");
	    } catch (IOException e) {
		throw new Error("Missing Kaffe Unicode Database table: " + e);
	    }
	    byte tbl[];
	    try {
	    	tbl = getResource("/kaffe/lang/unicode.idx");
	    } catch (IOException e) {
		throw new Error("Missing Kaffe Unicode Database index: " + e);
	    }
	    if (tbl.length % 7 != 0) {
		throw new Error("Corrupted Kaffe Unicode Database");
	    }
	    int n = tbl.length / 7;
	    rangeStart = new char[n];
	    rangeEnd = new char[n];
	    rangeOffset = new int[n];
	    
	    for (int i = 0, o = 0; i < n; i++, o += 7) {
		rangeStart[i] = (char)(((tbl[o] & 0xFF) << 8)
				      + (tbl[o + 1] & 0xFF));
		rangeEnd[i] = (char)(((tbl[o + 2] & 0xFF) << 8)
				    + (tbl[o + 3] & 0xFF));
		rangeOffset[i] = (int)(((tbl[o + 4] & 0xFF) << 16)
				     + ((tbl[o + 5] & 0xFF) << 8)
				     +  (tbl[o + 6] & 0xFF));
	    }
	}

	// Read in a resource and convert it to a byte array
	private static byte[] getResource(String name) throws IOException {
		InputStream in = Character.class.getResourceAsStream(name);
		if (in == null) {
			throw new IOException("not found");
		}
		ByteArrayOutputStream out = new ByteArrayOutputStream(5000);
		byte[] buf = new byte[1024];
		int r;

		while ((r = in.read(buf)) != -1) {
			out.write(buf, 0, r);
		}
		return out.toByteArray();
	}

	/**
	 * Lookup the corresponding range for the character ch.
	 * @param ch the character searched.
	 * @return the index of his range or -1.
	 */
	private static int getIndex(char ch) {
	    // use local tables
	    char start[] = rangeStart;
	    char end[] = rangeEnd;

	    int lo = 0;
	    int hi = start.length - 1;

	    while (lo <= hi) {
		int med = (lo + hi) >> 1;
		if (ch < start[med]) {
		    hi = med - 1;
		}
		else if (ch > end[med]) {
		    lo = med + 1;
		}
		else {
		    return med;
		}
	    }
	    return -1;
	}


	/**
	 * Decode the properties of the character ch with the range index
	 * index.
	 * @param ch the query character.
	 * @param index the range index for this character.
	 * @return the properties of the character ch.
	 */
	private static CharacterProperties decodeProp(char ch, int index) {
	    byte tbl[] = propTable;
	    byte method = (byte)((rangeOffset[index] >> 20) & 0x3);
	    int offset = rangeOffset[index] & 0xFFFFF;

	    if (method == 0) {
		// not compressed, add delta to offset
		offset += 3 * ((int)ch - (int)rangeStart[index]);
	    }
	    else if (method == 3) {
		// not compressed, extended entry
		offset += 9 * ((int)ch - (int)rangeStart[index]);
	    }

	    boolean noBreak = false;
	    int category = tbl[offset] & 0x1F;
	    if (category == 31) {
		category = 12;
		noBreak = true;
	    }

	    if (method < 3) {
		// 0: not compressed
		// 1: compressed, same value
		// 2: compressed, one increment

		char generic = (char)(((tbl[offset + 1] & 0xFF) << 8)
				     + (tbl[offset + 2] & 0xFF));
		if (method == 2) {
		    // compressed, one increment, add delta to value
		    generic += (short)(0xFFFF & ((int)ch - (int)rangeStart[index]));
		}

		switch ((tbl[offset] >> 5) & 0x3) {	// field
		case 0:	// none
		    return new CharacterProperties(ch, category, noBreak,
			    (short)-1, (char)0x0000, (char)0x0000, (char)0x0000);
		case 1:	// uppercase and titlecase
		    return new CharacterProperties(ch, category, noBreak,
			    (short)-1, generic, (char)0x0000, generic);
		case 2:	// lowercase
		    return new CharacterProperties(ch, category, noBreak,
			    (short)-1, (char)0x0000, generic, (char)0x0000);
		default: // numeric
		    return new CharacterProperties(ch, category, noBreak,
			    (short)generic, (char)0x0000, (char)0x0000, (char)0x0000);
		}
	    }

	    // 3: not compressed, extended entry
	    short numeric = (short)(((tbl[offset + 1] & 0xFF) << 8)
				   + (tbl[offset + 2] & 0xFF));
	    char upper = (char)(((tbl[offset + 3] & 0xFF) << 8)
			       + (tbl[offset + 4] & 0xFF));
	    char lower = (char)(((tbl[offset + 5] & 0xFF) << 8)
			       + (tbl[offset + 6] & 0xFF));
	    char title = (char)(((tbl[offset + 7] & 0xFF) << 8)
			       + (tbl[offset + 8] & 0xFF));
	    return new CharacterProperties(ch, category, noBreak,
		    numeric, upper, lower, title);
	}
    }

    private static CharacterProperties getCharProp(char ch) {
	// consult the cache
	CharacterProperties chProp = (CharacterProperties)CharacterProperties.cache.get((int)ch);
	if (chProp != null) {
	    return chProp;
	}

	// lookup
	int index = CharacterProperties.getIndex(ch);

	if (index < 0) {
	    chProp = new CharacterProperties(ch, UNASSIGNED, false, (short)-1,
		    (char)0x0000, (char)0x0000, (char)0x0000);
	}
	else {
	    chProp = CharacterProperties.decodeProp(ch, index);
	}
	return chProp;
    }

}
