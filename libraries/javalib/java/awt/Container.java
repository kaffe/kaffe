package java.awt;

import java.awt.event.ContainerEvent;
import java.awt.event.ContainerListener;
import java.awt.event.PaintEvent;
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
	ContainerListener cntrListener;
	Insets insets = Insets.noInsets;

protected Container () {
}

public Component add (Component child) {
	add(child, null, -1);
	return (child);
}

public void add(Component child, Object constraints) {
	// seems to be mapped to add(Component,Object,int) - since this is public
	// (and can be redefined) we have to do the same <sigh>
	add(child, constraints, -1);
}

public void add(Component child, Object constraints, int index) {
	addImpl(child, constraints, index);
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

	synchronized ( treeLock ) {
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

		if ( child.parent != null ) {
			child.parent.remove(child);
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

		if ( (flags & IS_VALID) != 0 )
			invalidate();

		// if we are already addNotified (this is a subsequent add),
		// we immediately have to addNotify the child, too
		if ( (flags & IS_ADD_NOTIFIED) != 0 ) {
			child.addNotify();
		
			// This isn't required in case we are subsequently validated (what is the
			// correct thing to do), but native widgets would cause a repaint regardless
			// of that. Comment this out if you believe in correct apps
			if ( (child.flags & IS_NATIVE_LIKE) != 0 )
				child.repaint();
		}

		// inherit parent attributes (if not overriden by child)
		if ( (flags & (IS_PARENT_SHOWING | IS_VISIBLE)) == (IS_PARENT_SHOWING | IS_VISIBLE) ){
			child.flags |= IS_PARENT_SHOWING;
			child.propagateParentShowing();
		}
		if ( (child.flags & IS_BG_COLORED) == 0 )
			child.propagateBgClr( bgClr);
		if ( (child.flags & IS_FG_COLORED) == 0 )
			child.propagateFgClr( fgClr);
		if ( (child.flags & IS_FONTIFIED) == 0 )
			child.propagateFont( font);
			

		// Some LayoutManagers track adding/removing components. Since this seems to be
		// done after a potential addNotify, inform them here
		// (wouldn't it be nice to have a single LayoutManager interface?)
		if ( layoutm != null ) {
			if ( layoutm instanceof LayoutManager2 ) {
				((LayoutManager2)layoutm).addLayoutComponent( child, constraints);
			}
			if (constraints instanceof String) {
				layoutm.addLayoutComponent( (String)constraints, child);
			}
		}
	
		if ( hasToNotify( this, AWTEvent.CONTAINER_EVENT_MASK, cntrListener) ){
			AWTEvent.sendEvent( ContainerEvt.getEvent( this,
			                       ContainerEvent.COMPONENT_ADDED, child), false);
		}
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

	xClip = g.xClip + g.xOffset;
	yClip = g.yClip + g.yOffset;
	xwClip = xClip + g.wClip;
	yhClip = yClip + g.hClip;
	
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

void dump ( String prefix ) {
	String prfx = prefix + "  ";

	super.dump( prefix);
	
	for ( int i=0; i<nChildren; i++ ) {
		children[i].dump( prfx + i + " ");
	}
}

void emitRepaints ( int ux, int uy, int uw, int uh ) {
	// This looks too similiar to NativeGraphics.paintChild(), but we can't
	// move that one (because of clip bounds access). On the other hand, we
	// don't want to give up precise clipping here (flicker)

	for ( int i=0; i<nChildren; i++ ) {
		Component c = children[i];
		
		if ( (c.flags & (IS_VISIBLE|IS_DIRTY)) == (IS_VISIBLE|IS_DIRTY) ) {
			int xw = c.x + c.width;
			int yh = c.y + c.height;
			int uxw = ux + uw;
			int uyh = uy + uh;

			int clx = (c.x > ux) ? c.x : ux;
			int cly = (c.y > uy) ? c.y : uy;
			int clw = ((xw > uxw) ? uxw : xw) - clx;
			int clh = ((yh > uyh) ? uyh : yh) - cly;

			clx -= c.x;
			cly -= c.y;

			c.repaint( clx, cly, clw, clh);
		
			if ( c instanceof Container )
				((Container)c).emitRepaints( clx, cly, clw, clh);
		}
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

boolean hasDirties () {
	for ( int i=0; i<nChildren; i++ ) {
		Component c = children[i];
		
		if ( (c.flags & IS_DIRTY) != 0 )
			return true;

		if ( c instanceof Container ) {
			if ( ((Container)c).hasDirties() )
				return true;
		}
	}
	
	return false;
}

public void hide () {
	if ( (flags & IS_PARENT_SHOWING) != 0){
		for ( int i=0; i<nChildren; i++ ) {
			Component c = children[i];
			
			c.flags &= ~IS_PARENT_SHOWING;
			if ( (c.flags & IS_VISIBLE) != 0 )
				c.propagateParentShowing();
		}
	}

	super.hide();
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

	synchronized ( treeLock ) {
		if ( (flags & IS_VALID) != 0 ) {
			// apparently, the JDK does inform the layout *before* invalidation
			if ( layoutm instanceof LayoutManager2 ) {
				((LayoutManager2)layoutm).invalidateLayout( this);
			}
		
			flags &= ~IS_VALID;
		
			// we have to check the parent since we might be a Window
			if ( (parent != null) && ((parent.flags & IS_VALID) != 0) )
				parent.invalidate();
		}
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
	// We keep the 'isLayouting' state regardless of the validation scheme, since
	// it is the way to prevent lots of redraws during recursive layout descents

	// another slow-downer: the async validation of swing forces us to sync on treeLock
	synchronized ( treeLock ) {
		if ( (layoutm != null) && (nChildren > 0) && ((flags & IS_LAYOUTING) == 0) ) {
			flags |= IS_LAYOUTING;
			layoutm.layoutContainer( this);
			flags &= ~IS_LAYOUTING;
		}
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

void markRepaints ( int ux, int uy, int uw, int uh ) {
	for ( int i=0; i<nChildren; i++ ) {
		Component c = children[i];
		
		if ( (c.flags & IS_VISIBLE) != 0 ){
			if ( ((c.flags & IS_NATIVE_LIKE) != 0) &&
			  	 (ux < (c.x + c.width)) &&
			     (uy < (c.y + c.height)) &&
			     (c.x < (ux + uw)) &&
			     (c.y < (uy + uh)) ){
				c.flags |= IS_DIRTY;
			}
		
			if ( c instanceof Container )
				((Container)c).markRepaints( ux+ x, uy+ y, uw, uh);
		}
	}
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
	// standard JDK behavior is to paint last added childs first, simulating
	// a first-to-last z order
	for ( int i=nChildren-1; i>=0; i-- ) {
		Component c = children[i];

		if ( (c.flags & IS_VISIBLE) != 0 ) {
			g.paintChild( c, (flags & IS_IN_UPDATE) != 0);
		}
	}
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

void processPaintEvent ( int id, int ux, int uy, int uw, int uh ) {
	NativeGraphics g = NativeGraphics.getClippedGraphics( null, this, 0,0,
	                                                      ux, uy, uw, uh,
	                                                      false);
	if ( g != null ){
		markRepaints( ux, uy, uw, uh);
	
		if ( id == PaintEvent.UPDATE ) {
			update( g);
		}
		else {
		  paint( g);
		}
		g.dispose();
		
		if ( hasDirties() )
			emitRepaints( ux, uy, uw, uh);
	}
}

void propagateBgClr ( Color clr ) {
	bgClr = clr;

	for ( int i=0; i<nChildren; i++ ){
		Component c = children[i];
		if ( (c.flags & IS_BG_COLORED) == 0 ){
			c.propagateBgClr( clr);
		}
	}
}

void propagateFgClr ( Color clr ) {
	fgClr = clr;

	for ( int i=0; i<nChildren; i++ ){
		Component c = children[i];
		if ( (c.flags & IS_FG_COLORED) == 0 ){
			c.propagateFgClr( clr);
		}
	}
}

void propagateFont ( Font fnt ) {
	font = fnt;

	for ( int i=0; i<nChildren; i++ ){
		Component c = children[i];
		if ( (c.flags & IS_FONTIFIED) == 0 ){
			c.propagateFont( fnt);
		}
	}
}

void propagateParentShowing () {
	if ( (flags & IS_VISIBLE) == 0 ) // nothing to do, we are a visibility leaf
		return;

	if ( (flags & IS_PARENT_SHOWING) != 0 ) {
		for ( int i=0; i<nChildren; i++ ){
			children[i].flags |= IS_PARENT_SHOWING;
			children[i].propagateParentShowing();
		}
	}
	else {
		for ( int i=0; i<nChildren; i++ ){
			children[i].flags &= ~IS_PARENT_SHOWING;
			children[i].propagateParentShowing();
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
	synchronized ( treeLock ) {
		int n = nChildren - 1;

		if (index < 0 && index > n) {
			return;
		}
		
		Component c = children[index];

		if ( (c.flags & IS_ADD_NOTIFIED) != 0 ){
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

		if ( (flags & IS_VALID) != 0 )
			invalidate();

		c.flags &= ~IS_PARENT_SHOWING;
		c.propagateParentShowing();

		// Like in addImpl, this wouldn't be required in case we are subsequently
		// validated, again. However, native widgets cause a repaint regardless
		// of this validation
		if ( (c.flags & IS_NATIVE_LIKE) != 0 )
			repaint( c.x, c.y, c.width, c.height);
	}
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
	// removeNotify children first (symmetric to addNotify)
	for ( int i=0; i<nChildren; i++ ) {
		if ( (children[i].flags & IS_ADD_NOTIFIED) != 0 ) {
			children[i].removeNotify();
		}
	}
	super.removeNotify();
}

public void setLayout ( LayoutManager newLayout ) {
	layoutm = newLayout;
	
	// this doesn't directly cause a doLayout in JDK, it just enables it
	if ( (flags & IS_VALID) != 0)
		invalidate();
}

public void show () {
	// we have to propagate first to enable subsequent child drawing by super.show()
	if ( (flags & IS_PARENT_SHOWING) != 0){
		for ( int i=0; i<nChildren; i++ ) {
			Component c = children[i];
			
			c.flags |= IS_PARENT_SHOWING;
			if ( (c.flags & IS_VISIBLE) != 0 )
				c.propagateParentShowing();
		}
	}
	
	super.show();
}

public void update ( Graphics g ) {
	flags |= IS_IN_UPDATE;

	// clear and draw yourself
	g.clearRect( 0, 0, width, height);
	paint( g);

	flags &= ~IS_IN_UPDATE;
}

public void validate() {
	synchronized ( treeLock ) {
		if ( (flags & (IS_VALID | IS_ADD_NOTIFIED)) == IS_ADD_NOTIFIED ){
			// we have to descent before validating ourself
			validateTree();
			flags |= IS_VALID;
		}
	}
}

protected void validateTree () {
	doLayout();
	
	for ( int i=0; i<nChildren; i++ )
		children[i].validate();
}
}
