package java.awt;

import java.awt.peer.FontPeer;
import java.util.Hashtable;
import kaffe.util.Ptr;

/**
 * Font - class to access native Fonts suitable for rendering text
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
public class Font
{
	Ptr nativeData;
	protected String name;
	protected int style;
	protected int size;
	final public static int PLAIN = 0;
	final public static int BOLD = 1;
	final public static int ITALIC = 2;
	static Hashtable nativeSpecs = new Hashtable();

static {
	nativeSpecs.put( "Default", Defaults.FsDefault);
	nativeSpecs.put( "Monospaced", Defaults.FsMonospaced);
	nativeSpecs.put( "SansSerif", Defaults.FsSansSerif);
	nativeSpecs.put( "Serif", Defaults.FsSerif);
	nativeSpecs.put( "Dialog", Defaults.FsDialog);
	nativeSpecs.put( "DialogInput", Defaults.FsDialogInput);
	nativeSpecs.put( "ZapfDingbats", Defaults.FsZapfDingbats);
	
	nativeSpecs.put( "Helvetica", Defaults.FsSansSerif);
	nativeSpecs.put( "TimesRoman", Defaults.FsSerif);
	nativeSpecs.put( "Courier", Defaults.FsMonospaced);
}

public Font ( String fntName, int fntStyle, int fntSize ) {
	Object v;
	String spec;

	name = fntName;
	style = fntStyle;
	size = fntSize;
	
	if ( (v = nativeSpecs.get( name)) != null )
		spec = (String)v;
	else
		spec = name;

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
		if ( fntSpec.regionMatches( true, i, "bold-", 0, 5) ){
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
			i += 11;
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

protected void finalize () {
  if ( nativeData != null ) {
    Toolkit.fntFreeFont( nativeData);
    nativeData = null;
  }
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

public int hashCode () {
	return (name.hashCode() ^ style ^ size);
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

	return getClass().getName() + " [" + name + ',' +	s + ',' + size + ']';
}
}
