package java.awt.image;


public class DirectColorModel
  extends ColorModel
{
	int rmask;
	int gmask;
	int bmask;
	int amask;
	int shift_alpha;
	int shift_red;
	int shift_green;
	int shift_blue;

public DirectColorModel( int bits, int rmask, int gmask, int bmask) {
	this( bits, rmask, gmask, bmask, 0);
}

public DirectColorModel( int bits, int rmask, int gmask, int bmask, int amask) {
	super( bits);

	this.rmask = rmask;
	this.gmask = gmask;
	this.bmask = bmask;
	this.amask = amask;
	
	shift_alpha = getOffset( amask) - 8;
	shift_red   = getOffset( rmask) - 8;
	shift_green = getOffset( gmask) - 8;
	shift_blue  = getOffset( bmask) - 8;
}

final public int getAlpha( int pixel) {
	return (pixel & amask) >>> shift_alpha;
}

final public int getAlphaMask() {
	return amask;
}

final public int getBlue( int pixel) {
	return (pixel & bmask) >>> shift_blue;
}

final public int getBlueMask() {
	return bmask;
}

final public int getGreen( int pixel) {
	return (pixel & gmask) >>> shift_green;
}

final public int getGreenMask() {
	return gmask;
}

static int getOffset( int mask) {
	if (mask == 0) {
		return (0);
	}
	int offs = 0;
	for (; (mask & 1) == 0; mask >>>= 1, offs++);
	for (; (mask & 1) != 0; mask >>>= 1, offs++);
	return offs;
}

final public int getRGB( int pixel) {
	if ( this == defaultCM )
		return pixel;
	else
		return ((pixel & amask) << (24 - shift_alpha)) |
		       ((pixel & rmask) << (16 - shift_red))   |
		       ((pixel & gmask) << (8 - shift_green)) |
		       ((pixel & bmask) << shift_blue);
}

final public int getRed( int pixel) {
	return (pixel & rmask) >>> shift_red;
}

final public int getRedMask() {
	return rmask;
}
}
