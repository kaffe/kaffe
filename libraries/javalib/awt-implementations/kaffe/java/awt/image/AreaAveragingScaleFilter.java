package java.awt.image;


public class AreaAveragingScaleFilter
  extends ReplicateScaleFilter
{
public AreaAveragingScaleFilter ( int width, int height) {
	super( width, height);
}

public void setHints( int hints) {
	super.setHints( hints);
}

public void setPixels( int x, int y, int w, int h, ColorModel cm, byte[] pels, int off, int scan) {
	super.setPixels(x, y, w, h, cm, pels, off, scan);
}

public void setPixels( int x, int y, int w, int h, ColorModel cm, int[] pels, int off, int scan) {
	super.setPixels(x, y, w, h, cm, pels, off, scan);
}
}
