package java.awt;

import java.io.Serializable;
import java.util.Enumeration;
import java.util.Hashtable;

public class CardLayout
  implements LayoutManager2, Serializable
{
	/** @serial undocumented */
	int hgap;
	/** @serial undocumented */
	int vgap;
	/** @serial undocumented */
	Hashtable tab = new Hashtable();

	/* Sun's doesn't hardcode, we do */
	private static final long serialVersionUID = -4328196481005934313L;

public CardLayout () {
	this( 0, 0);
}

public CardLayout ( int hgap, int vgap) {
	this.hgap = hgap;
	this.vgap = vgap;
}

public void addLayoutComponent ( Component comp, Object constraints) {
	if ( constraints instanceof String) {
		addLayoutComponent( (String)constraints, comp);
	}
	else {
		throw new IllegalArgumentException("non-string constraint");
	}
}

/**
 * @deprecated
 */
public void addLayoutComponent ( String name, Component comp) {
	tab.put( name, comp);
	if ( tab.size() > 1 )
		comp.setVisible( false);
}

public void first ( Container parent) {
	Component fc = null;
	int cc = parent.getComponentCount();

	for ( int i=0; i<cc; i++) {
		Component c = parent.getComponent(i);
		if ( fc == null )
			fc = c;
		if ( (c.flags & Component.IS_VISIBLE) != 0 ) {
			if ( c != fc ) {
				c.setVisible( false);
				fc.setVisible( true);
			}
			return;
		}
	}
}

public int getHgap () {
	return hgap;
}

public float getLayoutAlignmentX ( Container parent ) {
	return (Component.CENTER_ALIGNMENT);
}

public float getLayoutAlignmentY ( Container parent ) {
	return (Component.CENTER_ALIGNMENT);
}

private Dimension getLayoutSize (  Container parent, boolean preferred) {
	Dimension d = new Dimension();
	int cc = parent.getComponentCount();

	for ( int i=0; i<cc; i++) {
		Component c = parent.getComponent(i);
		Dimension cd = preferred ? c.getPreferredSize() : c.getMinimumSize();
		d.width = Math.max( d.width, cd.width );
		d.height = Math.max( d.height, cd.height );
	}
	
	// use getInsets() instead of fields (might be redefined)
	Insets in = parent.getInsets();
	d.width += in.left + in.right;
	d.height += in.top + in.bottom;
	
	return d;
}

public int getVgap () {
	return vgap;
}

public void invalidateLayout ( Container parent) {
}

public void last ( Container parent) {
	Component lc = null;
	int cc = parent.getComponentCount();

	for ( int i=cc-1; i>=0; i--) {
		Component c = parent.getComponent(i);
		if ( lc == null )
			lc = c;
		if ( (c.flags & Component.IS_VISIBLE) != 0) {
			if ( c != lc ) {
				c.setVisible( false);
				lc.setVisible( true);
			}
			return;
		}
	}
}

public void layoutContainer ( Container parent) {
	Insets in = parent.getInsets();  // getInsets() might be redefined (swing)
	int cc = parent.getComponentCount();

	for ( int i=0; i<cc; i++) {
		parent.getComponent(i).setBounds(	in.left + hgap,
																	in.top + vgap,
																	parent.width - 2*hgap - in.left - in.right,
																	parent.height - 2*vgap - in.top - in.bottom );
	}
}

public Dimension maximumLayoutSize ( Container parent ) {
	return Toolkit.singleton.getScreenSize();
}

public Dimension minimumLayoutSize ( Container parent ) {
	return getLayoutSize( parent, false);
}

public void next ( Container parent) {
	Component lc = null;
	int cc = parent.getComponentCount();

	for ( int i=0; i<cc; i++) {
		Component c = parent.getComponent(i);
		if ( (c.flags & Component.IS_VISIBLE) != 0 )
			lc = c;
		else if ( lc != null) {
			lc.setVisible( false);
			c.setVisible( true);
			return;
		}
	}
}

public Dimension preferredLayoutSize ( Container parent) {
	return getLayoutSize( parent, true);
}

public void previous ( Container parent) {
	Component lc = null;
	int cc = parent.getComponentCount();

	for ( int i=cc-1; i>=0; i--) {
		Component c = parent.getComponent(i);
		if ( (c.flags & Component.IS_VISIBLE) != 0 )
			lc = c;
		else if ( lc != null) {
			lc.setVisible( false);
			c.setVisible( true);
			return;
		}
	}
}

public void removeLayoutComponent ( Component comp) {
	for( Enumeration e = tab.keys(); e.hasMoreElements(); ) {
		String key = (String)e.nextElement();
		Component c = (Component)tab.get( key);
		if ( c == comp ) {
			tab.remove( key);
			return;
		}
	}
}

public void setHgap ( int hgap) {
	this.hgap = hgap;
}

public void setVgap ( int vgap) {
	this.vgap = vgap;
}

public void show ( Container parent, String name) {
	Component nc = (Component)tab.get( name);
	if ( nc == null )
		return;
	int cc = parent.getComponentCount();
	
	for ( int i=0; i<cc; i++) {
		Component c = parent.getComponent(i);
		if ( (c.flags & Component.IS_VISIBLE) != 0 ) {
			if ( c != nc) {
				c.setVisible( false);
				nc.setVisible( true);
			}
			return;
		}
	}
}

public String toString () {
	return ("CardLayout: hgap: " + hgap + ",vgap: " + vgap);
}
}
