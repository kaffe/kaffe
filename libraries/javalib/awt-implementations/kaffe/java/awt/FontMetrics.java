/**
 * Component - abstract root of all widgets
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

package java.awt;

import gnu.classpath.Pointer;
import java.util.Hashtable;

public class FontMetrics
  implements java.io.Serializable
{
        // We store the font because the font metrics
        // depends on the native data maintained by the font
        // in this way the original font object will not be
        // finalized (and the native data freed).
        // Note that since fontmetrics instances are cached
        // their life span is higher than the Font object one.
        // (Maurizio De Cecco maurizio@mandrakesoft.com).

        Font font;  

	transient Pointer nativeData;
	protected String fontSpec;
	transient int height;
	transient int descent;
	transient int ascent;
	transient int leading;
	transient int maxAdvance;
	transient int maxDescent;
	transient int maxAscent;
	transient int fixedWidth;
	transient int[] widths;
	transient boolean isWideFont;
	static transient Hashtable cache = new Hashtable();
	final private static long serialVersionUID = 1681126225205050147L;

protected FontMetrics ( Font font ) {
        this.font = font;
	fontSpec = font.encode();

	nativeData = Toolkit.fntInitFontMetrics( font.nativeData);	
	
	height     = Toolkit.fntGetHeight( nativeData);
	descent    = Toolkit.fntGetDescent( nativeData);
	maxDescent = Toolkit.fntGetMaxDescent( nativeData);
	ascent     = Toolkit.fntGetAscent( nativeData);
	leading    = Toolkit.fntGetLeading( nativeData);
	maxAscent  = Toolkit.fntGetMaxAscent( nativeData);
	maxAdvance = Toolkit.fntGetMaxAdvance( nativeData);
	fixedWidth = Toolkit.fntGetFixedWidth( nativeData);

	// we defer the widths init because it is rarely used and quite expensive

	isWideFont = Toolkit.fntIsWideFont( nativeData);
}

public int bytesWidth ( byte data[], int off, int len ) {
	if ( fixedWidth != 0 ) {
		return len*fixedWidth;
  }
	else if ( !isWideFont ) {
		int i, w, n=off+len;
		if ( widths == null ) {
			widths = Toolkit.fntGetWidths( nativeData);
		}
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
	else if ( c < 256 ){
		if ( widths == null ) {
			widths = Toolkit.fntGetWidths( nativeData);
		}
		return widths[c];
	}
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
		if ( widths == null ) {
			widths = Toolkit.fntGetWidths( nativeData);
		}
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

protected void finalize () throws Throwable {
	if ( nativeData != null ) {
		Toolkit.fntFreeFontMetrics( nativeData);
		nativeData = null;
	}
	super.finalize();
}

public int getAscent() {
	return ascent;
}

public int getDescent() {
	return descent;
}

public Font getFont() {
	return Font.decode( fontSpec);
}

static FontMetrics getFontMetrics ( Font font ) {
	String key = font.encode();
	FontMetrics metrics = (FontMetrics) cache.get( key);

	if ( metrics == null ) {
		metrics = new FontMetrics( font);
		cache.put( key, metrics);
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

/**
 * @deprecated, use getMaxDescent()
 */
public int getMaxDecent() {
	return (getMaxDescent());
}

public int getMaxDescent() {
	return maxDescent;
}

public int[] getWidths () {
	if ( widths == null ) {
		widths = Toolkit.fntGetWidths( nativeData);
	}

	return widths;
}

public int stringWidth ( String s ) {
	return Toolkit.fntStringWidth( nativeData, s);
}

public String toString () {
	return getClass().getName() +
	       " [" + fontSpec + ']';
}
}
