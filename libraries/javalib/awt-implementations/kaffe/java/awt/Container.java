package java.awt;

import java.awt.event.ContainerEvent;
import java.awt.event.ContainerListener;
import java.awt.event.PaintEvent;
import java.io.PrintStream;
import java.io.PrintWriter;

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
	final private static long serialVersionUID = 4613797578919906343L;
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
	add(child, name, -1);
	return (child);
}

public void addContainerListener ( ContainerListener newListener ) {
	cntrListener = AWTEventMulticaster.add( cntrListener, newListener);
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
			child.propagateParentShowing( false);
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
	
		if ( (cntrListener != null) || (eventMask & AWTEvent.CONTAINER_EVENT_MASK) != 0 ){
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
	// don't want to give up precise clipping, here (flicker).
	// It also does a lot of redundant computation compared to markRepaints, and
	// that is why it should be called after checking "hasDirties()". It's a petty
	// we have to go through all of this because of embedded, app-specific IS_NATIVE_LIKE
	// components (Panels, Canvases), which get their repaints by the native windowing system
	// (and that is NOT specified!!)

	int uxw = ux + uw;
	int uyh = uy + uh;

	for ( int i=0; i<nChildren; i++ ) {
		Component c = children[i];
		
		if ( (c.flags & IS_VISIBLE) != 0 ) {
			int cxw = c.x + c.width;
			int cyh = c.y + c.height;

			int uxx = (ux < c.x ) ? 0 : ux - c.x;
			int uyy = (uy < c.y ) ? 0 : uy - c.y;
			int uww = (uxw < cxw) ? (uxw - c.x - uxx) : c.width;
			int uhh = (uyh < cyh) ? (uyh - c.y - uyy) : c.height;
			
			if ( (c.flags & IS_DIRTY) != 0 ) {
				// Oh what a joy - Panels & Canvases don't get update() called in respond to
				// window manager initiated draw requests (resize). Of course, this is different
				// for explicit repaint() calls, which triggers update(). This is likely to be a bug
				// (in the CanvasPeer), but again we have to be compatible (for a while).
				// Also shows up in NativeGraphics.paintChild()
				Toolkit.eventQueue.postPaintEvent(
				    ((c.flags & IS_ASYNC_UPDATED) != 0 ? PaintEvent.PAINT : PaintEvent.UPDATE),
				    c, uxx, uyy, uww, uhh);
			}
		
			if ( c instanceof Container )
				((Container)c).emitRepaints( uxx, uyy, uww, uhh);
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
				c.propagateParentShowing( false);
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

    // swing books need layout even without children
		if ( (layoutm != null) && ((flags & IS_LAYOUTING) == 0) ) {
			flags |= IS_LAYOUTING;
			layoutm.layoutContainer( this);
			flags &= ~IS_LAYOUTING;
		}
	}
}

public void list(PrintStream out, int indent) {
    list(new PrintWriter(out), indent);
}

public void list(PrintWriter out, int indent) {
    super.list(out, indent);

    Component[] comps = getComponents();
    for (int i = comps.length - 1; i >= 0; --i) {
	comps[i].list(out, indent + 2);
    }

    out.flush();
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
	int uxw = ux + uw;
	int uyh = uy + uh;

	for ( int i=0; i<nChildren; i++ ) {
		Component c = children[i];

		if ( (c.flags & IS_VISIBLE) != 0 ){
			int cxw = c.x + c.width;
			int cyh = c.y + c.height;
			
			if ( (ux < cxw) && (uy < cyh) && (c.x < uxw) && (c.y < uyh) ){
				if ( (c.flags & IS_NATIVE_LIKE) != 0) {
					c.flags |= IS_DIRTY;
				}
		
				if ( c instanceof Container ) {
					int uxx = (ux < c.x ) ? 0 : ux - c.x;
					int uyy = (uy < c.y ) ? 0 : uy - c.y;
					int uww = (uxw < cxw) ? (uxw - c.x - uxx) : c.width;
					int uhh = (uyh < cyh) ? (uyh - c.y - uyy) : c.height;
				
					((Container)c).markRepaints( uxx, uyy, uww, uhh);
				}
			}
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

	validateTree();

	for ( int i=nChildren-1; i>=0; i-- ) {
		Component c = children[i];

		if ( (c.flags & IS_VISIBLE) != 0 ) {
			g.paintChild( c, (flags & IS_IN_UPDATE) != 0);
		}
	}
}

public void paintComponents(Graphics gc) {
	Component[] comps = getComponents();

	for (int i = comps.length; i > 0; --i) {
		comps[i].paintAll(gc);
	}
}

protected String paramString() {
    return super.paramString() + ",layout=" + getLayout().getClass().getName();
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

void process ( ContainerEvent e ) {
	if ( (cntrListener != null) || (eventMask & AWTEvent.CONTAINER_EVENT_MASK) != 0)
		processEvent( e);
}

public void processContainerEvent ( ContainerEvent event ) {
	if ( cntrListener != null ) {
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

void propagateParentShowing ( boolean isTemporary ) {
	if ( (flags & IS_VISIBLE) == 0 ) // nothing to do, we are a visibility leaf
		return;

	if ( (flags & IS_PARENT_SHOWING) != 0 ) {
		for ( int i=0; i<nChildren; i++ ){
			children[i].flags |= IS_PARENT_SHOWING;
			children[i].propagateParentShowing( isTemporary);
		}
	}
	else {
		for ( int i=0; i<nChildren; i++ ){
			children[i].flags &= ~IS_PARENT_SHOWING;
			children[i].propagateParentShowing( isTemporary);
		}
	}
	
	if ( !isTemporary ) {
		// notify resident Graphics objects
		if ( linkedGraphs != null )
			updateLinkedGraphics();
	}
}

void propagateReshape () {
	for ( int i=0; i<nChildren; i++ ){
		children[i].propagateReshape();
	}
	
	// notify resident Graphics objects
	if ( linkedGraphs != null )
		updateLinkedGraphics();
}

void propagateTempEnabled ( boolean isEnabled ) {
	super.propagateTempEnabled ( isEnabled );

	for ( int i=0; i<nChildren; i++ ){
		children[i].propagateTempEnabled( isEnabled);
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

		if ( (cntrListener != null) || (eventMask & AWTEvent.CONTAINER_EVENT_MASK) != 0 ){
			AWTEvent.sendEvent( ContainerEvt.getEvent( this,
			                       ContainerEvent.COMPONENT_REMOVED, c), false);
		}

		if ( (flags & IS_VALID) != 0 )
			invalidate();

		c.flags &= ~IS_PARENT_SHOWING;
		c.propagateParentShowing( false);

		// Like in addImpl, this wouldn't be required in case we are subsequently
		// validated, again. However, native widgets cause a repaint regardless
		// of this validation
		if ( (c.flags & IS_NATIVE_LIKE) != 0 )
			repaint( c.x, c.y, c.width, c.height);
	}
}

public void removeAll () {
	// let's try to do some upfront paint solicitation in case we have
	// a lot of children (note that we have to call remove(idx) since it
	// might be reimplemented in a derived class)
	if ( nChildren > 3 ) {
		int oldFlags = flags;
		flags &= ~IS_VISIBLE;

		for ( int i = nChildren-1; i >= 0; i-- )
			remove(i);

		flags = oldFlags;
		repaint();
	}
	else {	
		for ( int i = nChildren-1; i >= 0; i-- )
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
				c.propagateParentShowing( false);
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
