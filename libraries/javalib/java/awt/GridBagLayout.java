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

package java.awt;

import java.util.Hashtable;

public class GridBagLayout
  implements LayoutManager2, java.io.Serializable
{
	final protected static int MINSIZE = 1;
	final protected static int PREFERREDSIZE = 2;
	final protected static int ACTUALSIZE = 3;
	final protected static int MAXGRIDSIZE = 512;
	/* NB: Sun doesn't hardcode */
	private static final long serialVersionUID = 8838754796412211005L;

	/** @serial */
	protected Hashtable comptable = new Hashtable();
	/** @serial */
	protected GridBagConstraints defaultConstraints = new GridBagConstraints();
// XXX fix serial form! /** @serial */
//protected GridBagLayoutInfo layoutInfo;
	/** @serial */
	public int[] columnWidths;
	/** @serial */
	public int[] rowHeights;
	/** @serial */
	public double[] columnWeights;
	/** @serial */
	public double[] rowWeights;

	transient Point offset = new Point();

protected void AdjustForGravity(GridBagConstraints cons, Rectangle display) {
	throw new kaffe.util.NotImplemented(GridBagLayout.class.getName() + ".AdjustForGravity()");
}

protected void ArrangeGrid(Container container) {
	layoutContainer(container);
}

public GridBagLayout() {
}

public void addLayoutComponent( Component c, Object constraints) {
	try {
		if (constraints != null) {
			setConstraints(c, (GridBagConstraints)constraints);
		}
	}
	catch (ClassCastException _) {
		throw new IllegalArgumentException("not GridBagConstraints");
	}
}

public void addLayoutComponent( String name, Component c) {
}

void adjustWeightsRelative( Container parent) {
	// Don't use the insets field directly, since getInsets() might be resolved (e.g. in
	// swing popups). Remember BorderLayout isVisible? It's spelled c o n s i s t e n c y
	Insets pin = parent.getInsets();

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
	
	int piw = pin.left + pin.right;
	int pih = pin.top + pin.bottom;
	
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
	else {
		offset.x = (parent.width - sw - piw) / 2;
	}
	
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
	else {
		offset.y = (parent.height - sh - pih) / 2;
	}
}

int columnStart( int idx) {
	int w = 0;
	for ( int i=0; i<idx; i++)
		w += columnWidths[i];
	return w;
}

void dumpDims() {
	System.out.print( "\t");
	for ( int i=0; i<columnWidths.length; i++) {
		System.out.print( columnWidths[i] + " ");
	}
	System.out.println();
	for ( int i=0; i<rowHeights.length; i++) {
		System.out.println( rowHeights[i] );
	}
}

void dumpWeights() {
	System.out.print( "\t");
	for ( int i=0; i<columnWeights.length; i++) {
		System.out.print( columnWeights[i] + " ");
	}
	System.out.println();
	for ( int i=0; i<rowWeights.length; i++) {
		System.out.println( rowWeights[i] );
	}
}

void getCellDims( Container parent, int mode) {
	Dimension d  = getLayoutExtends( parent);
	Dimension cd;
	int x = -1;
	int y = 0;
	int nc = parent.getComponentCount();

/*
	if ( (rowHeights == null) || (rowHeights.length != d.height) ) {
		rowHeights   = new int[d.height];
	}
	if ( (columnWidths == null) || (columnWidths.length != d.width) ) {
		columnWidths = new int[d.width];
	}
	if ( (columnWeights == null) || (columnWeights.length != d.width) ) {
		columnWeights = new double[d.width];
	}
	if ( (rowWeights == null) || (rowWeights.length != d.height) ) {
		rowWeights = new double[d.height];
	}
*/
	rowHeights = new int[d.height];
	columnWidths = new int[d.width];
	columnWeights = new double[d.width];
	rowWeights = new double[d.height];

	for ( int ic=0; ic<nc; ic++){
		Component c = parent.getComponent(ic);
		GridBagConstraints cc = lookupConstraints( c);
		if ( mode == MINSIZE ) {
			cd = c.getMinimumSize();
		}
		else {
			cd = c.getPreferredSize();
		}

		if ( cc.gridx == cc.RELATIVE ) {
			x++;
		}
		else {
			x = cc.gridx;
		}
		
		if ( cc.gridy > cc.RELATIVE ) {
			y = cc.gridy;
		}

		int cw = cd.width + cc.insets.left + cc.insets.right + 2 * cc.ipadx;
		int ch = cd.height + cc.insets.top + cc.insets.bottom + 2 * cc.ipady;
		
		int gw = cc.gridwidth;
		int gh = cc.gridheight;
		if (gw == cc.REMAINDER) {
			gw = d.width - x;
		}
		else if (gw == cc.RELATIVE) {
			gw = d.width - x - 1;
		}
		if (gh == cc.REMAINDER) {
			gh = d.height - y;
		}
		else if (gh == cc.RELATIVE) {
			gh = d.height - y - 1;
		}
		if (gw < 1) {
			gw = 1;
		}
		if (gh < 1) {
			gh = 1;
		}

		cw = cw / gw;
		ch = ch / gh;

		for ( int i=0; i<gw; i++) {
			if ( columnWidths[x+i] < cw ) {
				columnWidths[x+i] = cw;
			}
		}
		if ( columnWeights[x+gw-1] < cc.weightx ) {
			columnWeights[x+gw-1] = cc.weightx;
		}

		for ( int i=0; i<gh; i++) {
			if ( rowHeights[y+i] < ch ) {
				rowHeights[y+i] = ch;
			}
		}
		if ( rowWeights[y+gh-1] < cc.weighty ) {
			rowWeights[y+gh-1] = cc.weighty;
		}
		
		x += gw - 1;
		if ( cc.gridwidth == cc.REMAINDER ){
			y++;
			x = -1;
		}
	}
	
	if ( mode == ACTUALSIZE) {
		adjustWeightsRelative( parent);
	}
}

public GridBagConstraints getConstraints( Component c) {
	return ((GridBagConstraints)lookupConstraints(c).clone());
}

public float getLayoutAlignmentX( Container parent) {
	return Component.CENTER_ALIGNMENT;
}

public float getLayoutAlignmentY( Container parent) {
	return Component.CENTER_ALIGNMENT;
}

public int[][] getLayoutDimensions() {
	return (new int[][]{columnWidths, rowHeights});
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
		if ( cc.gridx >= 0) {
			x = cc.gridx;
		}
		else {
			x++;
		}
		if ( cc.gridy >= 0) {
			y = cc.gridy;
		}

		switch(cc.gridwidth) {
		case cc.REMAINDER:	//last in row
			if ( maxX < x ) {
				maxX = x;
			}
			x = -1;
			y++;
			break;

		case cc.RELATIVE:
			if ( maxX < x ) {
				maxX = x;
			}
			break;			  

		default:
			x += cc.gridwidth - 1;
			if ( maxX < x ) {
				maxX = x;
			}
			break;
		}

		switch(cc.gridheight) {
		case cc.RELATIVE:
		case cc.REMAINDER:
			break;

		default:
			if (x != -1 && maxY < y + cc.gridheight) {
				maxY = y + cc.gridheight;
			}
			break;
		}
	}
	return new Dimension(maxX + 1, Math.max(maxY, y));
}

public Point getLayoutOrigin() {
	return offset;
}

public double[][] getLayoutWeights() {
	return (new double[][]{columnWeights, rowWeights});
}

public void invalidateLayout( Container parent) {
	layoutContainer( parent);
}

public void layoutContainer( Container parent) {
	// Don't use the insets field directly, since getInsets() might be resolved (e.g. in
	// swing popups). Remember BorderLayout isVisible? It's spelled c o n s i s t e n c y
	Insets pin = parent.getInsets();
	int ix = -1, iy = 0;
	int x  = pin.left, y  = pin.top;
	int dx = 0, dy = 0;
	int i;
	Rectangle relRect = new Rectangle();
	int nc = parent.getComponentCount();
	
	getCellDims(parent, ACTUALSIZE);

	for ( int ci=0; ci < nc; ci++){
		Component c = parent.getComponent(ci);
		GridBagConstraints cc = lookupConstraints( c);
		boolean newRow = false;

		if ( cc.gridy >= 0 ) {
			iy = cc.gridy;
			y = rowStart( iy) + pin.top;
		}
		if ( cc.gridx < 0 ) {
			ix++;
			if ( iy > relRect.y ) {
				ix += relRect.x + relRect.width;
			}
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
		for ( i=0; i<gw; i++) {
			dx += columnWidths[ix++];
		}
		ix--;
		dy = 0;
		for ( i=0; i<gh; i++) {
			dy += rowHeights[iy+i]; 
		}
			
		if ( gh > 1) {
			relRect.setBounds( ix, iy, gw, gh);
		}
		else if ( iy >= relRect.y + relRect.height ) {
			relRect.setBounds( 0, 0, 0, 0);
		}
			
		Insets in = cc.insets;
		Dimension cd = c.getPreferredSize();
		int cx = x + offset.x;
		int cy = y + offset.y;
		int cw = cd.width;
		int ch = cd.height;

		switch ( cc.fill ) {
		case cc.BOTH:
			cx += in.left + cc.ipadx;
			cy += in.top + cc.ipady;
			cw = dx - (in.left + in.right + 2 * cc.ipadx);
			ch = dy - (in.top + in.bottom + 2 * cc.ipady);
			break;

		case cc.HORIZONTAL:
			cx += in.left + cc.ipadx;
			cw = dx - (in.left + in.right + 2 * cc.ipadx);
			switch ( cc.anchor ) {
			case cc.WEST:
			case cc.CENTER:
			case cc.EAST:
			default:
				cy += (dy - ch) / 2;
				break;

			case cc.NORTH:
			case cc.NORTHEAST:
			case cc.NORTHWEST:
				cy += in.top + cc.ipady;
				break;

			case cc.SOUTHEAST:
			case cc.SOUTH:
			case cc.SOUTHWEST:
				cy += dy - (cd.height + in.bottom + cc.ipady);
				break;
			}
			break;

		case cc.VERTICAL:
			cy += in.top + cc.ipady;
			ch = dy - (in.top + in.bottom + 2 * cc.ipady);
			switch ( cc.anchor ) {
			case cc.NORTH:
			case cc.CENTER:
			case cc.SOUTH:
			default:
				cx += (dx - cw) / 2;
				break;

			case cc.NORTHWEST:
			case cc.WEST:
			case cc.SOUTHWEST:
				cx += in.left + cc.ipadx;
				break;

			case cc.NORTHEAST:
			case cc.SOUTHEAST:
			case cc.EAST:
				cx += dx - (cw + in.right + cc.ipadx);
				break;
			}
			break;

		case cc.NONE:
		default:
			switch ( cc.anchor ) {
			case cc.NORTH:
				cy += in.top + cc.ipady;
				cx += (dx - cw) / 2;
				break;

			case cc.NORTHEAST:
				cy += in.top + cc.ipady;
				cx += dx - (cw + in.right + cc.ipadx);
				break;

			case cc.EAST:
				cy += (dy - ch) / 2;
				cx += dx - (cw + in.right + cc.ipadx);
				break;

			case cc.SOUTHEAST:
				cy += dy - (ch + in.bottom + cc.ipady);
				cx += dx - (cw + in.right + cc.ipadx);
				break;

			case cc.SOUTH:
				cy += dy - (ch + in.bottom + cc.ipady);
				cx += (dx - cw) / 2;
				break;

			case cc.SOUTHWEST:
				cy += dy - (ch + in.bottom + cc.ipady);
				cx += in.left + cc.ipadx;
				break;

			case cc.WEST:
				cy += (dy - ch) / 2;
				cx += in.left + cc.ipadx;
				break;

			case cc.NORTHWEST:
				cy += in.top + cc.ipady;
				cx += in.left + cc.ipadx;
				break;

			case cc.CENTER:
			default:
				cy += (dy - ch) / 2;
				cx += (dx - cw) / 2;
				break;
			}
			break;
		}
		c.setBounds( cx, cy, cw, ch);

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
	
	for ( ix = 0; ix < columnWidths.length; ix++){
		x0 += columnWidths[ix];
		if ( x < x0 ) {
			break;
		}
	}
	for ( iy = 0; ix < rowHeights.length; iy++){
		y0 += rowHeights[iy];
		if ( y < y0 ) {
			break;
		}
	}
	
	return new Point( ix, iy);
}

protected GridBagConstraints lookupConstraints(Component c) {
	GridBagConstraints cons = (GridBagConstraints)comptable.get(c);
	if (cons == null) {
		cons = defaultConstraints;
	}
	return (cons);
}

public Dimension maximumLayoutSize( Container parent) {
	return (new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));
}

public Dimension minimumLayoutSize( Container parent) {
	// Don't use the insets field directly, since getInsets() might be resolved (e.g. in
	// swing popups). Remember BorderLayout isVisible? It's spelled c o n s i s t e n c y
	Insets in = parent.getInsets();
	
	getCellDims( parent, MINSIZE);
	return new Dimension( sumWidths()+in.left+in.right, sumHeights()+in.top+in.bottom );
}

public Dimension preferredLayoutSize( Container parent) {
	// Don't use the insets field directly, since getInsets() might be resolved (e.g. in
	// swing popups). Remember BorderLayout isVisible? It's spelled c o n s i s t e n c y
	Insets in = parent.getInsets();
	
	getCellDims( parent, PREFERREDSIZE);
	return new Dimension( sumWidths()+in.left+in.right, sumHeights()+in.top+in.bottom );
}

public void removeLayoutComponent( Component c) {
	comptable.remove(c);
}

int rowStart( int idx) {
	int h = 0;
	for ( int i=0; i<idx; i++) {
		h += rowHeights[i];
	}
	return h;
}

public void setConstraints( Component comp, GridBagConstraints cons) {
	comptable.put(comp, (GridBagConstraints)cons.clone());
}

int sumHeights() {
	int h = 0;
	for ( int i=0; i<rowHeights.length; i++) {
		h += rowHeights[i];
	}
	return h;
}

int sumWidths() {
	int w = 0;
	for ( int i=0; i<columnWidths.length; i++) {
		w += columnWidths[i];
	}
	return w;
}

public String toString() {
	int w = (columnWidths != null) ? columnWidths.length : 0;
	int h = (rowHeights != null) ? rowHeights.length : 0;
	return ("GridBagLayout columns: " + w + ",rows: " + h);
}
}
