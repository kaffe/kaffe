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
import java.io.UnsupportedEncodingException;
import java.io.Serializable;
import java.util.Locale;
import kaffe.io.ByteToCharConverter;
import kaffe.io.CharToByteConverter;

final public class String implements Serializable {

	// Note: value, offset, and count are not private, because
	// StringBuffer uses them for faster access
	char[] value;
	int offset;
	int count;
	int hash;
	boolean interned;

	/* This is what Sun's JDK1.1 "serialver java.lang.String" spits out */
	static final long serialVersionUID = -6849794470754667710L;

public String() {
	value = new char[0];
}

public String(String other) {
	value = other.value;
	offset = other.offset;
	count = other.count;
	hash = other.hash;
}

public String (StringBuffer sb) {

	// mark this StringBuffer so that it knows we are using it
	sb.isStringized = true; 

	count = sb.used;
	value = sb.buffer;
}

public String( byte[] bytes) {
	initString( bytes, 0, bytes.length, ByteToCharConverter.getDefault());
}

public String( byte[] bytes, String enc) throws UnsupportedEncodingException
{
	initString( bytes, 0, bytes.length, ByteToCharConverter.getConverter(enc));
}

/**
 * @deprecated
 */
public String( byte ascii[], int hibyte) {
	this( ascii, hibyte, 0, ascii.length);
}

public String( byte[] bytes, int offset, int length)
{
	initString( bytes, offset, length, ByteToCharConverter.getDefault());
}

public String( byte[] bytes, int offset, int length, String enc) throws UnsupportedEncodingException
{
	initString( bytes, offset, length, ByteToCharConverter.getConverter(enc));
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
	value = new char[count];
	this.count = count;    

	if ( count > 0)
		System.arraycopy( other, offset, value, 0, count);
}

private String( int sIdx, int eIdx, char[] val) {
	value  = val;
	offset = sIdx;
	count  = eIdx - sIdx;
}

public char charAt ( int index ) {
	if (( index < 0) || ( index >= count))
		throw new StringIndexOutOfBoundsException();

	return value[offset+index];
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
	if (offset < 0 || offset >= data.length) {
		throw new IndexOutOfBoundsException();
	}
	if (count < 0 || offset + count > data.length) {
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
		final int stop = offset + count;
		for (int index = offset; index < stop; index++) {
			hash = (31 * hash) + value[index];
		}
	}
	return hash;
}

public int indexOf( String str) {
	return indexOf( str, 0);
}

public int indexOf( String str, int sIdx) {
	int it  = offset+sIdx;
	int ic  = str.offset;
	int ma  = 0;

	if ( str.count > count-sIdx )
		return -1;

	if ( str.count == 0 )
		return (sIdx < 0) ? 0 : (sIdx < count) ? sIdx : count;

	for ( ; it<value.length; it++){
		if ( value[it] == str.value[ic] ){
			if (++ma == str.count)
				return ( it-ma-offset+1);
			ic++;
		}
		else if ( ma > 0) {
			//	it -= ma;
			it--;
			ma  = 0;
			ic  = str.offset;
		}
	}

	return -1;
}

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

private void initString( byte[] bytes, int offset, int length, ByteToCharConverter encoding) {
	StringBuffer sbuf = new StringBuffer( length);
	char[] out = new char[512];
	int outlen = encoding.convert( bytes, offset, length, out, 0, out.length);
	while ( outlen > 0) {
		sbuf.append( out, 0, outlen);
		outlen = encoding.flush( out, 0, out.length);
	}

	String str = sbuf.toString();

	value = str.value;
	offset = str.offset;
	count  = str.count;
}

public int lastIndexOf( String str) {
	return lastIndexOf( str, count);
}

public int lastIndexOf( String str, int eIdx) {
	int ic = str.offset+str.count-1;
	int it = offset+eIdx+ic;
	int ma = 0;

	if (it >= offset+count) {       // clip index
		it = offset+count-1;
	}

	if (str.count == 0)
		return (eIdx < 0) ? -1 : (eIdx < count) ? eIdx : count;

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

public boolean regionMatches(boolean ignoreCase, int toffset, String other, int ooffset, int len) {
	if (toffset < 0 || ooffset < 0) {
		return false;
	}
	if ( (toffset+len > count) || (ooffset+len > other.count) )
		return false;

	char c, c1;
	for (int pos=0; pos<len; pos++) {
		c  = value[offset+toffset+pos];
		c1 = other.value[other.offset+ooffset+pos];
		if ( (c != c1) && ignoreCase) {
			c  = Character.toLowerCase( c);
			c1 = Character.toLowerCase( c1);
		}
		if ( c != c1)
			return false;
	}

	return true;
}

public boolean regionMatches( int toffset, String other, int ooffset, int len) {
	return regionMatches( false, toffset, other, ooffset, len);
}

public String replace(char oldChar, char newChar) {
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

	if (replaced == false) {
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

}
