package java.awt.event;

import java.awt.Component;
import java.awt.Rectangle;

public class PaintEvent
  extends ComponentEvent
{
	protected int x;
	protected int y;
	protected int width;
	protected int height;
	public static final int PAINT = 800;
	public static final int PAINT_FIRST = 800;
	public static final int PAINT_LAST = 801;
	public static final int UPDATE = 801;
	private static final long serialVersionUID = 1267492026433337593L;

public PaintEvent ( Component src, int evtId, Rectangle updateRect ) {
	super( src, evtId);

	x = updateRect.x;
	y = updateRect.y;
	width = updateRect.width;
	height = updateRect.height;
}

protected PaintEvent ( Component src, int evtId, int x, int y, int width, int height ) {
	super( src, evtId);

	this.x = x;
	this.y = y;
	this.width = width;
	this.height = height;
}

public Rectangle getUpdateRect() {
	return new Rectangle( x, y, width, height);
}

protected boolean isLiveEventFor ( Object src ) {
	return (source == src);
}

public String paramString () {
	String ps;
	
	if ( id == PAINT ) ps = "PAINT";
	else if ( id == UPDATE ) ps = "UPDATE";
	else ps = "unknown Paint";
	
	ps += " ["+x+','+y+','+width+','+height+"]";
	
	return ps;
}

public void setUpdateRect( Rectangle r ) {
	x = r.x;
	y = r.y;
	width = r.width;
	height = r.height;
}
}
