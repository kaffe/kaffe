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
	LayoutManager layoutm;
	boolean isLayouting;
	ContainerListener cntrListener;
	Insets insets = Insets.noInsets;

protected Container () {
}

public Component add (Component child) {
	add(child, null, -1);
	return (child);
}

public void add(Component child, Object constraints) {
	add(child, constraints, -1);
}

public void add(Component child, Object constraints, int index) {

	if ( child.parent != null ) {
		child.parent.remove(child);
	}

	addImpl(child, constraints, index);
	
	if ( layoutm != null ) {
		if ( layoutm instanceof LayoutManager2 ) {
			((LayoutManager2)layoutm).addLayoutComponent( child, constraints);
		}
		if (constraints instanceof String) {
			layoutm.addLayoutComponent( (String)constraints, child);
		}
	}
}

public Component add(Component child, int index) {
	add(child, null, index);
	return (child);
}

public Component add(String name, Component child) {
	add(child, (Object)name, -1);
	return (child);
}

public void addContainerListener ( ContainerListener newListener ) {
	cntrListener = AWTEventMulticaster.add( cntrListener, newListener);
				// no need to traverse upwards
	eventMask |= AWTEvent.CONTAINER_EVENT_MASK;
}

protected void addImpl(Component child, Object constraints, int index ) {

	if (index < -1 || index > nChildren) {
		throw new IllegalArgumentException("bad index: " + index);
	}
	// This test isn't done because we actually need this functionality
	// for the native windowing system
	//else if (child instanceof Window) {
	//	throw new IllegalArgumentException("component is Window");
	//}
	else if (child instanceof Container && this.parent == child) {
		throw new IllegalArgumentException("child is a bad container");
	}

	if ( children == null ) {
		children= new Component[3];
	}
	else if ( nChildren == children.length ) {
		Component[] old = children;
		children = new Component[ nChildren * 2];
		System.arraycopy( old, 0, children, 0, nChildren);
	}
		
	if (index < 0 || nChildren == 0 || index == nChildren) {  // append
		children[nChildren] = child;
	}
	else if (index < nChildren) {     // insert at index
		System.arraycopy( children, index, children, index+1, nChildren - index);
		children[index] = child;
	}
	
	nChildren++;
	child.parent = this;

	if ( isValid )
		invalidate();

	// if we are already addNotified (this is a subsequent add),
	// we immediately have to addNotify the child, too
	if (peer != null) {
		child.addNotify();
		
		// This isn't required in case we are subsequently validated (what is the
		// correct thing to do), but native widgets would cause a repaint regardless
		// of that. Comment this out if you believe in correct apps
		if ( child.props.isNativeLike )
			child.repaint();
	}

	if ( hasToNotify( this, AWTEvent.CONTAINER_EVENT_MASK, cntrListener) ){
		AWTEvent.sendEvent( ContainerEvt.getEvent( this,
		                       ContainerEvent.COMPONENT_ADDED, child), false);
	}
}

public void addNotify() {
	super.addNotify();
	for ( int i=0; i<nChildren; i++ ) {
		children[i].addNotify();
	}
}

Graphics clipSiblings ( Component child, NativeGraphics g ) {
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

/**
 * @deprecated, use getComponentCount()
 */
public int countComponents() {
	return nChildren;
}

public void doLayout() {
	layout();
}

void dumpChildren () {
	for ( int i=0; i<nChildren; i++ ) {
		System.out.print( i);
		System.out.print( ": ");
		System.out.println( children[i]);
	}
}

public float getAlignmentX () {
	if ( layoutm instanceof LayoutManager2 ) {
		return ((LayoutManager2)layoutm).getLayoutAlignmentX( this);
	}
	else {
		return super.getAlignmentX();
	}
}

public float getAlignmentY () {
	if ( layoutm instanceof LayoutManager2 ) {
		return ((LayoutManager2)layoutm).getLayoutAlignmentY( this);
	}
	else {
		return super.getAlignmentY();
	}
}

public Component getComponent ( int index ) {
	return children[index];
}

public Component getComponentAt ( Point pt ) {
	return getComponentAt( pt.x, pt.y);
}

public Component getComponentAt ( int x, int y ) {
	return locate ( x, y);
}

public int getComponentCount() {
	return countComponents();
}

public Component[] getComponents () {
	Component ca[] = new Component[nChildren];
	
	if ( nChildren > 0 )
		System.arraycopy( children, 0, ca, 0, nChildren);
	
	return ca;
}

public Insets getInsets () {
	return insets();
}

public LayoutManager getLayout () {
	return (layoutm);
}

public Dimension getMaximumSize () {
	if (layoutm instanceof LayoutManager2) {
		return (((LayoutManager2)layoutm).maximumLayoutSize(this));
	}
	else {
		return (super.getMaximumSize());
	}
}

public Dimension getMinimumSize () {
	return (minimumSize());
}

public Dimension getPreferredSize () {
	return (preferredSize());
}

/**
 * @deprecated, use getInsets()
 */
public Insets insets () {
	// DEP - this should be in getInsets()

	// a Swing LabelPane outcoming (since peers seem to return
	// the *real* inset object, it wouldn't be necessary to copy, otherwise)
	return new Insets( insets.top, insets.left, insets.bottom, insets.right);
}

public void invalidate () {

	if ( isValid ) {
		// apparently, the JDK does inform the layout *before* invalidation
		if ( layoutm instanceof LayoutManager2 ) {
			((LayoutManager2)layoutm).invalidateLayout( this);
		}
		
		isValid = false;
		
		// we have to check the parent since we might be a Window
		if ( (parent != null) && !parent.isValid )
			parent.invalidate();
	}
}

public boolean isAncestorOf ( Component c ) {
	for ( c = c.parent; c != null; c = c.parent) {
		if ( c == this ) return true;
	}

	return false;
}

public void layout() {
	// DEP - this should be in doLayout() (another nasty compat issue)
	// We keep the 'isLayouting' regardless of the validation scheme, since it is
	// the way to prevent lots of redraws during recursive layout descents

	if ( (layoutm != null) && (nChildren > 0) && !isLayouting ) {
		isLayouting = true;
		layoutm.layoutContainer( this);
		isLayouting = false;

/*
		if ( isShowing() ) {
			if ( parent != null ){
				// no need to traverse upwards
				if ( !parent.isLayouting ) {
					if (this instanceof OpaqueComponent) {
						repaint();
					}
					else {
						parent.repaint( x, y, width, height);
					}
				}
			}
			else {
				repaint();
			}
		}
*/
	}
}

/**
 * @deprecated, use getComponentAt().
 */
public Component locate ( int x, int y ) {
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

/**
 * @deprecated, use getMinimumSize()
 */
public Dimension minimumSize () {
	if ( layoutm != null ) {
		return layoutm.minimumLayoutSize( this);
	}
	else {
		return super.minimumSize();
	}
}

public void paint ( Graphics g ) {
	paintChildren( g);
}

void paintChildren ( Graphics g ) {

	if ( g == null ) return;  // not visible

	int         i;
	Component   c;
	Rectangle   clip = g.getClipBounds();
	int         xClip  = clip.x;
	int         yClip  = clip.y;
	int         xwClip = xClip + clip.width;
	int         yhClip = yClip + clip.height;
	int         cx, cy, cxw, cyh, clx, cly, clw, clh;
	Font        fnt = g.getFont();
	Color       fg = g.getColor();
	Color       bg = g.getBackColor();

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

		// we can't call paint directly in case we have a Panel
		// (which wouldn't get its children drawn)

		if ( c instanceof Panel ) {
			c.paintChild( g);
		}
		else
			c.paint( g);
		
		g.translate( -cx, -cy);
		g.setFont( fnt);
		g.setColor( fg);
		g.setBackColor( bg);
	}
	
	g.setClip( xClip, yClip, (xwClip - xClip), (yhClip - yClip));
}

/**
 * @deprecated, use getPreferredSize().
 */
public Dimension preferredSize () {
	if ( layoutm != null ) {
		return (layoutm.preferredLayoutSize(this));
	}
	else {
		return (super.preferredSize());
	}
}

public void printComponents ( Graphics g ) {
}

public void processContainerEvent ( ContainerEvent event ) {
	if ( hasToNotify( this, AWTEvent.CONTAINER_EVENT_MASK, cntrListener) ) {
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
			remove(i);
			break;
		}
	}
}

public void remove ( int index ) {
	int n = nChildren - 1;

	if (index < 0 && index > n) {
		return;
	}
		
	Component c = children[index];
	
	if ( c.peer != null ) {
		c.removeNotify();
	}

	if ( layoutm != null ) {
		layoutm.removeLayoutComponent( c);
	}

	// Remove from container
	c.parent = null;
	if (index > -1 && index < n) {
		System.arraycopy(children, index+1, children, index, n-index);
	}
	children[n] = null;
	nChildren--;
	
	if ( hasToNotify( this, AWTEvent.CONTAINER_EVENT_MASK, cntrListener) ){
		AWTEvent.sendEvent( ContainerEvt.getEvent( this,
		                       ContainerEvent.COMPONENT_REMOVED, c), false);
	}

	if ( isValid )
		invalidate();
		
	// Like in addImpl, this wouldn't be required in case we are subsequently
	// validated, again. However, native widgets cause a repaint regardless
	// of this validation
	if ( c.props.isNativeLike )
		repaint( c.x, c.y, c.width, c.height);
}

public void removeAll () {
	// Nice though it might be to optimize this, we can't without
	// breaking those who might inherit this class and overload
	// things  ...
	for ( int i = nChildren-1; i >= 0; i-- ) {
		remove(i);
	}
}

public void removeContainerListener ( ContainerListener listener ) {
	cntrListener = AWTEventMulticaster.remove( cntrListener, listener);
}

public void removeNotify() {
	for ( int i=0; i<nChildren; i++ ) {
		if (children[i].peer != null) {
			children[i].removeNotify();
		}
	}
	super.removeNotify();
}

public void setEnabled ( boolean isEnable ) {
	super.setEnabled( isEnable);
	
	for ( int i=0; i<nChildren; i++ )
		children[i].setEnabled( isEnable);
}

public void setLayout ( LayoutManager newLayout ) {
	layoutm = newLayout;
	
	// this doesn't directly cause a doLayout in JDK, it just enables it
	if ( isValid )
		invalidate();
}

public void validate() {
	synchronized(getTreeLock()) {
		if ( !isValid && (peer != null) ){
			// we have to descent before validating ourself
			validateTree();
			isValid = true;
		}
	}
}

protected void validateTree () {
	doLayout();
	
	for ( int i=0; i<nChildren; i++ )
		children[i].validate();
}
}
