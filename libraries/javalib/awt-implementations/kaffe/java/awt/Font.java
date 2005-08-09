package java.awt;

import gnu.classpath.Pointer;

import java.awt.font.FontRenderContext;
import java.awt.font.GlyphVector;
import java.awt.peer.FontPeer;
import java.io.Serializable;
import java.text.CharacterIterator;

/**
 * XXX: implement serial form! 
 */
public class Font
  implements Serializable
{
	Pointer nativeData;
	protected String name;
	protected int style;
	protected int size;
	final public static int PLAIN = 0;
	final public static int BOLD = 1;
	final public static int ITALIC = 2;
	final public static int ROMAN_BASELINE = 0;
	final public static int CENTER_BASELINE = 1;
	final public static int HANGING_BASELINE = 2;
	final private static long serialVersionUID = -4206021311591459213L;

public Font ( String fntName, int fntStyle, int fntSize ) {
	String spec;

	name = fntName;
	style = fntStyle;
	size = fntSize;
	
	/* Translate a font name into the true font - we have a number
	 * of aliases to deal with which are kept in java.awt.Defaults.
	 * We no longer cache them since this means Default changes will
	 * not be seen.  However to speed the comparision we intern the
	 * search string so we can do simple pointer checks (since string
	 * constants are interned by default).
	 */

	name = name.intern();
	if (name == "Default") {
		spec = Defaults.FsDefault;
	}
	else if (name == "Monospaced") {
		spec = Defaults.FsMonospaced;
	}
	else if (name == "SansSerif") {
		spec = Defaults.FsSansSerif;
	}
	else if (name == "Serif") {
		spec = Defaults.FsSerif;
	}
	else if (name == "Dialog") {
		spec = Defaults.FsDialog;
	}
	else if (name == "DialogInput") {
		spec = Defaults.FsDialogInput;
	}
	else if (name == "ZapfDingbats") {
		spec = Defaults.FsZapfDingbats;
	}
	else if (name == "Helvetica") {
		spec = Defaults.FsSansSerif;
	}
	else if (name == "TimesRoman") {
		spec = Defaults.FsSerif;
	}
	else if (name == "Courier") {
		spec = Defaults.FsMonospaced;
	}
	else {
		spec = name;
	}

	nativeData = Toolkit.fntInitFont( spec, style, size);
}

/**
 * @rework
 */
public static Font decode ( String fntSpec ) {
	Font     fnt;
	String   name;
	int      style = PLAIN;
	int      size = 12;
	int      i, n, l = 0;
	char     c;
	
	name = fntSpec;

	if ( (i = fntSpec.indexOf( '-')) >= 0 ) {  // format : <name>[-<style>[-<size>]]
		name = fntSpec.substring( 0, i);
		
		i++;
		if ( fntSpec.regionMatches( true, i, "plain-", 0, 6) ){
			l = 6;
		}
		else if ( fntSpec.regionMatches( true, i, "bold-", 0, 5) ){
			style = BOLD;
			l = 5;
		}
		else if ( fntSpec.regionMatches( true, i, "italic-", 0, 7) ){
			style = ITALIC;
			l = 7;
		}
		else if ( fntSpec.regionMatches( true, i, "bolditalic-", 0, 11) ) {
			style = BOLD | ITALIC;
			l = 11;
		}

		if ( l > 0 ) {
			i += l;
			size = 0;
			for ( n = fntSpec.length(); i < n; i++ ) {
				c = fntSpec.charAt( i);
				if ( c >= '0' && c <= '9' )
					size = size*10 + (c - '0');
				else
					break;
			}
		}
	}
	
	fnt = new Font( name, style, size);
	return fnt;
}

String encode () {
	String s;
	
	if ( style == PLAIN ){
		s = "-plain-";
	}
	else if ( style == ITALIC ){
		s = "-italic-";
	}
	else if ( style == BOLD ) {
		s = "-bold-";
	}
	else {
		s = "-bolditalic-";
	}
	
	return (name + s + size);
}

public boolean equals ( Object o ) {
	if ( o instanceof Font ) {
		Font fnt = (Font) o;
		if ( !fnt.name.equals( name) ) return false;
		if ( fnt.style != style ) return false;
		if ( fnt.size != size ) return false;
		return true;
	}

	return false;	
}

protected void finalize () throws Throwable {
  if ( nativeData != null ) {
    Toolkit.fntFreeFont( nativeData);
    nativeData = null;
  }
  super.finalize();
}

public String getFamily() {
	return System.getProperty( ("awt.font." + name.toLowerCase()), name);
}

public static Font getFont ( String key ) {
	return getFont( key, null);
}

public static Font getFont ( String key, Font defFont ) {
	String fSpec;

	if ( (fSpec = System.getProperty( key)) != null )
		return decode( fSpec);

	return defFont;	
}

public String getName() {
	return name;
}

public FontPeer getPeer(){
	return null;
}

public int getSize() {
	return size;
}

public int getStyle() {
	return style;
}

public boolean isBold () {
	return ((style & BOLD) != 0);
}

public boolean isItalic () {
	return ((style & ITALIC) != 0);
}

public boolean isPlain() {
	return (style == 0);
}

public String toString() {
	String	s = "";

	if ( style == 0 )
		s = "plain";
	else {
		if ( (style & BOLD) != 0 )   s = "bold";
		if ( (style & ITALIC) != 0 ) s += "italic";
	}

	return getClass().getName() + "[family=" +getFamily() + ",name=" + name
	    + ",style=" + s + ",size=" + size + ']';
}

/* taken from GNU Classpath */
public GlyphVector
createGlyphVector(FontRenderContext ctx, String str)
{
  throw new UnsupportedOperationException ();
}

public GlyphVector
createGlyphVector(FontRenderContext ctx, CharacterIterator i)
{
  throw new UnsupportedOperationException ();
}

public GlyphVector
createGlyphVector(FontRenderContext ctx, char[] chars)
{
  throw new UnsupportedOperationException ();
}

public GlyphVector
createGlyphVector(FontRenderContext ctx, int[] glyphCodes)
{
  throw new UnsupportedOperationException ();
}
}
