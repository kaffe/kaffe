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
	final public static int PAINT_FIRST = 800;
	final public static int PAINT = 800;
	final public static int UPDATE = 801;
	final public static int PAINT_LAST = 802;

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
