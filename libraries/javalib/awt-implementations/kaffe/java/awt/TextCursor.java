package java.awt;


public class TextCursor
{
	int index;
	int x;
	int y;
	int width;
	int height;
	Color clr;
	int yindex;

public TextCursor() {
	this( Defaults.TextCursorClr, 2, 0);
}

public TextCursor( Color clr, int width, int x){
	this.x = x;
	this.clr = clr;
	this.width = width;
}

public TextCursor( int x) {
	this( Color.blue, 2, x);
}

public void blank( Graphics g ){
	for ( int i=0; i<width; i++)
		g.drawLine( x+i, y, x+i, y+height);   
}

public void blank( Graphics g, int xoffs ){
	int x = this.x + xoffs;
	for ( int i=0; i<width; i++)
		g.drawLine( x+i, y, x+i, y+height);   
}

public void blank( Graphics g, int xoffs, int y ){
	int x = this.x + xoffs;
	for ( int i=0; i<width; i++)
		g.drawLine( x+i, y, x+i, y+height);   
}

public void paint( Graphics g ){
	g.setColor( clr);
	for ( int i=0; i<width; i++)
		g.drawLine( x+i, y, x+i, y+height);   
}

public void paint( Graphics g, int xoffs ){
	int x = this.x + xoffs;
	g.setColor( clr);
	for ( int i=0; i<width; i++)
		g.drawLine( x+i, y, x+i, y+height);   
}

public void paint( Graphics g, int xoffs, int y ){
	int x = this.x + xoffs;
	g.setColor( clr);
	for ( int i=0; i<width; i++)
		g.drawLine( x+i, y, x+i, y+height);   
}

public void setHeight(int height){
	this.height = height;
}

public void setIndex( int index, int x){
	this.index = index;
	this.x = x;
}

public void setPos( int y, int height){
	this.y = y;
	this.height = height;
}

public void setYIndex( int index, int y){
	this.yindex = index;
	this.y = y;
}

public String toString( ){
	return "TextCursor [" + index + "," + yindex + "]"; 
}
}
