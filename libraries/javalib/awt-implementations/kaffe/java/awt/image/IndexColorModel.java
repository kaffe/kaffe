package java.awt.image;


public class IndexColorModel
  extends ColorModel
{
	public int[] rgbs;
	public int trans = -1;

public IndexColorModel( int bits, int size, byte[] r, byte[] g, byte[] b) {
	this( bits, size, r, g, b, (byte[])null);
}

public IndexColorModel( int bits, int size, byte[] r, byte[] g, byte[] b, byte[] a) {
	super( bits);
	
	int i;
	rgbs = new int[size];

	if ( a == null ) {
		for ( i=0; i<size; i++) {
			rgbs[i] = 0xff000000 |
			          ((r[i] & 0xff) << 16) |
			          ((g[i] & 0xff) <<  8) |
			           (b[i] & 0xff);
		}
	}
	else {
		for ( i=0; i<size; i++) {
			rgbs[i] = ((a[i] & 0xff) << 24) |
			          ((r[i] & 0xff) << 16) |
			          ((g[i] & 0xff) <<  8) |
			           (b[i] & 0xff);
		}
	}
}

public IndexColorModel( int bits, int size, byte[] r, byte[] g, byte[] b, int trans) {
	this( bits, size, r, g, b, (byte[])null);
	this.trans = trans;
}

public IndexColorModel( int bits, int size, byte[] cmap, int start, boolean hasalpha) {
	this( bits, size, cmap, start, hasalpha, -1);
}

public IndexColorModel( int bits, int size, byte[] cmap, int start, boolean hasalpha, int trans) {
	super( bits);

	int  i, idx;
	int  n = hasalpha ? 4 : 3;
	int  rs = (size-start) / n;

	this.trans = trans;
	rgbs = new int[rs];

	if ( hasalpha ) {
		for ( i=0, idx=0; idx<rs; idx++ ) {
			rgbs[idx] = ((cmap[i++] & 0xff) << 24) |
			            ((cmap[i++] & 0xff) << 16) |
			            ((cmap[i++] & 0xff) <<  8) |
			             (cmap[i++] & 0xff);
		}
	}
	else {
		for ( i=0, idx=0; idx<rs; idx++ ) {
			rgbs[idx] = 0xff000000 |
			            ((cmap[i++] & 0xff) << 16) |
			            ((cmap[i++] & 0xff) <<  8) |
			             (cmap[i++] & 0xff);
		}
	}
}

IndexColorModel ( int bits, int[] rgbs, int trans) {
	super( bits);
	this.rgbs  = rgbs;
	this.trans = trans;
}

final public int getAlpha( int pixel) {
	return ( (rgbs[pixel] >> 24) & 0xff);	
}

final public void getAlphas( byte[] a) {
	for ( int i=0; i<rgbs.length; i++)
		a[i] = (byte) (rgbs[i] >> 24);
}

final public int getBlue( int pixel) {
	return ( rgbs[pixel] & 0xff);	
}

final public void getBlues( byte[] b) {
	for ( int i=0; i<rgbs.length; i++)
		b[i] = (byte) rgbs[i];
}

final public int getGreen( int pixel) {
	return ( (rgbs[pixel] >> 8) & 0xff);	
}

final public void getGreens( byte[] g) {
	for ( int i=0; i<rgbs.length; i++)
		g[i] = (byte) (rgbs[i] >> 8);
}

final public int getMapSize() {
	return rgbs.length;
}

final public int getRGB( int pixel) {
	return rgbs[pixel];
}

final public void getRGBs( int[] rgb) {
	for ( int i=0; i<rgbs.length; i++)
		rgb[i] = rgbs[i];
}

final public int getRed( int pixel) {
	return ( (rgbs[pixel] >> 16) & 0xff);	
}

final public void getReds( byte[] r) {
	for ( int i=0; i<rgbs.length; i++)
		r[i] = (byte) (rgbs[i] >> 16);
}

final public int getTransparentPixel() {
	return trans;
}
}
