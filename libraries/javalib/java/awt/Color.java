package java.awt;


/**
 * Color - class to represent 8-8-8 RGB color values
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
public class Color
{
	int rgbValue;
	int nativeValue = 0xffffffff;
	final public static Color white = new Color(255, 255, 255);
	final public static Color lightGray = new Color(192, 192, 192);
	final public static Color gray = new Color(128, 128, 128);
	final public static Color darkGray = new Color(64, 64, 64);
	final public static Color black = new Color(0, 0, 0);
	final public static Color red = new Color(255, 0, 0);
	final public static Color pink = new Color(255, 175, 175);
	final public static Color orange = new Color(255, 200, 0);
	final public static Color yellow = new Color(255, 255, 0);
	final public static Color green = new Color(0, 255, 0);
	final public static Color magenta = new Color(255, 0, 255);
	final public static Color cyan = new Color(0, 255, 255);
	final public static Color blue = new Color(0, 0, 255);

public Color ( float r, float g, float b ) {
	rgbValue = 0xff000000 |                      // const alpha channel
	           (((int)(r * 255) & 0xff) << 16) | 
	           (((int)(g * 255) & 0xff) << 8)  |
	            ((int)(b * 255) & 0xff);

	nativeValue = Toolkit.clrGetPixelValue( rgbValue);
}

public Color ( int rgb ) {
	rgbValue = rgb | 0xff000000;
	
	nativeValue = Toolkit.clrGetPixelValue( rgbValue);
}

public Color ( int r, int g, int b ) {
	rgbValue = 0xff000000 | ((r & 0xff) << 16) | ((g & 0xff) << 8)  | (b & 0xff);
	
	nativeValue = Toolkit.clrGetPixelValue( rgbValue);
}

Color ( long pixRgb ) {
	rgbValue    = (int) (pixRgb & 0xffffffff);
	nativeValue = (int) (pixRgb >> 32);
}

/**
 * @rework
 */
public static int HSBtoRGB ( float hue, float sat, float bri ) {
	int   r, g, b, hi, bi, x, y, z;
	float hfrac;
	
	if ( bri == 0.0 ) {
		return 0xff000000;
	}
	else if ( sat == 0.0 ) {
		r = (int) (bri * 255.0 + 0.5);
		return ((r << 16) | (r << 8) | r) | 0xff000000;
	}
	else {
		hue *= 6.0;                 // remove scaling
		hi = (int) Math.floor( hue);
		if ( hi == 6 ) hi = 0;       // 360° == 0°
		
		hfrac = hue - hi;
		bri *= 255;
		bi = (int) (bri + 0.5);
		
		x = (int) ((1 - sat) * bri + 0.5);
		y = (int) ((1 - sat*hfrac) * bri + 0.5);
		z = (int) ((1 - sat*(1 - hfrac)) * bri + 0.5);
		
		switch ( hi ) {
		case 0:   r = bi;   g = z;    b = x;    break;
		case 1:   r = y;    g = bi;   b = x;    break;
		case 2:   r = x;    g = bi;   b = z;    break;
		case 3:   r = x;    g = y;    b = bi;   break;
		case 4:   r = z;    g = x;    b = bi;   break;
		case 5:   r = bi;   g = x;    b = y;    break;
		default:  r = 0;    g = 0;    b = 0;    // can't be, hue is wrapped
		}
		
		return ((r << 16) | (g << 8) | b) | 0xff000000;
	}	
}

/**
 * @rework
 */
public static float[] RGBtoHSB ( int r, int g, int b, float[] hsb ) {
	float min, max, dif, rf, gf, bf;

	rf = (float) r / (float)255.0;
	gf = (float) g / (float)255.0;
	bf = (float) b / (float)255.0;	

	if ( hsb == null )	hsb = new float[3];

	if ( r > g ) {                  // get maximum/minimum color component
		if ( r > b )			max = rf;
		else			        max = bf;
		
		if ( b < g )			min = bf;
		else              min = gf;
	}
	else {
		if ( b > g )			max = bf;
		else			        max = gf;

		if ( r < b )      min = rf;
		else              min = bf;
	}

	hsb[2] = max;                                   // value / brightness

	if ( max > 0 ) {
		dif    = max - min;
		hsb[1] = dif / max;                           // saturation
		
		if ( dif > 0 ) {
			if ( max == rf )                            // hue is scaled
				hsb[0] = ((gf - bf)/dif) / (float)6.0;
			else if ( max == gf )
				hsb[0] = (float)(2.0 + (bf - rf)/dif) / (float)6.0;
			else
				hsb[0] = (float)(4.0 + (rf - gf)/dif) / (float)6.0;
			
			if ( hsb[0] < 0 ) hsb[0] += 1.0;            // wrap hue around 360°
		}
		else {                                         // we don't want NaNs
			hsb[0] = (float)0.0;
		}
	}
	else {                                          // all black (0.0)
		hsb[0] = (float)0.0;
		hsb[1] = (float)0.0;
	}	

	return hsb;
}

public Color brighter() {
	// we need to do this native because we don't know what native visual we use
	// (could be VGA 16 PseudoColor, which isn't realy suitable for arithmetic RGB ops)
	return new Color( Toolkit.clrBright( rgbValue));
}

public Color darker () {
	// we need to do this native because we don't know what native visual we use
	// (could be VGA 16 PseudoColor, which isn't realy suitable for arithmetic RGB ops)
	return new Color( Toolkit.clrDark( rgbValue));
}

public static Color decode ( String s ) throws NumberFormatException {
	return new Color( (Integer.decode( s)).intValue());
}

public boolean equals ( Object obj ) {
	return (obj instanceof Color) && ((Color)obj).rgbValue == rgbValue;
}

public int getBlue () {
	return (rgbValue & 0x0000ff);
}

public static Color getColor ( String s ) {
	return getColor( s, (Color)null);
}

public static Color getColor ( String s, Color defClr ) {
	Integer rgb = Integer.getInteger( s);
	if ( rgb == null )
		return defClr;
	else
		return new Color( rgb.intValue());
}

public static Color getColor ( String s, int defRgb ) {
	Integer rgb = Integer.getInteger( s);
	
	if ( rgb == null )
		return new Color( defRgb);
	else
		return new Color( rgb.intValue());
}

public int getGreen () {
	return ((rgbValue & 0x00ff00) >> 8);
}

public static Color getHSBColor ( float h, float s, float b ) {
	return new Color( HSBtoRGB( h, s, b));
}

public int getRGB () {
	return rgbValue;
}

public int getRed () {
	return ((rgbValue & 0xff0000) >> 16);
}

public int hashCode() {
	return rgbValue;
}

public String toString() {
	return "Color [r=" + getRed() + 
	             ",g=" + getGreen() +
	             ",b=" + getBlue() + "]";
}
}
