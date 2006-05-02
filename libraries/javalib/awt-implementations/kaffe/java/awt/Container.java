/**
 * Container - abstract base for all Components capable of having children
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 * Copyright (c) 2006
 *      Kaffe.org developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * original code P.C.Mehlitz
 * some code taken or adapted from Classpath
 */

package java.awt;

import java.awt.event.ContainerEvent;
import java.awt.event.ContainerListener;
import java.awt.event.PaintEvent;
import java.io.PrintStream;
import java.io.PrintWriter;


abstract public class Container
  extends Component
{
 /**
   * Compatible with JDK 1.0+.
   */
  private static final long serialVersionUID = 4613797578919906343L;

  /* Serialized fields from the serialization spec. */
  int ncomponents;
  Component[] component;
  LayoutManager layoutMgr;
  
  /* Anything else is non-serializable, and should be declared "transient". */
  transient ContainerListener containerListener;
  
  // ContainerListener cntrListener;
  Insets insets = Insets.noInsets;

protected Container () {
}

  /**
   * Adds the specified component to this container at the end of the
   * component list.
   *
   * @param comp The component to add to the container.
   *
   * @return The same component that was added.
   */
  public Component add(Component comp)
  {
    addImpl(comp, null, -1);
    return comp;
  }
  
    /**
   * Adds the specified component to the container at the end of the
   * component list.  This method should not be used. Instead, use
   * <code>add(Component, Object)</code>.
   *
   * @param name The name of the component to be added.
   * @param comp The component to be added.
   *
   * @return The same component that was added.
   *
   * @see #add(Component,Object)
   */
  public Component add(String name, Component comp)
  {
    addImpl(comp, name, -1);
    return comp;
  }


  /**
   * Adds the specified component to this container at the end of the
   * component list.  The layout manager will use the specified constraints
   * when laying out this component.
   *
   * @param comp The component to be added to this container.
   * @param constraints The layout constraints for this component.
   */
  public void add(Component comp, Object constraints)
  {
    addImpl(comp, constraints, -1);
  }
  
  /**
   * Adds the specified component to this container at the specified index
   * in the component list.  The layout manager will use the specified
   * constraints when layout out this component.
   *
   * @param comp The component to be added.
   * @param constraints The layout constraints for this component.
   * @param index The index in the component list to insert this child
   * at, or -1 to add at the end of the list.
   *
   * @throws ArrayIndexOutOfBoundsException If the specified index is invalid.
   */
  public void add(Component comp, Object constraints, int index)
  {
    addImpl(comp, constraints, index);
  }

  /**
   * Adds the specified component to this container at the specified index
   * in the component list.
   *
   * @param comp The component to be added.
   * @param index The index in the component list to insert this child
   * at, or -1 to add at the end of the list.
   *
   * @return The same component that was added.
   *
   * @throws ArrayIndexOutOfBoundsException If the specified index is invalid.
   */
  public Component add(Component comp, int index)
  {
    addImpl(comp, null, index);
    return comp;
  }



public void addContainerListener ( ContainerListener newListener ) {
	containerListener = AWTEventMulticaster.add( containerListener, newListener);
}

  /**
   * @since 1.4
   */
  public synchronized ContainerListener[] getContainerListeners()
  {
    return (ContainerListener[])
      AWTEventMulticaster.getListeners(containerListener,
                                       ContainerListener.class);
  }


protected void addImpl(Component child, Object constraints, int index ) {

	synchronized ( treeLock ) {
		if (index < -1 || index > ncomponents) {
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

		if ( component == null ) {
			component= new Component[3];
		}
		else if ( ncomponents == component.length ) {
			Component[] old = component;
			component = new Component[ ncomponents * 2];
			System.arraycopy( old, 0, component, 0, ncomponents);
		}
		
		if (index < 0 || ncomponents == 0 || index == ncomponents) {  // append
			component[ncomponents] = child;
		}
		else if (index < ncomponents) {     // insert at index
			System.arraycopy( component, index, component, index+1, ncomponents - index);
			component[index] = child;
		}
	
		ncomponents++;
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
		if ( layoutMgr != null ) {
			if ( layoutMgr instanceof LayoutManager2 ) {
				((LayoutManager2)layoutMgr).addLayoutComponent( child, constraints);
			}
			if (constraints instanceof String) {
				layoutMgr.addLayoutComponent( (String)constraints, child);
			}
		}
	
		if ( (containerListener != null) || (eventMask & AWTEvent.CONTAINER_EVENT_MASK) != 0 ){
			AWTEvent.sendEvent( ContainerEvt.getEvent( this,
			                       ContainerEvent.COMPONENT_ADDED, child), false);
		}
	}
}

public void addNotify() {
	super.addNotify();
	for ( int i=0; i<ncomponents; i++ ) {
		component[i].addNotify();
	}
}

Graphics clipSiblings ( Component child, NativeGraphics g ) {
	int        i, xClip, yClip, xwClip, yhClip, cxw, cyh;
	Component  c;

	xClip = g.xClip + g.xOffset;
	yClip = g.yClip + g.yOffset;
	xwClip = xClip + g.wClip;
	yhClip = yClip + g.hClip;
	
	for ( i=ncomponents-1; (i >= 0) && (component[i] != child) ; i-- );
	for ( i--; i >= 0; i-- ) {
		c = component[i];

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
	return ncomponents;
}

public void doLayout() {
	layout();
}

void dump ( String prefix ) {
	String prfx = prefix + "  ";

	super.dump( prefix);
	
	for ( int i=0; i<ncomponents; i++ ) {
		component[i].dump( prfx + i + " ");
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

	for ( int i=0; i<ncomponents; i++ ) {
		Component c = component[i];
		
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
	if ( layoutMgr instanceof LayoutManager2 ) {
		return ((LayoutManager2)layoutMgr).getLayoutAlignmentX( this);
	}
	else {
		return super.getAlignmentX();
	}
}

public float getAlignmentY () {
	if ( layoutMgr instanceof LayoutManager2 ) {
		return ((LayoutManager2)layoutMgr).getLayoutAlignmentY( this);
	}
	else {
		return super.getAlignmentY();
	}
}

public Component getComponent ( int index ) {
	return component[index];
}

  /**
   * Returns the component located at the specified point.  This is done
   * by checking whether or not a child component claims to contain this
   * point.  The first child component that does is returned.  If no
   * child component claims the point, the container itself is returned,
   * unless the point does not exist within this container, in which
   * case <code>null</code> is returned.
   *
   * The top-most child component is returned in the case where components overlap.
   * This is determined by finding the component closest to (x,y) and contains 
   * that location. Heavyweight components take precedence of lightweight components.
   * 
   * This function does not ignore invisible components. If there is an invisible
   * component at (x,y), it will be returned.
   * 
   * @param p The point to return the component at.
   * @return The component containing the specified point, or <code>null</code>
   * if there is no such point.
   */
  public Component getComponentAt(Point p)
  {
    return getComponentAt (p.x, p.y);
  }

  /**
   * Locates the visible child component that contains the specified position. 
   * The top-most child component is returned in the case where there is overlap
   * in the components. If the containing child component is a Container,
   * this method will continue searching for the deepest nested child 
   * component. Components which are not visible are ignored during the search.
   * 
   * findComponentAt differs from getComponentAt, because it recursively 
   * searches a Container's children.
   * 
   * @param x - x coordinate
   * @param y - y coordinate
   * @return null if the component does not contain the position. 
   * If there is no child component at the requested point and the point is 
   * within the bounds of the container the container itself is returned.
   * @since 1.2
   */
  public Component findComponentAt(int x, int y)
  {
    synchronized (getTreeLock ())
      {
        if (! contains(x, y))
          return null;

        for (int i = 0; i < ncomponents; ++i)
          {
            // Ignore invisible children...
            if (!component[i].isVisible())
              continue;

            int x2 = x - component[i].x;
            int y2 = y - component[i].y;
            // We don't do the contains() check right away because
            // findComponentAt would redundantly do it first thing.
            if (component[i] instanceof Container)
              {
                Container k = (Container) component[i];
                Component r = k.findComponentAt(x2, y2);
                if (r != null)
                  return r;
              }
            else if (component[i].contains(x2, y2))
              return component[i];
          }

        return this;
      }
  }
  
  /**
   * Locates the visible child component that contains the specified position. 
   * The top-most child component is returned in the case where there is overlap
   * in the components. If the containing child component is a Container,
   * this method will continue searching for the deepest nested child 
   * component. Components which are not visible are ignored during the search.
   * 
   * findComponentAt differs from getComponentAt, because it recursively 
   * searches a Container's children.
   * 
   * @param p - the component's location
   * @return null if the component does not contain the position. 
   * If there is no child component at the requested point and the point is 
   * within the bounds of the container the container itself is returned.
   * @since 1.2
   */
  public Component findComponentAt(Point p)
  {
    return findComponentAt(p.x, p.y);
  }


public Component getComponentAt ( int x, int y ) {
	return locate ( x, y);
}

public int getComponentCount() {
	return countComponents();
}

public Component[] getComponents () {
	Component ca[] = new Component[ncomponents];
	
	if ( ncomponents > 0 )
		System.arraycopy( component, 0, ca, 0, ncomponents);
	
	return ca;
}

public Insets getInsets () {
	return insets();
}

public LayoutManager getLayout () {
	return (layoutMgr);
}

public Dimension getMaximumSize () {
	if (layoutMgr instanceof LayoutManager2) {
		return (((LayoutManager2)layoutMgr).maximumLayoutSize(this));
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
	for ( int i=0; i<ncomponents; i++ ) {
		Component c = component[i];
		
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
		for ( int i=0; i<ncomponents; i++ ) {
			Component c = component[i];
			
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
			if ( layoutMgr instanceof LayoutManager2 ) {
				((LayoutManager2)layoutMgr).invalidateLayout( this);
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
		if ( (layoutMgr != null) && ((flags & IS_LAYOUTING) == 0) ) {
			flags |= IS_LAYOUTING;
			layoutMgr.layoutContainer( this);
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
		
	for ( int i=0; i<ncomponents; i++ ) {
		c = component[i];
		if ( c.contains( x - c.x, y - c.y)) {
			return (c.isShowing() ? c : this);
		}
	}
	
	return this;
}

void markRepaints ( int ux, int uy, int uw, int uh ) {
	int uxw = ux + uw;
	int uyh = uy + uh;

	for ( int i=0; i<ncomponents; i++ ) {
		Component c = component[i];

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
	if ( layoutMgr != null ) {
		return layoutMgr.minimumLayoutSize( this);
	}
	else {
		return super.minimumSize();
	}
}

public void paint ( Graphics g ) {
	// standard JDK behavior is to paint last added childs first, simulating
	// a first-to-last z order

	validateTree();

	for ( int i=ncomponents-1; i>=0; i-- ) {
		Component c = component[i];

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
	if ( layoutMgr != null ) {
		return (layoutMgr.preferredLayoutSize(this));
	}
	else {
		return (super.preferredSize());
	}
}

public void printComponents ( Graphics g ) {
}

void process ( ContainerEvent e ) {
	if ( (containerListener != null) || (eventMask & AWTEvent.CONTAINER_EVENT_MASK) != 0)
		processEvent( e);
}

public void processContainerEvent ( ContainerEvent event ) {
	if ( containerListener != null ) {
		switch ( event.getID() ) {
		case ContainerEvent.COMPONENT_ADDED:
			containerListener.componentAdded( event);
			break;
		case ContainerEvent.COMPONENT_REMOVED:
			containerListener.componentRemoved( event);
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

	for ( int i=0; i<ncomponents; i++ ){
		Component c = component[i];
		if ( (c.flags & IS_BG_COLORED) == 0 ){
			c.propagateBgClr( clr);
		}
	}
}

void propagateFgClr ( Color clr ) {
	fgClr = clr;

	for ( int i=0; i<ncomponents; i++ ){
		Component c = component[i];
		if ( (c.flags & IS_FG_COLORED) == 0 ){
			c.propagateFgClr( clr);
		}
	}
}

void propagateFont ( Font fnt ) {
	font = fnt;

	for ( int i=0; i<ncomponents; i++ ){
		Component c = component[i];
		if ( (c.flags & IS_FONTIFIED) == 0 ){
			c.propagateFont( fnt);
		}
	}
}

void propagateParentShowing ( boolean isTemporary ) {
	if ( (flags & IS_VISIBLE) == 0 ) // nothing to do, we are a visibility leaf
		return;

	if ( (flags & IS_PARENT_SHOWING) != 0 ) {
		for ( int i=0; i<ncomponents; i++ ){
			component[i].flags |= IS_PARENT_SHOWING;
			component[i].propagateParentShowing( isTemporary);
		}
	}
	else {
		for ( int i=0; i<ncomponents; i++ ){
			component[i].flags &= ~IS_PARENT_SHOWING;
			component[i].propagateParentShowing( isTemporary);
		}
	}
	
	if ( !isTemporary ) {
		// notify resident Graphics objects
		if ( linkedGraphs != null )
			updateLinkedGraphics();
	}
}

void propagateReshape () {
	for ( int i=0; i<ncomponents; i++ ){
		component[i].propagateReshape();
	}
	
	// notify resident Graphics objects
	if ( linkedGraphs != null )
		updateLinkedGraphics();
}

void propagateTempEnabled ( boolean isEnabled ) {
	super.propagateTempEnabled ( isEnabled );

	for ( int i=0; i<ncomponents; i++ ){
		component[i].propagateTempEnabled( isEnabled);
	}
}

public void remove ( Component c ) {
	// usually children are added/removed in a stack like fashion
	for ( int i=ncomponents-1; i>=0; i-- ){
		if ( component[i] == c ) {
			remove(i);
			break;
		}
	}
}

public void remove ( int index ) {
	synchronized ( treeLock ) {
		int n = ncomponents - 1;

		if (index < 0 && index > n) {
			return;
		}
		
		Component c = component[index];

		if ( (c.flags & IS_ADD_NOTIFIED) != 0 ){
			c.removeNotify();
		}

		if ( layoutMgr != null ) {
			layoutMgr.removeLayoutComponent( c);
		}

		// Remove from container
		c.parent = null;
		if (index > -1 && index < n) {
			System.arraycopy(component, index+1, component, index, n-index);
		}
		component[n] = null;
		ncomponents--;

		if ( (containerListener != null) || (eventMask & AWTEvent.CONTAINER_EVENT_MASK) != 0 ){
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
	if ( ncomponents > 3 ) {
		int oldFlags = flags;
		flags &= ~IS_VISIBLE;

		for ( int i = ncomponents-1; i >= 0; i-- )
			remove(i);

		flags = oldFlags;
		repaint();
	}
	else {	
		for ( int i = ncomponents-1; i >= 0; i-- )
			remove(i);
	}
}

public void removeContainerListener ( ContainerListener listener ) {
	containerListener = AWTEventMulticaster.remove( containerListener, listener);
}

public void removeNotify() {
	// removeNotify children first (symmetric to addNotify)
	for ( int i=0; i<ncomponents; i++ ) {
		if ( (component[i].flags & IS_ADD_NOTIFIED) != 0 ) {
			component[i].removeNotify();
		}
	}
	super.removeNotify();
}

public void setLayout ( LayoutManager newLayout ) {
	layoutMgr = newLayout;
	
	// this doesn't directly cause a doLayout in JDK, it just enables it
	if ( (flags & IS_VALID) != 0)
		invalidate();
}

public void show () {
	// we have to propagate first to enable subsequent child drawing by super.show()
	if ( (flags & IS_PARENT_SHOWING) != 0){
		for ( int i=0; i<ncomponents; i++ ) {
			Component c = component[i];
			
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
	
	for ( int i=0; i<ncomponents; i++ )
		component[i].validate();
}



  /**
   * Sets the Z ordering for the component <code>comp</code> to
   * <code>index</code>. Components with lower Z order paint above components
   * with higher Z order.
   *
   * @param comp the component for which to change the Z ordering
   * @param index the index to set
   *
   * @throws NullPointerException if <code>comp == null</code>
   * @throws IllegalArgumentException if comp is an ancestor of this container
   * @throws IllegalArgumentException if <code>index</code> is not in
   *         <code>[0, getComponentCount()]</code> for moving between
   *         containers or <code>[0, getComponentCount() - 1]</code> for moving
   *         inside this container
   * @throws IllegalArgumentException if <code>comp == this</code>
   * @throws IllegalArgumentException if <code>comp</code> is a
   *         <code>Window</code>
   *
   * @see #getComponentZOrder(Component)
   *
   * @since 1.5
   */
  public final void setComponentZOrder(Component comp, int index)
  {
    if (comp == null)
      throw new NullPointerException("comp must not be null");
    if (comp instanceof Container && ((Container) comp).isAncestorOf(this))
      throw new IllegalArgumentException("comp must not be an ancestor of "
                                         + "this");
    if (comp instanceof Window)
      throw new IllegalArgumentException("comp must not be a Window");

    if (comp == this)
      throw new IllegalArgumentException("cannot add component to itself");

    // FIXME: Implement reparenting.
    if ( comp.getParent() != this)
      throw new AssertionError("Reparenting is not implemented yet");
    else
      {
        // Find current component index.
        int currentIndex = getComponentZOrder(comp);
        if (currentIndex < index)
          {
            System.arraycopy(component, currentIndex + 1, component,
                             currentIndex, index - currentIndex);
          }
        else
          {
            System.arraycopy(component, index, component, index + 1,
                             currentIndex - index);
          }
        component[index] = comp;
      }
  }

  /**
   * Returns the Z ordering index of <code>comp</code>. If <code>comp</code>
   * is not a child component of this Container, this returns <code>-1</code>.
   *
   * @param comp the component for which to query the Z ordering
   *
   * @return the Z ordering index of <code>comp</code> or <code>-1</code> if
   *         <code>comp</code> is not a child of this Container
   *
   * @see #setComponentZOrder(Component, int)
   *
   * @since 1.5
   */
  public final int getComponentZOrder(Component comp)
  {
    int index = -1;
    if (component != null)
      {
        for (int i = 0; i < component.length; i++)
          {
            if (component[i] == comp)
              {
                index = i;
                break;
              }
          }
      }
    return index;
  }

}
