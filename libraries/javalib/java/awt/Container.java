package java.awt;

import java.awt.event.ContainerEvent;
import java.awt.event.ContainerListener;
import kaffe.awt.OpaqueComponent;

/**
 * Container - abstract base for all Components capable of having children
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */
abstract public class Container
  extends Component
{
	Component[] children;
	int nChildren;
	LayoutManager layout = dummyLayout;
	boolean isLayouting;
	ContainerListener cntrListener;
	Insets insets = Insets.noInsets;
	private static LayoutManager dummyLayout = new FlowLayout();

protected Container () {

}

public Component add ( Component child ) {
	addImpl( child, null, -1);
	return child;
}

public void add ( Component child, Object constraints ) {
	addImpl( child, constraints, -1);
}

public void add ( Component child, Object constraints, int index ) {
	addImpl(child, constraints, index);
}

public Component add ( Component child, int index ) {
	addImpl( child, null, index);
	return child;
}

public Component add ( String name, Component child) {
	addImpl( child, name, -1);
	if ( layout != null)
		layout.addLayoutComponent(name, child);
	return child;
}

public void addContainerListener ( ContainerListener newListener ) {
	cntrListener = AWTEventMulticaster.add( cntrListener, newListener);
	eventMask |= AWTEvent.CONTAINER_EVENT_MASK;
}

protected void addImpl ( Component child, Object constraints, int index ) {
	if ( child.parent != null )
		child.parent.remove( child);

	if ( children == null )
		children= new Component[3];
	else if ( nChildren == children.length ) {
		Component[] old = children;
		children = new Component[ nChildren * 2];
		System.arraycopy( old, 0, children, 0, nChildren);
	}
		
	if ( (index < 0) || (nChildren == 0) || (index == nChildren) ) {  // append
		children[nChildren] = child;
	}
	else if ( index < nChildren ) {     // insert at index
		System.arraycopy( children, index, children, index+1, nChildren - index);
		children[index] = child;
	}
	nChildren++;
	
	child.parent = this;
	
	if ( layout == dummyLayout )
		layout = getDefaultLayout();
		
	if ( (layout != null) && (layout instanceof LayoutManager2) )
			((LayoutManager2)layout).addLayoutComponent( child, constraints);

	if ( hasToNotify( AWTEvent.CONTAINER_EVENT_MASK, cntrListener) )
		Toolkit.eventQueue.postEvent( new ContainerEvent( this, ContainerEvent.COMPONENT_ADDED, child));
}

public void addNotify() {
	super.addNotify();

	for ( int i=0; i<nChildren; i++ )
		children[i].addNotify();
}

Graphics clipSiblings ( Component child, Graphics g ) {
	int        i, xClip, yClip, xwClip, yhClip, cxw, cyh;
	Component  c;

	xClip = g.clip.x + g.xOffset;
	yClip = g.clip.y + g.yOffset;
	xwClip = xClip + g.clip.width;
	yhClip = yClip + g.clip.height;
	
	for ( i=nChildren-1; (i >= 0) && (children[i] != child) ; i-- );
	for ( i--; i >= 0; i-- ) {
		c = children[i];

		cxw = c.x + c.width;
		cyh = c.y + c.height;
		
		if ( (c.x > xwClip) || (c.y > yhClip) || (cxw < xClip) || (cyh < yClip) )
			continue;
			
		// not yet implemented
	}
	
	return g;
}

public void doLayout() {
	if ( (layout != null) && (nChildren > 0) && !isLayouting ) {
		isLayouting = true;
		layout.layoutContainer( this);
		isLayouting = false;

		if ( isShowing() ) {
			if ( parent != null ){
				if ( !parent.isLayouting ) { // no need to traverse upwards
					if ( this instanceof  OpaqueComponent )
						repaint();
					else
						parent.repaint( x, y, width, height);
				}
			}
			else
				repaint();
		}
	}
}

public float getAlignmentX () {
	if ( layout instanceof LayoutManager2 )
		return ((LayoutManager2)layout).getLayoutAlignmentX( this);
	else
		return super.getAlignmentX();
}

public float getAlignmentY () {
	if ( layout instanceof LayoutManager2 )
		return ((LayoutManager2)layout).getLayoutAlignmentY( this);
	else
		return super.getAlignmentY();
}

public Component getComponent ( int index ) {
	return children[index];
}

public Component getComponentAt ( Point pt ) {
	return getComponentAt( pt.x, pt.y);
}

public Component getComponentAt ( int x, int y ) {
	Component c;

	if ( !isShowing() || (x<0) || (x > this.x+width) || (y<0) || (y > this.y+height) )
		return null;
		
	for ( int i=0; i<nChildren; i++ ) {
		c = children[i];
		if ( c.contains( x - c.x, y - c.y)) {
			return (c.isShowing() ? c : this);
		}
	}
	
	return this;
}

public int getComponentCount() {
	return nChildren;
}

public Component[] getComponents () {
	Component ca[] = new Component[nChildren];
	
	if ( nChildren > 0 )
		System.arraycopy( children, 0, ca, 0, nChildren);
	
	return ca;
}

LayoutManager getDefaultLayout() {
	return null;
}

public Insets getInsets () {
	return insets;
}

public LayoutManager getLayout () {
	return layout;
}

public Dimension getMaximumSize () {
	// 'LayoutManager' does not have a maximumLayoutSize()
	if ( (layout != null) && (layout instanceof LayoutManager2) ) 
		return ((LayoutManager2) layout).maximumLayoutSize( this);
	else
		return super.getMaximumSize();			
}

public Dimension getMinimumSize () {
	if ( layout != null )
		return layout.minimumLayoutSize( this);
	else
		return super.getMinimumSize();
}

public Dimension getPreferredSize() {
	if ( layout != null )
		return layout.preferredLayoutSize( this);
	else
		return super.getPreferredSize();
}

/**
 * @deprecated, use getInsets()
 */
public Insets insets () {
	return getInsets();
}

public boolean isAncestorOf ( Component c ) {
	for ( c = c.parent; c != null; c = c.parent) {
		if ( c == this ) return true;
	}

	return false;
}

public boolean isValid () {
	return true;
}

public void paint ( Graphics g ) {

	if ( g == null ) return;  // not visible

	int         i;
	Component   c;
	Rectangle   clip = g.clip;
	int         xClip  = clip.x;
	int         yClip  = clip.y;
	int         xwClip = xClip + clip.width;
	int         yhClip = yClip + clip.height;
	int         cx, cy;
	int         cxw, cyh, clx, cly, clw, clh;
	Font        fnt = g.fnt;
	Color       fg = g.fg;
	Color       bg = g.bg;

	// standard JDK behavior is to paint last added childs first, simulating
	// a first-to-last z order
	for ( i=nChildren-1; i>=0; i-- ) {
		c = children[i];

		if ( !c.isVisible )
			continue;

		cx = c.x;
		cy = c.y;
		cxw = cx + c.width;
		cyh = cy + c.height;

		// do we have an intersection ?
		if ( (cxw < xClip) || (cyh < yClip) || (xwClip < cx) || (yhClip < cy) )
			continue;
		
		clx = (xClip < cx) ? 0 : xClip - cx;
		cly = (yClip < cy) ? 0 : yClip - cy;
		
		clw = ((xwClip < cxw) ? xwClip : cxw) - cx - clx;
		clh = ((yhClip < cyh) ? yhClip : cyh) - cy - cly;
		
		g.translate( cx, cy);
		g.setClip( clx, cly, clw, clh);
		if ( c.font != null ) g.setFont( c.font);
		if ( c.fgClr != null ) g.setColor( c.fgClr);
		if ( c.bgClr != null ) g.setBackColor( c.bgClr);

		c.paint( g);
		
		g.translate( -cx, -cy);
		g.setFont( fnt);
		g.setColor( fg);
		g.setBackColor( bg);
	}
	
	g.setClip( xClip, yClip, (xwClip - xClip), (yhClip - yClip));
}

public void printComponents ( Graphics g ) {
}

public void processContainerEvent ( ContainerEvent event ) {
	if ( hasToNotify( AWTEvent.CONTAINER_EVENT_MASK, cntrListener) ) {
		switch ( event.getID() ) {
		case ContainerEvent.COMPONENT_ADDED:
			cntrListener.componentAdded( event);
			break;
		case ContainerEvent.COMPONENT_REMOVED:
			cntrListener.componentRemoved( event);
			break;
		}
	}
}

public void remove ( Component c ) {

	// usually children are added/removed in a stack like fashion
	for ( int i=nChildren-1; i>=0; i-- ){
		if ( children[i] == c ) {
			remove( i);
			break;
		}
	}
}

public void remove ( int index ) {
	int n = nChildren - 1;

	if ( (index < 0) && (index > n)  )
		return;
		
	Component c = children[index];

	if ( layout != null )
		layout.removeLayoutComponent( c);

	if ( (index < n) && (index > -1) )
		System.arraycopy( children, index+1, children, index, n-index);
	children[n] = null;
	nChildren--;
	
	c.removeNotify();

	if ( hasToNotify( AWTEvent.CONTAINER_EVENT_MASK, cntrListener) ){
		Toolkit.eventQueue.postEvent( new ContainerEvent( this, ContainerEvent.COMPONENT_REMOVED, c));
	}

	doLayout();
}

public void removeAll () {
	Component c;

	for ( int i=nChildren -1; i>=0; i-- ) {
		c = children[i];
		c.removeNotify();

		if ( layout != null )  // maybe the layout stores private references
			layout.removeLayoutComponent( c);

		children[i] = null;       // we leave the array in case of subsequent adds

		if ( ((eventMask & AWTEvent.CONTAINER_EVENT_MASK) != 0) && (cntrListener != null) )
			cntrListener.componentRemoved( new ContainerEvent( this, ContainerEvent.COMPONENT_REMOVED, c));
	}
	
	nChildren = 0;
	repaint();  // no need to layout again, we are empty
}

public void removeContainerListener ( ContainerListener listener ) {
	cntrListener = AWTEventMulticaster.remove( cntrListener, listener);
}

public void removeNotify() {
	for ( int i=0; i<nChildren; i++ )
		children[i].removeNotify();
		
	super.removeNotify();
}

public void setEnabled ( boolean isEnable ) {
	super.setEnabled( isEnable);
	
	for ( int i=0; i<nChildren; i++ )
		children[i].setEnabled( isEnable);
}

public void setLayout ( LayoutManager newLayout ) {
	layout = newLayout;
	
	if ( (layout != null) && (nChildren > 0) ) // doLayout() might be resolved (incorrectly)
		doLayout();
}

protected void validateTree() {
	doLayout();
}
}
