/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

// This is only a partial implementation of the JDK 1.1 spec.  We need
// to do all the necessary work to get Unicode happening - this is just
// an ASCII subset.

package java.lang;

public final class Character extends Object {

  public static final int MIN_RADIX = 2;
  public static final int MAX_RADIX = 36;
  public static final char MIN_VALUE = 0x0000;
  public static final char MAX_VALUE = 0xffff;
  public static final int UNASSIGNED = 0;
  public static final int UPPERCASE_LETTER = 1;
  public static final int LOWERCASE_LETTER = 2;
  public static final int TITLECASE_LETTER = 3;
  public static final int MODIFIER_LETTER = 4;
  public static final int OTHER_LETTER = 5;
  public static final int NON_SPACING_MARK = 6;
  public static final int ENCLOSING_MARK = 7;
  public static final int COMBINING_SPACING_MARK = 8;
  public static final int DECIMAL_DIGIT_NUMBER = 9;
  public static final int LETTER_NUMBER = 10;
  public static final int OTHER_NUMBER = 11;
  public static final int SPACE_SEPARATOR = 12;
  public static final int LINE_SEPARATOR = 13;
  public static final int PARAGRAPH_SEPARATOR = 14;
  public static final int CONTROL = 15;
  public static final int FORMAT = 16;
  public static final int PRIVATE_USE = 18;
  public static final int SURROGATE = 19;
  public static final int DASH_PUNCTUATION = 20;
  public static final int START_PUNCTUATION = 21;
  public static final int END_PUNCTUATION = 22;
  public static final int CONNECTOR_PUNCTUATION = 23;
  public static final int OTHER_PUNCTUATION = 24;
  public static final int MATH_SYMBOL = 25;
  public static final int CURRENCY_SYMBOL = 26;
  public static final int MODIFIER_SYMBOL = 27;
  public static final int OTHER_SYMBOL = 28;

  public static final Class TYPE = Class.getPrimitiveClass("char");

  private final char value;

  public Character(char value)
  {
    this.value=value;
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
    if (obj instanceof Character) {
      return (charValue() == ((Character)obj).charValue());
    }
    else {
      return (false);
    }    
  }

  public String toString()
  {
    return (String.valueOf(value));
  }

  public static boolean isSpace(char ch)
  {
    return (isWhitespace(ch));
  }

  public static boolean isLetterOrDigit(char ch)
  {
    return (isLetter(ch) || isDigit(ch));
  }
  
  public static boolean isJavaLetter(char ch)
  {
    return (isJavaIdentifierStart(ch));
  }
  
  public static boolean isJavaLetterOrDigit(char ch)
  {
    return (isJavaIdentifierPart(ch));
  }

  public static boolean isTitleCase(char ch)
  {
    return (ch == 0x01C5 || ch == 0x01C8 || ch == 0x01CB || ch == 0x01F2);
  }
  
  public static char toTitleCase(char ch)
  {
    if (ch == 0x01C6) {
      return (0x01C6);
    }
    else if (ch == 0x01C9) {
      return (0x01C8);
    }
    else if (ch == 0x01CC) {
      return (0x01CB);
    }
    else if (ch == 0x01F3) {
      return (0x01F2);
    }
    else {
      return (ch);
    }
  }

  /**
   * Determines if a character has a defined meaning in Unicode. 
   * A character is defined if at least one of the following is true:  <br>
   *   It has an entry in the Unicode attribute table.  <br>
   *   Its value is in the range 0x3040 <= ch <= 0x9FA5.  <br>
   *   Its value is in the range 0xF900 <= ch <= 0xFA2D.  <br>
   */
  public static boolean isDefined(char ch) {
	if (0x3040 <= ch && ch <= 0x9FA5 ||
	    0xF900 <= ch && ch <= 0xFA2D) {
		return (true);
	}
	/* FIXME: we do not check for entry in Unicode attribute table.
	 * See comment at beginning of file that this impl is partial.
	 */
	return (ch < 256);
  }

  public static boolean isIdentifierIgnorable(char ch)
  {
    if ((ch >= 0x0000 && ch <= 0x0008) ||
        (ch >= 0x000E && ch <= 0x001B) ||
        (ch >= 0x007F && ch <= 0x009F) ||
        (ch >= 0x200C && ch <= 0x200F) ||
        (ch >= 0x202A && ch <= 0x202E) ||
        (ch >= 0x206A && ch <= 0x206F) ||
        (ch == 0xFEFF)) {
      return (true);
    }
    else {
      return (false);
    }
  }

  public static boolean isLowerCase(char ch)
  {
    return (ch >= 'a' && ch <= 'z');
  }

  public static boolean isUpperCase(char ch)
  {
    return (ch >= 'A' && ch <= 'Z');
  }

  public static boolean isDigit(char ch)
  {
    return (ch >= '0' && ch <= '9');
  }

  public static boolean isLetter(char ch)
  {
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
  }

  public static char toLowerCase(char ch)
  {
    char result=ch;

    if (isUpperCase(ch)) result=(char )(((int )ch)+32);

    return result;
  }

  public static char toUpperCase(char ch)
  {
    char result=ch;

    if (isLowerCase(ch)) result=(char )(((int )ch)-32);

    return result;
  }

  public static int digit(char ch, int radix)
  {
    if (radix < MIN_RADIX || radix > MAX_RADIX) {
      return -1;
    }

    int val = radix;
    if (isDigit(ch)) {
      val = ch - '0';
    }
    else if (isLetter(ch)) {
      val = ((int)toLowerCase(ch)) - ((int)'a') + 10;
    }

    if (val < radix) {
      return (val);
    }
    else {
      return (-1);
    }
  }

  public static char forDigit(int digit, int radix)
  {
    if ((radix<MIN_RADIX) || (radix>MAX_RADIX)) return 0x0000;
    if (digit>radix) return 0x0000;

    if (digit<10) {
      return (char )(((int )'0')+digit);
    }
    else {
      return (char )(((int )'a')+(digit-10));
    }
  }

  public static int getNumericValue(char ch)
  {
    return (digit(ch, 10));
  }

  public static int getType(char ch)
  {
    return (UNASSIGNED);
  }

  public static boolean isISOControl(char ch)
  {
    if ((ch >= 0x0000 && ch <= 0x001F) ||
        (ch >= 0x007F && ch <= 0x009F)) {
      return (true);
    }
    else {
      return (false);
    }
  }

  public static boolean isJavaIdentifierPart(char ch)
  {
      return ((ch >= 0x0000 && ch <= 0x0008) ||
	(ch >= 0x000e && ch <= 0x001b) ||
	(ch >= 0x007f && ch <= 0x009f) ||
	ch == '$' || ch == '_' ||
        isLetter(ch) || isDigit(ch));
  }

  public static boolean isJavaIdentifierStart(char ch)
  {
    return (isLetter(ch) || ch == '$' || ch == '_');
  }

  public static boolean isSpaceChar(char ch)
  {
      return (ch == ' ');
  }

  public static boolean isUnicodeIdentifierPart(char ch)
  {
      return (ch != '$' && (isLetter(ch) || isJavaIdentifierPart(ch)));
  }

  public static boolean isUnicodeIdentifierStart(char ch)
  {
      return (isLetter(ch));
  }

  public static boolean isWhitespace(char ch)
  {
    return((ch == ' ')
	|| (ch >= 0x0009 && ch <= 0x000d)
	|| (ch >= 0x001c && ch <= 0x001f) );
  }
}

