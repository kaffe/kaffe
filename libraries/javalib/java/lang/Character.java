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
import java.util.StringTokenizer;
import java.util.zip.ZipFile;
import java.util.zip.ZipEntry;
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

  private static final char FULLWIDTH_LATIN_CAPITAL_LETTER_A = '\uff21';
  private static final char FULLWIDTH_LATIN_CAPITAL_LETTER_Z = '\uff3a';
  private static final char FULLWIDTH_LATIN_SMALL_LETTER_A = '\uff41';
  private static final char FULLWIDTH_LATIN_SMALL_LETTER_Z = '\uff5a';

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

  public int compareTo(Character c)
  {
    return (int)value - (int)c.value;
  }

  public int compareTo(Object o)
  {
    return compareTo((Character)o);
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
	val =  getCharProp(ch).numeric;
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
    else if((FULLWIDTH_LATIN_CAPITAL_LETTER_A <= ch)
	    && (ch <= FULLWIDTH_LATIN_CAPITAL_LETTER_Z)) {
	val = (int)ch - (FULLWIDTH_LATIN_CAPITAL_LETTER_A - 10);
    }
    else if((FULLWIDTH_LATIN_SMALL_LETTER_A <= ch)
	    && (ch <= FULLWIDTH_LATIN_SMALL_LETTER_Z)) {
	val = (int)ch - (FULLWIDTH_LATIN_SMALL_LETTER_A - 10);
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
    int val = getCharProp(ch).numeric;

    if (val == -1) {
      return digit(ch, Character.MAX_RADIX);
    }
    else {
      return val;
    }
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

    /* taken from GNU Classpath */
  /**
   * A subset of Unicode blocks.
   *
   * @author Paul N. Fisher
   * @author Eric Blake <ebb9@email.byu.edu>
   * @since 1.2
   */
  public static class Subset
  {
    /** The name of the subset. */
    private final String name;

    /**
     * Construct a new subset of characters.
     *
     * @param name the name of the subset
     * @throws NullPointerException if name is null
     */
    protected Subset(String name)
    {
      // Note that name.toString() is name, unless name was null.
      this.name = name.toString();
    }

    /**
     * Compares two Subsets for equality. This is <code>final</code>, and
     * restricts the comparison on the <code>==</code> operator, so it returns
     * true only for the same object.
     *
     * @param o the object to compare
     * @return true if o is this
     */
    public final boolean equals(Object o)
    {
      return o == this;
    }

    /**
     * Makes the original hashCode of Object final, to be consistent with
     * equals.
     *
     * @return the hash code for this object
     */
    public final int hashCode()
    {
      return super.hashCode();
    }

    /**
     * Returns the name of the subset.
     *
     * @return the name
     */
    public final String toString()
    {
      return name;
    }
  } // class Subset

    /* taken from GNU Classpath */
  /**
   * A family of character subsets in the Unicode specification. A character
   * is in at most one of these blocks.
   *
   * This inner class was generated automatically from
   * <code>doc/unicode/Block-3.txt</code>, by some perl scripts.
   * This Unicode definition file can be found on the
   * <a href="http://www.unicode.org">http://www.unicode.org</a> website.
   * JDK 1.4 uses Unicode version 3.0.0.
   *
   * @author scripts/unicode-blocks.pl (written by Eric Blake)
   * @since 1.2
   */
  public static final class UnicodeBlock extends Subset
  {
    /** The start of the subset. */
    private final char start;

    /** The end of the subset. */
    private final char end;

    /**
     * Constructor for strictly defined blocks.
     *
     * @param start the start character of the range
     * @param end the end character of the range
     * @param name the block name
     */
    private UnicodeBlock(char start, char end, String name)
    {
      super(name);
      this.start = start;
      this.end = end;
    }

    /**
     * Returns the Unicode character block which a character belongs to.
     *
     * @param ch the character to look up
     * @return the set it belongs to, or null if it is not in one
     */
    public static UnicodeBlock of(char ch)
    {
      // Special case, since SPECIALS contains two ranges.
      if (ch == '\uFEFF')
        return SPECIALS;
      // Simple binary search for the correct block.
      int low = 0;
      int hi = sets.length - 1;
      while (low <= hi)
        {
          int mid = (low + hi) >> 1;
          UnicodeBlock b = sets[mid];
          if (ch < b.start)
            hi = mid - 1;
          else if (ch > b.end)
            low = mid + 1;
          else
            return b;
        }
      return null;
    }

    /**
     * Basic Latin.
     * '\u0000' - '\u007F'.
     */
    public final static UnicodeBlock BASIC_LATIN
      = new UnicodeBlock('\u0000', '\u007F',
                         "BASIC_LATIN");

    /**
     * Latin-1 Supplement.
     * '\u0080' - '\u00FF'.
     */
    public final static UnicodeBlock LATIN_1_SUPPLEMENT
      = new UnicodeBlock('\u0080', '\u00FF',
                         "LATIN_1_SUPPLEMENT");

    /**
     * Latin Extended-A.
     * '\u0100' - '\u017F'.
     */
    public final static UnicodeBlock LATIN_EXTENDED_A
      = new UnicodeBlock('\u0100', '\u017F',
                         "LATIN_EXTENDED_A");

    /**
     * Latin Extended-B.
     * '\u0180' - '\u024F'.
     */
    public final static UnicodeBlock LATIN_EXTENDED_B
      = new UnicodeBlock('\u0180', '\u024F',
                         "LATIN_EXTENDED_B");

    /**
     * IPA Extensions.
     * '\u0250' - '\u02AF'.
     */
    public final static UnicodeBlock IPA_EXTENSIONS
      = new UnicodeBlock('\u0250', '\u02AF',
                         "IPA_EXTENSIONS");

    /**
     * Spacing Modifier Letters.
     * '\u02B0' - '\u02FF'.
     */
    public final static UnicodeBlock SPACING_MODIFIER_LETTERS
      = new UnicodeBlock('\u02B0', '\u02FF',
                         "SPACING_MODIFIER_LETTERS");

    /**
     * Combining Diacritical Marks.
     * '\u0300' - '\u036F'.
     */
    public final static UnicodeBlock COMBINING_DIACRITICAL_MARKS
      = new UnicodeBlock('\u0300', '\u036F',
                         "COMBINING_DIACRITICAL_MARKS");

    /**
     * Greek.
     * '\u0370' - '\u03FF'.
     */
    public final static UnicodeBlock GREEK
      = new UnicodeBlock('\u0370', '\u03FF',
                         "GREEK");

    /**
     * Cyrillic.
     * '\u0400' - '\u04FF'.
     */
    public final static UnicodeBlock CYRILLIC
      = new UnicodeBlock('\u0400', '\u04FF',
                         "CYRILLIC");

    /**
     * Armenian.
     * '\u0530' - '\u058F'.
     */
    public final static UnicodeBlock ARMENIAN
      = new UnicodeBlock('\u0530', '\u058F',
                         "ARMENIAN");

    /**
     * Hebrew.
     * '\u0590' - '\u05FF'.
     */
    public final static UnicodeBlock HEBREW
      = new UnicodeBlock('\u0590', '\u05FF',
                         "HEBREW");

    /**
     * Arabic.
     * '\u0600' - '\u06FF'.
     */
    public final static UnicodeBlock ARABIC
      = new UnicodeBlock('\u0600', '\u06FF',
                         "ARABIC");

    /**
     * Syriac.
     * '\u0700' - '\u074F'.
     * @since 1.4
     */
    public final static UnicodeBlock SYRIAC
      = new UnicodeBlock('\u0700', '\u074F',
                         "SYRIAC");

    /**
     * Thaana.
     * '\u0780' - '\u07BF'.
     * @since 1.4
     */
    public final static UnicodeBlock THAANA
      = new UnicodeBlock('\u0780', '\u07BF',
                         "THAANA");

    /**
     * Devanagari.
     * '\u0900' - '\u097F'.
     */
    public final static UnicodeBlock DEVANAGARI
      = new UnicodeBlock('\u0900', '\u097F',
                         "DEVANAGARI");

    /**
     * Bengali.
     * '\u0980' - '\u09FF'.
     */
    public final static UnicodeBlock BENGALI
      = new UnicodeBlock('\u0980', '\u09FF',
                         "BENGALI");

    /**
     * Gurmukhi.
     * '\u0A00' - '\u0A7F'.
     */
    public final static UnicodeBlock GURMUKHI
      = new UnicodeBlock('\u0A00', '\u0A7F',
                         "GURMUKHI");

    /**
     * Gujarati.
     * '\u0A80' - '\u0AFF'.
     */
    public final static UnicodeBlock GUJARATI
      = new UnicodeBlock('\u0A80', '\u0AFF',
                         "GUJARATI");

    /**
     * Oriya.
     * '\u0B00' - '\u0B7F'.
     */
    public final static UnicodeBlock ORIYA
      = new UnicodeBlock('\u0B00', '\u0B7F',
                         "ORIYA");

    /**
     * Tamil.
     * '\u0B80' - '\u0BFF'.
     */
    public final static UnicodeBlock TAMIL
      = new UnicodeBlock('\u0B80', '\u0BFF',
                         "TAMIL");

    /**
     * Telugu.
     * '\u0C00' - '\u0C7F'.
     */
    public final static UnicodeBlock TELUGU
      = new UnicodeBlock('\u0C00', '\u0C7F',
                         "TELUGU");

    /**
     * Kannada.
     * '\u0C80' - '\u0CFF'.
     */
    public final static UnicodeBlock KANNADA
      = new UnicodeBlock('\u0C80', '\u0CFF',
                         "KANNADA");

    /**
     * Malayalam.
     * '\u0D00' - '\u0D7F'.
     */
    public final static UnicodeBlock MALAYALAM
      = new UnicodeBlock('\u0D00', '\u0D7F',
                         "MALAYALAM");

    /**
     * Sinhala.
     * '\u0D80' - '\u0DFF'.
     * @since 1.4
     */
    public final static UnicodeBlock SINHALA
      = new UnicodeBlock('\u0D80', '\u0DFF',
                         "SINHALA");

    /**
     * Thai.
     * '\u0E00' - '\u0E7F'.
     */
    public final static UnicodeBlock THAI
      = new UnicodeBlock('\u0E00', '\u0E7F',
                         "THAI");

    /**
     * Lao.
     * '\u0E80' - '\u0EFF'.
     */
    public final static UnicodeBlock LAO
      = new UnicodeBlock('\u0E80', '\u0EFF',
                         "LAO");

    /**
     * Tibetan.
     * '\u0F00' - '\u0FFF'.
     */
    public final static UnicodeBlock TIBETAN
      = new UnicodeBlock('\u0F00', '\u0FFF',
                         "TIBETAN");

    /**
     * Myanmar.
     * '\u1000' - '\u109F'.
     * @since 1.4
     */
    public final static UnicodeBlock MYANMAR
      = new UnicodeBlock('\u1000', '\u109F',
                         "MYANMAR");

    /**
     * Georgian.
     * '\u10A0' - '\u10FF'.
     */
    public final static UnicodeBlock GEORGIAN
      = new UnicodeBlock('\u10A0', '\u10FF',
                         "GEORGIAN");

    /**
     * Hangul Jamo.
     * '\u1100' - '\u11FF'.
     */
    public final static UnicodeBlock HANGUL_JAMO
      = new UnicodeBlock('\u1100', '\u11FF',
                         "HANGUL_JAMO");

    /**
     * Ethiopic.
     * '\u1200' - '\u137F'.
     * @since 1.4
     */
    public final static UnicodeBlock ETHIOPIC
      = new UnicodeBlock('\u1200', '\u137F',
                         "ETHIOPIC");

    /**
     * Cherokee.
     * '\u13A0' - '\u13FF'.
     * @since 1.4
     */
    public final static UnicodeBlock CHEROKEE
      = new UnicodeBlock('\u13A0', '\u13FF',
                         "CHEROKEE");

    /**
     * Unified Canadian Aboriginal Syllabics.
     * '\u1400' - '\u167F'.
     * @since 1.4
     */
    public final static UnicodeBlock UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS
      = new UnicodeBlock('\u1400', '\u167F',
                         "UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS");

    /**
     * Ogham.
     * '\u1680' - '\u169F'.
     * @since 1.4
     */
    public final static UnicodeBlock OGHAM
      = new UnicodeBlock('\u1680', '\u169F',
                         "OGHAM");

    /**
     * Runic.
     * '\u16A0' - '\u16FF'.
     * @since 1.4
     */
    public final static UnicodeBlock RUNIC
      = new UnicodeBlock('\u16A0', '\u16FF',
                         "RUNIC");

    /**
     * Khmer.
     * '\u1780' - '\u17FF'.
     * @since 1.4
     */
    public final static UnicodeBlock KHMER
      = new UnicodeBlock('\u1780', '\u17FF',
                         "KHMER");

    /**
     * Mongolian.
     * '\u1800' - '\u18AF'.
     * @since 1.4
     */
    public final static UnicodeBlock MONGOLIAN
      = new UnicodeBlock('\u1800', '\u18AF',
                         "MONGOLIAN");

    /**
     * Latin Extended Additional.
     * '\u1E00' - '\u1EFF'.
     */
    public final static UnicodeBlock LATIN_EXTENDED_ADDITIONAL
      = new UnicodeBlock('\u1E00', '\u1EFF',
                         "LATIN_EXTENDED_ADDITIONAL");

    /**
     * Greek Extended.
     * '\u1F00' - '\u1FFF'.
     */
    public final static UnicodeBlock GREEK_EXTENDED
      = new UnicodeBlock('\u1F00', '\u1FFF',
                         "GREEK_EXTENDED");

    /**
     * General Punctuation.
     * '\u2000' - '\u206F'.
     */
    public final static UnicodeBlock GENERAL_PUNCTUATION
      = new UnicodeBlock('\u2000', '\u206F',
                         "GENERAL_PUNCTUATION");

    /**
     * Superscripts and Subscripts.
     * '\u2070' - '\u209F'.
     */
    public final static UnicodeBlock SUPERSCRIPTS_AND_SUBSCRIPTS
      = new UnicodeBlock('\u2070', '\u209F',
                         "SUPERSCRIPTS_AND_SUBSCRIPTS");

    /**
     * Currency Symbols.
     * '\u20A0' - '\u20CF'.
     */
    public final static UnicodeBlock CURRENCY_SYMBOLS
      = new UnicodeBlock('\u20A0', '\u20CF',
                         "CURRENCY_SYMBOLS");

    /**
     * Combining Marks for Symbols.
     * '\u20D0' - '\u20FF'.
     */
    public final static UnicodeBlock COMBINING_MARKS_FOR_SYMBOLS
      = new UnicodeBlock('\u20D0', '\u20FF',
                         "COMBINING_MARKS_FOR_SYMBOLS");

    /**
     * Letterlike Symbols.
     * '\u2100' - '\u214F'.
     */
    public final static UnicodeBlock LETTERLIKE_SYMBOLS
      = new UnicodeBlock('\u2100', '\u214F',
                         "LETTERLIKE_SYMBOLS");

    /**
     * Number Forms.
     * '\u2150' - '\u218F'.
     */
    public final static UnicodeBlock NUMBER_FORMS
      = new UnicodeBlock('\u2150', '\u218F',
                         "NUMBER_FORMS");

    /**
     * Arrows.
     * '\u2190' - '\u21FF'.
     */
    public final static UnicodeBlock ARROWS
      = new UnicodeBlock('\u2190', '\u21FF',
                         "ARROWS");

    /**
     * Mathematical Operators.
     * '\u2200' - '\u22FF'.
     */
    public final static UnicodeBlock MATHEMATICAL_OPERATORS
      = new UnicodeBlock('\u2200', '\u22FF',
                         "MATHEMATICAL_OPERATORS");

    /**
     * Miscellaneous Technical.
     * '\u2300' - '\u23FF'.
     */
    public final static UnicodeBlock MISCELLANEOUS_TECHNICAL
      = new UnicodeBlock('\u2300', '\u23FF',
                         "MISCELLANEOUS_TECHNICAL");

    /**
     * Control Pictures.
     * '\u2400' - '\u243F'.
     */
    public final static UnicodeBlock CONTROL_PICTURES
      = new UnicodeBlock('\u2400', '\u243F',
                         "CONTROL_PICTURES");

    /**
     * Optical Character Recognition.
     * '\u2440' - '\u245F'.
     */
    public final static UnicodeBlock OPTICAL_CHARACTER_RECOGNITION
      = new UnicodeBlock('\u2440', '\u245F',
                         "OPTICAL_CHARACTER_RECOGNITION");

    /**
     * Enclosed Alphanumerics.
     * '\u2460' - '\u24FF'.
     */
    public final static UnicodeBlock ENCLOSED_ALPHANUMERICS
      = new UnicodeBlock('\u2460', '\u24FF',
                         "ENCLOSED_ALPHANUMERICS");

    /**
     * Box Drawing.
     * '\u2500' - '\u257F'.
     */
    public final static UnicodeBlock BOX_DRAWING
      = new UnicodeBlock('\u2500', '\u257F',
                         "BOX_DRAWING");

    /**
     * Block Elements.
     * '\u2580' - '\u259F'.
     */
    public final static UnicodeBlock BLOCK_ELEMENTS
      = new UnicodeBlock('\u2580', '\u259F',
                         "BLOCK_ELEMENTS");

    /**
     * Geometric Shapes.
     * '\u25A0' - '\u25FF'.
     */
    public final static UnicodeBlock GEOMETRIC_SHAPES
      = new UnicodeBlock('\u25A0', '\u25FF',
                         "GEOMETRIC_SHAPES");

    /**
     * Miscellaneous Symbols.
     * '\u2600' - '\u26FF'.
     */
    public final static UnicodeBlock MISCELLANEOUS_SYMBOLS
      = new UnicodeBlock('\u2600', '\u26FF',
                         "MISCELLANEOUS_SYMBOLS");

    /**
     * Dingbats.
     * '\u2700' - '\u27BF'.
     */
    public final static UnicodeBlock DINGBATS
      = new UnicodeBlock('\u2700', '\u27BF',
                         "DINGBATS");

    /**
     * Braille Patterns.
     * '\u2800' - '\u28FF'.
     * @since 1.4
     */
    public final static UnicodeBlock BRAILLE_PATTERNS
      = new UnicodeBlock('\u2800', '\u28FF',
                         "BRAILLE_PATTERNS");

    /**
     * CJK Radicals Supplement.
     * '\u2E80' - '\u2EFF'.
     * @since 1.4
     */
    public final static UnicodeBlock CJK_RADICALS_SUPPLEMENT
      = new UnicodeBlock('\u2E80', '\u2EFF',
                         "CJK_RADICALS_SUPPLEMENT");

    /**
     * Kangxi Radicals.
     * '\u2F00' - '\u2FDF'.
     * @since 1.4
     */
    public final static UnicodeBlock KANGXI_RADICALS
      = new UnicodeBlock('\u2F00', '\u2FDF',
                         "KANGXI_RADICALS");

    /**
     * Ideographic Description Characters.
     * '\u2FF0' - '\u2FFF'.
     * @since 1.4
     */
    public final static UnicodeBlock IDEOGRAPHIC_DESCRIPTION_CHARACTERS
      = new UnicodeBlock('\u2FF0', '\u2FFF',
                         "IDEOGRAPHIC_DESCRIPTION_CHARACTERS");

    /**
     * CJK Symbols and Punctuation.
     * '\u3000' - '\u303F'.
     */
    public final static UnicodeBlock CJK_SYMBOLS_AND_PUNCTUATION
      = new UnicodeBlock('\u3000', '\u303F',
                         "CJK_SYMBOLS_AND_PUNCTUATION");

    /**
     * Hiragana.
     * '\u3040' - '\u309F'.
     */
    public final static UnicodeBlock HIRAGANA
      = new UnicodeBlock('\u3040', '\u309F',
                         "HIRAGANA");

    /**
     * Katakana.
     * '\u30A0' - '\u30FF'.
     */
    public final static UnicodeBlock KATAKANA
      = new UnicodeBlock('\u30A0', '\u30FF',
                         "KATAKANA");

    /**
     * Bopomofo.
     * '\u3100' - '\u312F'.
     */
    public final static UnicodeBlock BOPOMOFO
      = new UnicodeBlock('\u3100', '\u312F',
                         "BOPOMOFO");

    /**
     * Hangul Compatibility Jamo.
     * '\u3130' - '\u318F'.
     */
    public final static UnicodeBlock HANGUL_COMPATIBILITY_JAMO
      = new UnicodeBlock('\u3130', '\u318F',
                         "HANGUL_COMPATIBILITY_JAMO");

    /**
     * Kanbun.
     * '\u3190' - '\u319F'.
     */
    public final static UnicodeBlock KANBUN
      = new UnicodeBlock('\u3190', '\u319F',
                         "KANBUN");

    /**
     * Bopomofo Extended.
     * '\u31A0' - '\u31BF'.
     * @since 1.4
     */
    public final static UnicodeBlock BOPOMOFO_EXTENDED
      = new UnicodeBlock('\u31A0', '\u31BF',
                         "BOPOMOFO_EXTENDED");

    /**
     * Enclosed CJK Letters and Months.
     * '\u3200' - '\u32FF'.
     */
    public final static UnicodeBlock ENCLOSED_CJK_LETTERS_AND_MONTHS
      = new UnicodeBlock('\u3200', '\u32FF',
                         "ENCLOSED_CJK_LETTERS_AND_MONTHS");

    /**
     * CJK Compatibility.
     * '\u3300' - '\u33FF'.
     */
    public final static UnicodeBlock CJK_COMPATIBILITY
      = new UnicodeBlock('\u3300', '\u33FF',
                         "CJK_COMPATIBILITY");

    /**
     * CJK Unified Ideographs Extension A.
     * '\u3400' - '\u4DB5'.
     * @since 1.4
     */
    public final static UnicodeBlock CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A
      = new UnicodeBlock('\u3400', '\u4DB5',
                         "CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A");

    /**
     * CJK Unified Ideographs.
     * '\u4E00' - '\u9FFF'.
     */
    public final static UnicodeBlock CJK_UNIFIED_IDEOGRAPHS
      = new UnicodeBlock('\u4E00', '\u9FFF',
                         "CJK_UNIFIED_IDEOGRAPHS");

    /**
     * Yi Syllables.
     * '\uA000' - '\uA48F'.
     * @since 1.4
     */
    public final static UnicodeBlock YI_SYLLABLES
      = new UnicodeBlock('\uA000', '\uA48F',
                         "YI_SYLLABLES");

    /**
     * Yi Radicals.
     * '\uA490' - '\uA4CF'.
     * @since 1.4
     */
    public final static UnicodeBlock YI_RADICALS
      = new UnicodeBlock('\uA490', '\uA4CF',
                         "YI_RADICALS");

    /**
     * Hangul Syllables.
     * '\uAC00' - '\uD7A3'.
     */
    public final static UnicodeBlock HANGUL_SYLLABLES
      = new UnicodeBlock('\uAC00', '\uD7A3',
                         "HANGUL_SYLLABLES");

    /**
     * Surrogates Area.
     * '\uD800' - '\uDFFF'.
     */
    public final static UnicodeBlock SURROGATES_AREA
      = new UnicodeBlock('\uD800', '\uDFFF',
                         "SURROGATES_AREA");

    /**
     * Private Use Area.
     * '\uE000' - '\uF8FF'.
     */
    public final static UnicodeBlock PRIVATE_USE_AREA
      = new UnicodeBlock('\uE000', '\uF8FF',
                         "PRIVATE_USE_AREA");

    /**
     * CJK Compatibility Ideographs.
     * '\uF900' - '\uFAFF'.
     */
    public final static UnicodeBlock CJK_COMPATIBILITY_IDEOGRAPHS
      = new UnicodeBlock('\uF900', '\uFAFF',
                         "CJK_COMPATIBILITY_IDEOGRAPHS");

    /**
     * Alphabetic Presentation Forms.
     * '\uFB00' - '\uFB4F'.
     */
    public final static UnicodeBlock ALPHABETIC_PRESENTATION_FORMS
      = new UnicodeBlock('\uFB00', '\uFB4F',
                         "ALPHABETIC_PRESENTATION_FORMS");

    /**
     * Arabic Presentation Forms-A.
     * '\uFB50' - '\uFDFF'.
     */
    public final static UnicodeBlock ARABIC_PRESENTATION_FORMS_A
      = new UnicodeBlock('\uFB50', '\uFDFF',
                         "ARABIC_PRESENTATION_FORMS_A");

    /**
     * Combining Half Marks.
     * '\uFE20' - '\uFE2F'.
     */
    public final static UnicodeBlock COMBINING_HALF_MARKS
      = new UnicodeBlock('\uFE20', '\uFE2F',
                         "COMBINING_HALF_MARKS");

    /**
     * CJK Compatibility Forms.
     * '\uFE30' - '\uFE4F'.
     */
    public final static UnicodeBlock CJK_COMPATIBILITY_FORMS
      = new UnicodeBlock('\uFE30', '\uFE4F',
                         "CJK_COMPATIBILITY_FORMS");

    /**
     * Small Form Variants.
     * '\uFE50' - '\uFE6F'.
     */
    public final static UnicodeBlock SMALL_FORM_VARIANTS
      = new UnicodeBlock('\uFE50', '\uFE6F',
                         "SMALL_FORM_VARIANTS");

    /**
     * Arabic Presentation Forms-B.
     * '\uFE70' - '\uFEFE'.
     */
    public final static UnicodeBlock ARABIC_PRESENTATION_FORMS_B
      = new UnicodeBlock('\uFE70', '\uFEFE',
                         "ARABIC_PRESENTATION_FORMS_B");

    /**
     * Halfwidth and Fullwidth Forms.
     * '\uFF00' - '\uFFEF'.
     */
    public final static UnicodeBlock HALFWIDTH_AND_FULLWIDTH_FORMS
      = new UnicodeBlock('\uFF00', '\uFFEF',
                         "HALFWIDTH_AND_FULLWIDTH_FORMS");

    /**
     * Specials.
     * '\uFEFF', '\uFFF0' - '\uFFFD'.
     */
    public final static UnicodeBlock SPECIALS
      = new UnicodeBlock('\uFFF0', '\uFFFD',
                         "SPECIALS");

    /**
     * The defined subsets.
     */
    private static final UnicodeBlock sets[] = {
      BASIC_LATIN,
      LATIN_1_SUPPLEMENT,
      LATIN_EXTENDED_A,
      LATIN_EXTENDED_B,
      IPA_EXTENSIONS,
      SPACING_MODIFIER_LETTERS,
      COMBINING_DIACRITICAL_MARKS,
      GREEK,
      CYRILLIC,
      ARMENIAN,
      HEBREW,
      ARABIC,
      SYRIAC,
      THAANA,
      DEVANAGARI,
      BENGALI,
      GURMUKHI,
      GUJARATI,
      ORIYA,
      TAMIL,
      TELUGU,
      KANNADA,
      MALAYALAM,
      SINHALA,
      THAI,
      LAO,
      TIBETAN,
      MYANMAR,
      GEORGIAN,
      HANGUL_JAMO,
      ETHIOPIC,
      CHEROKEE,
      UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS,
      OGHAM,
      RUNIC,
      KHMER,
      MONGOLIAN,
      LATIN_EXTENDED_ADDITIONAL,
      GREEK_EXTENDED,
      GENERAL_PUNCTUATION,
      SUPERSCRIPTS_AND_SUBSCRIPTS,
      CURRENCY_SYMBOLS,
      COMBINING_MARKS_FOR_SYMBOLS,
      LETTERLIKE_SYMBOLS,
      NUMBER_FORMS,
      ARROWS,
      MATHEMATICAL_OPERATORS,
      MISCELLANEOUS_TECHNICAL,
      CONTROL_PICTURES,
      OPTICAL_CHARACTER_RECOGNITION,
      ENCLOSED_ALPHANUMERICS,
      BOX_DRAWING,
      BLOCK_ELEMENTS,
      GEOMETRIC_SHAPES,
      MISCELLANEOUS_SYMBOLS,
      DINGBATS,
      BRAILLE_PATTERNS,
      CJK_RADICALS_SUPPLEMENT,
      KANGXI_RADICALS,
      IDEOGRAPHIC_DESCRIPTION_CHARACTERS,
      CJK_SYMBOLS_AND_PUNCTUATION,
      HIRAGANA,
      KATAKANA,
      BOPOMOFO,
      HANGUL_COMPATIBILITY_JAMO,
      KANBUN,
      BOPOMOFO_EXTENDED,
      ENCLOSED_CJK_LETTERS_AND_MONTHS,
      CJK_COMPATIBILITY,
      CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A,
      CJK_UNIFIED_IDEOGRAPHS,
      YI_SYLLABLES,
      YI_RADICALS,
      HANGUL_SYLLABLES,
      SURROGATES_AREA,
      PRIVATE_USE_AREA,
      CJK_COMPATIBILITY_IDEOGRAPHS,
      ALPHABETIC_PRESENTATION_FORMS,
      ARABIC_PRESENTATION_FORMS_A,
      COMBINING_HALF_MARKS,
      CJK_COMPATIBILITY_FORMS,
      SMALL_FORM_VARIANTS,
      ARABIC_PRESENTATION_FORMS_B,
      HALFWIDTH_AND_FULLWIDTH_FORMS,
      SPECIALS,
    };
  } // class UnicodeBlock

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
	    	propTable = getResource("kaffe/lang/unicode.tbl");
	    } catch (IOException e) {
		throw new Error("Missing Kaffe Unicode Database table: " + e);
	    }
	    byte tbl[];
	    try {
	    	tbl = getResource("kaffe/lang/unicode.idx");
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
		String pathSep = System.getProperties().getProperty("path.separator");
		String classpath = System.getProperties().getProperty("java.class.path");
		StringTokenizer t = new StringTokenizer(classpath, pathSep);
		InputStream in = null;
		while (t.hasMoreTokens()) {
			try {
				ZipFile zf = new ZipFile(t.nextToken());
				if (zf != null) {
					ZipEntry ze = zf.getEntry(name);
					if (ze != null) {
						in = zf.getInputStream(ze);
						break;
					}
				}
			}
			catch (IOException e) {
				/* Be more error tolerant: if a classpath
				 * entry is not existant or corrupted,
				 * ignore it.
				 *
				 * We can not print the exception since
				 * CharacterProperties are not initialized yet.
				 */
			}
		}
//		InputStream in = Character.class.getResourceAsStream(name);
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
