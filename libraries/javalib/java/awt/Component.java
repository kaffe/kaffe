/**
 * Component - abstract root of all widgets
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */

package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.AdjustmentEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.ContainerEvent;
import java.awt.event.ContainerListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.PaintEvent;
import java.awt.event.TextEvent;
import java.awt.event.WindowEvent;
import java.awt.image.ColorModel;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.awt.peer.ActiveEvent;
import java.awt.peer.ComponentPeer;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.util.EventListener;
import java.util.Locale;
import java.util.Vector;
import kaffe.awt.OpaqueComponent;

abstract public class Component
  extends Object
  implements ImageObserver
{
	Container parent;
	int x;
	int y;
	int width;
	int height;
	Color fgClr;
	Color bgClr;
	Font font;
	Cursor cursor;
	ComponentListener cmpListener;
	KeyListener keyListener;
	FocusListener focusListener;
	MouseListener mouseListener;
	MouseMotionListener motionListener;
	String name;
	Vector popups;
	int eventMask;
	boolean isVisible;
	Dimension dim;
	Rectangle bounds;
	ComponentPeer peer;
	final public static float TOP_ALIGNMENT = (float)0.0;
	final public static float CENTER_ALIGNMENT = (float)0.5;
	final public static float BOTTOM_ALIGNMENT = (float)1.0;
	final public static float LEFT_ALIGNMENT = (float)0.0;
	final public static float RIGHT_ALIGNMENT = (float)1.0;
	final static int BORDER_WIDTH = 2;

protected Component () {
	isVisible = true; // non Windows are visible by default
	cursor = Cursor.defaultCursor;
}

public void add ( PopupMenu menu ) {
	if ( popups == null)
		popups = new Vector( 2);

	popups.addElement( menu);
}

public void addComponentListener ( ComponentListener newListener ) {
	cmpListener = AWTEventMulticaster.add( cmpListener, newListener);
	eventMask |= AWTEvent.COMPONENT_EVENT_MASK;
}

public void addFocusListener ( FocusListener newListener ) {
	focusListener = AWTEventMulticaster.add( focusListener, newListener);
	eventMask |= AWTEvent.FOCUS_EVENT_MASK;
}

public void addKeyListener ( KeyListener newListener ) {
	keyListener = AWTEventMulticaster.add( keyListener, newListener);
	eventMask |= AWTEvent.KEY_EVENT_MASK;
}

public void addMouseListener ( MouseListener newListener ) {
	mouseListener = AWTEventMulticaster.add( mouseListener, newListener);
	eventMask |= AWTEvent.MOUSE_EVENT_MASK;
}

public void addMouseMotionListener ( MouseMotionListener newListener ) {
	motionListener = AWTEventMulticaster.add( motionListener, newListener);
	eventMask |= AWTEvent.MOUSE_MOTION_EVENT_MASK;
}

public void addNotify () {
	peer = createPeer();
}

/**
 * @deprecated, use getBounds()
 */
public Rectangle bounds () {
	return getBounds();
}

public int checkImage ( Image image, ImageObserver obs ) {
	return Image.checkImage( image, -1, -1, obs, false);
}

public int checkImage ( Image image, int width, int height, ImageObserver obs ) {
	return Image.checkImage( image, width, height, obs, false);
}

/**
 * @deprecated
 */
public boolean inside(int x, int y) {
	return (contains(x, y));
}

public boolean contains ( Point pt ) {
	return contains( pt.x, pt.y);
}

public boolean contains ( int x, int y ) {
	if ( !isVisible ) return false;
	if ( x < 0 || y < 0 || x > width || y > height ) return false;

	x += this.x; y += this.y;
	for ( Container c=parent; c!= null; x += c.x, y += c.y, c = c.parent ) {
		if ( !c.isVisible )
			return false;

		if ( (x < 0) || (y < 0) || (x > c.width) || (y > c.height) )
			return false;
	}

	return true;
}

public Image createImage ( ImageProducer producer ) {
	return new Image( producer);
}

public Image createImage ( int width, int height ) {
	return new Image( width, height);
}

ComponentPeer createPeer () {
	return Toolkit.singleton.createLightweight( this);
}

public void disableEvents ( long disableMask ) {
	eventMask &= ~disableMask;  
}

final public void dispatchEvent ( AWTEvent evt ) {
	dispatchEventImpl( evt);
}

void dispatchEventImpl ( AWTEvent event ) {
	processEvent( event);
}

public void doLayout () {
}

/**
 * @deprecated, use setEnabled()
 */
public void enable() {
	setEnabled(true);
}

/**
 * @deprecated, use setEnabled()
 */
public void enable( boolean isEnabled) {
	setEnabled(isEnabled);
}

/**
 * @deprecated, use setEnable()
 */
public void disable() {
	setEnabled(false);
}

public void enableEvents ( long enableMask ) {
	eventMask |= enableMask;  
}

public float getAlignmentX() {
	return CENTER_ALIGNMENT;
}

public float getAlignmentY() {
	return CENTER_ALIGNMENT;
}

public Color getBackground () {
	if ( bgClr != null )
		return bgClr;

	for ( Component c=parent; c != null; c = c.parent ) {
		if ( c.bgClr != null ) return c.bgClr;
	}
	
	return Color.white;
}

public Rectangle getBounds () {
	// many Java programs call this frequently during paint operations, avoid
	// creating new objects all the time

	if ( bounds == null )
		bounds = new Rectangle( x, y, width, height);
	else {
		// reset values in case somebody has changed it
		bounds.x = x;
		bounds.y = y;
		bounds.width = width;
		bounds.height = height;
	}
	
	return bounds;
}

public Component getComponentAt ( Point pt ) {
	return getComponentAt( pt.x, pt.y );
}

public Component getComponentAt ( int x, int y ) {
	// don't do a direct comparison here since transparent Components resolve contains()
	return (contains( x, y) ? this : null);
}

public Cursor getCursor() {
	return cursor;
}

public Font getFont () {
	if ( font != null )
		return font;

	for ( Component c=parent; c != null; c = c.parent ) {
		if ( c.font != null ) return c.font;
	}
	
	return null;
}

public FontMetrics getFontMetrics ( Font font ) {
	return FontMetrics.getFontMetrics( font);
}

public Color getForeground () {
	if ( fgClr != null )
		return fgClr;

	for ( Component c=parent; c != null; c = c.parent ) {
		if ( c.fgClr != null ) return c.fgClr;
	}
	
	return Color.black;
}

public Graphics getGraphics () {
	return Graphics.getClippedGraphics( this, 0, 0, width, height, false);
}

public Locale getLocale () {
	return null;
}

public Point getLocation () {
	return new Point( x, y);
}

public Point getLocationOnScreen () {
	// this has to be resolved for Applets

	int u=0, v=0;
	
	for ( Component c=this; c != null; c=c.parent ) {
		u += c.x;
		v += c.y;
	}
	
	return new Point( u, v);
}

public Dimension getMaximumSize() {
	return getPreferredSize();
}

public Dimension getMinimumSize() {
	return getPreferredSize();
}

public String getName () {
	return (name == null) ? getClass().getName() : name;
}

public Container getParent() {
	return parent;
}

public ComponentPeer getPeer() {
	return peer;
}

public Dimension getPreferredSize() {
	return new Dimension( 50, 50);
}

public Dimension getSize () {
	// many Java programs call this frequently during paint operations, avoid
	// creating new objects all the time

	if ( dim == null )
		dim = new Dimension( width, height);
	else {
		// reset values in case somebody has changed it
		dim.width = width;
		dim.height = height;
	}
	
	return dim;
}

public Toolkit getToolkit () {
	return Toolkit.singleton;
}

Component getToplevel () {
	Component c;
	for ( c=this; !(c instanceof Window) && c != null; c= c.parent );
	return c;
}

boolean hasToNotify ( int mask, EventListener listener ) {
	return ((eventMask & mask) != 0) && (listener != null);
}

/**
 * @deprecated, use setVisible( false)
 *
 * this is never called automatically, override setVisible in derived classes
 * to change the default behavior
 */
public void hide () {
	setVisible( false);
}

public boolean imageUpdate ( Image img, int infoflags, int x, int y, int width, int height ) {
	if ( ( infoflags & ALLBITS) > 0 )
		repaint();
	return (( infoflags & ( ALLBITS | ABORT)) == 0);
}

boolean intersects ( Component c ) {
	return intersects( c.x, c.y, c.width, c.height);
}

boolean intersects ( Rectangle r ) {
	return intersects( r.x, r.y, r.width, r.height);
}

boolean intersects ( int u, int v, int w, int h ) {
	if ( (x > (u + w))  ||
	     (y > (v + h)) ||
	     (u > (x + width))    ||
	     (v > (y + height)) )
		return false;
	else
		return true;
}

public void invalidate () {
}

public boolean isEnabled () {
	return (eventMask & AWTEvent.DISABLED_MASK) == 0;
}

public boolean isFocusTraversable() {
	return isShowing() && isEnabled();
}

public boolean isShowing () {
	for ( Component c=this; c!= null; c = c.parent ) {
		if ( !c.isVisible ) return false;
	}
	
	return (parent != null);
}

public boolean isValid () {
	return true;
}

public boolean isVisible () {
	return isVisible;
}

/**
 * @deprecated, use setLocation(int, int)
 */
public void move(int x, int y) {
	setLocation(x, y);
}

public void paint ( Graphics g ) {
}

public void paintAll ( Graphics g ) {
	paint( g);
}

void paintBorder () {
	Graphics g = getGraphics();
	if ( g != null ) {
		paintBorder( g);
		g.dispose();
	}
}

void paintBorder ( Graphics g ) {
	paintBorder( g, 0, 0, 0, 0);
}

void paintBorder ( Graphics g, int left, int top, int right, int bottom ) {
	int w = width - (left + right);
	int h = height - (top + bottom);

	if ( this == AWTEvent.keyTgt )
		g.setColor( Defaults.FocusClr);
	else
		g.setColor( Defaults.BorderClr);

	g.draw3DRect( left, top,  w-1, h-1, true);
	g.draw3DRect( left+1, top+1, w-3, h-3, false);
}

protected String paramString () {
	String s = "" + name + ',' + ' ' + x + ',' + y + ',' + width + ',' + height;
	
	if ( !isVisible() ) s += ',' + " hidden";
	if ( !isEnabled() ) s += ',' + " disabled";
	
	return s;
}

/**
 * @deprecated - use getPreferredSize()
 */
public Dimension preferredSize () {
	return getPreferredSize();
}

public boolean prepareImage ( Image image, ImageObserver obs ){
	return (Image.checkImage( image, -1, -1, obs, true) > 0);
}

public boolean prepareImage ( Image image, int width, int height, ImageObserver obs ){
	return (Image.checkImage( image, width, height, obs, true) > 0);
}

public void print ( Graphics g ) {
}

public void printAll ( Graphics g ) {
}

protected void processActionEvent ( ActionEvent e ) {
}

protected void processAdjustmentEvent ( AdjustmentEvent e ) {
}

protected void processComponentEvent ( ComponentEvent event ) {

	if ( hasToNotify( AWTEvent.COMPONENT_EVENT_MASK, cmpListener) ){
		switch ( event.getID() ) {
		case ComponentEvent.COMPONENT_RESIZED:
			cmpListener.componentResized( event);
			break;
		case ComponentEvent.COMPONENT_MOVED:
			cmpListener.componentMoved( event);
			break;
		case ComponentEvent.COMPONENT_SHOWN:
			cmpListener.componentShown( event);
			break;
		case ComponentEvent.COMPONENT_HIDDEN:
			cmpListener.componentHidden( event);
			break;
		}
	}
}

protected void processContainerEvent ( ContainerEvent e ) {
}

protected void processEvent ( AWTEvent e ) {
  // Wasn't there something called "object oriented programming"?
  // Would be nice if we could do a simple "e.dispatch()", but that doesn't work
  // since the JDK allows processing events with "source != this"
	
	if ( e instanceof MouseEvent ) {
		switch ( e.getID() ){
		case MouseEvent.MOUSE_MOVED:
		case MouseEvent.MOUSE_DRAGGED:
			processMouseMotionEvent( (MouseEvent)e); break;
		default:
		  processMouseEvent( (MouseEvent)e);
		}
	}
	else if ( e instanceof KeyEvent )
		processKeyEvent( (KeyEvent)e);
	else if ( e instanceof PaintEvent )
    processPaintEvent( (PaintEvent) e);  		
	else if ( e instanceof FocusEvent )
		processFocusEvent( (FocusEvent)e);
	else if ( e instanceof ComponentEvent )
		processComponentEvent( (ComponentEvent)e);
	else if ( e instanceof ContainerEvent )
		processContainerEvent( (ContainerEvent)e);
	else if ( e instanceof WindowEvent )
		processWindowEvent( (WindowEvent)e);
	else if ( e instanceof TextEvent )
		processTextEvent( (TextEvent)e);
	else if ( e instanceof ItemEvent )
		processItemEvent( (ItemEvent)e);
	else if ( e instanceof ActionEvent )
		processActionEvent( (ActionEvent)e);
	else if ( e instanceof AdjustmentEvent )
		processAdjustmentEvent( (AdjustmentEvent)e);
}

protected void processFocusEvent ( FocusEvent event ) {
	if ( hasToNotify( AWTEvent.FOCUS_EVENT_MASK, focusListener) ) {
		switch ( event.getID() ) {
		case FocusEvent.FOCUS_GAINED:
			focusListener.focusGained( event);
			break;
		case FocusEvent.FOCUS_LOST:
			focusListener.focusLost( event);
			break;
		}
	}
}

protected void processItemEvent ( ItemEvent e ) {
}

protected void processKeyEvent ( KeyEvent event ) {
	if ( hasToNotify( AWTEvent.KEY_EVENT_MASK, keyListener) ) {
		switch ( event.getID() ) {
		case KeyEvent.KEY_TYPED:
			keyListener.keyTyped( event);
			break;
		case KeyEvent.KEY_PRESSED:
			keyListener.keyPressed( event);
			break;
		case KeyEvent.KEY_RELEASED:
			keyListener.keyReleased( event);
			break;
		}
	}
}

protected void processMouseEvent ( MouseEvent event ) {
	
	if ( hasToNotify( AWTEvent.MOUSE_EVENT_MASK, mouseListener) ) {
		switch ( event.getID() ) {
			case MouseEvent.MOUSE_PRESSED:
				mouseListener.mousePressed( event);
				break;
			case MouseEvent.MOUSE_RELEASED:
				mouseListener.mouseReleased( event);
				break;
			case MouseEvent.MOUSE_CLICKED:
				mouseListener.mouseClicked( event);
				break;
			case MouseEvent.MOUSE_EXITED:
				mouseListener.mouseExited( event);
				break;
			case MouseEvent.MOUSE_ENTERED:
				mouseListener.mouseEntered( event);
				break;
		}
	}
}

protected void processMouseMotionEvent ( MouseEvent event ) {
	if ( hasToNotify( AWTEvent.MOUSE_MOTION_EVENT_MASK, motionListener) ) {
		switch ( event.getID() ) {
		case MouseEvent.MOUSE_MOVED:
			motionListener.mouseMoved( event);
			return;
		case MouseEvent.MOUSE_DRAGGED:
			motionListener.mouseDragged( event);
			return;
		}
	}
}

void processPaintEvent ( PaintEvent e ) {
	Rectangle r = e.getUpdateRect();
	Graphics g = Graphics.getClippedGraphics( this, r.x, r.y, r.width, r.height,
	                                          (parent == null));	
	if ( g != null ) {
		paint( g);
		g.dispose();
	}
}

protected void processTextEvent ( TextEvent e ) {
}

protected void processWindowEvent ( WindowEvent e ) {
}

public void removeComponentListener ( ComponentListener client ) {
	cmpListener = AWTEventMulticaster.remove( cmpListener, client);
}

public void removeFocusListener ( FocusListener listener ) {
	focusListener = AWTEventMulticaster.remove( focusListener, listener);
}

public void removeKeyListener ( KeyListener listener ) {
	keyListener = AWTEventMulticaster.remove( keyListener, listener);
}

public void removeMouseListener ( MouseListener listener ) {
	mouseListener = AWTEventMulticaster.remove( mouseListener, listener);
}

public void removeMouseMotionListener ( MouseMotionListener listener ) {
	motionListener = AWTEventMulticaster.remove( motionListener, listener);
}

public void removeNotify () {
	HotKeyHandler.removeHotKeys( this);
	peer = null;
}

public void repaint () {
	repaint( 0, 0, width, height);
}

public void repaint ( int x, int y, int width, int height ) {
	Graphics g = Graphics.getClippedGraphics( this, x, y, width, height, false);	
	if ( g != null ) {
		update( g);
		g.dispose();
	}
}

public void repaint ( long ms ) {
	Toolkit.eventQueue.postEvent( new RepaintEvent( this, ms, x, y, width, height));
}

public void repaint ( long ms, int x, int y, int width, int height ) {
	Toolkit.eventQueue.postEvent( new RepaintEvent( this, ms, x, y, width, height));
}

public void requestFocus () {
	Component topNew, topOld = null;

	if ( AWTEvent.keyTgt == this ) return;  // nothing to do
	topNew = getToplevel();
	
	// there are bad apps out there requesting the focus for Components
	// which have not even been addNotified yet (hence no parent)
	if ( topNew == null ) {
		// most native AWTs will fail here, but with our mechanism, we
		// can try harder: store request in the hope it will be honored
		// by a subsequent requestFocus of the toplevel
		AWTEvent.keyTgtRequest = this;
	}
	else {	
		if ( AWTEvent.keyTgt != null )
			topOld = AWTEvent.keyTgt.getToplevel();

		if ( topNew != topOld ) {  // this involves a change of active toplevels
			AWTEvent.keyTgtRequest = this;
			topNew.requestFocus();
		}
		else {                                 // intra toplevel focus change
			AWTEvent.sendFocusEvent( this, true, false);
		}
	}
}

/**
 * @deprecated, use setBounds(x,y,w,h)
 * this is never called automatically, override setBounds in derived classes
 * to change the default behavior
 */
public void reshape ( int xNew, int yNew, int wNew, int hNew ) {
	setBounds( xNew, yNew, wNew, hNew );
}

/**
 * @deprecated, use setSize( d)
 * this is never called automatically, override setSize in derived classes
 * to change the default behavior
 */
public void resize ( Dimension d ) {
	setSize( d);
}

/**
 * @deprecated, use setSize(w,h)
 * this is never called automatically, override setSize in derived classes
 * to change the default behavior
 */
public void resize ( int wNew, int hNew ) {
	setSize( wNew, hNew);
}

public void setBackground ( Color clr ) {
	bgClr = clr;
	
	if ( isShowing() )
		repaint();
}

public void setBounds ( Rectangle r ) {
	setBounds( r.x, r.y, r.width, r.height);
}

public void setBounds ( int xNew, int yNew, int wNew, int hNew ) {
	int      x0=0, x1=0, y0=0, y1=0, a, b;
	boolean  isShowing = isShowing();
	boolean  hasParent = (parent != null);
	boolean  sized = ( (width != wNew) || (height != hNew) );
	boolean  moved = ( !sized && ((x != xNew) || (y != yNew)) );

	if ( hasParent ) {
		if ( parent.isLayouting )
			isShowing = false;           // parent will paint after completing layout

		if ( isShowing ) {             // get hull (to draw parent background in one sweep)
			x0 = (xNew < x) ? xNew : x;
			y0 = (yNew < y) ? yNew : y;
			x1 = (a = xNew + wNew) > (b = x + width)  ? a : b;
			y1 = (a = yNew + hNew) > (b = y + height) ? a : b;
		}
	}

	x = xNew; y = yNew; width = wNew; height = hNew;
	dim = null;	// Clear the cached value

	doLayout();  // in case we are a Container

	if ( isShowing ) {
		if ( hasParent /*&& !(this instanceof OpaqueComponent)*/ )
			parent.repaint( x0, y0, (x1-x0), (y1-y0));
		else
			repaint();
	}

	if ( sized || moved ) {
		if (((eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0) && (cmpListener !=null)){
			int id = sized ? ComponentEvent.COMPONENT_RESIZED : ComponentEvent.COMPONENT_MOVED;
			Toolkit.eventQueue.postEvent( AWTEvent.getComponentEvent( this, id) );
		}
	}
}

public void setCursor ( Cursor newCursor ) {
	Component c;

	cursor = newCursor;
	
	// go native, but don't change toplevels cursor field
	for ( c = this; c.parent != null; c = c.parent );
	c.setNativeCursor( cursor);
}

public void setEnabled ( boolean isEnabled ) {
	if ( isEnabled)
		eventMask &= ~AWTEvent.DISABLED_MASK;
	else
		eventMask |= AWTEvent.DISABLED_MASK;
}

public void setFont ( Font newFont ) {
	font = newFont;
	
	if ( isShowing() )
		repaint();
}

public void setForeground ( Color clr ) {
	fgClr = clr;
	
	if ( isShowing() )
		repaint();
}

public void setLocation ( Point pt ) {
	setBounds( pt.x, pt.y, width, height);
}

public void setLocation ( int x, int y ) {
	setBounds( x, y, width, height);
}

public void setName ( String newName ) {
	name = newName;
}

void setNativeCursor ( Cursor cursor ) {
}

public void setSize ( Dimension dim ) {
	setBounds( x, y, dim.width, dim.height);
}

public void setSize ( int newWidth, int newHeight ) {
	setBounds( x, y, newWidth, newHeight);
}

public void setVisible ( boolean showIt ) {
	if ( showIt != isVisible ) {
		isVisible = showIt;

		if ( parent != null ) {
			if ( parent.isShowing() && !parent.isLayouting ){
				parent.repaint( x, y, width, height);
			}
		}
		// if we are a toplevel, the native window manager will take care
		// of repainting
		
		if (((eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0) && (cmpListener !=null)){
			int id = showIt ? ComponentEvent.COMPONENT_SHOWN : ComponentEvent.COMPONENT_HIDDEN;
			Toolkit.eventQueue.postEvent( new ComponentEvent( this, id));
		}		
	}
}

/**
 * @deprecated, use setVisible( true)
 * this is never called automatically, override setVisible in derived classes
 * to change the default behavior
 */
public void show () {
	setVisible( true);
}

/**
 * @deprecated, use getSize()
 * this is never called automatically, override getSize in derived classes
 * to change the default behavior
 */
public Dimension size () {
	return getSize();
}

public String toString () {
	return (getClass().getName() + '[' + paramString() + ']');
}

PopupMenu triggerPopup ( int idx, int x, int y ) {
	if ( popups != null && (popups.size() > idx) ){
		PopupMenu p = (PopupMenu)popups.elementAt( idx);
		p.show( this, x, y);
		return p;
	}
	
	return null;
}

public void update ( Graphics g ) {
	paint( g);
}

public void validate () {
}

final public Object getTreeLock() {
        return Toolkit.singleton;
}

}
