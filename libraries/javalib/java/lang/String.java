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
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.util.Comparator;
import java.util.Locale;
import kaffe.io.ByteToCharConverter;
import kaffe.io.CharToByteConverter;

public final class String implements Serializable, Comparable {

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
		if (sb.buffer.length > sb.used + STRINGBUFFER_SLOP) {
			value = new char[sb.used];
			offset = 0;
			count = sb.used;
			System.arraycopy(sb.buffer, 0, value, 0, count);
		}
		else {
			value = sb.buffer;
			offset = 0;
			count = sb.used;
			sb.isStringized = true;
		}
	}
}

public String(byte[] bytes) {
	this(decodeBytes(bytes, 0,
	      bytes.length, ByteToCharConverter.getDefault()));
}

public String(byte[] bytes, String enc) throws UnsupportedEncodingException {
	this(decodeBytes(bytes, 0,
	    bytes.length, ByteToCharConverter.getConverter(enc)));
}

/**
 * @deprecated
 */
public String(byte ascii[], int hibyte) {
	this(ascii, hibyte, 0, ascii.length);
}

public String(byte[] bytes, int offset, int length) {
	this(decodeBytes(bytes, offset,
	    length, ByteToCharConverter.getDefault()));
}

public String(byte[] bytes, int offset, int length, String enc)
		throws UnsupportedEncodingException {
	this(decodeBytes(bytes, offset,
	    length, ByteToCharConverter.getConverter(enc)));
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

public byte[] getBytes() {
	return ( getBytes( CharToByteConverter.getDefault()));
}

private byte[] getBytes( CharToByteConverter encoding) {
	ByteArrayOutputStream out = new ByteArrayOutputStream( value.length);

	byte[] buf = new byte[512];
	int buflen = encoding.convert( value, offset, count, buf, 0, buf.length);
	while (buflen > 0) {
		out.write(buf, 0, buflen);
		buflen = encoding.flush(buf, 0, buf.length);
	}

	return (out.toByteArray());
}

public byte[] getBytes( String enc) throws UnsupportedEncodingException
{
	return ( getBytes( CharToByteConverter.getConverter(enc)));
}

/**
 * @deprecated
 */
public void getBytes( int srcBegin, int srcEnd, byte dst[], int dstBegin) {
	if (srcBegin < 0 || srcEnd > offset + count) {
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

private static StringBuffer decodeBytes(byte[] bytes, int offset,
		int len, ByteToCharConverter encoding) {
	StringBuffer sbuf = new StringBuffer(len);
	char[] out = new char[512];
	int outlen = encoding.convert(bytes, offset, len, out, 0, out.length);
	while (outlen > 0) {
		sbuf.append(out, 0, outlen);
		outlen = encoding.flush(out, 0, out.length);
	}
	return sbuf;
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

public static String valueOf( Object obj) {
	return (obj == null) ? "null" : obj.toString();
}

public static String valueOf( boolean b) {
	return ( new Boolean(b)).toString();
}

public static String valueOf( char c) {
	return new String(new char[] { c });
}

public static String valueOf(char data[]) {
	return new String(data);
}

public static String valueOf( char data[], int offset, int count) {
	return new String( data, offset, count);
}

public static String valueOf( double d) {
	return Double.toString(d);
}

public static String valueOf( float f) {
	return Float.toString(f);
}

public static String valueOf( int i) {
	return Integer.toString(i);
}

public static String valueOf( long l) {
	return Long.toString(l);
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

}
