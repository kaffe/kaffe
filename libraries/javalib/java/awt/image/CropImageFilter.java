package java.awt.image;

import java.util.Hashtable;

public class CropImageFilter
  extends ImageFilter
{
	int x;
	int y;
	int width;
	int height;

public CropImageFilter ( int x, int y, int w, int h) {
	this.x = x;
	this.y = y;
	this.width = w;
	this.height = h;
}

public void setDimensions ( int w, int h) {
	consumer.setDimensions( width, height);
}

public void setPixels ( int x, int y, int w, int h, ColorModel cm, byte[] pels, int off, int scan) {
	int xw = this.x + this.width;
	int yh = this.y + this.height;
	
	if ( (this.x > x+w) || (this.y > y+h) || (xw < x) || (yh < y) )
			return;
			
	int xl = (this.x > x) ? this.x : x;
	int yt = (this.y > y) ? this.y : y;
	int xr = (xw < x+w) ? xw : x+w;
	int yb = (yh < y+h) ? yh : y+h;
	
	consumer.setPixels( xl - this.x, yt - this.y, xr - xl, yb - yt, cm, pels,
									    off + (yt -y ) * scan + (xl - x), scan);
}

public void setPixels ( int x, int y, int w, int h, ColorModel cm, int[] pels, int off, int scan) {
	int xw = this.x + this.width;
	int yh = this.y + this.height;
	
	if ( (this.x > x+w) || (this.y > y+h) || (xw < x) || (yh < y) )
			return;
			
	int xl = (this.x > x) ? this.x : x;
	int yt = (this.y > y) ? this.y : y;
	int xr = (xw < x+w) ? xw : x+w;
	int yb = (yh < y+h) ? yh : y+h;
	
	consumer.setPixels( xl - this.x, yt - this.y, xr - xl, yb - yt, cm, pels,
									    off + (yt -y ) * scan + (xl - x), scan);
}

public void setProperties ( Hashtable props) {
	super.setProperties( props);
}
}
