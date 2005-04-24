/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

import java.io.ByteArrayOutputStream;
import java.io.CharConversionException;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;
import java.nio.charset.CharsetEncoder;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CodingErrorAction;
import java.nio.charset.CharacterCodingException;
import java.nio.charset.IllegalCharsetNameException;
import java.nio.charset.UnsupportedCharsetException;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.util.Comparator;
import java.util.Locale;
import java.util.regex.Pattern;

public final class String implements Serializable, Comparable, CharSequence {

	/**
	 * Maximum slop (extra unused chars in the char[]) that
	 * will be accepted in a StringBuffer -> String conversion.
	 * This helps avoid certain pathological cases where lots of
	 * extra buffer space is maintained for short strings that
	 * were created from StringBuffer objects.
	 */
	static final int STRINGBUFFER_SLOP = 32;

	// Note: value, offset, and count are not private, because
	// StringBuffer uses them for faster access
	final char[] value;
	final int offset;
	final int count;
	private boolean interned;
	private int hash;

	/* This is what Sun's JDK1.1 "serialver java.lang.String" spits out */
	private static final long serialVersionUID = -6849794470754667710L;

	public static final Comparator CASE_INSENSITIVE_ORDER = new ICComp();

	private static class ICComp implements Comparator, Serializable {
	  public int compare(Object s1, Object s2) {
	    return ((String)s1).compareToIgnoreCase((String)s2);
	  }
	}

public String() {
	value = new char[0];
	offset = 0;
	count = 0;
}

public String(String other) {
	value = other.value;
	offset = other.offset;
	count = other.count;
	hash = other.hash;
}

public String(StringBuffer sb) {
	synchronized (sb) {
		if (sb.value.length > sb.count + STRINGBUFFER_SLOP) {
			value = new char[sb.count];
			offset = 0;
			count = sb.count;
			sb.getChars(0, count, value, 0);
		}
		else {
			value = sb.value;
			offset = 0;
			count = sb.count;
			sb.shared = true;
		}
	}
}

public String(byte[] bytes) {
	this(bytes, 0, bytes.length);
}

/* taken from GNU Classpath */
  public String(byte[] data, String encoding)
    throws UnsupportedEncodingException
  {
    this(data, 0, data.length, encoding);
  }

/**
 * @deprecated
 */
public String(byte ascii[], int hibyte) {
	this(ascii, hibyte, 0, ascii.length);
}

/* taken from GNU Claspath */
  public String(byte[] data, int offset, int count)
  {
    if (offset < 0 || count < 0 || offset + count > data.length)
      throw new StringIndexOutOfBoundsException();
    int o, c;
    char[] v;
    String encoding;
    try 
	{
	  encoding = System.getProperty("file.encoding");
	  CharsetDecoder csd = Charset.forName(encoding).newDecoder();
	  csd.onMalformedInput(CodingErrorAction.REPLACE);
	  csd.onUnmappableCharacter(CodingErrorAction.REPLACE);
	  CharBuffer cbuf = csd.decode(ByteBuffer.wrap(data, offset, count));
	  if(cbuf.hasArray())
	    {
              v = cbuf.array();
	      o = cbuf.position();
	      c = cbuf.remaining();
	    } else {
	      // Doubt this will happen. But just in case.
	      v = new char[cbuf.remaining()];
	      cbuf.get(v);
	      o = 0;
	      c = v.length;
	    }
	} catch(Exception ex){
	    // If anything goes wrong (System property not set,
	    // NIO provider not available, etc)
	    // Default to the 'safe' encoding ISO8859_1
	    v = new char[count];
	    o = 0;
	    c = count;
	    for (int i=0;i<count;i++)
	      v[i] = (char)data[offset+i];
	}
    this.value = v;
    this.offset = o;
    this.count = c;
  }

/* taken from GNU Claspath */
  public String(byte[] data, int offset, int count, String encoding)
    throws UnsupportedEncodingException
  {
    if (offset < 0 || count < 0 || offset + count > data.length)
      throw new StringIndexOutOfBoundsException();
    try 
      {
        CharsetDecoder csd = Charset.forName(encoding).newDecoder();
	csd.onMalformedInput(CodingErrorAction.REPLACE);
	csd.onUnmappableCharacter(CodingErrorAction.REPLACE);
	CharBuffer cbuf = csd.decode(ByteBuffer.wrap(data, offset, count));
 	if(cbuf.hasArray())
 	  {
 	    value = cbuf.array();
	    this.offset = cbuf.position();
	    this.count = cbuf.remaining();
 	  } else {
	    // Doubt this will happen. But just in case.
	    value = new char[cbuf.remaining()];
	    cbuf.get(value);
	    this.offset = 0;
	    this.count = value.length;
	  }
      } catch(CharacterCodingException e){
	  throw new UnsupportedEncodingException("Encoding: "+encoding+
						 " not found.");	  
      } catch(IllegalCharsetNameException e){
	  throw new UnsupportedEncodingException("Encoding: "+encoding+
						 " not found.");
      } catch(UnsupportedCharsetException e){
	  throw new UnsupportedEncodingException("Encoding: "+encoding+
						 " not found.");
      }    
  }

/**
 * @deprecated
 */
public String( byte ascii[], int hibyte, int offset, int count) {
	// Test needed to conform to the spec. - TIM
	if (ascii == null) {
		throw new NullPointerException();
	}
	value = new char[count];
	this.offset = 0;
	this.count = count;

	hibyte = (hibyte & 0xFF) << 8;
	for (int pos = 0; pos < count; pos++) {
		value[pos]=(char)(hibyte | (ascii[pos+offset] & 0xFF));	
	}
}

public String( char other[]) {
	this( other, 0, other.length);
}

public String( char other[], int offset, int count) {
	if (count < 0)
		throw new StringIndexOutOfBoundsException();
	value = new char[count];
	this.offset = 0;
	this.count = count;    
	System.arraycopy( other, offset, value, 0, count);
}

String(int sIdx, int eIdx, char[] val) {
	value  = val;
	offset = sIdx;
	count  = eIdx - sIdx;
}

public char charAt ( int index ) {
	if (( index < 0) || ( index >= count))
		throw new StringIndexOutOfBoundsException("index = "+index+", length="+count);

	return value[offset+index];
}

public int compareTo(Object o) {
	return compareTo((String)o);
}

public int compareTo( String s1) {
	/* lexicographical comparison, assume they mean English lexiographical, since Character has no ordering */

	int minLen = Math.min( count, s1.count);
	char c, c1;

	for ( int pos=0; pos<minLen; pos++) {
		/* Can we guarantee that the Unicode '<' relation means also numerically '<'.. Probably, but just incase */
		c = value[offset+pos]; c1 = s1.value[s1.offset+pos];
		if ( c != c1) return ( c-c1);
	}

	/* Both equal up to min length, therefore check lengths for lexiographical ordering */
	return ( count - s1.count);
}

public int compareToIgnoreCase(String that) {
	return this.toUpperCase().toLowerCase().compareTo(
		that.toUpperCase().toLowerCase());
}

public String concat(String str) {
	if (str.count == 0) {
		return (this);
	}
	char buf[] = new char[count + str.count];
	getChars(0, count, buf, 0);
	str.getChars(0, str.count, buf, count);
	return (new String(0, buf.length, buf));
}

public static String copyValueOf( char data[]) {
	return copyValueOf( data, 0, data.length);
}

public static String copyValueOf(char data[], int offset, int count) {
	if (offset < 0 || count < 0 || offset + count > data.length) {
		throw new IndexOutOfBoundsException();
	}
	char buf[]=new char[count];
	if ( count > 0) {
		System.arraycopy( data, offset, buf, 0, count);
	}
	return (new String( 0, count, buf));
}

public boolean endsWith( String suffix) {
	return regionMatches( false, count-suffix.count, suffix, 0, suffix.count);
}

// This is one of the most frequently called methods; it must be as
// efficient as possible.
public boolean equals (Object anObject) {

	if (anObject == this) {
		return (true);
	}
	if (!(anObject instanceof String)) {
		return (false);
	}
	final String that = (String)anObject;

	if (this.count != that.count) {
		return (false);
	}

	int i = this.offset;
	int j = that.offset;
	final int n = i + this.count;
	final char[] this_value = this.value;
	final char[] that_value = that.value;
	for (; i < n; i++, j++) {
		if (this_value[i] != that_value[j] ) {
			return (false);
		}
	}
	return (true);
}

// This also should be as fast as possible
public boolean equalsIgnoreCase (String that) {
	if (that == null || this.count != that.count) {
		return (false);
	}

	int i = this.offset;
	int j = that.offset;
	final int n = i + this.count;
	final char[] this_value = this.value;
	final char[] that_value = that.value;
	for (; i < n; i++, j++) {
		if (this_value[i] != that_value[j]
		   && Character.toUpperCase(this_value[i])
		     != Character.toUpperCase(that_value[j])) {
			return (false);
		}
	}
	return (true);
}

    /* Taken from GNU Classpath */
  public byte[] getBytes()
  { 
      try 
	  {
	      return getBytes(System.getProperty("file.encoding"));
	  } catch(Exception e) {
	      // XXX - Throw an error here? 
	      // For now, default to the 'safe' encoding.
	      byte[] bytes = new byte[count];
	      for(int i=0;i<count;i++)
		  bytes[i] = (byte)((value[offset+i] <= 0xFF)?
				    value[offset+i]:'?');
	      return bytes;
      }
  }

    /* Taken from GNU Classpath */
  public byte[] getBytes(String enc) throws UnsupportedEncodingException
  {
    try 
      {
	CharsetEncoder cse = Charset.forName(enc).newEncoder();
	cse.onMalformedInput(CodingErrorAction.REPLACE);
	cse.onUnmappableCharacter(CodingErrorAction.REPLACE);
	ByteBuffer bbuf = cse.encode(CharBuffer.wrap(value, offset, count));
	if(bbuf.hasArray())
	  return bbuf.array();

	// Doubt this will happen. But just in case.
	byte[] bytes = new byte[bbuf.remaining()];
	bbuf.get(bytes);
	return bytes;

      } catch(IllegalCharsetNameException e){
	  throw new UnsupportedEncodingException("Encoding: "+enc+
						 " not found.");
      } catch(UnsupportedCharsetException e){
	  throw new UnsupportedEncodingException("Encoding: "+enc+
						 " not found.");
      } catch(CharacterCodingException e){
	  // XXX - Ignore coding exceptions? They shouldn't really happen.
	  return null;
      }	  
  }

/**
 * @deprecated
 */
public void getBytes( int srcBegin, int srcEnd, byte dst[], int dstBegin) {
	if (srcBegin < 0 
	    || srcBegin > srcEnd
	    ||  dstBegin < 0
	    ||  dstBegin + (srcEnd - srcBegin) >  dst.length
	    || srcEnd > offset + count) {
		throw new IndexOutOfBoundsException("");
	}
	int len = srcEnd-srcBegin;
	for (int pos = 0; pos < len; pos++) {
		dst[dstBegin+pos] = (byte)value[offset+srcBegin+pos];
	}
}

public void getChars(int srcBegin, int srcEnd, char dst[], int dstBegin) {
	System.arraycopy( value, offset+srcBegin, dst, dstBegin, srcEnd-srcBegin);
}

public int hashCode() {
	if (hash == 0 && count > 0) {
		int tempHash = 0;
		final int stop = offset + count;
		for (int index = offset; index < stop; index++) {
			tempHash = (31 * tempHash) + value[index];
		}
		hash = tempHash;	// race condition here is ok
	}
	return hash;
}

public int indexOf( String str) {
	return indexOf( str, 0);
}

native public int indexOf( String str, int sIdx);

public int indexOf( int ch) {
	return indexOf( ch, 0);
}

public int indexOf( int ch, int sIdx) {
	char c = (char)ch;
	if (sIdx < 0) { // calling indexOf with sIdx < 0 is apparently okay
		sIdx = 0;
	}
	for (int pos=sIdx; pos<count; pos++) {
		if ( value[offset+pos] == c )
			return pos;
	}

	return -1;
}

public int lastIndexOf( String str) {
	return lastIndexOf( str, count);
}

public int lastIndexOf( String str, int eIdx) {
	int ic = str.offset+str.count-1;
	int it = offset+eIdx+str.count-1;
	int ma = 0;

	if (it >= offset+count) {       // clip index
		it = offset+count-1;
	}

	if (str.count == 0) {
		return (eIdx < 0) ? -1 : (eIdx < count) ? eIdx : count;
	}

	for ( ; it>=offset; it--) {
		if ( value[it] == str.value[ic] ) {
			ic--;
			if ( ++ma == str.count) {
				return (it-offset);
			}
		}
		else if (ma > 0) {
			it++;
			ma = 0;
			ic = str.offset+str.count-1;
		}
	}
	return -1;
}

public int lastIndexOf(int ch) {
	return lastIndexOf( ch, count-1);
}

public int lastIndexOf(int ch, int eIdx) {
	final char c = (char)ch;

	/* If the character is out of range we'll never find it */
	if ((int)c != ch) {
		return (-1);
	}

	/* Clip the index to be within the valid range (if non-empty) */
	if (eIdx >= count) {
		eIdx = count - 1;
	}
	if (eIdx < 0) {
		return(-1);
	}

	/* Search for character */
	for (int pos = eIdx; pos >= 0; pos--) {
		if ( value[offset+pos] == c) {
			return (pos);
		}
	}
	return (-1);
}

public int length() {
	return count;
}

public boolean regionMatches(boolean ignoreCase, int thisOffset,
		String that, int thatOffset, int len) {

	// Check bounds
	if ((thisOffset < 0 || thisOffset + len > this.count)
	    || (thatOffset < 0 || thatOffset + len > that.count)) {
		return false;
	}

	int thisPos = this.offset + thisOffset;
	int thatPos = that.offset + thatOffset;
	if (!ignoreCase) {
		while (len-- > 0) {
			if (this.value[thisPos] != that.value[thatPos]) {
				return false;
			}
			thisPos++;
			thatPos++;
		}
	} else {
		while (len-- > 0) {
			if (Character.toLowerCase(this.value[thisPos])
			    != Character.toLowerCase(that.value[thatPos])
			  && Character.toUpperCase(this.value[thisPos])
			    != Character.toUpperCase(that.value[thatPos])) {
				return false;
			}
			thisPos++;
			thatPos++;
		}
	}
	return true;
}

public boolean regionMatches( int toffset, String other, int ooffset, int len) {
	return regionMatches( false, toffset, other, ooffset, len);
}

public String replace(char oldChar, char newChar) {
	if (oldChar == newChar) {
		return (this);
	}

	char buf[] = new char[count];
	boolean replaced = false;

	for (int pos = 0; pos < count; pos++) {
		char cc = value[offset+pos];
		if ( cc == oldChar) {
			replaced = true;
			buf[pos] = newChar;
		}
		else {
			buf[pos] = cc;
		}
	}

	if (!replaced) {
		return (this);
	}
	else {
		return (new String( 0, count, buf));
	}
}

public boolean startsWith( String prefix) {
	return regionMatches( false, 0, prefix, 0, prefix.count);
}

public boolean startsWith( String prefix, int toffset) {
	return regionMatches( false, toffset, prefix, 0, prefix.count);
}

public String substring( int sIdx) {
	return substring( sIdx, count);
}

/*
 * shared data
 */
public String substring( int sIdx, int eIdx) {
	if ( sIdx < 0)     throw new StringIndexOutOfBoundsException( sIdx);
	if ( eIdx > count) throw new StringIndexOutOfBoundsException( eIdx);
	if ( sIdx > eIdx)  throw new StringIndexOutOfBoundsException( eIdx-sIdx);

	if ( ( sIdx == 0) && ( eIdx  == count ) )
		return this;

	return new String( offset+sIdx, offset+eIdx, value);

}

public CharSequence subSequence( int sIdx, int eIdx) {
        return substring( sIdx, eIdx);
}

public char[] toCharArray() {
	char buf[] = new char[count];
	if ( count > 0)
		getChars( 0, count, buf, 0);

	return buf;
}

public String toLowerCase() {
	return toLowerCase( Locale.getDefault());
}

public String toLowerCase( Locale lcl) {
	char buf[] = new char[count];
	for (int pos = 0; pos < count; pos++)
		buf[pos] = Character.toLowerCase( value[offset+pos]);

	return new String( 0, count, buf);
}

public String toString() {
	return this;
}

public String toUpperCase() {
	return toUpperCase( Locale.getDefault());
}

public String toUpperCase( Locale lcl) {
	char buf[] = new char[count];
	for (int pos=0; pos < count; pos++)
		buf[pos] = Character.toUpperCase( value[offset+pos]);

	return new String( 0, count, buf);
}

public String trim() {
	int i0 = offset;
	int i1 = offset+count-1;

	for ( ;(i0 <= i1) && (value[i0] <= ' '); i0++ );
	if ( i0 > i1 ) return "";
	for ( ;(i1 > i0) && (value[i1] <= ' '); i1-- );

	return substring( i0-offset, i1+1-offset);
}

    /* valueOf methods taken from GNU Classpath */
  /**
   * Returns a String representation of an Object. This is "null" if the
   * object is null, otherwise it is <code>obj.toString()</code> (which
   * can be null).
   *
   * @param obj the Object
   * @return the string conversion of obj
   */
  public static String valueOf(Object obj)
  {
    return obj == null ? "null" : obj.toString();
  }

  /**
   * Returns a String representation of a character array. Subsequent
   * changes to the array do not affect the String.
   *
   * @param data the character array
   * @return a String containing the same character sequence as data
   * @throws NullPointerException if data is null
   * @see #valueOf(char[], int, int)
   * @see #String(char[])
   */
  public static String valueOf(char[] data)
  {
    return valueOf (data, 0, data.length);
  }

  /**
   * Returns a String representing the character sequence of the char array,
   * starting at the specified offset, and copying chars up to the specified
   * count. Subsequent changes to the array do not affect the String.
   *
   * @param data character array
   * @param offset position (base 0) to start copying out of data
   * @param count the number of characters from data to copy
   * @return String containing the chars from data[offset..offset+count]
   * @throws NullPointerException if data is null
   * @throws IndexOutOfBoundsException if (offset &lt; 0 || count &lt; 0
   *         || offset + count &gt; data.length)
   *         (while unspecified, this is a StringIndexOutOfBoundsException)
   * @see #String(char[], int, int)
   */
  public static String valueOf(char[] data, int offset, int count)
  {
    return new String(data, offset, count, false);
  }

  /**
   * Returns a String representing a boolean.
   *
   * @param b the boolean
   * @return "true" if b is true, else "false"
   */
  public static String valueOf(boolean b)
  {
    return b ? "true" : "false";
  }

  /**
   * Returns a String representing a character.
   *
   * @param c the character
   * @return String containing the single character c
   */
  public static String valueOf(char c)
  {
    // Package constructor avoids an array copy.
    return new String(new char[] { c }, 0, 1, true);
  }

  /**
   * Returns a String representing an integer.
   *
   * @param i the integer
   * @return String containing the integer in base 10
   * @see Integer#toString(int)
   */
  public static String valueOf(int i)
  {
    // See Integer to understand why we call the two-arg variant.
    return Integer.toString(i, 10);
  }

  /**
   * Returns a String representing a long.
   *
   * @param l the long
   * @return String containing the long in base 10
   * @see Long#toString(long)
   */
  public static String valueOf(long l)
  {
    return Long.toString(l);
  }

  /**
   * Returns a String representing a float.
   *
   * @param f the float
   * @return String containing the float
   * @see Float#toString(float)
   */
  public static String valueOf(float f)
  {
    return Float.toString(f);
  }

  /**
   * Returns a String representing a double.
   *
   * @param d the double
   * @return String containing the double
   * @see Double#toString(double)
   */
  public static String valueOf(double d)
  {
    return Double.toString(d);
  }

public String intern() {
	return interned ? this : intern0(this);
}

private native static synchronized String intern0(String str);

/* Custom only ....
protected void finalize() throws Throwable {
	if (interned == true) {
		unintern0(this);
	}
	super.finalize();
}

final native public static synchronized void unintern0(String str);
*/

  /**
   * Creates a new String using the character sequence represented by
   * the StringBuilder. Subsequent changes to buf do not affect the String.
   *
   * @param buffer StringBuilder to copy
   * @throws NullPointerException if buffer is null
   */
  public String(StringBuilder buffer)
  {
    this(buffer.value, 0, buffer.count);
  }

  /**
   * Special constructor which can share an array when safe to do so.
   *
   * @param data the characters to copy
   * @param offset the location to start from
   * @param count the number of characters to use
   * @param dont_copy true if the array is trusted, and need not be copied
   * @throws NullPointerException if chars is null
   * @throws StringIndexOutOfBoundsException if bounds check fails
   */
  String(char[] data, int offset, int count, boolean dont_copy)
  {
    if (offset < 0 || count < 0 || offset + count > data.length)
      throw new StringIndexOutOfBoundsException();
    if (dont_copy)
      {
        value = data;
        this.offset = offset;
      }
    else
      {
        value = new char[count];
        System.arraycopy(data, offset, value, 0, count);
        this.offset = 0;
      }
    this.count = count;
  }


  /**
   * Returns the value array of the given string if it is zero based or a
   * copy of it that is zero based (stripping offset and making length equal
   * to count). Used for accessing the char[]s of gnu.java.lang.CharData.
   * Package private for use in Character.
   */
  static char[] zeroBasedStringValue(String s)
  {
    char[] value;

    if (s.offset == 0 && s.count == s.value.length)
      value = s.value;
    else
      {
        int count = s.count;
        value = new char[count];
        System.arraycopy(s.value, s.offset, value, 0, count);
      }

    return value;
  }

  /**
   * Test if this String matches a regular expression. This is shorthand for
   * <code>{@link Pattern}.matches(regex, this)</code>.
   *
   * @param regex the pattern to match
   * @return true if the pattern matches
   * @throws NullPointerException if regex is null
   * @throws PatternSyntaxException if regex is invalid
   * @see Pattern#matches(String, CharSequence)
   * @since 1.4
   */
  public boolean matches(String regex)
  {
    return Pattern.matches(regex, this);
  }

  /**
   * Replaces the first substring match of the regular expression with a
   * given replacement. This is shorthand for <code>{@link Pattern}
   *   .compile(regex).matcher(this).replaceFirst(replacement)</code>.
   *
   * @param regex the pattern to match
   * @param replacement the replacement string
   * @return the modified string
   * @throws NullPointerException if regex or replacement is null
   * @throws PatternSyntaxException if regex is invalid
   * @see #replaceAll(String, String)
   * @see Pattern#compile(String)
   * @see Pattern#matcher(CharSequence)
   * @see Matcher#replaceFirst(String)
   * @since 1.4
   */
  public String replaceFirst(String regex, String replacement)
  {
    return Pattern.compile(regex).matcher(this).replaceFirst(replacement);
  }

  /**
   * Replaces all matching substrings of the regular expression with a
   * given replacement. This is shorthand for <code>{@link Pattern}
   *   .compile(regex).matcher(this).replaceAll(replacement)</code>.
   *
   * @param regex the pattern to match
   * @param replacement the replacement string
   * @return the modified string
   * @throws NullPointerException if regex or replacement is null
   * @throws PatternSyntaxException if regex is invalid
   * @see #replaceFirst(String, String)
   * @see Pattern#compile(String)
   * @see Pattern#matcher(CharSequence)
   * @see Matcher#replaceAll(String)
   * @since 1.4
   */
  public String replaceAll(String regex, String replacement)
  {
    return Pattern.compile(regex).matcher(this).replaceAll(replacement);
  }

  /**
   * Split this string around the matches of a regular expression. Each
   * element of the returned array is the largest block of characters not
   * terminated by the regular expression, in the order the matches are found.
   *
   * <p>The limit affects the length of the array. If it is positive, the
   * array will contain at most n elements (n - 1 pattern matches). If
   * negative, the array length is unlimited, but there can be trailing empty
   * entries. if 0, the array length is unlimited, and trailing empty entries
   * are discarded.
   *
   * <p>For example, splitting "boo:and:foo" yields:<br>
   * <table border=0>
   * <th><td>Regex</td> <td>Limit</td> <td>Result</td></th>
   * <tr><td>":"</td>   <td>2</td>  <td>{ "boo", "and:foo" }</td></tr>
   * <tr><td>":"</td>   <td>t</td>  <td>{ "boo", "and", "foo" }</td></tr>
   * <tr><td>":"</td>   <td>-2</td> <td>{ "boo", "and", "foo" }</td></tr>
   * <tr><td>"o"</td>   <td>5</td>  <td>{ "b", "", ":and:f", "", "" }</td></tr>
   * <tr><td>"o"</td>   <td>-2</td> <td>{ "b", "", ":and:f", "", "" }</td></tr>
   * <tr><td>"o"</td>   <td>0</td>  <td>{ "b", "", ":and:f" }</td></tr>
   * </table>
   *
   * <p>This is shorthand for
   * <code>{@link Pattern}.compile(regex).split(this, limit)</code>.
   *
   * @param regex the pattern to match
   * @param limit the limit threshold
   * @return the array of split strings
   * @throws NullPointerException if regex or replacement is null
   * @throws PatternSyntaxException if regex is invalid
   * @see Pattern#compile(String)
   * @see Pattern#split(CharSequence, int)
   * @since 1.4
   */
  public String[] split(String regex, int limit)
  {
    return Pattern.compile(regex).split(this, limit);
  }

  /**
   * Split this string around the matches of a regular expression. Each
   * element of the returned array is the largest block of characters not
   * terminated by the regular expression, in the order the matches are found.
   * The array length is unlimited, and trailing empty entries are discarded,
   * as though calling <code>split(regex, 0)</code>.
   *
   * @param regex the pattern to match
   * @return the array of split strings
   * @throws NullPointerException if regex or replacement is null
   * @throws PatternSyntaxException if regex is invalid
   * @see #split(String, int)
   * @see Pattern#compile(String)
   * @see Pattern#split(CharSequence, int)
   * @since 1.4
   */
  public String[] split(String regex)
  {
    return Pattern.compile(regex).split(this, 0);
  }
}
