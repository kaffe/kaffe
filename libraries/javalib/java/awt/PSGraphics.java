/*
 * class PSGraphics - a PrintGraphics PostScript implementation
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author J.Mehlitz
 */

package java.awt;

import java.awt.image.ColorModel;
import java.awt.image.ImageConsumer;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Hashtable;

public class PSGraphics
  extends Graphics
  implements PrintGraphics
{
	PrintStream ps;
	Font fnt;
	Color clr;
	static float dpm = (float)2.835;
	static float dpi = 72;
	static float scaleX;
	static float scaleY;
	static int maxScaledWidth;
	static int maxScaledHeight;
	static int bxl;
	static int bxr;
	static int byt;
	static int byb;
	PrintJob pj;

static {
	tuneDevice( 10, 10, 10, 10, "Letter");
}

public PSGraphics ( PrintStream ps) {
	this.ps = ps;
	printProlog();
}

public PSGraphics ( String pathName) {
	try {
		ps = new PrintStream( new FileOutputStream( pathName));
		printProlog();
	}
	catch ( IOException e) {}
}

void arc ( int x0, int y0, float r, int startAngle, int endAngle){
	ps.print( x0);
	ps.print( " ");
	ps.print( y0);
	ps.print( " ");
	ps.print( r);
	ps.print( " ");
	ps.print( startAngle);
	ps.print( " ");
	ps.print( endAngle);
	
	ps.println( " arc");
}

void arcTo ( int x1, int y1, int x2, int y2, int ah){
	ps.print( x1);
	ps.print( " ");
	ps.print( y1);
	ps.print( " ");
	ps.print( x2);
	ps.print( " ");
	ps.print( y2);
	ps.print( " ");
	ps.print( ah);
	
	ps.println( " arcto");
	ps.println( "4 {pop} repeat");
}

public void clearRect ( int x, int y, int width, int height ) {
	ps.println( "gsave");
	ps.println( "1 1 1 setrgbcolor");
	psRect( x, y, width, height, true);
	ps.println( "grestore");
}

public void clipRect ( int x, int y, int width, int height ) {
}

public void copyArea ( int x, int y, int width, int height, int dx, int dy ){
}

public Graphics create () {
	return this;
}

public void dispose () {
	ps.flush();
	try { ps.close(); }
	catch ( Exception e) {}
}

public void draw3DRect ( int x, int y, int width, int height, boolean raised ){
	Color c1 = clr.brighter();
	Color c2 = clr.darker();
	
	setColor( raised ? c1 : c2);
	drawLine( x, y, x, y + height);
	drawLine( x + 1, y, x + width - 1, y);
	setColor( raised ? c2 : c1);
	drawLine( x + 1, y + height, x + width, y + height);
	drawLine( x + width, y, x + width, y + height);
	
	setColor( clr);
}

public void drawArc ( int x, int y, int width, int height) {
	psArc( x, y, width, height, 0, 360, false);
}

public void drawArc ( int x, int y, int width, int height, int startAngle, int arcAngle ){
	psArc( x, y, width, height, startAngle, arcAngle, false);
}

public void drawBytes ( byte data[], int offset, int length, int x, int y ){
	y = maxScaledHeight - y;
	
	moveTo( x, y, true);
	ps.print( "(" );
	ps.write( data, offset, length);
	ps.println( ") show stroke");
}

public void drawChars ( char data[], int offset, int length, int x, int y ){
	drawString( new String( data, offset, length), x, y);
}

public boolean drawImage (Image img, int x, int y, Color bgcolor, ImageObserver observer) {
	return drawImage( img, x, y, 0, 0, bgcolor, observer);
}

public boolean drawImage ( Image img, int x, int y, ImageObserver observer) {
	return drawImage( img, x, y, 0, 0, null, observer);
}

public boolean drawImage ( Image img, int x, int y, int width, int height,
		                Color background, ImageObserver observer ) {
	return psImage( img, x, y, width, height, observer, background);
}

public boolean drawImage ( Image img, int x, int y, int width, int height, ImageObserver observer){
	return drawImage( img, x, y, width, height, null, observer);
}

public boolean drawImage ( Image img, int dx0, int dy0, int dx1, int dy1,
				            int sx0, int sy0, int sx1, int sy1,
				            Color bgColor, ImageObserver observer) {
	return false;
}

public boolean drawImage ( Image img, int dx1, int dy1, int dx2, int dy2,
				            int sx1, int sy1, int sx2, int sy2,
				            ImageObserver observer) {
	return false;
}

void drawImgScaled ( Image img,
	       	           int dx0, int dy0, int dx1, int dy1,
		                 int sx0, int sy0, int sx1, int sy1,
		                 Color background ){
}

public void drawLine ( int x1, int y1, int x2, int y2 ){
	y1 = maxScaledHeight - y1;
	y2 = maxScaledHeight - y2;
	
	moveTo( x1, y1, true);
	lineTo( x2, y2, true);
	
	stroke( false);
}

public void drawOval ( int x, int y, int width, int height ){
	psArc( x, y, width, height, 0, 360, false);
}

public void drawPolygon ( Polygon p ){
	psPoly( p.xpoints, p.ypoints, p.npoints, false);
}

public void drawPolygon ( int xPoints[], int yPoints[], int nPoints ){
	psPoly( xPoints, yPoints, nPoints, false);
}

public void drawPolyline ( int xPoints[], int yPoints[], int nPoints ){
	psPoly( xPoints, yPoints, nPoints, false);
}

public void drawRect ( int x, int y, int width, int height ){
	psRect( x, y, width, height, false);
}

public void drawRoundRect ( int x, int y, int width, int height, int arcWidth, int arcHeight){
	psRoundRect( x, y, width, height, arcWidth, arcHeight, false);
}

public void drawString ( String str, int x, int y ){
	y = maxScaledHeight - y;
	
	moveTo( x, y, true);
	ps.print( '(');
	ps.print( str);
	ps.print( ") show " );
	
	stroke( false);
}

public void fill3DRect ( int x, int y, int width, int height, boolean raised ){
	Color c1 = clr.brighter();
	Color c2 = clr.darker();
	
	
	if ( !raised )
		setColor( c2);
	fillRect( x + 1, y + 1, width - 2, height - 2);
	
	setColor( raised ? c1 : c2);
	drawLine( x, y, x, y + height - 1);
	drawLine( x + 1, y, x + width - 2, y);
	setColor( raised ? c2 : c1);
	drawLine( x + 1, y + height - 1, x + width - 1, y + height - 1);
	drawLine( x + width - 1, y, x + width - 1, y + height - 1);

	setColor( clr);
}

public void fillArc ( int x, int y, int width, int height, int startAngle, int arcAngle ){
	psArc( x, y, width, height, startAngle, arcAngle, true);
}

public void fillOval ( int x, int y, int width, int height ){
	psArc( x, y, width, height, 0, 360, true);
}

public void fillPolygon ( Polygon p ){
	psPoly( p.xpoints, p.ypoints, p.npoints, true);
}

public void fillPolygon ( int xPoints[], int yPoints[], int nPoints ){
	psPoly( xPoints, yPoints, nPoints, false);
}

public void fillRect ( int x, int y, int width, int height ){
	psRect( x, y, width, height, true);
}

public void fillRoundRect ( int x, int y, int width, int height, int arcWidth, int arcHeight ){
	psRoundRect( x, y, width, height, arcWidth, arcHeight, true);
}

protected void finalize () throws Throwable {
	dispose();
	super.finalize();
}

public Shape getClip (){
	return null;
}

public Rectangle getClipBounds() {
        return null;
}

public Rectangle getClipBounds(Rectangle rect) {
        return rect;
}

public Color getColor() {
	return clr;
}

public Font getFont() {
	return fnt;
}

public FontMetrics getFontMetrics () {
	return FontMetrics.getFontMetrics( fnt);
}

public FontMetrics getFontMetrics ( Font fnt ) {
	return FontMetrics.getFontMetrics( fnt);
}

String getPSFont (){
	String psn;
	String jn = fnt.getName();
	int    js = fnt.getStyle();
	
	if ( jn.equals( "Times") ) {
		psn = "Times-";
		if ( js == Font.PLAIN )
			psn += "Roman";
		if ( (js & Font.BOLD) > 0 )
			psn += "Bold";
		if ( (js & Font.ITALIC)  > 0 )
			psn += "Italic";
	}
	else if ( jn.equals( "Helvetica") ) {
		psn = "Helvetica";
		if ( js != Font.PLAIN )
			psn += "-";
		if ( (js & Font.BOLD) > 0 )
			psn += "Bold";
		if ( (js & Font.ITALIC)  > 0 )
			psn += "Oblique";
	}
	else if ( jn.equals( "Courier") ) {
		psn = "Courier";
		if ( js != Font.PLAIN )
			psn += "-";
		if ( (js & Font.BOLD) > 0 )
			psn += "Bold";
		if ( (js & Font.ITALIC)  > 0 )
			psn += "Oblique";
	}
	else if ( jn.equals( "Symbol") )
		psn = "Symbol";
	else
		psn = "Courier";
		
	return psn;
}

public PrintJob getPrintJob() {
	return pj;
}

void lineTo ( int x, int y, boolean cont){
	ps.print( x);
	ps.print( " ");
	ps.print( y);
	if ( cont )	ps.print( " lineto ");
	else				ps.println( " lineto ");
}

void moveTo ( int x, int y, boolean cont){
	ps.print( x);
	ps.print( " ");
	ps.print( y);
	if ( cont )	ps.print( " moveto ");
	else				ps.println( " moveto ");
}

void printColorImageProlog ( int w){
	ps.print( "/pix ");
	ps.print( 3*w);
	ps.println( " string def");
	
	ps.print( "/grays ");
	ps.print( w);
	ps.println( " string def");
	ps.println( "/npixls 0 def");
	ps.println( "/rgbindx 0 def");
	
	ps.println( "/colorimage where");
	ps.println( "{ pop }" );
	ps.println( "{" );
	ps.println( "/colortogray {" );
	ps.println( "/rgbdata exch store");
	ps.println( "rgbdata length 3 idiv");
	ps.println( "/npixls exch store");
	ps.println( "/rgbindx 0 store");
	ps.println( "0 1 npixls 1 sub {");
	ps.println( "grays exch");
	ps.println( "rgbdata rgbindx       get 20 mul");
	ps.println( "rgbdata rgbindx 1 add get 32 mul");
	ps.println( "rgbdata rgbindx 2 add get 12 mul");
	ps.println( "add add 64 idiv");
	ps.println( "put");
	ps.println( "/rgbindx rgbindx 3 add store");
	ps.println( "} for");
	ps.println( "grays 0 npixls getinterval");
	ps.println( "} bind def");
	ps.println( "");
	
	ps.println( "");
	ps.println( "/mergeprocs {");
	ps.println( "dup length");
	ps.println( "3 -1 roll");
	ps.println( "dup");
	ps.println( "length");
	ps.println( "dup");
	ps.println( "5 1 roll");
	ps.println( "3 -1 roll");
	ps.println( "add");
	ps.println( "array cvx");
	ps.println( "dup");
	ps.println( "3 -1 roll");
	ps.println( "0 exch");
	ps.println( "putinterval");
	ps.println( "dup");
	ps.println( "4 2 roll");
	ps.println( "putinterval");
	ps.println( "} bind def");
	ps.println( "");
	
	ps.println( "/colorimage {");
	ps.println( "pop pop");
	ps.println( "{colortogray} mergeprocs");
	ps.println( "image");
	ps.println( "} bind def");
	ps.println( "} ifelse");
	
}

void printProlog (){
	ps.println("%!PS-Adobe-2.0 generated by PSGraphics");
	ps.println("% (C)1998 Transvirtual Technologies, Inc.");
	translate( bxl, byt);
	scale( scaleX, scaleY);
	setFont( Defaults.TextFont);
	setColor( Color.black);
}

void psArc ( int x, int y, int width, int height, int startAngle, int arcAngle, boolean opaque ){
	int x0, y0;
	y = maxScaledHeight - y;
	
	ps.println( "gsave");
	
	x0 = x + width/2;
	y0 = y - height/2;
	translate( x0, y0);
	scale( (float) 1, (float) height / width );
	
	if ( opaque )
		moveTo( 0, 0, false);
		
	arc( 0, 0, (float)width/2, startAngle, startAngle + arcAngle);
	
	ps.println( opaque ? "closepath eofill" : "stroke" );
	ps.println( "grestore");
}

boolean psImage ( Image img, int x, int y, int width, int height, ImageObserver observer, Color bgClr ){
	y = maxScaledHeight - y;
	
	char[] dth = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
								 'A', 'B', 'C', 'D', 'E', 'F' };
								
	PSImageConsumer psc = new PSImageConsumer( img);
	
	ps.println("gsave");

	ps.println("20 dict begin");
	printColorImageProlog( psc.width);
	
	translate( x, y);
	
	if ( (height == 0) || (width == 0) ){
		height = psc.height;
		width = psc.width;
	}
	
	if ( (height == 0) || (width == 0) )
		return false;

	scale( width, height);
	ps.println( psc.width + " " + psc.height + " 8");
	ps.println( "[" + psc.width + " 0 0 -" + psc.height + " 0 "  + 0 + "]" );

	ps.println( "{currentfile pix readhexstring pop}" );
	ps.println( "false 3 colorimage" );
	ps.println( "");
	
	char[] buf = new char[psc.width*6];
	int boff;
	
	for ( int i=0; i<psc.height; i++) {
		boff = 0;

		if ( bgClr == null) {
			for ( int j=0; j<psc.width; j++) {
				int v = psc.pels[j][i];
				
				buf[boff++] = ( dth[ (v & 0xf00000) >> 20] );
				buf[boff++] = ( dth[ (v & 0xf0000)  >> 16] );
				buf[boff++] = ( dth[ (v & 0xf000)   >> 12] );
				buf[boff++] = ( dth[ (v & 0xf00)    >>  8] );
				buf[boff++] = ( dth[ (v & 0xf0)     >>  4] );
				buf[boff++] = ( dth[ (v & 0xf)           ] );
			}
			ps.println( buf);
		}
		else {
			for ( int j=0; j<psc.width; j++) {
				int v;
				
				if ( psc.pels[j][i] == 1)
					v = (clr.getGreen() << 16) + (clr.getBlue() << 8) + clr.getRed();
				else
					v = (bgClr.getGreen() << 16) + (bgClr.getBlue() << 8) + bgClr.getRed();

				buf[boff++] = ( dth[ (v & 0xf0)    ] );
				buf[boff++] = ( dth[ (v & 0xf)     ] );
				buf[boff++] = ( dth[ (v & 0xf000)  ] );
				buf[boff++] = ( dth[ (v & 0xf00)   ] );
				buf[boff++] = ( dth[ (v & 0xf00000)] );
				buf[boff++] = ( dth[ (v & 0xf0000) ] );

			}
			ps.println( buf);

		}
		
	}
	
	ps.println( "");
	ps.println( "end");
	ps.println( "grestore");
	
System.out.println( "image ready");
	return true;
}

void psPoly ( int[] px, int[] py, int np, boolean opaque ){
	int i;
	int[] pyt = new int[ np];
	
	for ( i=0; i<np; i++)
		pyt[i] = maxScaledHeight - py[i];
		
	moveTo( px[0], pyt[0], false);
	
	for ( i=1; i<np; i++)
		lineTo( px[i], pyt[i], false);
	lineTo( px[0], pyt[0], false);
		
	stroke( opaque);
}

void psRect ( int x, int y, int width, int height, boolean opaque ){
	y = maxScaledHeight - y;
	
	moveTo( x, y, false);
	lineTo( x + width, y, false);
	lineTo( x + width, y - height, false);
	lineTo( x, y - height, false);
	lineTo( x, y, false);
	
	stroke( opaque);
}

void psRoundRect ( int x, int y, int width, int height, int arcWidth, int arcHeight, boolean opaque ){
	y = maxScaledHeight - y;
	
	moveTo( x + arcHeight, y, false);		
	arcTo( x + width, y, x + width, y - height, arcHeight);
	arcTo( x + width, y - height, x, y - height, arcHeight);
	arcTo( x, y - height, x, y, arcHeight);
	arcTo( x, y, x + width, y, arcHeight);
	
	stroke( opaque);
}

void scale ( float sx, float sy){
	ps.print( sx);
	ps.print( " ");
	ps.print( sy);
	ps.println( " scale");
}

public void setClip ( Shape clip ){
}

public void setClip ( int x, int y, int width, int height ) {
}

public void setColor ( Color clr ){
	if ( ( clr == null ) || clr.equals( this.clr))
		return;
		
	this.clr = clr;
	
	float psr = (float)clr.getRed() / 255; 
	float psg = (float)clr.getGreen() / 255; 
	float psb = (float)clr.getBlue() / 255; 
	
	ps.print( psr);
	ps.print( " ");
	ps.print( psg);
	ps.print( " ");
	ps.print( psb);
	ps.println( " setrgbcolor");
}

public void setFont ( Font fnt ) {
	if ( (fnt == null) || (fnt.equals( this.fnt) ) )
		return;
		
	this.fnt = fnt;
	
	ps.print( "/");
	ps.print( getPSFont());
	ps.println( " findfont");
	ps.print( fnt.getSize() );
	ps.println( " scalefont setfont");
}

public void setPaintMode() {
}

public void setXORMode ( Color newXorClr ) {
}

void stroke ( boolean opaque){
	ps.println( opaque ? "eofill" : "stroke");
}

public String toString() {
	return "PSGraphics";
}

public void translate ( int x, int y ) {
	ps.print( x);
	ps.print( " ");
	ps.print( y);
	ps.println( " translate");
}

public static void tuneDevice( int xLeft, int xRight, int yTop, int yBottom, String format) {
	// set device borders [mm] and format A4 / Letter
	
	bxl = (int)( dpm * xLeft);
	byt = (int)( dpm * yTop);
	bxr = (int)( dpm * xRight);
	byb = (int)( dpm * yBottom);
	
	scaleX = dpi / Defaults.XResolution;
	scaleY = dpi / Defaults.YResolution;
	
	double mhp, mwp;
	if ( format.equals( "A4") ) {
		mhp = 842;
		mwp = 594;
	}
	else {
		mhp = 792;
		mwp = 612;
	}
	
	maxScaledHeight = (int) ((mhp - byt - byb) / scaleY );
	maxScaledWidth  = (int) ((mwp - bxl - bxr) / scaleX );

}
}

class PSImageConsumer
  implements ImageConsumer
{
	int width;
	int height;
	int[][] pels;

public PSImageConsumer( Image img) {
	ImageProducer ip = img.getSource();
	ip.startProduction( this);
}

public void imageComplete( int status) {
}

public void setColorModel( ColorModel model) {
}

public void setDimensions( int width, int height) {
	this.width = width;
	this.height = height;
	pels = new int[width][height];
}

public void setHints( int hints) {
}

public void setPixels( int x, int y, int w, int h, ColorModel model, byte[] pels, int offs, int scan) {
	int x1, y1 = offs;
	
	for ( int i=y; i<y+h; i++) {
		x1 = y1;
		for ( int j=x; j<x+w; j++)
			this.pels[j][i] = model.getRGB( pels[ x1++] );
		y1 += scan;
	}
}

public void setPixels( int x, int y, int w, int h, ColorModel model, int[] pels, int offs, int scan) {
	int x1, y1 = offs;
	
	for ( int i=y; i<y+h; i++) {
		x1 = y1;
		for ( int j=x; j<x+w; j++)
			this.pels[j][i] = pels[ x1++];
		y1 += scan;
	}
}

public void setProperties( Hashtable props) {
}
}
