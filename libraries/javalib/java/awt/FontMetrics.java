/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */


package java.awt;

import java.util.Hashtable;
import kaffe.util.Ptr;

public class FontMetrics
{
	Ptr nativeData;
	protected Font font;
	int height;
	int descent;
	int ascent;
	int leading;
	int maxAdvance;
	int maxDescent;
	int maxAscent;
	int fixedWidth;
	int[] widths;
	boolean isWideFont;
	static Hashtable cache = new Hashtable();

FontMetrics ( Font fnt ) {
	font = fnt;
	
	nativeData = Toolkit.fntInitFontMetrics( font.nativeData);	
	
	height     = Toolkit.fntGetHeight( nativeData);
	descent    = Toolkit.fntGetDescent( nativeData);
	maxDescent = Toolkit.fntGetMaxDescent( nativeData);
	ascent     = Toolkit.fntGetAscent( nativeData);
	leading    = Toolkit.fntGetLeading( nativeData);
	maxAscent  = Toolkit.fntGetMaxAscent( nativeData);
	maxAdvance = Toolkit.fntGetMaxAdvance( nativeData);
	fixedWidth = Toolkit.fntGetFixedWidth( nativeData);
	widths     = Toolkit.fntGetWidths( nativeData);

	isWideFont = Toolkit.fntIsWideFont( nativeData);
}

public int bytesWidth ( byte data[], int off, int len ) {
	if ( fixedWidth != 0 ) {
		return len*fixedWidth;
  }
	else if ( !isWideFont ) {
		int i, w, n=off+len;
		try {
			for ( i=off, w=0; i<n; i++ )
				w += widths[data[i]];
		}
		catch ( ArrayIndexOutOfBoundsException x ) { return 0; }
		return w;
	}
	else {
		return Toolkit.fntBytesWidth( nativeData, data, off, len);
	}
}

public int charWidth ( char c ) {
	if ( fixedWidth != 0 )
		return fixedWidth;
	else if ( c < 256 )
		return widths[c];
	else
		return Toolkit.fntCharWidth( nativeData, c);
}

public int charWidth ( int c ) {
	return charWidth( (char) c);
}

public int charsWidth ( char data[], int off, int len ) {
	if ( fixedWidth != 0 ) {
		return len*fixedWidth;
  }
	else if ( !isWideFont ) {
		int i, w, n=off+len;
		try {
			for ( i=off, w=0; i<n; i++ )
				w += widths[data[i]];
		}
		catch ( ArrayIndexOutOfBoundsException x ) { return 0; }
		return w;
	}
	else {
		return Toolkit.fntCharsWidth( nativeData, data, off, len);
	}
}

protected void finalize () {
  if ( nativeData != null ) {
		Toolkit.fntFreeFontMetrics( nativeData);
		nativeData = null;
	}
}

public int getAscent() {
	return ascent;
}

public int getDescent() {
	return descent;
}

static FontMetrics getFontMetrics ( Font font ) {
	FontMetrics metrics = (FontMetrics) cache.get( font);
	
	if ( metrics == null ) {
		metrics = new FontMetrics( font);
		cache.put( font, metrics);
	}
	
	return metrics;
}

public int getHeight() {
	return height;
}

public int getLeading() {
	return leading;
}

public int getMaxAdvance() {
	return maxAdvance;
}

public int getMaxAscent() {
	return maxAscent;
}

public int getMaxDescent() {
	return maxDescent;
}

public int[] getWidths () {
	return widths;
}

public int stringWidth ( String s ) {
	return Toolkit.fntStringWidth( nativeData, s);
}

public String toString () {
	return getClass().getName() +
	       " [" + font + 
	        ',' + ascent +
	        ',' + descent +
	        ',' + height + ']';
}
}
