package java.awt;

import java.util.Hashtable;

/**
 * class GridBagLayout - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author J.Mehlitz
 */
public class GridBagLayout
  implements LayoutManager2
{
	final protected static int MINSIZE = 1;
	final protected static int PREFERREDSIZE = 2;
	final protected static int ACTUALSIZE = 3;
	protected Hashtable comptable = new Hashtable();
	protected GridBagConstraints defaultConstraints = new GridBagConstraints();
	public int[] columnWidths;
	public int[] rowHeights;
	public double[] columnWeights;
	public double[] rowWeights;
	Point offset = new Point();

public GridBagLayout() {
}

public void addLayoutComponent( Component c, Object constraints) {
	if ( constraints instanceof GridBagConstraints)
		comptable.put( c, ((GridBagConstraints)constraints).clone());
}

public void addLayoutComponent( String name, Component c) {
}

void adjustWeightsRelative( Container parent) {
	int i;
	int nxw = 0;
	int nyw = 0;
	int sw = sumWidths();
	int sh = sumHeights();
	double twx = 0;
	double twy = 0;
	
	for ( i=0; i<columnWeights.length; i++) {
		if ( columnWeights[i] > 0.0 ) {
			twx += columnWeights[i];
			nxw++;
		}
	}		
	for ( i=0; i<rowWeights.length; i++) {
		if ( rowWeights[i] > 0.0 ) {
			twy += rowWeights[i];
			nyw++;
		}
	}		
	
	int piw = parent.insets.left + parent.insets.right;
	int pih = parent.insets.top + parent.insets.bottom;
	
	if ( nxw > 0) {
		int dx = parent.width - sw - piw;
		for ( i=0; i<columnWeights.length; i++){
			double cw = columnWeights[i];
			if ( cw > 0.0 ) {
				int dcx = (int)(cw/twx*dx);
				columnWidths[i] += dcx;
				twx -= cw;
				dx -= dcx;
			}
			if ( twx == 0)
				break;
		}
		if ( dx > 0)
			columnWidths[i] += dx;
		offset.x = 0;
	}
	else
		offset.x = (parent.width - sw - piw) / 2;
	
	if ( nyw > 0) {
		int dy = parent.height - sh - pih;
		for ( i=0; i<rowWeights.length; i++){
			double rw = rowWeights[i];
			if ( rw > 0.0 ) {
				int dcy = (int)(rw/twy*dy);
				rowHeights[i] += dcy;
				twy -= rw;
				dy -= dcy;
			}
			if ( twy == 0)
				break;
		}
		if ( dy > 0)
			rowHeights[i] += dy;
		offset.y = 0;
	}
	else
		offset.y = (parent.height - sh - pih) / 2;

}

int columnStart( int idx) {
	int w = 0;
	for ( int i=0; i<idx; i++)
		w += columnWidths[i];
	return w;
}

void dumpDims() {
	System.out.print( "\t");
	for ( int i=0; i<columnWidths.length; i++)
		System.out.print( columnWidths[i] + " ");
	System.out.println();
	for ( int i=0; i<rowHeights.length; i++)
		System.out.println( rowHeights[i] );
}

void dumpWeights() {
	System.out.print( "\t");
	for ( int i=0; i<columnWeights.length; i++)
		System.out.print( columnWeights[i] + " ");
	System.out.println();
	for ( int i=0; i<rowWeights.length; i++)
		System.out.println( rowWeights[i] );
}

void getCellDims( Container parent, int mode) {
	Dimension d  = getLayoutExtends( parent);
	Dimension cd;
	int x = -1;
	int y = 0;
	int nc = parent.getComponentCount();

	if ( (rowHeights == null) || (rowHeights.length != d.height) )
		rowHeights   = new int[d.height];
	if ( (columnWidths == null) || (columnWidths.length != d.width) )
		columnWidths = new int[d.width];
	if ( (columnWeights == null) || (columnWeights.length != d.width) )
		columnWeights = new double[d.width];
	if ( (rowWeights == null) || (rowWeights.length != d.height) )
		rowWeights = new double[d.height];
	
	for ( int ic=0; ic<nc; ic++){
		Component c = parent.getComponent(ic);
		GridBagConstraints cc = lookupConstraints( c);
		if ( cc == null)
			continue;
		if ( mode == MINSIZE )
			cd = c.getMinimumSize();
		else
			cd = c.getPreferredSize();

		if ( cc.gridx == cc.RELATIVE )
			x++;
		else
			x = cc.gridx;
		
		if ( cc.gridy > cc.RELATIVE )
			y = cc.gridy;

		int cw = cd.width + cc.insets.left + cc.insets.right + 2 * cc.ipadx;
		int ch = cd.height + cc.insets.top + cc.insets.bottom + 2 * cc.ipady;
		
		int gw = Math.max( cc.gridwidth, 1);
		int gh = Math.max( cc.gridheight, 1);
		cw = cw / gw;
		ch = ch / gh;

		for ( int i=0; i<gw; i++) {
			if ( columnWidths[x+i] < cw )
				columnWidths[x+i] = cw;
		}
		if ( columnWeights[x+gw-1] < cc.weightx )
			columnWeights[x+gw-1] = cc.weightx;

		for ( int i=0; i<gh; i++) {
			if ( rowHeights[y+i] < ch )
				rowHeights[y+i] = ch;
		}
		if ( rowWeights[y+gh-1] < cc.weighty )
			rowWeights[y+gh-1] = cc.weighty;

		
		x += gw - 1;
		if ( cc.gridwidth == cc.REMAINDER ){
			y++;
			x = -1;
		}
	}
	
	if ( mode == ACTUALSIZE)
		adjustWeightsRelative( parent);
}

public GridBagConstraints getConstraints( Component c) {
	GridBagConstraints gc = defaultConstraints;
	if ( comptable.containsKey( c) )
		gc = (GridBagConstraints)comptable.get( c);
		
	return (GridBagConstraints)gc.clone();
}

public float getLayoutAlignmentX( Container parent) {
	return (float)0.5;
}

public float getLayoutAlignmentY( Container parent) {
	return (float)0.5;
}

public int[][] getLayoutDimensions() {
	int ld[][] = new int[2][];
	ld[0] = columnWidths;
	ld[1] = rowHeights;
	return ld;
}

Dimension getLayoutExtends( Container parent ) {
	int maxX = 0;
	int maxY = 0;
	int x = -1;
	int y = 0;
	int nc = parent.getComponentCount();
	
	for ( int i=0; i<nc; i++){
		Component c = parent.getComponent(i);
		GridBagConstraints cc = lookupConstraints( c);
		if ( cc == null )
			continue;
		if ( cc.gridx >= 0)
			x = cc.gridx;
		else
			x++;
		if ( cc.gridy >= 0)
			y = cc.gridy;

		switch( cc.gridwidth) {
			case cc.REMAINDER:	//last in row
				if ( maxX < x+1 )
					maxX = x+1;
				x = -1;
				y++;
				break;
			case cc.RELATIVE:
				if ( maxX < x+1 )
					maxX = x+1;
				break;			  
			default:
				x += cc.gridwidth - 1;
				if ( maxX < x+1 )
					maxX = x+1;
				break;
		}
		switch( cc.gridheight) {
			case cc.RELATIVE:
				break;
			case cc.REMAINDER:
				break;
			default:
				if ( x != -1 )
					maxY = Math.max( maxY, y+cc.gridheight);
				break;
		}

	}
	maxY = Math.max( maxY, y);
	return new Dimension( maxX, maxY);
}

public Point getLayoutOrigin() {
	return offset;
}

public double[][] getLayoutWeights() {
	double lw[][] = new double[2][];
	lw[0] = columnWeights;
	lw[1] = rowWeights;
	return lw;
}

public void invalidateLayout( Container parent) {
layoutContainer( parent);
}

public void layoutContainer( Container parent) {
	Insets pin = parent.insets;
	int ix = -1, iy = 0;
	int x  = pin.left, y  = pin.top;
	int dx = 0, dy = 0;
	int i;
	Rectangle relRect = new Rectangle();
	int nc = parent.getComponentCount();
	
	getCellDims( parent, ACTUALSIZE);

	for ( int ci=0; ci<nc; ci++){
		Component c = parent.getComponent(ci);
		GridBagConstraints cc = lookupConstraints( c);
		if ( cc == null )
			continue;
		boolean newRow = false;

		if ( cc.gridy >= 0 ) {
			iy = cc.gridy;
			y = rowStart( iy) + pin.top;
		}
		if ( cc.gridx < 0 ) {
			ix++;
			if ( iy > relRect.y )
				ix += relRect.x + relRect.width;
			x = columnStart( ix) + pin.left;
		}
		else {
			ix = cc.gridx;
			x = columnStart( ix) + pin.left;
		}
		
				
		int gw = cc.gridwidth;
		if ( gw == cc.REMAINDER ) {
			gw = columnWidths.length - ix;
			newRow = true;
		}
		else if ( gw == cc.RELATIVE ) {
			gw = columnWidths.length - ix - 1;
		}

		int gh = cc.gridheight;
		if ( gh == cc.REMAINDER ) {
			gh = rowHeights.length - iy;
		}
		else if ( gh == cc.RELATIVE ) {
			gh = rowHeights.length - iy - 1;
		}

		dx = 0;
		for ( i=0; i<gw; i++)
			dx += columnWidths[ix++];
		ix--;
		dy = 0;
		for ( i=0; i<gh; i++)
			dy += rowHeights[iy+i]; 
			
		if ( gh > 1)
			relRect.setBounds( ix, iy, gw, gh);
		else if ( iy >= relRect.y + relRect.height )
			relRect.setBounds( 0, 0, 0, 0);
			
		Insets in = cc.insets;
		if ( cc.fill == cc.BOTH ) {
			c.setBounds( x+in.left+cc.ipadx+offset.x,
									 y+in.top+cc.ipady+offset.y,
									 dx-in.left-in.right-2*cc.ipadx,
									 dy-in.top-in.bottom-2*cc.ipady);
		}
		else {
			int cx, cy, cw, ch;
			Dimension cd = c.getPreferredSize();
			if ( cc.fill == cc.HORIZONTAL ){
				cx = x+in.left+cc.ipadx+offset.x;
				cw = dx-in.left-in.right-2*cc.ipadx;
				ch = cd.height;
				if ( (cc.anchor & cc.SOUTHWEST) > 0)
					cy = y+dy-cd.height-cc.ipady-in.bottom+offset.y;
				else if ( (cc.anchor & cc.WEST) > 0)
					cy = y+(dy-ch)/2+offset.y;
				else
					cy = y+cc.ipady+in.top+offset.y;
			}
			else if ( cc.fill == cc.VERTICAL ) {
				cy = y+in.top+cc.ipady+offset.y;
				ch = dy-in.top-in.bottom-2*cc.ipady;
				cw = cd.width;
				if ( (cc.anchor & cc.NORTHEAST) > 0)
					cx = x+dx-cw-cc.ipadx-in.right+offset.x;
				else if ( (cc.anchor & cc.NORTH) > 0)
					cx = x+(dx-cw)/2+offset.x;
				else
					cx = x+cc.ipadx+in.left+offset.x;
			}
			else {
				cw = cd.width;
				ch = cd.height;
				if ( (cc.anchor & cc.SOUTHWEST) > 0)
					cy = y+dy-cd.height-cc.ipady-in.bottom+offset.y;
				else if ( (cc.anchor & cc.WEST) > 0)
					cy = y+(dy-ch)/2+offset.y;
				else
					cy = y+cc.ipady+in.top+offset.y;
				if ( (cc.anchor & cc.NORTHEAST) > 0)
					cx = x+dx-cw-cc.ipadx-in.right+offset.x;
				else if ( (cc.anchor & cc.NORTH) > 0)
					cx = x+(dx-cw)/2+offset.x;
				else
					cx = x+cc.ipadx+in.left+offset.x;
			}
			
			c.setBounds( cx, cy, cw, ch);
		}

		if ( newRow ) {
			ix = -1;
			y += dy;
			iy++;
		}
	}
}

public Point location( int x, int y) {
	int x0 = offset.x;
	int y0 = offset.y;
	int ix, iy;
	
	for ( ix=0; ix<columnWidths.length; ix++){
		x0 += columnWidths[ix];
		if ( x < x0 )
			break;
	}
	for ( iy=0; ix<rowHeights.length; iy++){
		y0 += rowHeights[iy];
		if ( y < y0 )
			break;
	}
	
	return new Point( ix, iy);
}

protected GridBagConstraints lookupConstraints( Component c) {
	if ( comptable.containsKey( c) )
		return (GridBagConstraints)comptable.get( c);
//	return defaultConstraints;
	return null;
}

public Dimension maximumLayoutSize( Container parent) {
	return Toolkit.singleton.getScreenSize();
}

public Dimension minimumLayoutSize( Container parent) {
	Insets in = parent.insets;
	getCellDims( parent, MINSIZE);
	return new Dimension( sumWidths()+in.left+in.right, sumHeights()+in.top+in.bottom );
}

public Dimension preferredLayoutSize( Container parent) {
	Insets in = parent.insets;
	getCellDims( parent, PREFERREDSIZE);
	return new Dimension( sumWidths()+in.left+in.right, sumHeights()+in.top+in.bottom );
}

public void removeLayoutComponent( Component c) {
	comptable.remove( c);
}

int rowStart( int idx) {
	int h = 0;
	for ( int i=0; i<idx; i++)
		h += rowHeights[i];
	return h;
}

public void setConstraints( Component comp, GridBagConstraints cons) {
	GridBagConstraints gc = (GridBagConstraints)cons.clone();
	comptable.put( comp, gc);
}

int sumHeights() {
	int h = 0;
	for ( int i=0; i<rowHeights.length; i++)
		h += rowHeights[i];
	return h;
}

int sumWidths() {
	int w = 0;
	for ( int i=0; i<columnWidths.length; i++)
		w += columnWidths[i];
	return w;
}

public String toString() {
	int w = (columnWidths != null) ? columnWidths.length : 0;
	int h = (rowHeights != null) ? rowHeights.length : 0;
	return ("GridBagLayout columns: " + w + ",rows: " + h);
}
}
