/**
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


public class GridLayout
  implements LayoutManager, java.io.Serializable
{
	/** @serial */
	int cols;
	/** @serial */
	int rows;
	/** @serial */
	int hgap;
	/** @serial */
	int vgap;

	/* NB: Sun doesn't hardcode */
	private static final long serialVersionUID = -7411804673224730901L;

public GridLayout () {
	this( 1, 0, 0, 0);
}

public GridLayout (int rows, int cols) {
	this( rows, cols, 0, 0);
}

public GridLayout (int rows, int cols, int hgap, int vgap) {
  if ( (rows == 0) && (cols == 0) )
    throw new IllegalArgumentException("GridLayout rows and cols cannot both be zero");
  
  this.rows = rows;
  this.cols = cols;
  this.hgap = hgap;
  this.vgap = vgap;
}

public void addLayoutComponent ( String name, Component comp) {
}

Dimension adjustDim ( Container parent) {
  Dimension d = new Dimension( cols, rows);
  int nChildren = parent.getComponentCount(); // beware of Frame Menubars

  // If rows > 0 then ignore cols.
  if (rows > 0) {
    d.height = rows;
    d.width = nChildren / rows;
    if (nChildren % rows != 0) {
      d.width++;
    }
  }
  // Just work from cols
  else {
    d.width = cols;
    d.height = nChildren / cols;
    if (nChildren % cols != 0) {
      d.height++;
    }
  }

  return d;
}

public int getColumns () {
	return cols;
}

public int getHgap () {
	return hgap;
}

Dimension getLayoutSize ( Container parent, boolean preferred) {	
	int maxW = 0;
	int maxH = 0;
	
	Dimension d = adjustDim( parent);
	int nChildren = parent.getComponentCount(); // beware of Frame Menubars
	
	for ( int i=0; i<nChildren; i++) {
		Component c = parent.getComponent( i);
		Dimension cd = preferred ? c.getPreferredSize() : c.getMinimumSize();
		maxW = Math.max( maxW, cd.width);
		maxH = Math.max( maxH, cd.height );
	}

	Insets in = parent.getInsets(); // getInsets() might be reimplemented (swing)
	return new Dimension(	in.left + in.right + maxW * d.width + hgap * ( d.width + 1),
												in.top + in.bottom + maxH * d.height + vgap * ( d.height + 1) );
}

public int getRows () {
	return rows;
}

public int getVgap () {
	return vgap;
}

public void layoutContainer ( Container parent) {
	int nChildren = parent.getComponentCount(); // beware of Frame Menubars
	if (nChildren == 0) return;

	Insets in = parent.getInsets(); // getInsets() might be reimplemented (swing)
	int tw = parent.width - in.left - in.right - hgap;
	int th = parent.height - in.top - in.bottom - vgap;
	
	Dimension d = adjustDim( parent); // #rows & #columns
	int cw = tw / d.width;
	int ch = th / d.height;
	
	int x0 = in.left + hgap;
	int x = x0;
	int y = in.top + vgap;
	int ix = 0;
	
	for ( int i=0; i<nChildren; i++) {
		Component c = parent.getComponent( i);
		c.setBounds( x, y, cw-hgap, ch-vgap);
		if ( ix == d.width-1 ){
			ix = 0;
			x = x0;
			y += ch;
		}
		else {
			ix++;
			x += cw;
		}
	}
}

public Dimension minimumLayoutSize ( Container parent) {
	return getLayoutSize( parent, false);
}

public Dimension preferredLayoutSize ( Container parent) {
	return getLayoutSize( parent, true);
}

public void removeLayoutComponent ( Component comp) {
}

public void setColumns ( int cols) {
	this.cols = cols;
}

public void setHgap ( int hgap) {
	this.hgap = hgap;
}

public void setRows ( int rows) {
	this.rows = rows;
}

public void setVgap ( int vgap) {
	this.vgap = vgap;
}

public String toString () {
	return ("GridLayout: rows: " + rows + ",cols: " + cols + ",hgap: " + hgap + ",vgap: " + vgap);
}
}
