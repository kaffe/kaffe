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
  implements LayoutManager2, java.io.Serializable
{
	final protected static int MINSIZE = 1;
	final protected static int PREFERREDSIZE = 2;
	final protected static int ACTUALSIZE = 3;
	final protected static int MAXGRIDSIZE = 100;
/* NB: Sun doesn't hardcode */
	final private static long serialVersionUID = 8838754796412211005L;
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
	private static Component[][] defGrid = new Component[MAXGRIDSIZE][MAXGRIDSIZE];

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
	Insets pin = parent.getInsets();
	int i;
	int nxw = 0;
	int nyw = 0;
	int sw = sumWidths();
	int sh = sumHeights();
	double dd;
	double twx = 0;
	double twy = 0;

	for ( i=0; i<columnWeights.length; i++) {
		dd = columnWeights[i];
		if ( dd > 0.0 ) {
			twx += dd;
			nxw++;
		}
	}		
	for ( i=0; i<rowWeights.length; i++) {
		dd = rowWeights[i];
		if ( dd > 0.0 ) {
			twy += dd;
			nyw++;
		}
	}		

	int piw = pin.left + pin.right;
	int pih = pin.top + pin.bottom;

	if ( nxw > 0 ) {
		int dx = parent.width - sw - piw;
		for ( i=0; i<columnWeights.length-1; i++){
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
		for ( i=0; i<rowWeights.length-1; i++){
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

void clearDefGrid() {
	for ( int y=0; y<MAXGRIDSIZE; y++) {
		for ( int x=0; x<MAXGRIDSIZE; x++) {
			defGrid[x][y] = null;
		}
	}
}

Component componentAt( Container parent, int x, int y) {
	int nc = parent.getComponentCount();
	for ( int i=0; i<nc; i++ ) {
		Component c = parent.getComponent( i);
		GridBagConstraints gbc = lookupConstraints( c);
		if ( gbc.gridx <= x && gbc.gridy <= y &&
			gbc.gridx + gbc.gridwidth > x &&
			gbc.gridy + gbc.gridheight > y ) {
			return c;
		}
	}

	return null;
}

void dump() {
	if ( (columnWidths == null) || (rowHeights == null ) ){
		return;
	}

	int i;
	System.out.print( "\t\t");
	for ( i=0; i<columnWidths.length; i++) {
		System.out.print( columnWidths[i] + "[" + columnWeights[i] + "]" );
		System.out.print( '\t');
	}
	System.out.println();
	for ( i=0; i<rowHeights.length; i++) {
		System.out.print( rowHeights[i] + "[" + rowWeights[i] + "]" );
		for ( int i2=0; i2<columnWidths.length; i2++ ) {
			Component c = defGrid[i2][i];
			System.out.print( '\t');
			System.out.print( (c != null) ? Integer.toHexString( c.hashCode() ) : "-------" );
			System.out.print( '\t');
		}
		System.out.println();
	}

	System.out.println();
}

void fillGrid( Container parent, int mxr, int myr, int mode ) {
	int maxX = 0;
	int maxY = 0;
	int x, y, di;
	GridBagConstraints cc;
	Dimension cd = new Dimension();

	// get max cell extends
	for ( y=MAXGRIDSIZE-1; y>-1; y--) {
		if ( defGrid[MAXGRIDSIZE-1][y] != null ) {
			maxX = Math.max( maxX, mxr);
		}
		else {
			for ( x=MAXGRIDSIZE-1; x>-1; x--) {
				if ( defGrid[x][y] != null ) {
					maxX = Math.max( maxX, x+1);
					break;
				}
			}
		}
	}		
	for ( x=MAXGRIDSIZE-1; x>-1; x--) {
		if ( defGrid[x][MAXGRIDSIZE-1] != null ) {
			maxY = Math.max( maxY, myr);
		}
		else {
			for ( y=MAXGRIDSIZE-1; y>-1; y--) {
				if ( defGrid[x][y] != null ) {
					maxY = Math.max( maxY, y+1);
					break;
				}
			}
		}
	}		

	boolean allocNew = (columnWidths == null) || (rowHeights == null) ||
	(columnWidths.length != maxX) || (rowHeights.length != maxY);

	if ( allocNew ) {
		columnWidths = new int[maxX];
		columnWeights = new double[maxX];
		rowHeights = new int[maxY];
		rowWeights = new double[maxY];
	}
	else {
		for ( x=0; x<maxX; x++ ) {
			columnWidths[x] = 0;
			columnWeights[x] = 0.0;
		}
		for ( y=0; y<maxY; y++ ) {
			rowHeights[y] = 0;
			rowWeights[y] = 0.0;
		}
	}

	// convert relatives and remainders to real values	
	updateConstraints( parent);

	//fill widths and heights
	Rectangle cRect = new Rectangle();
	int nc = parent.getComponentCount();

	for ( int idx=0; idx<nc; idx++) {
		Component c = parent.getComponent( idx);
		cc = lookupConstraints( c);	
		cRect.setBounds( cc.gridx, cc.gridy, cc.gridwidth, cc.gridheight);	

		if ( (cRect.width == 1) || (cRect.height == 1) ) {
			getComponentExt( c, cc, mode, cd);

			if ( cRect.width == 1) {
				columnWidths[cRect.x] = Math.max( columnWidths[cRect.x], cd.width );
			}
			if ( cRect.height == 1) {
				rowHeights[cRect.y] = Math.max( rowHeights[cRect.y], cd.height);
			}			
		}

		di = cRect.x + cRect.width - 1;
		columnWeights[di] = Math.max( columnWeights[di] ,cc.weightx);
		di = cRect.y + cRect.height - 1;
		rowWeights[di] = Math.max( rowWeights[di] ,cc.weighty);
	}

	/* first call of getComponentExt() / getPreferredSize() could
	*  scramble the defGrid array ( cascaded GridBagLayouts ).
	*  Use the (updated) Constraints!!!
	*/
	for ( x=0; x<maxX; x++) {
		if ( columnWidths[x] == 0 ) {
			for ( y=0; y<maxY; y++ ) {
				Component c = componentAt( parent, x, y);
//				Component c = defGrid[x][y];
				if ( c != null ) {
					cc = lookupConstraints( c);
					getComponentExt( c, cc, mode, cd);
					cRect.setBounds( cc.gridx, cc.gridy, cc.gridwidth, cc.gridheight);	
					columnWidths[x] = Math.max( columnWidths[x], cd.width / cRect.width );
				}
			}
		}
	}

	for ( y=0; y<maxY; y++) {
		if ( rowHeights[y] == 0 ) {
			for ( x=0; x<maxX; x++ ) {
				Component c = componentAt(parent, x, y);
//				Component c = defGrid[x][y];
				if ( c != null ) {
					cc = lookupConstraints( c);
					getComponentExt( c, cc, mode, cd);
					cRect.setBounds( cc.gridx, cc.gridy, cc.gridwidth, cc.gridheight);	
					rowHeights[y] = Math.max( rowHeights[y], cd.height / cRect.height );
				}
			}
		}
	}

	if ( mode == ACTUALSIZE) {
		adjustWeightsRelative( parent);
	}
}

Dimension getComponentExt( Component c, GridBagConstraints cc, int mode, Dimension dim) {
	dim.setSize( (mode == MINSIZE) ? c.getMinimumSize() : c.getPreferredSize());
	dim.width += cc.insets.left + cc.insets.right + 2 * cc.ipadx;
	dim.height += cc.insets.top + cc.insets.bottom + 2 * cc.ipady;
	return dim;
}

public GridBagConstraints getConstraints( Component c) {
	return ((GridBagConstraints)lookupConstraints(c).clone());
}

void getGrid( Container parent, int mode ) {
	int x1 = 0;
	int y1 = 0;
	int cx, cy;
	int mxr = -1;
	int myr = -1;
	GridBagConstraints lcc = null;
	int nc = parent.getComponentCount();

	synchronized( defGrid) {	
		clearDefGrid();

		for ( int i=0; i<nc; i++){
			Component c = parent.getComponent(i);
			GridBagConstraints cc = lookupConstraints( c);

			if ( cc.gridx == cc.RELATIVE) {
				if ( lcc == null )
					x1 = 0;
				else if ( lcc.gridwidth > 0 )
					x1 += lcc.gridwidth;
				else
					x1 = -1;
			}
			else {
				x1 = cc.gridx;
			}

			if ( cc.gridy == cc.RELATIVE) {
				if ( lcc == null )
					y1 = 0;
				else if ( lcc.gridheight == cc.REMAINDER )
					y1 = -1;
				else if ( lcc.gridwidth == cc.REMAINDER )
					y1 += Math.max( lcc.gridheight, 1);
				else
					y1 = -1;
			}
			else {
				y1 = cc.gridy;
			}

			if ( x1 == -1 ) {
				for ( x1=0; defGrid[x1][y1] != null; x1++);
			}
			if ( y1 == -1 ) {
				for ( y1=0; defGrid[x1][y1] != null; y1++);
			}


			cx = cc.gridwidth;
			cy = cc.gridheight;

			if ( cx == cc.REMAINDER ){
				mxr = Math.max( mxr, x1);
				cx = MAXGRIDSIZE - x1;
			}
			else if ( cx == cc.RELATIVE ) {
				cx = 1;
			}
			if ( cy == cc.REMAINDER ) {
				myr = Math.max( myr, y1);
				cy = MAXGRIDSIZE - y1;
			}
			else if ( cy == cc.RELATIVE) {
				cy = 1;
			}

			for( cy--; cy > -1; cy--) {
				for ( int icx=cx-1; icx > -1; icx--) {
					defGrid[x1+icx][y1+cy] = c;
				}
			}

			lcc = cc;
		}

		fillGrid( parent, mxr+1, myr+1, mode);
	}
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

public Point getLayoutOrigin() {
	return offset;
}

public double[][] getLayoutWeights() {
	return (new double[][]{columnWeights, rowWeights});
}

Rectangle gridToPels( Rectangle r ) {
	int x, y, xp, yp, wp, hp;

	for ( xp=0, x=0; x<r.x; x++) {
		xp += columnWidths[x];
	}
	for ( wp=0; x<r.x+r.width; x++) {
		wp += columnWidths[x];
	}
	for ( yp=0, y=0; y<r.y; y++) {
		yp += rowHeights[y];
	}
	for ( hp=0; y<r.y+r.height; y++) {
		hp += rowHeights[y];
	}

	r.setBounds( xp, yp, wp, hp);
	return r;
}

public void invalidateLayout( Container parent) {
}

public void layoutContainer( Container parent) {
	Insets pin = parent.getInsets();
	Rectangle cRect = new Rectangle();
	int nc = parent.getComponentCount();

	getGrid( parent, ACTUALSIZE);	

	for ( int idx=0; idx<nc; idx++) {
		Component c = parent.getComponent( idx);
		GridBagConstraints cc = lookupConstraints( c);
		Insets in = cc.insets;

		cRect.setBounds( cc.gridx, cc.gridy, cc.gridwidth, cc.gridheight);	
		gridToPels( cRect);

		Dimension cd = c.getPreferredSize();
		int cx = cRect.x + pin.left + offset.x;
		int cy = cRect.y + pin.top + offset.y;
		int cw = Math.min( cd.width, cRect.width);
		int ch = Math.min( cd.height, cRect.height);

		switch ( cc.fill ) {
		case cc.BOTH:
			cx += in.left + cc.ipadx;
			cy += in.top + cc.ipady;
			cw = cRect.width - (in.left + in.right + 2 * cc.ipadx);
			ch = cRect.height - (in.top + in.bottom + 2 * cc.ipady);
			break;

		case cc.HORIZONTAL:
			cx += in.left + cc.ipadx;
			cw = cRect.width - (in.left + in.right + 2 * cc.ipadx);
			switch ( cc.anchor ) {
			case cc.WEST:
			case cc.CENTER:
			case cc.EAST:
			default:
				cy += (cRect.height - ch) / 2;
				break;

			case cc.NORTH:
			case cc.NORTHEAST:
			case cc.NORTHWEST:
				cy += in.top + cc.ipady;
				break;

			case cc.SOUTHEAST:
			case cc.SOUTH:
			case cc.SOUTHWEST:
				cy += cRect.height - (cd.height + in.bottom + cc.ipady);
				break;
			}
			break;

		case cc.VERTICAL:
			cy += in.top + cc.ipady;
			ch = cRect.height - (in.top + in.bottom + 2 * cc.ipady);
			switch ( cc.anchor ) {
			case cc.NORTH:
			case cc.CENTER:
			case cc.SOUTH:
			default:
				cx += (cRect.width - cw) / 2;
				break;

			case cc.NORTHWEST:
			case cc.WEST:
			case cc.SOUTHWEST:
				cx += in.left + cc.ipadx;
				break;

			case cc.NORTHEAST:
			case cc.SOUTHEAST:
			case cc.EAST:
				cx += cRect.width - (cw + in.right + cc.ipadx);
				break;
			}
			break;

		case cc.NONE:
		default:
			switch ( cc.anchor ) {
			case cc.NORTH:
				cy += in.top + cc.ipady;
				cx += (cRect.width - cw) / 2;
				break;

			case cc.NORTHEAST:
				cy += in.top + cc.ipady;
				cx += cRect.width - (cw + in.right + cc.ipadx);
				break;

			case cc.EAST:
				cy += (cRect.height - ch) / 2;
				cx += cRect.width - (cw + in.right + cc.ipadx);
				break;

			case cc.SOUTHEAST:
				cy += cRect.height - (ch + in.bottom + cc.ipady);
				cx += cRect.width - (cw + in.right + cc.ipadx);
				break;

			case cc.SOUTH:
				cy += cRect.height - (ch + in.bottom + cc.ipady);
				cx += (cRect.width - cw) / 2;
				break;

			case cc.SOUTHWEST:
				cy += cRect.height - (ch + in.bottom + cc.ipady);
				cx += in.left + cc.ipadx;
				break;

			case cc.WEST:
				cy += (cRect.height - ch) / 2;
				cx += in.left + cc.ipadx;
				break;

			case cc.NORTHWEST:
				cy += in.top + cc.ipady;
				cx += in.left + cc.ipadx;
				break;

			case cc.CENTER:
			default:
				cy += (cRect.height - ch) / 2;
				cx += (cRect.width - cw) / 2;
				break;
			}
		}

		c.setBounds( cx, cy, cw, ch);
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

	getGrid( parent, MINSIZE);
	return new Dimension( sumWidths()+in.left+in.right, sumHeights()+in.top+in.bottom );
}

public Dimension preferredLayoutSize( Container parent) {
	Insets in = parent.getInsets();
	getGrid( parent, PREFERREDSIZE);
	return new Dimension( sumWidths()+in.left+in.right, sumHeights()+in.top+in.bottom );
}

public void removeLayoutComponent( Component c) {
}

public void setConstraints( Component comp, GridBagConstraints cons) {
	//	System.out.println( "set: " + this + " " + comp.getClass().getName() + " " + cons);
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
	//	return Integer.toHexString( hashCode() );

	int w = (columnWidths != null) ? columnWidths.length : 0;
	int h = (rowHeights != null) ? rowHeights.length : 0;
	return ("GridBagLayout columns: " + w + ",rows: " + h);
}

void updateConstraints( Container parent) {
	// convert relative and remainder constraints
	// to real values
	int mx = columnWidths.length;
	int my = rowHeights.length;
	int nc = parent.getComponentCount();

	for ( int idx=0; idx<nc; idx++) {
		Component c = parent.getComponent( idx);
		GridBagConstraints cc = lookupConstraints( c);
		for ( int y=0; y<my; y++) {
			for ( int x=0; x<mx; x++) {
				if ( c == defGrid[x][y] ) {
					cc.gridx = x;
					cc.gridy = y;
					while ( (++x<mx) && (defGrid[x][y] == c) );
					while ( (++y<my) && (defGrid[x-1][y] == c) );
					cc.gridwidth = x-cc.gridx;
					cc.gridheight = y-cc.gridy;
					y = my;
					break;
				}
			}
		}
	}
}
}
