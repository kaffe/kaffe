package java.awt;

import java.lang.String;
import java.awt.Event;
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
import java.util.Hashtable;
import java.util.Locale;
import java.util.Vector;
import kaffe.awt.OpaqueComponent;

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
	boolean isValid = false;
	ComponentPeer peer;
	Locale locale;
	Rectangle deco = noDeco;
	ClassProperties props;
	protected boolean oldEvents;
	final public static float TOP_ALIGNMENT = (float)0.0;
	final public static float CENTER_ALIGNMENT = (float)0.5;
	final public static float BOTTOM_ALIGNMENT = (float)1.0;
	final public static float LEFT_ALIGNMENT = (float)0.0;
	final public static float RIGHT_ALIGNMENT = (float)1.0;
	final static int BORDER_WIDTH = 2;
	static Rectangle noDeco = new Rectangle();

protected Component () {
	isVisible = true; // non Windows are visible by default
	cursor = Cursor.defaultCursor;

	props = getClassProperties();
}

/**
 * @deprecated
 */
public boolean action(Event evt, Object what) {
	return (false);
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
	if (peer == null) {
		peer = createPeer();
	
		if ( parent != null ) {
			// Note that this only works in case the parent is addNotified
			// *before* its childs. The 'isNativeLike' test is just a
			// proxy for the fact that the old 1.0.2 AWT did not allow
			// lighweights (because of a missing protected ctor in Component/Container)
			if ( props.isNativeLike && (parent.oldEvents || props.useOldEvents) )
				oldEvents = true;
		}
		else {
			oldEvents = props.useOldEvents;
		}
	}
}

public Rectangle bounds () {
	// DUP - we have to return fresh objects because (1) there are apps out there
	// modifying the return values (causing trouble for concurrent access to bounds),
	// and (2) because some apps (like Swing) temporarily store return values, relying
	// on its constness (e.g. for InternalFrame dragging)
	return new Rectangle( x, y, width, height);
}

public int checkImage (Image image, ImageObserver obs) {
	return (checkImage(image, -1, -1, obs));
}

public int checkImage (Image image, int width, int height, ImageObserver obs) {
	return (image.checkImage(width, height, obs));
}

public boolean contains ( Point pt ) {
	return contains( pt.x, pt.y);
}

public boolean contains(int x, int y) {
	return (inside(x, y));
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

/**
 * @deprecated
 */
final public void deliverEvent(Event evt) {
	postEvent(evt);
}

/**
 * @deprecated, use setEnable()
 */
public void disable() {
	setEnabled(false);
}

public void disableEvents ( long disableMask ) {
	eventMask &= ~disableMask;  
}

final public void dispatchEvent ( AWTEvent evt ) {
	// this is NOT our main entry point for Component event processing
	// (processEvent() is). Because this is a 'final' method, it can't be overloaded
	// by user classes. Well, almost, because the JDK obviously calls a
	// hidden dispatchEventImpl() from it, turning this into a "somewhat" final
	// method. Anyway, this (still?) can be considered as undocumented,
	// non-portable, and we ignore it for now (regarding the main entry point)
	dispatchEventImpl( evt);
}

void dispatchEventImpl ( AWTEvent event ) {
	// A hidden method that seems to be called automatically by the JDKs
	// 'final' dispatchEvent() method. We just provide it to get some more
	// compatibility (in case dispatchEvent is called explicitly), but
	// we don't route all events through it (since this is a private,
	// undocumented method)
	event.dispatch();
}

public void doLayout () {
	layout();
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

/**
 * @deprecated, use getBounds()
 */
public Rectangle getBounds () {
	return bounds();
}

ClassProperties getClassProperties () {
	// direct Component / Container derived classes can't use old events
	// (they had no protected ctor in 1.0.2)
	return ClassAnalyzer.analyzeProcessEvent( getClass(), true);
}

public Component getComponentAt ( Point pt ) {
	return getComponentAt( pt.x, pt.y );
}

public Component getComponentAt ( int x, int y ) {
	return locate( x, y);
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
	if ( peer != null )
		return NativeGraphics.getClippedGraphics( null, this, 0, 0, 0, 0, width, height, false);
	else
		return null;
}

public Locale getLocale () {
	if (locale != null) {
		return (locale);
	}
	else if (parent != null) {
		return (parent.getLocale());
	}
	else {
		return (Locale.getDefault());
	}
}

public Point getLocation () {
	return location();
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
	return Toolkit.singleton.getScreenSize();
}

public Dimension getMinimumSize() {
	return minimumSize();
}

public String getName () {
	return (name == null) ? getClass().getName() : name;
}

public Container getParent() {
	return parent;
}

/**
 * @deprecated, should not be called.
 */
public ComponentPeer getPeer() {
	return peer;
}

public Dimension getPreferredSize() {
	return (preferredSize());
}

/**
 * @deprecated, use getSize()
 * this is never called automatically, override getSize in derived classes
 * to change the default behavior
 */
public Dimension getSize () {
	return size();
}

public Toolkit getToolkit () {
	return Toolkit.singleton;
}

Component getToplevel () {
	Component c;
	for ( c=this; !(c instanceof Window) && c != null; c= c.parent );
	return c;
}

final public Object getTreeLock() {
	// this way, we sync with all graphic output, too
	// (which is important for no-native WMs)
	return Toolkit.class;
}

/**
 * @deprecated
 */
public boolean gotFocus(Event evt, Object what) {
	return (false);
}

/**
 * @deprecated
 */
public boolean handleEvent(Event evt) {
	switch (evt.id) {
	case Event.ACTION_EVENT:
		return (action(evt, evt.arg));
	case Event.GOT_FOCUS:
		return (gotFocus(evt, evt.arg));
	case Event.KEY_PRESS:
		return (keyDown(evt, evt.key));
	case Event.KEY_RELEASE:
		return (keyUp(evt, evt.key));
	case Event.LOST_FOCUS:
		return (lostFocus(evt, evt.arg));
	case Event.MOUSE_DOWN:
		return (mouseDown(evt, evt.x, evt.y));
	case Event.MOUSE_DRAG:
		return (mouseDrag(evt, evt.x, evt.y));
	case Event.MOUSE_ENTER:
		return (mouseEnter(evt, evt.x, evt.y));
	case Event.MOUSE_EXIT:
		return (mouseExit(evt, evt.x, evt.y));
	case Event.MOUSE_MOVE:
		return (mouseMove(evt, evt.x, evt.y));
	case Event.MOUSE_UP:
		return (mouseUp(evt, evt.x, evt.y));
	default:
		return (false);
	}
}

static boolean hasToNotify ( Component c, int mask, EventListener listener ) {
	// This is a helper which saves some typing, but slows down event processing
	// (since this is still much faster than the subsequent invokeinterface, we
	// accept that). Should be inlined for high-frequency events
	return ((listener != null) && ((c.eventMask & mask) != 0));
}

public void hide () {
	// DEP this should be in setVisible !! But we have to keep it here
	// for compatibility reasons (Swing etc.)

	if ( isVisible ) {
		isVisible = false;

		if ( parent != null ) {
			if ( parent.isShowing() && !parent.isLayouting ){
				parent.repaint( x, y, width, height);
			}
		}
		// if we are a toplevel, the native window manager will take care
		// of repainting
		
		if (((eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0) && (cmpListener !=null)){
		  // sync because it might be used to control Graphics visibility
		  // CAUTION: this might cause incompatibilities in ill-behaving apps
		  // (doing explicit sync on show/hide/reshape), but otherwise we would have
		  // to implement a completely redundant mechanism for Graphics updates
			AWTEvent.sendEvent(  ComponentEvt.getEvent( this,
			                            ComponentEvent.COMPONENT_HIDDEN), true);
		}
	}
}

public boolean imageUpdate (Image img, int infoflags, int x, int y, int width, int height ) {
	if ((infoflags & ALLBITS) != 0) {
		repaint();
	}
	/* We return false if we're no longer interested in updates.
	 * This is *NOT* what is says in the Addison-Wesley documentation,
	 * but is what is says in the JDK javadoc documentation.
	 */
	if ((infoflags & (ALLBITS|ABORT|ERROR)) != 0) {
		return (false);
	}
	else {
		return (true);
	}
}

/**
 * @deprecated
 */
public boolean inside ( int x, int y ) {
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
	// invalidation means invalid yourself *and* all your parents (if they
	// arent't already)

	if ( isValid ) {
		isValid = false;
		
		// maybe, it's overloaded (we have to sacrifice speed fr compat, here)
		// parent can't be null, because we can't get valid without being addNotifyed
		if ( parent.isValid )
			parent.invalidate();
	}
}

/**
 * PersonalJava 1.1 method
 */
public boolean isDoubleBuffered() {
	return (false);
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
	
	return (peer != null);
}

public boolean isValid () {
	return isValid;
}

public boolean isVisible () {
	return isVisible;
}

/**
 * @deprecated
 */
public boolean keyDown(Event evt, int key) {
	return (false);
}

/**
 * @deprecated
 */
public boolean keyUp(Event evt, int key) {
	return (false);
}

/**
 * @deprecated, use doLayout()
 */
public void layout() {
}

/**
 * @deprecated, use getComponentAt(int, int)
 */
public Component locate(int x, int y) {
	// don't do a direct comparison here since transparent Components resolve contains()
	return (contains( x, y) ? this : null);
}

/**
 * @deprecated, use getLocation()
 */
public Point location() {
	return new Point( x, y);
}

/**
 * @deprecated
 */
public boolean lostFocus(Event evt, Object what) {
	return (false);
}

/**
 * @deprecated, use getMinimumSize()
 */
public Dimension minimumSize() {
	return new Dimension( width, height);
}

/**
 * @deprecated
 */
public boolean mouseDown(Event evt, int x, int y) {
	return (false);
}

/**
 * @deprecated
 */
public boolean mouseDrag(Event evt, int x, int y) {
	return (false);
}

/**
 * @deprecated
 */
public boolean mouseEnter(Event evt, int x, int y) {
	return (false);
}

/**
 * @deprecated
 */
public boolean mouseExit(Event evt, int x, int y) {
	return (false);
}

/**
 * @deprecated
 */
public boolean mouseMove(Event evt, int x, int y) {
	return (false);
}

/**
 * @deprecated
 */
public boolean mouseUp(Event evt, int x, int y) {
	return (false);
}

/**
 * @deprecated, use setLocation(int, int)
 */
public void move(int x, int y) {
	setBounds(x, y, width, height);
}

/**
 * @deprecated, use transferFocus()
 */
public void nextFocus() {
	transferFocus();
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

void paintChild ( Graphics g ) {
	paint( g);
}

protected String paramString () {
	String s = "" + name + ',' + ' ' + x + ',' + y + ',' + width + ',' + height;
	
	if ( !isVisible() ) s += ',' + " hidden";
	if ( !isEnabled() ) s += ',' + " disabled";
	
	return s;
}

/**
 * @deprecated
 */
public boolean postEvent ( Event evt ) {
	if ( evt != null ) {
		// travel all the way up in the parent chain until we find someone who handles it
		for ( Component c = this; c != null; c = c.parent ) {
			if ( c.handleEvent( evt) ) {
				evt.recycle();
				return (true);
			}
			
			evt.x += c.x;
			evt.y += c.y;
		}
		
		evt.recycle();
	}

	return (false);
}

/**
 * @deprecated - use getPreferredSize()
 */
public Dimension preferredSize () {
	// DEP - this should go into getPreferredSize (just here because of JDK compat)
	// Huhh, a deprecated method calling a non-deprecated one?? But that's the
	// way JDK obviously does it (just directly calling getMinimumSize)
	return getMinimumSize();
}

public boolean prepareImage ( Image image, ImageObserver obs ){
	return (prepareImage (image, -1, -1, obs));
}

public boolean prepareImage ( Image image, int width, int height, ImageObserver obs ) {
	return (image.loadImageAsync(width, height, obs));
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

	if ( hasToNotify( this, AWTEvent.COMPONENT_EVENT_MASK, cmpListener) ){
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
	// Would be nice if we could do a simple "e.dispatch()" (to rely on
	// method overloading to do the branching), but we can't because this
	// is a protected method, which constantly gets overloaded for modification
	// of standard event processing (which is mostly silly because it can be done
	// with listeners)

	switch ( e.id ){
	case MouseEvent.MOUSE_CLICKED:	//500..506
	case MouseEvent.MOUSE_PRESSED:
	case MouseEvent.MOUSE_RELEASED:
	case MouseEvent.MOUSE_ENTERED:
	case MouseEvent.MOUSE_EXITED:
		processMouseEvent( (MouseEvent)e);
		break;
		
	case MouseEvent.MOUSE_MOVED:
	case MouseEvent.MOUSE_DRAGGED:
		processMouseMotionEvent( (MouseEvent)e);
		break;
		
	case KeyEvent.KEY_TYPED:			//400..402
	case KeyEvent.KEY_PRESSED:
	case KeyEvent.KEY_RELEASED:
		processKeyEvent( (KeyEvent)e);
		break;
			
	case FocusEvent.FOCUS_GAINED:	//1004..1005
	case FocusEvent.FOCUS_LOST:
		processFocusEvent( (FocusEvent)e);
		break;
		
	case ComponentEvent.COMPONENT_MOVED:	//100..103
	case ComponentEvent.COMPONENT_RESIZED:
	case ComponentEvent.COMPONENT_SHOWN:
	case ComponentEvent.COMPONENT_HIDDEN:
		processComponentEvent( (ComponentEvent)e);
		break;
		
	case ContainerEvent.COMPONENT_ADDED:	//300..301
	case ContainerEvent.COMPONENT_REMOVED:
		processContainerEvent( (ContainerEvent)e);
		break;
		
	case WindowEvent.WINDOW_OPENED:			//200..206
	case WindowEvent.WINDOW_CLOSING:
	case WindowEvent.WINDOW_CLOSED:
	case WindowEvent.WINDOW_ICONIFIED:
	case WindowEvent.WINDOW_DEICONIFIED:
	case WindowEvent.WINDOW_ACTIVATED:
	case WindowEvent.WINDOW_DEACTIVATED:
		processWindowEvent( (WindowEvent)e);
		break;
			
	case TextEvent.TEXT_VALUE_CHANGED:	//900
		processTextEvent( (TextEvent)e);
		break;
			
	case ItemEvent.ITEM_STATE_CHANGED:	//701
		processItemEvent( (ItemEvent)e);
		break;
			
	case ActionEvent.ACTION_PERFORMED:	//1001
		processActionEvent( (ActionEvent)e);
		break;
			
	case AdjustmentEvent.ADJUSTMENT_VALUE_CHANGED:	//601
		processAdjustmentEvent( (AdjustmentEvent)e);
		break;
	}
}

protected void processFocusEvent ( FocusEvent event ) {
	if ( hasToNotify( this, AWTEvent.FOCUS_EVENT_MASK, focusListener) ) {
		switch ( event.getID() ) {
		case FocusEvent.FOCUS_GAINED:
			focusListener.focusGained( event);
			break;
		case FocusEvent.FOCUS_LOST:
			focusListener.focusLost( event);
			break;
		}
	}
	
	if ( oldEvents ) postEvent( Event.getEvent( event));
}

protected void processItemEvent ( ItemEvent e ) {
}

protected void processKeyEvent ( KeyEvent event ) {
	if ( hasToNotify( this, AWTEvent.KEY_EVENT_MASK, keyListener) ) {
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

	if ( oldEvents ) postEvent( Event.getEvent( event));
}

protected void processMouseEvent ( MouseEvent event ) {
	if ( hasToNotify( this, AWTEvent.MOUSE_EVENT_MASK, mouseListener) ) {
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

	if ( oldEvents ) postEvent( Event.getEvent( event));
}

protected void processMouseMotionEvent ( MouseEvent event ) {
	if ( hasToNotify( this, AWTEvent.MOUSE_MOTION_EVENT_MASK, motionListener) ) {
		switch ( event.id ) {
		case MouseEvent.MOUSE_MOVED:
			motionListener.mouseMoved( event);
			return;
		case MouseEvent.MOUSE_DRAGGED:
			motionListener.mouseDragged( event);
			return;
		}
	}
	
	if ( oldEvents ) postEvent( Event.getEvent( event));
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
	repaint( 0, 0, 0, width, height);
}

public void repaint ( int x, int y, int width, int height ) {
	repaint( 0, x, y, width, height);
}

public void repaint ( long ms ) {
	repaint( ms, 0, 0, width, height);
}

public void repaint ( long ms, int x, int y, int width, int height ) {
	Toolkit.eventQueue.repaint( this, x, y, width, height);
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
		FocusEvt.keyTgtRequest = this;
	}
	else {	
		if ( AWTEvent.keyTgt != null )
			topOld = AWTEvent.keyTgt.getToplevel();

		if ( topNew != topOld ) {  // this involves a change of active toplevels
			FocusEvt.keyTgtRequest = this;
			topNew.requestFocus();
		}
		else {                                 // intra toplevel focus change
			AWTEvent.sendEvent( FocusEvt.getEvent( this, FocusEvent.FOCUS_GAINED), false);
		}
	}
}

/**
 * @deprecated, use setBounds(x,y,w,h)
 * this is never called automatically, override setBounds in derived classes
 * to change the default behavior
 */
public void reshape ( int xNew, int yNew, int wNew, int hNew ) {
	// DEP - this should be in setBounds !! But we have to keep it here
	// for compatibility reasons (Swing etc.)

	int      x0=0, x1=0, y0=0, y1=0, a, b;
	boolean  isShowing;
	boolean  hasParent = (parent != null);
	boolean  sized = ( (width != wNew) || (height != hNew) );
	boolean  moved = ( !sized && ((x != xNew) || (y != yNew)) );

	// Don't do anything if we don't change anything.
	if (sized || moved) {
		isShowing = isShowing();
		if ( hasParent ) {
			if ( parent.isLayouting ) {
				isShowing = false;           // parent will paint after completing layout
			}
			else {
				// Strange, but happens (e.g. for Swing InternalFrames): somebody
				// explicitly moved the mouseTgt or one of its parents
				for ( Component c=AWTEvent.mouseTgt; c!= null; c=c.parent ){
					if ( c == this ) {
						MouseEvt.moveMouseTgt( (xNew - x), (yNew - y));
						break;
					}
				}
			}

			if ( isShowing ) {             // get hull (to draw parent background in one sweep)
				x0 = (xNew < x) ? xNew : x;
				y0 = (yNew < y) ? yNew : y;
				x1 = (a = xNew + wNew) > (b = x + width)  ? a : b;
				y1 = (a = yNew + hNew) > (b = y + height) ? a : b;
			}
		}

		x = xNew; y = yNew; width = wNew; height = hNew;

		invalidate();

		if ( isShowing ) {
			if ( hasParent /*&& !(this instanceof OpaqueComponent)*/ )
				parent.repaint( x0, y0, (x1-x0), (y1-y0));
			else
				repaint();
		}
	}
	
	if (((eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0) && (cmpListener !=null)){
		int id = sized ? ComponentEvent.COMPONENT_RESIZED : ComponentEvent.COMPONENT_MOVED;
	  // sync because it might be used to control Graphics visibility
	  // CAUTION: this might cause incompatibilities in ill-behaving apps
	  // (doing explicit sync on show/hide/reshape), but otherwise we would have
	  // to implement a completely redundant mechanism for Graphics updates
		AWTEvent.sendEvent( ComponentEvt.getEvent( this, id), true);
	}
}

/**
 * @deprecated, use setSize( d)
 * this is never called automatically, override setSize in derived classes
 * to change the default behavior
 */
public void resize ( Dimension d ) {
	setSize( d.width, d.height);
}

/**
 * @deprecated, use setSize(w,h)
 * this is never called automatically, override setSize in derived classes
 * to change the default behavior
 */
public void resize ( int wNew, int hNew ) {
	setBounds( x, y, wNew, hNew);
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
	reshape( xNew, yNew, wNew, hNew );
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

public void setLocale(Locale loc) {
	locale = loc;
}

public void setLocation ( Point pt ) {
	setLocation( pt.x, pt.y );
}

public void setLocation ( int x, int y ) {
	move( x, y);
}

public void setName ( String newName ) {
	name = newName;
}

void setNativeCursor ( Cursor cursor ) {
}

public void setSize ( Dimension dim ) {
	resize( dim);
}

public void setSize ( int newWidth, int newHeight ) {
	resize( newWidth, newHeight);
}

public void setVisible ( boolean b ) {
	show( b);
}

public void show () {
	// DEP this should be in setVisible !! But we have to keep it here
	// for compatibility reasons (Swing etc.)

	if ( !isVisible ) {
		isVisible = true;

		if ( parent != null ) {
			if ( parent.isShowing() && !parent.isLayouting ){
				parent.repaint( x, y, width, height);
			}
		}
		// if we are a toplevel, the native window manager will take care
		// of repainting
		
		if (((eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0) && (cmpListener !=null)){
		  // sync because it might be used to control Graphics visibility, see hide, reshape
			AWTEvent.sendEvent( ComponentEvt.getEvent( this,
			                         ComponentEvent.COMPONENT_SHOWN), true);
		}
	}
}

public void show ( boolean b ) {
	if ( b) show();
	else	hide();
}

/**
 * @deprecated, use getSize()
 */
public Dimension size () {
	// DEP - should be in getSize() (but can't because of JDK compatibility)
	// We have to return fresh objects because of the same reasons like in bounds()
	return new Dimension( width, height);
}

public String toString () {
	return (getClass().getName() + '[' + paramString() + ']');
}

/**
 * Transfer the focus to the next appropriate components in this
 * components container.
 */
public void transferFocus() {
	Component curr = this;

	while (curr.parent != null) {

		Container parent = curr.parent;
		int end = parent.getComponentCount();

		/* Find out where 'curr' is in its container so we can start
		 * looking for the next component after it
		 */
		int start;
		for (start = 0; start < end; start++) {
			Component c = parent.getComponent(start);
			if (c == curr) {
				break;
			}
		}

		/* This shouldn't happen but just in case ... */
		if (start == end) {
			return;
		}

		/* Look for next focusable component after me */
		for (start++; start < end; start++) {
			Component c = parent.getComponent(start);
			if (!(c.isEnabled() && c.isVisible() && c.isFocusTraversable())) {
				continue;
			}
			if (!(c instanceof Container)) {
				c.requestFocus();
				return;
			}

			/* We found a new container, drop into it */
			parent = (Container)c;
			end = parent.getComponentCount();
			start = -1;
		}

		curr = parent;
	}
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
	// we can't validate a not-yet-addNotifyed Component
	if ( peer != null )
		isValid = true;
}
}
