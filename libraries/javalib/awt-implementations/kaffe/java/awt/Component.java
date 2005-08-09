package java.awt;

import gnu.classpath.Pointer;
import java.awt.event.ActionEvent;
import java.awt.event.AdjustmentEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.ContainerEvent;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.ItemEvent;
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
import java.awt.peer.ComponentPeer;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.Serializable;
import java.util.Locale;

import kaffe.awt.DoNothingPeer;

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
  implements ImageObserver, MenuContainer, Serializable
{
	// We're not actually compatible with Sun's serialization format, so don't claim to be:
	//final private static long serialVersionUID = -7644114512714619750L;

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
	int eventMask;
	Locale locale;
	PopupMenu popup;
	Rectangle deco = noDeco;
	int flags = IS_VISIBLE;
/**
 * linkedGraphs is a list of WeakReferences to NativeGraphics
 * objects, which is used to keep track of resident Graphics objects
 * for native-like Components (which we have to update in case
 * of a visibility or position change). See GraphicsLink for details
 */
	GraphicsLink linkedGraphs;
	final public static float TOP_ALIGNMENT = 0.0f;
	final public static float CENTER_ALIGNMENT = 0.5f;
	final public static float BOTTOM_ALIGNMENT = 1.0f;
	final public static float LEFT_ALIGNMENT = 0.0f;
	final public static float RIGHT_ALIGNMENT = 1.0f;
	final static int BORDER_WIDTH = 2;
	static Object treeLock = new TreeLock();
	static Rectangle noDeco = new Rectangle();
	final static int IS_VISIBLE = 0x01;
	final static int IS_VALID = 0x02;
	final static int IS_PARENT_SHOWING = 0x04;
	final static int IS_LAYOUTING = 0x08;
	final static int IS_IN_UPDATE = 0x10;
	final static int IS_OLD_EVENT = 0x20;
	final static int IS_RESIZABLE = 0x40;
	final static int IS_MODAL = 0x80;
	final static int IS_NATIVE_LIKE = 0x100;
	final static int IS_OPENED = 0x200;
	final static int IS_ADD_NOTIFIED = 0x400;
	final static int IS_FG_COLORED = 0x800;
	final static int IS_BG_COLORED = 0x1000;
	final static int IS_FONTIFIED = 0x2000;
	final static int IS_ASYNC_UPDATED = 0x4000;
	final static int IS_DIRTY = 0x8000;
	final static int IS_MOUSE_AWARE = 0x10000;
	final static int IS_TEMP_HIDDEN = 0x20000;
	final static int IS_SHOWING = IS_ADD_NOTIFIED | IS_PARENT_SHOWING | IS_VISIBLE;

static class TreeLock
{
}
/*
 * JDK serialization.
 *  While this is serializing something like what JDK expects, we don't handle the more complex
 *  things like Properties or popupMenus yet (because I'm not sure how to convert what we do
 *  into what they expect).
 */
//	private Color bgColor;
//	private java.beans.PropertyChangeSupport changeSupport;
//	private int componentSerializedDataVersion;
//	private Cursor cursor;
//	private boolean enabled;
//	private long eventMask;
//	private Font font;
//	private Color foreground;
//	private boolean hasFocus;
//	private int height;
//	private boolean isPacked;
//	private Locale locale;
//	private Dimension minSize;
//	private String name;
//	private boolean nameExplicitlySet;
//	private boolean newEventsOnly;
//	private Font peerFont;
//	private Vector popups;
//	private Dimension prefSize;
//	private boolean valid;
//	private boolean visible;
//	private int width;
//	private int x;
//	private int y;
// 
// private void readDefaultObject() {
// 	setBackground(bgColor);
// 	setCursor(cursor);
// 	setEnabled(enabled);
// 	enableEvents(eventMask);
// 	setFont(font);
// 	setForeground(foreground);
// 	setSize(width, height);
// 	setLocale(locale);
// 	setName(name);
// 	setLocation(x, y);
// 	if (valid) {
// 		validate();
// 	}
// 	else {
// 		invalidate();
// 	}
// 	if (visible) {
// 		show();
// 	}
// 	else {
// 		hide();
// 	}
// }
// 
// private void writeDefaultObject() {
// 	bgColor = Component.this.bgClr;
// 	changeSupport = null;
// 	componentSerializedDataVersion = 0;
// 	cursor = Component.this.cursor;
// 	enabled = isEnabled();
// 	eventMask = Component.this.eventMask;
// 	font = Component.this.font;
// 	foreground = Component.this.fgClr;
// 	hasFocus = false;
// 	height = Component.this.height;
// 	isPacked = false;
// 	locale = Component.this.locale;
// 	minSize = getMinimumSize();
// 	name = Component.this.name;
// 	nameExplicitlySet = true;
// 	newEventsOnly = !getClassProperties().useOldEvents;
// 	peerFont = Component.this.font;
// 	popups = null;
// 	prefSize = getPreferredSize();
// 	valid = isValid();
// 	visible = isVisible();
// 	width = Component.this.width;
// 	x = Component.this.x;
// 	y = Component.this.y;
// }
// 
protected Component () {
	cursor = Cursor.defaultCursor;
}

/**
 * @deprecated
 */
public boolean action(Event evt, Object what) {
	return (false);
}

public void add ( PopupMenu menu ) {
	if ( menu.parent != null )
		menu.parent.remove( menu);
		
	if ( (flags & IS_ADD_NOTIFIED) > 0 ) {
		menu.parent = this;
		menu.owner = this;
		menu.addNotify();
	}
		
	if ( popup == null )
		popup = menu;
	else {
		popup.addSeparator();
		popup.addAll( menu);
	}
}

public void addComponentListener ( ComponentListener newListener ) {
	cmpListener = AWTEventMulticaster.add( cmpListener, newListener);
}

public void addFocusListener ( FocusListener newListener ) {
	focusListener = AWTEventMulticaster.add( focusListener, newListener);
}

public void addKeyListener ( KeyListener newListener ) {
	keyListener = AWTEventMulticaster.add( keyListener, newListener);
}

public void addMouseListener ( MouseListener newListener ) {
	mouseListener = AWTEventMulticaster.add( mouseListener, newListener);
	
	flags |= IS_MOUSE_AWARE;
}

public void addMouseMotionListener ( MouseMotionListener newListener ) {
	motionListener = AWTEventMulticaster.add( motionListener, newListener);
	
	flags |= IS_MOUSE_AWARE;
}

public void addNotify () {
	if ( (flags & IS_ADD_NOTIFIED) == 0 ) {
		flags |= IS_ADD_NOTIFIED;

		ClassProperties props = getClassProperties();
		if ( props.isNativeLike ){
			flags |= IS_NATIVE_LIKE;
		}
		if ( parent != null ) {
			// Note that this only works in case the parent is addNotified
			// *before* its childs. (we can't use isNativeLike to filter this out
			// unless we turn BarMenus into isNativeLike, which is bad)
			if ( (((parent.flags & IS_OLD_EVENT) != 0) || props.useOldEvents) ){
				flags |= (IS_OLD_EVENT | IS_MOUSE_AWARE);
			}
		}
		else { // Window
			if ( props.useOldEvents )
				flags |= (IS_OLD_EVENT | IS_MOUSE_AWARE);
		}
		
		if ( popup != null ) {
			popup.parent = this;
			popup.owner = this;
			popup.addNotify();
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
	return (image.checkImage( -1, -1, obs, false));
}

public int checkImage (Image image, int width, int height, ImageObserver obs) {
	return (image.checkImage( width, height, obs, false));
}

void checkMouseAware () {
	if ( ((eventMask & AWTEvent.DISABLED_MASK) == 0) &&
	     ((mouseListener != null) ||
	      (motionListener != null) ||
	      (eventMask & (AWTEvent.MOUSE_EVENT_MASK|AWTEvent.MOUSE_MOTION_EVENT_MASK)) != 0 ||
	      (flags & IS_OLD_EVENT) != 0 )) {
		flags |= IS_MOUSE_AWARE;
	}
	else {
		flags &= ~IS_MOUSE_AWARE;
	}
}

void cleanUpNative () {
	// nothing native, all lightweight
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

void createNative () {
	// nothing native, all lightweight
}

/**
 * @deprecated
 */
public void deliverEvent(Event evt) {
	postEvent(evt);
}

void destroyNative () {
	// nothing native, all lightweight
}

/**
 * @deprecated, use setEnabled()
 */
public void disable() {
	setEnabled(false);
}

public void disableEvents ( long disableMask ) {
	eventMask &= ~disableMask;
	checkMouseAware();
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

void dump ( String prefix ) {
	System.out.print( prefix);
	System.out.println( this);
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
	checkMouseAware();
}

public float getAlignmentX() {
	return CENTER_ALIGNMENT;
}

public float getAlignmentY() {
	return CENTER_ALIGNMENT;
}

public Color getBackground () {
	return bgClr;
/*
	if ( bgClr != null )
		return bgClr;

	for ( Component c=parent; c != null; c = c.parent ) {
		if ( c.bgClr != null ) return c.bgClr;
	}
	
	// even though not in the specs, some apps (e.g. swing) rely on the
	// JDK behavior of returning 'null' if there isn't a parent yet
	return null;
	//return Color.white;
*/
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
	return ClassAnalyzer.analyzeProcessEvent( getClass(), false);
}

public ColorModel getColorModel() {
  	return Toolkit.getDefaultToolkit().getColorModel();
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
	return font;
}

public FontMetrics getFontMetrics ( Font font ) {
	return FontMetrics.getFontMetrics( font);
}

public Color getForeground () {
	return fgClr;
}

public Graphics getGraphics () {
	if ( (flags & IS_ADD_NOTIFIED) != 0 )
		return NativeGraphics.getClippedGraphics( null, this, 0, 0, 0, 0, width, height, false);
	else
		return null;
}

public int getHeight() {
	return height;
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

Pointer getNativeData () {
	return null;  // no nativeData, all lightweight
}

public Container getParent() {
	return parent;
}

/**
 * @deprecated, should not be called.
 */
public ComponentPeer getPeer() {
	// this is just a dummy, i.e. we share a single object that can be used
	// ONLY to "(getPeer() != null)" check if we already passed addNotify()
	return ((flags & IS_ADD_NOTIFIED) != 0) ? DUMMY_PEER : null;
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
	return treeLock;
}

public int getWidth() {
	return width;
}

public int getX() {
	return x;
}

public int getY() {
	return y;
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

public void hide () {
	// DEP this should be in setVisible !! But we have to keep it here
	// for compatibility reasons (Swing etc.)

	if ( (flags & IS_VISIBLE) != 0 ) {
		flags &= ~IS_VISIBLE;

		// if we are a toplevel, the native window manager will take care
		// of repainting, otherwise we have to do it explicitly
		if ( (parent != null) && ((parent.flags & IS_LAYOUTING) == 0) ) {
			if ( (flags & IS_PARENT_SHOWING) != 0) {
				parent.repaint( x, y, width, height);
			}

			if ( (parent.flags & IS_VALID) != 0 )
				parent.invalidate();
		}
		
		if ( (cmpListener != null) || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0 ){
			Toolkit.eventQueue.postEvent( ComponentEvt.getEvent( this,
			                                  ComponentEvent.COMPONENT_HIDDEN));
		}
		
		if ( linkedGraphs != null )
			updateLinkedGraphics();
	}
}

public boolean imageUpdate (Image img, int infoflags, int x, int y, int width, int height ) {
	if ( (infoflags & (ALLBITS | FRAMEBITS)) != 0 ) {
		if ( (flags & IS_SHOWING) == IS_SHOWING )
			repaint();
	}

	// We return false if we're no longer interested in updates.This is *NOT*
	// what is said in the Addison-Wesley documentation, but is what is says
	// in the JDK javadoc documentation.
	if ( (infoflags & (ALLBITS | ABORT | ERROR)) != 0 ) {
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
	if ( (flags & IS_SHOWING) != IS_SHOWING ) return false;
	if ( (x < 0) || (y < 0) || (x > width) || (y > height) ) return false;

/***
	// If we want to deal with components being bigger than their parents,
	// we have to check for parent.contains(), too. However, this is not
	// done by the JDK, and we therefor skip it for now
	x += this.x; y += this.y;
	for ( Container c=parent; c!= null; x += c.x, y += c.y, c = c.parent ) {
		if ( (x < 0) || (y < 0) || (x > c.width) || (y > c.height) )
			return false;
	}
***/

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

	synchronized ( treeLock ) {
		if ( (flags & IS_VALID) != 0 ) {
			flags &= ~IS_VALID;
		
			// maybe, it's overloaded (we have to sacrifice speed for compat, here)
			// parent can't be null, because we can't get valid without being addNotifyed
			if ( (parent.flags & IS_VALID) != 0 )
				parent.invalidate();
		}
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
	return (((flags & (IS_SHOWING|IS_NATIVE_LIKE)) == (IS_SHOWING|IS_NATIVE_LIKE)) && 
	        ((eventMask & AWTEvent.DISABLED_MASK) == 0));
}

public boolean isShowing () {
	// compare the costs of this with the standard upward iteration
 return  ((flags & (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED)) ==
	                 (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED));
}

public boolean isValid () {
	return ((flags & IS_VALID) != 0);
}

public boolean isVisible () {
	return ((flags & IS_VISIBLE) != 0);
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

synchronized void linkGraphics ( NativeGraphics g ) {
	GraphicsLink li, last, next; 

	// do some cleanup as we go
	for ( li = linkedGraphs, last = null; li != null; ){
		if ( li.get() == null ){
			// recycle this one, its Graphics has been collected
			if ( last == null ){
				linkedGraphs = li.next;
			}
			else {
				last.next = li.next;
			}
			
			next = li.next;
			li = next;
		}
		else {
			last = li;
			li = li.next;
		}
	}
	
	// References are immutable, i.e. we can't cache them for later re-use.
	// Since we cache Graphics objects, the best we can do is to use GraphicsLinks
	// objects exclusively (sort of per-Graphics cache)
	if ( g.link == null ){
		li = new GraphicsLink( g);
		g.link = li;
	}
	else {
		li = g.link;
	}
	
	// set the target/link state (we don't want to use common Graphics objects
	// with all that fuzz which is just needed for linked Graphicses)
	li.xOffset   = g.xOffset;
	li.yOffset   = g.yOffset;
	li.next      = linkedGraphs;
	li.width     = width;
	li.height    = height;
	li.isVisible = ((flags & IS_SHOWING) == IS_SHOWING);

	g.target = this;
	
	linkedGraphs = li;
}


public void list() {
    list(System.out);
}

public void list(PrintStream out) {
    list(out, 0);
}

public void list(PrintStream out, int indent) {
    list (new PrintWriter(out), indent);
}

public void list(PrintWriter out) {
    list (out, 0);
}

public void list(PrintWriter out, int indent) {
    for (int i = indent; i > 0; --i) {
	out.print(' ');
    }
    out.println(toString());

    out.flush();
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
	// nothing to do here, that all has to be donw in subclasses
}

public void paintAll ( Graphics g ) {
	paint( g);
}

void kaffePaintBorder () {
	Graphics g = getGraphics();
	if ( g != null ) {
		kaffePaintBorder( g);
		g.dispose();
	}
}

void kaffePaintBorder ( Graphics g ) {
	kaffePaintBorder( g, 0, 0, 0, 0);
}

void kaffePaintBorder ( Graphics g, int left, int top, int right, int bottom ) {
	int w = width - (left + right);
	int h = height - (top + bottom);

	if ( this == AWTEvent.keyTgt )
		g.setColor( Defaults.FocusClr);
	else
		g.setColor( Defaults.BorderClr);

	if (w-1 > 0 && h-1 > 0) {
		g.draw3DRect( left, top,  w-1, h-1, true);
	}
	if (w-3 > 0 && h-3 > 0) {
		g.draw3DRect( left+1, top+1, w-3, h-3, false);
	}
}

protected String paramString () {
	String s = name + ',' + x + ',' + y + ',' + width + 'x' + height;
	
	if ( !isValid() )   s += ",invalid";	
	if ( !isVisible() ) s += ",hidden";
	if ( !isEnabled() ) s += ",disabled";
	
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

// Commented out since it doubles an event's x
// and y coordinates with the Main example for java.awt.Component
// from the Java Class Libraries book.
//			    evt.x += c.x;
//			    evt.y += c.y;
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
	return (Image.loadImage( image, width, height, obs));
}

public void print ( Graphics g ) {
}

public void printAll ( Graphics g ) {
}

void process ( ActionEvent e ) {
	// we don't know nothing about ActionEventListeners
}

void process ( AdjustmentEvent e ) {
	// we don't know nothing about AdjustmentEventListeners
}

void process ( ComponentEvent e ) {
	if ( (cmpListener != null) || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0)
		processEvent( e);
}

void process ( ContainerEvent e ) {
	// we don't know nothing about ContainerEventListeners
}

void process ( FocusEvent e ) {
	if ( AWTEvent.focusHook != null ){
		if ( AWTEvent.focusHook.intercept( e) )
			return;
	}

	if ( (focusListener != null) || (eventMask & AWTEvent.FOCUS_EVENT_MASK) != 0){
		processEvent( e);
	}
	
	if ( (flags & IS_OLD_EVENT) != 0 ) postEvent( Event.getEvent( e));
}

void process ( ItemEvent e ) {
	// we don't know nothing about ItemEventListeners
}

void process ( KeyEvent e ) {
	if ( AWTEvent.keyHook != null ){
		if ( AWTEvent.keyHook.intercept( e) )
			return;
	}

	if ( (keyListener != null) || (eventMask & AWTEvent.KEY_EVENT_MASK) != 0){
		processEvent( e);
	}

	if ( (flags & IS_OLD_EVENT) != 0 ) postEvent( Event.getEvent( e));
}

void process ( TextEvent e ) {
	// we don't know nothing about TextEventListeners
}

void process ( WindowEvent e ) {
	// we don't know nothing about WindowEventListeners
}

protected void processActionEvent ( ActionEvent e ) {
}

protected void processAdjustmentEvent ( AdjustmentEvent e ) {
}

protected void processComponentEvent ( ComponentEvent event ) {

	if ( cmpListener != null ){
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
		// Not hard to anticipate that there will be AWT test suites throwing synthetic
		// events against real components. However, this is "out of spec", most native window
		// environments would act on the emitter side (rather than the responder), like we do
		// (in MouseEvt). Moreover, processEvent() might be resolved by a derived class
		// if ( (eventMask & AWTEvent.DISABLED_MASK) == 0 )
		processMouseEvent( (MouseEvent)e);
		break;
		
	case MouseEvent.MOUSE_MOVED:
	case MouseEvent.MOUSE_DRAGGED:
		processMouseMotionEvent( (MouseEvent)e);
		break;
		
	case KeyEvent.KEY_TYPED:			//400..402
	case KeyEvent.KEY_PRESSED:
	case KeyEvent.KEY_RELEASED:
		// if ( (eventMask & AWTEvent.DISABLED_MASK) == 0 )
		processKeyEvent( (KeyEvent)e);
		break;
			
	case FocusEvent.FOCUS_GAINED:	//1004..1005
	case FocusEvent.FOCUS_LOST:
		processFocusEvent( (FocusEvent)e);
		break;

	case PaintEvent.PAINT:
	case PaintEvent.UPDATE:
		Rectangle r = ((PaintEvent)e).getUpdateRect();
		processPaintEvent( e.id, r.x, r.y, r.width, r.height);
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
	if ( focusListener != null ) {
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
	if ( keyListener != null ) {
		switch ( event.id ) {
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

void processMotion ( MouseEvent e ) {
	if ( AWTEvent.mouseHook != null ){
		if ( AWTEvent.mouseHook.intercept( e) )
			return;
	}

	if ( (motionListener != null) || (eventMask & AWTEvent.MOUSE_MOTION_EVENT_MASK) != 0)
		processEvent( e);

	if ( (flags & IS_OLD_EVENT) != 0 ){
		postEvent( Event.getEvent( e));
	}
}

void processMouse ( MouseEvent e ) {
	if ( AWTEvent.mouseHook != null ){
		if ( AWTEvent.mouseHook.intercept( e) )
			return;
	}

	if ( (mouseListener != null) || (eventMask & AWTEvent.MOUSE_EVENT_MASK) != 0)
		processEvent( e);

	if ( (flags & IS_OLD_EVENT) != 0 ){
		postEvent( Event.getEvent( e));
	}
}

protected void processMouseEvent ( MouseEvent event ) {
	if ( mouseListener != null ) {
		switch ( event.id ) {
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
	if ( motionListener != null ) {
		switch ( event.id ) {
		case MouseEvent.MOUSE_MOVED:
			motionListener.mouseMoved( event);
			return;
		case MouseEvent.MOUSE_DRAGGED:
			motionListener.mouseDragged( event);
			return;
		}
	}
}

void processPaintEvent ( int id, int ux, int uy, int uw, int uh ) {
	NativeGraphics g = NativeGraphics.getClippedGraphics( null, this, 0,0,
	                                                      ux, uy, uw, uh,
	                                                      false);
	if ( g != null ){
		if ( id == PaintEvent.UPDATE )
			update( g);
		else
			paint( g);
		g.dispose();
	}
}

protected void processTextEvent ( TextEvent e ) {
}

protected void processWindowEvent ( WindowEvent e ) {
}

void propagateBgClr ( Color clr ) {
	// should be called *only* on Components with unset IS_BG_COLORED
	bgClr = clr;
}

void propagateFgClr ( Color clr ) {
	// should be called *only* on Components with unset IS_FG_COLORED
	fgClr = clr;
}

void propagateFont ( Font fnt ) {
	// should be called *only* on Components with unset IS_FONTIFIED
	font = fnt;
}

void propagateParentShowing ( boolean isTemporary) {
	// no kids, we don't have to propagate anything
	
	// but we might have resident Graphics which have to be notified
	if ( !isTemporary ) {
		if ( linkedGraphs != null )
			updateLinkedGraphics();
	}
}

void propagateReshape () {
	// no kids, we don't have to propagate anything
	
	// but we might have resident Graphics which have to be notified
	if ( linkedGraphs != null )
		updateLinkedGraphics();
}

void propagateTempEnabled ( boolean isEnabled ) {
	if ( isEnabled) {
		if ( (eventMask & AWTEvent.TEMP_DISABLED_MASK) != 0 ) 
			eventMask &= ~(AWTEvent.DISABLED_MASK | AWTEvent.TEMP_DISABLED_MASK);
	}
	else {
		if ( (eventMask & AWTEvent.DISABLED_MASK) == 0 )
			eventMask |= (AWTEvent.DISABLED_MASK | AWTEvent.TEMP_DISABLED_MASK);
	}
	
	checkMouseAware();
}

public void remove( MenuComponent mc) {
	mc.removeNotify();
	mc.parent = null;
	mc.owner = null;
	
	if ( popup != null)
		popup.remove( mc);
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
	
	checkMouseAware();
}

public void removeMouseMotionListener ( MouseMotionListener listener ) {
	motionListener = AWTEventMulticaster.remove( motionListener, listener);
	
	checkMouseAware();
}

public void removeNotify () {
	flags &= ~IS_ADD_NOTIFIED;

	if ( popup != null ) {
		popup.removeNotify();
	}

	// the inflight video program will be ceased by now, clean up any
	// leftover global state (remember: removeNotify can be called anywhere, anytime)
	if ( this == AWTEvent.mouseTgt )
		AWTEvent.mouseTgt = null;
	if ( this == AWTEvent.keyTgt )
		AWTEvent.keyTgt = null;
	if ( this == AWTEvent.activeWindow )
		AWTEvent.activeWindow = null;
	if ( this == FocusEvt.keyTgtRequest )
		FocusEvt.keyTgtRequest = null;
	
	// this is arguable - we could also make the check in all relevant event dipatch()
	// methods, but it's probably more efficient to do it once, at the source
	// (note that we don't have to care for native events because of unregisterSource())
	if ( Toolkit.eventQueue.localQueue != null ) {
		Toolkit.eventQueue.dropLiveEvents( this);
	}
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
	if ( (flags & IS_SHOWING) == IS_SHOWING ){
	
		// be paranoid, some clients might request repaints outside their own turf
		if ( x < 0 ) x = 0;
		if ( y < 0 ) y = 0;
		if ( (x + width) > this.width )
			width = this.width - x;
		if ( (y + height) > this.height )
			height = this.height - y;
	
		Toolkit.eventQueue.postPaintEvent( PaintEvent.UPDATE, this, x, y, width, height);
	}
}

public void requestFocus () {
	Component topNew;

	if ( AWTEvent.keyTgt == this ){   // nothing to do
		return;
	}

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
		if (topNew != AWTEvent.activeWindow ) {  // this involves a change of active toplevels
			FocusEvt.keyTgtRequest = this;
			topNew.requestFocus();
		}
		else {                                 // intra toplevel focus change
			Toolkit.eventQueue.postFocusEvent( FocusEvt.getEvent( this, FocusEvent.FOCUS_GAINED, false));
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
	boolean  sized = ( (width != wNew) || (height != hNew) );
	boolean  moved = ( !sized && ((x != xNew) || (y != yNew)) );
	int      id = sized ? ComponentEvent.COMPONENT_RESIZED : ComponentEvent.COMPONENT_MOVED;

	// Don't do anything if we don't change anything.
	if (sized || moved) {

		if ( parent != null ) {
			// Strange, but happens (e.g. for Swing InternalFrames): somebody
			// explicitly moved the mouseTgt or one of its parents (maybe in a mouse modal drag!)
			if ( MouseEvt.mouseDragged ) {
				for ( Component c=AWTEvent.mouseTgt; c!= null; c=c.parent ){
					if ( c == this ) {
						MouseEvt.moveMouseTgt( (xNew - x), (yNew - y));
						break;
					}
				}
			}

			if ( (flags & IS_SHOWING) == IS_SHOWING ) {
				x0 = (xNew < x) ? xNew : x;
				y0 = (yNew < y) ? yNew : y;
				a = xNew + wNew;
				b = x + width;
				x1 = (a > b ? a : b);
				a = yNew + hNew;
				b = y + height;
				y1 = (a > b ? a : b);
			
				x = xNew; y = yNew; width = wNew; height = hNew;
				invalidate();
				
				if ( (cmpListener != null) || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0 ){
					Toolkit.eventQueue.postEvent( ComponentEvt.getEvent( this, id));
				}
				propagateReshape();

				// Redrawing the parent does not happen automatically, we can do it here
				// (regardless of IS_LAYOUTING) since we have repaint - solicitation, anyway
				parent.repaint( x0, y0, (x1-x0), (y1-y0));

				return;
			}
		}
		x = xNew; y = yNew; width = wNew; height = hNew;
		invalidate();

		if ( (cmpListener != null) || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0 ){
			Toolkit.eventQueue.postEvent( ComponentEvt.getEvent( this, id));
		}
		propagateReshape();
	}
}

/**
 * @deprecated, use setSize( d)
 * this is never called automatically, override setSize in derived classes
 * to change the default behavior
 */
public void resize ( Dimension d ) {
	// Part of the JDK resize/setSize/setBounds/reshape twist. Don't change or you
	// might break compatibility of derived app classes. The workhorse is still reshape
	setSize( d.width, d.height);
}

/**
 * @deprecated, use setSize(w,h)
 * this is never called automatically, override setSize in derived classes
 * to change the default behavior
 */
public void resize ( int wNew, int hNew ) {
	// Part of the JDK resize/setSize/setBounds/reshape twist. Don't change or you
	// might break compatibility of derived app classes. The workhorse is still reshape
	setBounds( x, y, wNew, hNew);
}

public void setBackground ( Color clr ) {
	if ( clr == bgClr )
		return;

	if ( clr != null ){
		flags |= IS_BG_COLORED;
	}
	else {
		flags &= ~IS_BG_COLORED;
		if ( parent != null )
			clr = parent.bgClr;
	}

	propagateBgClr( clr);

	// we follow the "Java class libraries" description here (in favor of the Sun class docu), i.e.
	// clients have to explicitly force a repaint after changing colors. But - since many apps
	// rely in this unspec. behavior - we have to repaint automatically for native-likes
	if ( (flags & (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED | IS_NATIVE_LIKE))
	        == (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED) ) {
		repaint();
	}
}

public void setBounds ( Rectangle r ) {
	// Part of the JDK resize/setSize/setBounds/reshape twist. Don't change or you
	// might break compatibility of derived app classes. The workhorse is still reshape
	setBounds( r.x, r.y, r.width, r.height);
}

public void setBounds ( int xNew, int yNew, int wNew, int hNew ) {
	// Part of the JDK resize/setSize/setBounds/reshape twist. Don't change or you
	// might break compatibility of derived app classes. The workhorse is still reshape
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
		
	checkMouseAware();
}

public void setFont ( Font fnt ) {
	if ( fnt == font )
		return;

	if ( fnt != null ){
		flags |= IS_FONTIFIED;
	}
	else {
		flags &= ~IS_FONTIFIED;
		if ( parent != null )
			fnt = parent.font;
	}

	propagateFont( fnt);

	// see setBackground for further details about why to repaint just visible native-likes
	if ( (flags & (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED | IS_NATIVE_LIKE))
	        == (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED) ) {
		repaint();
	}
}

public void setForeground ( Color clr ) {
	if ( clr == fgClr )
		return;

	if ( clr != null ){
		flags |= IS_FG_COLORED;
	}
	else {
		flags &= ~IS_FG_COLORED;
		if ( parent != null )
			clr = parent.fgClr;
	}

	propagateFgClr( clr);

	// see setBackground for further details about why to repaint just visible native-likes
	if ( (flags & (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED | IS_NATIVE_LIKE))
	        == (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED) ) {
		repaint();
	}
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
	// Part of the JDK resize/setSize/setBounds/reshape twist. Don't change or you
	// might break compatibility of derived app classes. The workhorse is still reshape
	resize( dim);
}

public void setSize ( int newWidth, int newHeight ) {
	// Part of the JDK resize/setSize/setBounds/reshape twist. Don't change or you
	// might break compatibility of derived app classes. The workhorse is still reshape
	resize( newWidth, newHeight);
}

public void setVisible ( boolean b ) {
	show( b);
}

public void show () {
	// DEP this should be in setVisible !! But we have to keep it here
	// for compatibility reasons (Swing etc.)

	if ( (flags & IS_VISIBLE) == 0 ) {
		flags |= IS_VISIBLE;
		flags &= ~IS_TEMP_HIDDEN;

	  // if we are a toplevel, the native window manager will take care
	  // of repainting
		if ( (parent != null) && ((parent.flags & IS_LAYOUTING) == 0) ) {
			if ( (flags & (IS_ADD_NOTIFIED | IS_PARENT_SHOWING))
			       == (IS_ADD_NOTIFIED | IS_PARENT_SHOWING) ){
			  //parent.repaint( x, y, width, height);
				repaint();
			}
			
			if ( (parent.flags & IS_VALID) != 0 )
				parent.invalidate();
		}

		if ( (cmpListener != null) || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0 ){
			Toolkit.eventQueue.postEvent( ComponentEvt.getEvent( this,
				                                    ComponentEvent.COMPONENT_SHOWN));
		}

		// update any resident graphics objects		
		if ( linkedGraphs != null )
			updateLinkedGraphics();
	}
}

public void show ( boolean b ) {
	// DEP this should map to setVisible but we have to keep it that way because of
	// compatibility, which in this case requires a double indirection)

	if ( b)
		show();
	else
		hide();
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

			if (c.isEnabled() && ((c.flags & IS_VISIBLE) !=0) && c.isFocusTraversable()) {
			  // Then if it is enabled, visible and focus traversable set the focus to it
			  c.requestFocus();
			  return;
			} else if (c instanceof Container) {
			  // If it is a container drop into it
			parent = (Container)c;
			end = parent.getComponentCount();
			start = -1;
			}
		}

		curr = parent;
	}
}

PopupMenu triggerPopup ( int x, int y ) {
	if ( popup != null ) {
		popup.show( this, x, y);
		return popup;
	}
	
	return null;
}

synchronized void unlinkGraphics ( NativeGraphics g ) {
	GraphicsLink li, last, next;
	Object       lg;

	// do some cleanup as we go
	for ( li = linkedGraphs, last = null; li != null; ){
		if ( ((lg = li.get()) == null) || (lg == g ) ){
			// recycle this one, its Graphics has been collected or disposed
			if ( last == null ){
				linkedGraphs = li.next;
			}
			else {
				last.next = li.next;
			}
			
			next = li.next;
			li = next;
		}
		else {
			last = li;
			li = li.next;
		}
	}
}

public void update ( Graphics g ) {
	g.clearRect( 0, 0, width, height);
	paint( g);
}

synchronized void updateLinkedGraphics () {
	GraphicsLink li, last, next;

	for ( li = linkedGraphs, last = null; li != null; ){
		if ( !li.updateGraphics( this) ){
			// recycle this one on-the-fly, its Graphics has been collected
			if ( last == null ){
				linkedGraphs = li.next;
			}
			else {
				last.next = li.next;
			}

			next = li.next;
			li = next;
		}
		else {
			last = li;
			li = li.next;
		}
	}
}

public void validate () {
	// we can't validate a not-yet-addNotifyed Component
	if ( (flags & IS_ADD_NOTIFIED) != 0 ) {
		flags |= IS_VALID;
	}
}

  /**
   * Dummy lightweight peer singleton.
   */
  private static final DoNothingPeer DUMMY_PEER = new DoNothingPeer ();
}
