/**
 * Component - abstract root of all widgets
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

import gnu.classpath.Pointer;
import java.awt.event.ActionEvent;
import java.awt.event.AdjustmentEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.ContainerEvent;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.HierarchyBoundsListener;
import java.awt.event.HierarchyEvent;
import java.awt.event.HierarchyListener;
import java.awt.event.InputMethodEvent;
import java.awt.event.InputMethodListener;
import java.awt.event.ItemEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseWheelListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.PaintEvent;
import java.awt.event.TextEvent;
import java.awt.event.WindowEvent;
import java.awt.image.BufferStrategy;
import java.awt.image.ColorModel;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.awt.image.VolatileImage;
import java.awt.peer.ComponentPeer;
import java.awt.peer.LightweightPeer;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.Serializable;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Locale;
import java.util.Set;
import java.util.Vector;
import java.awt.dnd.DropTarget;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

import javax.accessibility.Accessible;
import javax.accessibility.AccessibleComponent;
import javax.accessibility.AccessibleContext;
import javax.accessibility.AccessibleRole;
import javax.accessibility.AccessibleState;
import javax.accessibility.AccessibleStateSet;

import org.kaffe.awt.DoNothingPeer;


 
 
abstract public class Component
  extends Object
  implements ImageObserver, MenuContainer, Serializable
{
	// We're not actually compatible with Sun's serialization format, so don't claim to be:
	//final private static long serialVersionUID = -7644114512714619750L;
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

        /**
            * The x position of the component in the parent's coordinate system.
         *
         * @see #getLocation()
         * @serial the x position
         */
        int x;

        /**
            * The y position of the component in the parent's coordinate system.
         *
         * @see #getLocation()
         * @serial the y position
         */
        int y;

        /**
            * The component width.
         *
         * @see #getSize()
         * @serial the width
         */
        int width;

        /**
            * The component height.
         *
         * @see #getSize()
         * @serial the height
         */
        int height;

        /**
            * The foreground color for the component. This may be null.
         *
         * @see #getForeground()
         * @see #setForeground(Color)
         * @serial the foreground color
         */
        Color foreground;

        /**
            * The background color for the component. This may be null.
         *
         * @see #getBackground()
         * @see #setBackground(Color)
         * @serial the background color
         */
        Color background;

        /**
            * The default font used in the component. This may be null.
         *
         * @see #getFont()
         * @see #setFont(Font)
         * @serial the font
         */
        Font font;

        /**
            * The font in use by the peer, or null if there is no peer.
         *
         * @serial the peer's font
         */
        Font peerFont;

        /**
            * The cursor displayed when the pointer is over this component. This may
         * be null.
         *
         * @see #getCursor()
         * @see #setCursor(Cursor)
         */
        Cursor cursor;

        /**
            * The locale for the component.
         *
         * @see #getLocale()
         * @see #setLocale(Locale)
         */
        Locale locale = Locale.getDefault ();

        /**
            * True if the object should ignore repaint events (usually because it is
                                                               * not showing).
         *
         * @see #getIgnoreRepaint()
         * @see #setIgnoreRepaint(boolean)
         * @serial true to ignore repaints
         * @since 1.4
         */
        boolean ignoreRepaint;

        /**
            * True when the object is visible (although it is only showing if all
                                               * ancestors are likewise visible). For component, this defaults to true.
         *
         * @see #isVisible()
         * @see #setVisible(boolean)
         * @serial true if visible
         */
        boolean visible = true;

        /**
            * True if the object is enabled, meaning it can interact with the user.
         * For component, this defaults to true.
         *
         * @see #isEnabled()
         * @see #setEnabled(boolean)
         * @serial true if enabled
         */
        boolean enabled = true;

        /**
            * True if the object is valid. This is set to false any time a size
         * adjustment means the component need to be layed out again.
         *
         * @see #isValid()
         * @see #validate()
         * @see #invalidate()
         * @serial true if layout is valid
         */
        boolean valid;

        /**
            * The DropTarget for drag-and-drop operations.
         *
         * @see #getDropTarget()
         * @see #setDropTarget(DropTarget)
         * @serial the drop target, or null
         * @since 1.2
         */
        DropTarget dropTarget;

        /**
            * The list of popup menus for this component.
         *
         * @see #add(PopupMenu)
         * @serial the list of popups
         */
        Vector popups;

        /**
            * The component's name. May be null, in which case a default name is
         * generated on the first use.
         *
         * @see #getName()
         * @see #setName(String)
         * @serial the name
         */
        String name;

        /**
            * True once the user has set the name. Note that the user may set the name
         * to null.
         *
         * @see #name
         * @see #getName()
         * @see #setName(String)
         * @serial true if the name has been explicitly set
         */
        boolean nameExplicitlySet;

        /**
            * Indicates if the object can be focused. Defaults to true for components.
         *
         * @see #isFocusable()
         * @see #setFocusable(boolean)
         * @since 1.4
         */
        boolean focusable = true;

        /**
            * Tracks whether this component's {@link #isFocusTraversable}
         * method has been overridden.
         *
         * @since 1.4
         */
        int isFocusTraversableOverridden;

        /**
            * The focus traversal keys, if not inherited from the parent or
         * default keyboard focus manager. These sets will contain only
         * AWTKeyStrokes that represent press and release events to use as
         * focus control.
         *
         * @see #getFocusTraversalKeys(int)
         * @see #setFocusTraversalKeys(int, Set)
         * @since 1.4
         */
        Set[] focusTraversalKeys;

        /**
            * True if focus traversal keys are enabled. This defaults to true for
         * Component. If this is true, keystrokes in focusTraversalKeys are trapped
         * and processed automatically rather than being passed on to the component.
         *
         * @see #getFocusTraversalKeysEnabled()
         * @see #setFocusTraversalKeysEnabled(boolean)
         * @since 1.4
         */
        boolean focusTraversalKeysEnabled = true;

        /**
            * Cached information on the minimum size. Should have been transient.
         *
         * @serial ignore
         */
        Dimension minSize;

        /**
            * Cached information on the preferred size. Should have been transient.
         *
         * @serial ignore
         */
        Dimension prefSize;

        /**
            * Set to true if an event is to be handled by this component, false if
         * it is to be passed up the hierarcy.
         *
         * @see #dispatchEvent(AWTEvent)
         * @serial true to process event locally
         */
        boolean newEventsOnly;

        /**
            * Set by subclasses to enable event handling of particular events, and
         * left alone when modifying listeners. For component, this defaults to
         * enabling only input methods.
         *
         * @see #enableInputMethods(boolean)
         * @see AWTEvent
         * @serial the mask of events to process
         */
        long eventMask = AWTEvent.INPUT_ENABLED_EVENT_MASK;

        /**
            * Describes all registered PropertyChangeListeners.
         *
         * @see #addPropertyChangeListener(PropertyChangeListener)
         * @see #removePropertyChangeListener(PropertyChangeListener)
         * @see #firePropertyChange(String, Object, Object)
         * @serial the property change listeners
         * @since 1.2
         */
        PropertyChangeSupport changeSupport;

        /**
            * True if the component has been packed (layed out).
         *
         * @serial true if this is packed
         */
        boolean isPacked;

        /**
            * The serialization version for this class. Currently at version 4.
         *
         * XXX How do we handle prior versions?
         *
         * @serial the serialization version
         */
        int componentSerializedDataVersion = 4;

        /**
            * The accessible context associated with this component. This is only set
         * by subclasses.
         *
         * @see #getAccessibleContext()
         * @serial the accessibility context
         * @since 1.2
         */
        AccessibleContext accessibleContext;
        

        // Guess what - listeners are special cased in serialization. See
        // readObject and writeObject.

        /** Component listener chain. */
        transient ComponentListener componentListener;

        /** Focus listener chain. */
        transient FocusListener focusListener;

        /** Key listener chain. */
        transient KeyListener keyListener;

        /** Mouse listener chain. */
        transient MouseListener mouseListener;

        /** Mouse motion listener chain. */
        transient MouseMotionListener mouseMotionListener;

        /**
            * Mouse wheel listener chain.
         *
         * @since 1.4
         */
        transient MouseWheelListener mouseWheelListener;

        /**
            * Input method listener chain.
         *
         * @since 1.2
         */
        transient InputMethodListener inputMethodListener;

        /**
            * Hierarcy listener chain.
         *
         * @since 1.3
         */
        transient HierarchyListener hierarchyListener;

        /**
            * Hierarcy bounds listener chain.
         *
         * @since 1.3
         */
        transient HierarchyBoundsListener hierarchyBoundsListener;

        /** The parent. */
        transient Container parent;

        /** The associated native peer. */
        transient ComponentPeer peer;

        /** The preferred component orientation. */
        transient ComponentOrientation orientation = ComponentOrientation.UNKNOWN;

        /**
            * The associated graphics configuration.
         *
         * @since 1.4
         */
        transient GraphicsConfiguration graphicsConfig;

        /**
            * The buffer strategy for repainting.
         *
         * @since 1.4
         */
        transient BufferStrategy bufferStrategy;

        /**
            * true if requestFocus was called on this component when its
         * top-level ancestor was not focusable.
         */
        private transient FocusEvent pendingFocusRequest = null;


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
// 	bgColor = Component.this.background;
// 	changeSupport = null;
// 	componentSerializedDataVersion = 0;
// 	cursor = Component.this.cursor;
// 	enabled = isEnabled();
// 	eventMask = Component.this.eventMask;
// 	font = Component.this.font;
// 	foreground = Component.this.foreground;
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
	componentListener = AWTEventMulticaster.add( componentListener, newListener);
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
	mouseMotionListener = AWTEventMulticaster.add( mouseMotionListener, newListener);
	
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
                    if (parent.isLightweight())
                        new HeavyweightInLightweightListener(parent);
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
	      (mouseMotionListener != null) ||
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

/**
* Implementation of dispatchEvent. Allows trusted package classes
 * to dispatch additional events first.  This implementation first
 * translates <code>e</code> to an AWT 1.0 event and sends the
 * result to {@link #postEvent}.  If the AWT 1.0 event is not
 * handled, and events of type <code>e</code> are enabled for this
 * component, e is passed on to {@link #processEvent}.
 *
 * @param e the event to dispatch
 */

void dispatchEventImpl(AWTEvent e)
{
    // This boolean tells us not to process focus events when the focus
    // opposite component is the same as the focus component.
    boolean ignoreFocus =
    (e instanceof FocusEvent &&
     ((FocusEvent)e).getComponent() == ((FocusEvent)e).getOppositeComponent());

    if (eventTypeEnabled (e.id))
    {
        if (e.id != PaintEvent.PAINT && e.id != PaintEvent.UPDATE
            && !ignoreFocus)
            processEvent(e);

        // the trick we use to communicate between dispatch and redispatch
        // is to have KeyboardFocusManager.redispatch synchronize on the
        // object itself. we then do not redispatch to KeyboardFocusManager
        // if we are already holding the lock.
        if (! Thread.holdsLock(e))
        {
            switch (e.id)
            {
                case WindowEvent.WINDOW_GAINED_FOCUS:
                case WindowEvent.WINDOW_LOST_FOCUS:
                case KeyEvent.KEY_PRESSED:
                case KeyEvent.KEY_RELEASED:
                case KeyEvent.KEY_TYPED:
                case FocusEvent.FOCUS_GAINED:
                case FocusEvent.FOCUS_LOST:
                    if (KeyboardFocusManager
                        .getCurrentKeyboardFocusManager()
                        .dispatchEvent(e))
                        return;
                case MouseEvent.MOUSE_PRESSED:
                    if (isLightweight() && !e.isConsumed())
                        requestFocus();
                    break;
            }
        }
    }

    // here we differ from classpath since we have no peers
    e.dispatch();
}


/**
* Tells whether or not an event type is enabled.
 */
boolean eventTypeEnabled (int type)
{
    if (type > AWTEvent.RESERVED_ID_MAX)
        return true;

    switch (type)
    {
        case HierarchyEvent.HIERARCHY_CHANGED:
            return (hierarchyListener != null
                    || (eventMask & AWTEvent.HIERARCHY_EVENT_MASK) != 0);

        case HierarchyEvent.ANCESTOR_MOVED:
        case HierarchyEvent.ANCESTOR_RESIZED:
            return (hierarchyBoundsListener != null
                    || (eventMask & AWTEvent.HIERARCHY_BOUNDS_EVENT_MASK) != 0);

        case ComponentEvent.COMPONENT_HIDDEN:
        case ComponentEvent.COMPONENT_MOVED:
        case ComponentEvent.COMPONENT_RESIZED:
        case ComponentEvent.COMPONENT_SHOWN:
            return (componentListener != null
                    || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0);

        case KeyEvent.KEY_PRESSED:
        case KeyEvent.KEY_RELEASED:
        case KeyEvent.KEY_TYPED:
            return (keyListener != null
                    || (eventMask & AWTEvent.KEY_EVENT_MASK) != 0);

        case MouseEvent.MOUSE_CLICKED:
        case MouseEvent.MOUSE_ENTERED:
        case MouseEvent.MOUSE_EXITED:
        case MouseEvent.MOUSE_PRESSED:
        case MouseEvent.MOUSE_RELEASED:
            return (mouseListener != null
                    || (eventMask & AWTEvent.MOUSE_EVENT_MASK) != 0);
        case MouseEvent.MOUSE_MOVED:
        case MouseEvent.MOUSE_DRAGGED:
            return (mouseMotionListener != null
                    || (eventMask & AWTEvent.MOUSE_MOTION_EVENT_MASK) != 0);
        case MouseEvent.MOUSE_WHEEL:
            return (mouseWheelListener != null
                    || (eventMask & AWTEvent.MOUSE_WHEEL_EVENT_MASK) != 0);

        case FocusEvent.FOCUS_GAINED:
        case FocusEvent.FOCUS_LOST:
            return (focusListener != null
                    || (eventMask & AWTEvent.FOCUS_EVENT_MASK) != 0);

        case InputMethodEvent.INPUT_METHOD_TEXT_CHANGED:
        case InputMethodEvent.CARET_POSITION_CHANGED:
            return (inputMethodListener != null
                    || (eventMask & AWTEvent.INPUT_METHOD_EVENT_MASK) != 0);

        case PaintEvent.PAINT:
        case PaintEvent.UPDATE:
            return (eventMask & AWTEvent.PAINT_EVENT_MASK) != 0;

        default:
            return false;
    }
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
	return background;
/*
	if ( background != null )
		return background;

	for ( Component c=parent; c != null; c = c.parent ) {
		if ( c.background != null ) return c.bgClr;
	}
	
	// even though not in the specs, some apps (e.g. swing) rely on the
	// JDK behavior of returning 'null' if there isn't a parent yet
	return null;
	//return Color.white;
*/
}

public Rectangle getBounds () {
	return bounds();
}

// TODO this is a tentative implementation
public Rectangle getBounds (Rectangle rv) {
	if (rv == null)
		return bounds();	
	rv.x = x;
	rv.y = y;
	rv.width= width;
	rv.height = height;
	return rv;
}


ClassProperties getClassProperties () {
	// direct Component / Container derived classes can't use old events
	// (they had no protected ctor in 1.0.2)
	return ClassAnalyzer.analyzeProcessEvent( getClass(), false);
}

  /**
   * Report a change in a bound property to any registered property listeners.
   *
   * @param propertyName the property that changed
   * @param oldValue the old property value
   * @param newValue the new property value
   */
  protected void firePropertyChange(String propertyName, Object oldValue,
                                    Object newValue)
  {
    if (changeSupport != null)
      changeSupport.firePropertyChange(propertyName, oldValue, newValue);
  }

  /**
   * Report a change in a bound property to any registered property listeners.
   *
   * @param propertyName the property that changed
   * @param oldValue the old property value
   * @param newValue the new property value
   */
  protected void firePropertyChange(String propertyName, boolean oldValue,
                                    boolean newValue)
  {
    if (changeSupport != null)
      changeSupport.firePropertyChange(propertyName, oldValue, newValue);
  }

  /**
   * Report a change in a bound property to any registered property listeners.
   *
   * @param propertyName the property that changed
   * @param oldValue the old property value
   * @param newValue the new property value
   */
  protected void firePropertyChange(String propertyName, int oldValue,
                                    int newValue)
  {
    if (changeSupport != null)
      changeSupport.firePropertyChange(propertyName, oldValue, newValue);
  }

  /**
   * Report a change in a bound property to any registered property listeners.
   *
   * @param propertyName the property that changed
   * @param oldValue the old property value
   * @param newValue the new property value
   *
   * @since 1.5
   */
  public void firePropertyChange(String propertyName, byte oldValue,
                                    byte newValue)
  {
    if (changeSupport != null)
      changeSupport.firePropertyChange(propertyName, new Byte(oldValue),
                                       new Byte(newValue));
  }

  /**
   * Report a change in a bound property to any registered property listeners.
   *
   * @param propertyName the property that changed
   * @param oldValue the old property value
   * @param newValue the new property value
   *
   * @since 1.5
   */
  public void firePropertyChange(String propertyName, char oldValue,
                                    char newValue)
  {
    if (changeSupport != null)
      changeSupport.firePropertyChange(propertyName, new Character(oldValue),
                                       new Character(newValue));
  }

  /**
   * Report a change in a bound property to any registered property listeners.
   *
   * @param propertyName the property that changed
   * @param oldValue the old property value
   * @param newValue the new property value
   *
   * @since 1.5
   */
  public void firePropertyChange(String propertyName, short oldValue,
                                    short newValue)
  {
    if (changeSupport != null)
      changeSupport.firePropertyChange(propertyName, new Short(oldValue),
                                       new Short(newValue));
  }

  /**
   * Report a change in a bound property to any registered property listeners.
   *
   * @param propertyName the property that changed
   * @param oldValue the old property value
   * @param newValue the new property value
   *
   * @since 1.5
   */
  public void firePropertyChange(String propertyName, long oldValue,
                                    long newValue)
  {
    if (changeSupport != null)
      changeSupport.firePropertyChange(propertyName, new Long(oldValue),
                                       new Long(newValue));
  }

  /**
   * Report a change in a bound property to any registered property listeners.
   *
   * @param propertyName the property that changed
   * @param oldValue the old property value
   * @param newValue the new property value
   *
   * @since 1.5
   */
  public void firePropertyChange(String propertyName, float oldValue,
                                    float newValue)
  {
    if (changeSupport != null)
      changeSupport.firePropertyChange(propertyName, new Float(oldValue),
                                       new Float(newValue));
  }


  /**
   * Report a change in a bound property to any registered property listeners.
   *
   * @param propertyName the property that changed
   * @param oldValue the old property value
   * @param newValue the new property value
   *
   * @since 1.5
   */
  public void firePropertyChange(String propertyName, double oldValue,
                                 double newValue)
  {
    if (changeSupport != null)
      changeSupport.firePropertyChange(propertyName, new Double(oldValue),
                                       new Double(newValue));
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
	return foreground;
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

  /**
   * Tests if this component is opaque. All "heavyweight" (natively-drawn)
   * components are opaque. A component is opaque if it draws all pixels in
   * the bounds; a lightweight component is partially transparent if it lets
   * pixels underneath show through. Subclasses that guarantee that all pixels
   * will be drawn should override this.
   *
   * @return true if this is opaque
   * @see #isLightweight()
   * @since 1.2
   */
  public boolean isOpaque()
  {
    return ! isLightweight();
  }

  /**
   * Return whether the component is lightweight. That means the component has
   * no native peer, but is displayable. This applies to subclasses of
   * Component not in this package, such as javax.swing.
   *
   * @return true if the component has a lightweight peer
   * @see #isDisplayable()
   * @since 1.2
   */
  public boolean isLightweight()
  {
    return peer instanceof LightweightPeer;
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

/**
 * this is a stub only for now
 */
public void setDropTarget(DropTarget dt) {
// TODO this is only a stub	
}

/**
 * this is a stub only for now
 */
public DropTarget getDropTarget() {
// TODO this is only a stub
	return null;
}

/**
* Returns the toolkit in use for this component. The toolkit is associated
 * with the frame this component belongs to.
 *
 * @return the toolkit for this component
 */
public Toolkit getToolkit()
{
    if (peer != null)
    {
        Toolkit tk = peer.getToolkit();
        if (tk != null)
            return tk;
    }
    // Get toolkit for lightweight component.
    if (parent != null)
        return parent.getToolkit();
    return Toolkit.getDefaultToolkit();
}

Window getToplevel () {
	Component c;
	for ( c=this; !(c instanceof Window) && c != null; c= c.parent );
	return (Window)c;
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
		
		if ( (componentListener != null) || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0 ){
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

/**
* Tests if this component can receive focus.
 *
 * @return true if this component can receive focus
 * @since 1.4
 */
public boolean isFocusable()
{
    return focusable;
}

/**
* Specify whether this component can receive focus. This method also
 * sets the {@link #isFocusTraversableOverridden} field to 1, which
 * appears to be the undocumented way {@link
     * DefaultFocusTraversalPolicy#accept(Component)} determines whether to
 * respect the {@link #isFocusable()} method of the component.
 *
 * @param focusable the new focusable status
 * @since 1.4
 */
public void setFocusable(boolean focusable)
{
    firePropertyChange("focusable", this.focusable, focusable);
    this.focusable = focusable;
    this.isFocusTraversableOverridden = 1;
}

  /**
   * Sets the focus traversal keys for one of the three focus
   * traversal directions supported by Components:
   * {@link KeyboardFocusManager#FORWARD_TRAVERSAL_KEYS},
   * {@link KeyboardFocusManager#BACKWARD_TRAVERSAL_KEYS}, or
   * {@link KeyboardFocusManager#UP_CYCLE_TRAVERSAL_KEYS}. Normally, the
   * default values should match the operating system's native
   * choices. To disable a given traversal, use
   * <code>Collections.EMPTY_SET</code>. The event dispatcher will
   * consume PRESSED, RELEASED, and TYPED events for the specified
   * key, although focus can only transfer on PRESSED or RELEASED.
   *
   * <p>The defaults are:
   * <table>
   *   <th><td>Identifier</td><td>Meaning</td><td>Default</td></th>
   *   <tr><td>KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS</td>
   *     <td>Normal forward traversal</td>
   *     <td>TAB on KEY_PRESSED, Ctrl-TAB on KEY_PRESSED</td></tr>
   *   <tr><td>KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS</td>
   *     <td>Normal backward traversal</td>
   *     <td>Shift-TAB on KEY_PRESSED, Ctrl-Shift-TAB on KEY_PRESSED</td></tr>
   *   <tr><td>KeyboardFocusManager.UP_CYCLE_TRAVERSAL_KEYS</td>
   *     <td>Go up a traversal cycle</td><td>None</td></tr>
   * </table>
   *
   * If keystrokes is null, this component's focus traversal key set
   * is inherited from one of its ancestors.  If none of its ancestors
   * has its own set of focus traversal keys, the focus traversal keys
   * are set to the defaults retrieved from the current
   * KeyboardFocusManager.  If not null, the set must contain only
   * AWTKeyStrokes that are not already focus keys and are not
   * KEY_TYPED events.
   *
   * @param id one of FORWARD_TRAVERSAL_KEYS, BACKWARD_TRAVERSAL_KEYS, or
   *        UP_CYCLE_TRAVERSAL_KEYS
   * @param keystrokes a set of keys, or null
   * @throws IllegalArgumentException if id or keystrokes is invalid
   * @see #getFocusTraversalKeys(int)
   * @see KeyboardFocusManager#FORWARD_TRAVERSAL_KEYS
   * @see KeyboardFocusManager#BACKWARD_TRAVERSAL_KEYS
   * @see KeyboardFocusManager#UP_CYCLE_TRAVERSAL_KEYS
   * @since 1.4
   */
  public void setFocusTraversalKeys(int id, Set keystrokes)
  {
    if (keystrokes == null)
      {
        Container parent = getParent ();

        while (parent != null)
          {
            if (parent.areFocusTraversalKeysSet (id))
              {
                keystrokes = parent.getFocusTraversalKeys (id);
                break;
              }
            parent = parent.getParent ();
          }

        if (keystrokes == null)
          keystrokes = KeyboardFocusManager.getCurrentKeyboardFocusManager ().
            getDefaultFocusTraversalKeys (id);
      }

    Set sa;
    Set sb;
    String name;
    switch (id)
      {
      case KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS:
        sa = getFocusTraversalKeys
          (KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS);
        sb = getFocusTraversalKeys
          (KeyboardFocusManager.UP_CYCLE_TRAVERSAL_KEYS);
        name = "forwardFocusTraversalKeys";
        break;
      case KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS:
        sa = getFocusTraversalKeys
          (KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS);
        sb = getFocusTraversalKeys
          (KeyboardFocusManager.UP_CYCLE_TRAVERSAL_KEYS);
        name = "backwardFocusTraversalKeys";
        break;
      case KeyboardFocusManager.UP_CYCLE_TRAVERSAL_KEYS:
        sa = getFocusTraversalKeys
          (KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS);
        sb = getFocusTraversalKeys
          (KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS);
        name = "upCycleFocusTraversalKeys";
        break;
      default:
        throw new IllegalArgumentException ();
      }

    int i = keystrokes.size ();
    Iterator iter = keystrokes.iterator ();

    while (--i >= 0)
      {
        Object o = iter.next ();
        if (!(o instanceof AWTKeyStroke)
            || sa.contains (o) || sb.contains (o)
            || ((AWTKeyStroke) o).keyCode == KeyEvent.VK_UNDEFINED)
          throw new IllegalArgumentException ();
      }

    if (focusTraversalKeys == null)
      focusTraversalKeys = new Set[3];

    keystrokes = Collections.unmodifiableSet (new HashSet (keystrokes));
    firePropertyChange (name, focusTraversalKeys[id], keystrokes);

    focusTraversalKeys[id] = keystrokes;
  }

  /**
   * Returns the set of keys for a given focus traversal action, as
   * defined in <code>setFocusTraversalKeys</code>.  If not set, this
   * is inherited from the parent component, which may have gotten it
   * from the KeyboardFocusManager.
   *
   * @param id one of FORWARD_TRAVERSAL_KEYS, BACKWARD_TRAVERSAL_KEYS,
   * or UP_CYCLE_TRAVERSAL_KEYS
   *
   * @return set of traversal keys
   *
   * @throws IllegalArgumentException if id is invalid
   * 
   * @see #setFocusTraversalKeys (int, Set)
   * @see KeyboardFocusManager#FORWARD_TRAVERSAL_KEYS
   * @see KeyboardFocusManager#BACKWARD_TRAVERSAL_KEYS
   * @see KeyboardFocusManager#UP_CYCLE_TRAVERSAL_KEYS
   * 
   * @since 1.4
   */
  public Set getFocusTraversalKeys (int id)
  {
    if (id != KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS &&
        id != KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS &&
        id != KeyboardFocusManager.UP_CYCLE_TRAVERSAL_KEYS)
      throw new IllegalArgumentException();

    Set s = null;

    if (focusTraversalKeys != null)
      s = focusTraversalKeys[id];

    if (s == null && parent != null)
      s = parent.getFocusTraversalKeys (id);

    return s == null ? (KeyboardFocusManager.getCurrentKeyboardFocusManager()
                        .getDefaultFocusTraversalKeys(id)) : s;
  }

  /**
   * Tests whether the focus traversal keys for a given action are explicitly
   * set or inherited.
   *
   * @param id one of FORWARD_TRAVERSAL_KEYS, BACKWARD_TRAVERSAL_KEYS,
   * or UP_CYCLE_TRAVERSAL_KEYS
   * @return true if that set is explicitly specified
   * @throws IllegalArgumentException if id is invalid
   * @see #getFocusTraversalKeys (int)
   * @see KeyboardFocusManager#FORWARD_TRAVERSAL_KEYS
   * @see KeyboardFocusManager#BACKWARD_TRAVERSAL_KEYS
   * @see KeyboardFocusManager#UP_CYCLE_TRAVERSAL_KEYS
   * @since 1.4
   */
  public boolean areFocusTraversalKeysSet (int id)
  {
    if (id != KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS &&
        id != KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS &&
        id != KeyboardFocusManager.UP_CYCLE_TRAVERSAL_KEYS)
      throw new IllegalArgumentException ();

    return focusTraversalKeys != null && focusTraversalKeys[id] != null;
  }

  /**
   * Enable or disable focus traversal keys on this Component.  If
   * they are, then the keyboard focus manager consumes and acts on
   * key press and release events that trigger focus traversal, and
   * discards the corresponding key typed events.  If focus traversal
   * keys are disabled, then all key events that would otherwise
   * trigger focus traversal are sent to this Component.
   *
   * @param focusTraversalKeysEnabled the new value of the flag
   * @see #getFocusTraversalKeysEnabled ()
   * @see #setFocusTraversalKeys (int, Set)
   * @see #getFocusTraversalKeys (int)
   * @since 1.4
   */
  public void setFocusTraversalKeysEnabled (boolean focusTraversalKeysEnabled)
  {
    firePropertyChange ("focusTraversalKeysEnabled",
			this.focusTraversalKeysEnabled,
			focusTraversalKeysEnabled);
    this.focusTraversalKeysEnabled = focusTraversalKeysEnabled;
  }

  /**
   * Check whether or not focus traversal keys are enabled on this
   * Component.  If they are, then the keyboard focus manager consumes
   * and acts on key press and release events that trigger focus
   * traversal, and discards the corresponding key typed events.  If
   * focus traversal keys are disabled, then all key events that would
   * otherwise trigger focus traversal are sent to this Component.
   *
   * @return true if focus traversal keys are enabled
   * @see #setFocusTraversalKeysEnabled (boolean)
   * @see #setFocusTraversalKeys (int, Set)
   * @see #getFocusTraversalKeys (int)
   * @since 1.4
   */
  public boolean getFocusTraversalKeysEnabled ()
  {
    return focusTraversalKeysEnabled;
  }


/**
* Tests if this component is the focus owner. Use {@link
    * #isFocusOwner ()} instead.
 *
 * @return true if this component owns focus
 * @since 1.2
 */
public boolean hasFocus ()
{
    KeyboardFocusManager manager = KeyboardFocusManager.getCurrentKeyboardFocusManager ();

    Component focusOwner = manager.getFocusOwner ();

    return this == focusOwner;
}

/**
* Tests if this component is the focus owner.
 *
 * @return true if this component owns focus
 * @since 1.4
 */
public boolean isFocusOwner()
{
    return hasFocus ();
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
* Tests if the component is displayable. It must be connected to a native
 * screen resource.  This reduces to checking that peer is not null.  A
 * containment  hierarchy is made displayable when a window is packed or
 * made visible.
 *
 * @return true if the component is displayable
 * @see Container#add(Component)
 * @see Container#remove(Component)
 * @see Window#pack()
 * @see Window#show()
 * @see Window#dispose()
 * @since 1.2
 */
public boolean isDisplayable()
{
    // since we are peerless...
    return true;
//    return peer != null;
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

/**
* Paints this component on the screen. The clipping region in the graphics
 * context will indicate the region that requires painting. This is called
 * whenever the component first shows, or needs to be repaired because
 * something was temporarily drawn on top. It is not necessary for
 * subclasses to call <code>super.paint(g)</code>. Components with no area
 * are not painted.
 *
 * @param g the graphics context for this paint job
 * @see #update(Graphics)
 */
public void paint(Graphics g)
{
    // This is a callback method and is meant to be overridden by subclasses
    // that want to perform custom painting.
}

/**
* Paints this entire component, including any sub-components.
 *
 * @param g the graphics context for this paint job
 *
 * @see #paint(Graphics)
 */
public void paintAll(Graphics g)
{
    if ((flags & IS_VISIBLE) == 0)
        return;
    paint(g);
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
	if ( (componentListener != null) || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0)
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

	if ( componentListener != null ){
		switch ( event.getID() ) {
		case ComponentEvent.COMPONENT_RESIZED:
			componentListener.componentResized( event);
			break;
		case ComponentEvent.COMPONENT_MOVED:
			componentListener.componentMoved( event);
			break;
		case ComponentEvent.COMPONENT_SHOWN:
			componentListener.componentShown( event);
			break;
		case ComponentEvent.COMPONENT_HIDDEN:
			componentListener.componentHidden( event);
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

	if ( (mouseMotionListener != null) || (eventMask & AWTEvent.MOUSE_MOTION_EVENT_MASK) != 0)
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
	if ( mouseMotionListener != null ) {
		switch ( event.id ) {
		case MouseEvent.MOUSE_MOVED:
			mouseMotionListener.mouseMoved( event);
			return;
		case MouseEvent.MOUSE_DRAGGED:
			mouseMotionListener.mouseDragged( event);
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
	background = clr;
}

void propagateFgClr ( Color clr ) {
	// should be called *only* on Components with unset IS_FG_COLORED
	foreground = clr;
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
	componentListener = AWTEventMulticaster.remove( componentListener, client);
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
	mouseMotionListener = AWTEventMulticaster.remove( mouseMotionListener, listener);
	
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

/**
* Request that this Component be given the keyboard input focus and
 * that its top-level ancestor become the focused Window.
 *
 * For the request to be granted, the Component must be focusable,
 * displayable and showing and the top-level Window to which it
 * belongs must be focusable.  If the request is initially denied on
 * the basis that the top-level Window is not focusable, the request
 * will be remembered and granted when the Window does become
 * focused.
 *
 * Never assume that this Component is the focus owner until it
 * receives a FOCUS_GAINED event.
 *
 * The behaviour of this method is platform-dependent.
 * {@link #requestFocusInWindow()} should be used instead.
 *
 * @see #requestFocusInWindow ()
 * @see FocusEvent
 * @see #addFocusListener (FocusListener)
 * @see #isFocusable ()
 * @see #isDisplayable ()
 * @see KeyboardFocusManager#clearGlobalFocusOwner ()
 */
public void requestFocus ()
{
    if (isDisplayable ()
        && isShowing ()
        && isFocusable ())
    {
        synchronized (getTreeLock ())
    {
            // Find this Component's top-level ancestor.
            Container parent = (this instanceof Container) ? (Container) this
            : getParent();
            while (parent != null
                   && !(parent instanceof Window))
                parent = parent.getParent ();

            if (parent == null)
                return;

            Window toplevel = (Window) parent;
// we can't check for that or  choice windows won't get focus
//            if (toplevel.isFocusableWindow ())
//            {
                //if (peer != null && !isLightweight()) { // we don't have a peer
                if (!isLightweight()) {
                    // This call will cause a FOCUS_GAINED event to be
                    // posted to the system event queue if the native
                    // windowing system grants the focus request.
                    //peer.requestFocus ();
                    if (toplevel != AWTEvent.activeWindow ) {  // this involves a change of active toplevels
                        FocusEvt.keyTgtRequest = this;
                        toplevel.requestFocus();
                    }
                    else {                                 // intra toplevel focus change
                        Toolkit.eventQueue.postFocusEvent( FocusEvt.getEvent( this, FocusEvent.FOCUS_GAINED, false));
                    }
                } else
                {
                    // Either our peer hasn't been created yet or we're a
                    // lightweight component.  In either case we want to
                    // post a FOCUS_GAINED event.
                    EventQueue eq = Toolkit.getDefaultToolkit ().getSystemEventQueue ();
                    synchronized (eq)
                    {
                        KeyboardFocusManager manager = KeyboardFocusManager.getCurrentKeyboardFocusManager ();
                        Component currentFocusOwner = manager.getGlobalPermanentFocusOwner ();
                        if (currentFocusOwner != null)
                        {
                            eq.postEvent (new FocusEvent(currentFocusOwner, FocusEvent.FOCUS_LOST,
                                                         false, this));
                            eq.postEvent (new FocusEvent(this, FocusEvent.FOCUS_GAINED, false,
                                                         currentFocusOwner));
                        }
                        else
                            eq.postEvent (new FocusEvent(this, FocusEvent.FOCUS_GAINED, false));
                    }
                }
            //}
            //else
            //    pendingFocusRequest = new FocusEvent(this, FocusEvent.FOCUS_GAINED);
    }
    }
}

/**
* Request that this Component be given the keyboard input focus and
 * that its top-level ancestor become the focused Window.
 *
 * For the request to be granted, the Component must be focusable,
 * displayable and showing and the top-level Window to which it
 * belongs must be focusable.  If the request is initially denied on
 * the basis that the top-level Window is not focusable, the request
 * will be remembered and granted when the Window does become
 * focused.
 *
 * Never assume that this Component is the focus owner until it
 * receives a FOCUS_GAINED event.
 *
 * The behaviour of this method is platform-dependent.
 * {@link #requestFocusInWindow()} should be used instead.
 *
 * If the return value is false, the request is guaranteed to fail.
 * If the return value is true, the request will succeed unless it
 * is vetoed or something in the native windowing system intervenes,
 * preventing this Component's top-level ancestor from becoming
 * focused.  This method is meant to be called by derived
 * lightweight Components that want to avoid unnecessary repainting
 * when they know a given focus transfer need only be temporary.
 *
 * @param temporary true if the focus request is temporary
 * @return true if the request has a chance of success
 * @see #requestFocusInWindow ()
 * @see FocusEvent
 * @see #addFocusListener (FocusListener)
 * @see #isFocusable ()
 * @see #isDisplayable ()
 * @see KeyboardFocusManager#clearGlobalFocusOwner ()
 * @since 1.4
 */
protected boolean requestFocus (boolean temporary)
{
    if (isDisplayable ()
        && isShowing ()
        && isFocusable ())
    {
        synchronized (getTreeLock ())
    {
            // Find this Component's top-level ancestor.
            Container parent = getParent ();

            while (parent != null
                   && !(parent instanceof Window))
                parent = parent.getParent ();

            Window toplevel = (Window) parent;
            if (toplevel.isFocusableWindow ())
            {
                if (peer != null && !isLightweight())
                    // This call will cause a FOCUS_GAINED event to be
                    // posted to the system event queue if the native
                    // windowing system grants the focus request.
                    peer.requestFocus ();
                else
                {
                    // Either our peer hasn't been created yet or we're a
                    // lightweight component.  In either case we want to
                    // post a FOCUS_GAINED event.
                    EventQueue eq = Toolkit.getDefaultToolkit ().getSystemEventQueue ();
                    synchronized (eq)
                    {
                        KeyboardFocusManager manager = KeyboardFocusManager.getCurrentKeyboardFocusManager ();
                        Component currentFocusOwner = manager.getGlobalPermanentFocusOwner ();
                        if (currentFocusOwner != null)
                        {
                            eq.postEvent (new FocusEvent(currentFocusOwner,
                                                         FocusEvent.FOCUS_LOST,
                                                         temporary, this));
                            eq.postEvent (new FocusEvent(this,
                                                         FocusEvent.FOCUS_GAINED,
                                                         temporary,
                                                         currentFocusOwner));
                        }
                        else
                            eq.postEvent (new FocusEvent(this, FocusEvent.FOCUS_GAINED, temporary));
                    }
                }
            }
            else
                // FIXME: need to add a focus listener to our top-level
                // ancestor, so that we can post this event when it becomes
                // the focused window.
                pendingFocusRequest = new FocusEvent(this, FocusEvent.FOCUS_GAINED, temporary);
    }
    }
    // Always return true.
    return true;
}


/**
 * @deprecated, use setBounds(x,y,w,h)
 * this is never called automatically, override setBounds in derived classes
 * to change the default behavior
 */
public void reshape ( int xNew, int yNew, int wNew, int hNew ) {
	// DEP - this should be in setBounds !! But we have to keep it here
	// for compatibility reasons (Swing etc.)

    int oldx = this.x;
    int oldy = this.y;
    int oldwidth = this.width;
    int oldheight = this.height;
	int      x0=0, x1=0, y0=0, y1=0, a, b;
	boolean  sized = ( (width != wNew) || (height != hNew) );
	boolean  moved = ( !sized && ((x != xNew) || (y != yNew)) );
	int      id = sized ? ComponentEvent.COMPONENT_RESIZED : ComponentEvent.COMPONENT_MOVED;

	// Don't do anything if we don't change anything.
	if (sized || moved) {

            // Erase old bounds and repaint new bounds for lightweights.
            if (isLightweight() && isShowing())
            {
                if (parent != null)
                {
                    Rectangle oldBounds = new Rectangle(oldx, oldy, oldwidth,
                                                        oldheight);
                    Rectangle newBounds = new Rectangle(x, y, width, height);
                    Rectangle destroyed = oldBounds.union(newBounds);
                    if (!destroyed.isEmpty())
                        parent.repaint(0, destroyed.x, destroyed.y, destroyed.width,
                                       destroyed.height);
                }
            }

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
				
				if ( (componentListener != null) || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0 ){
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

		if ( (componentListener != null) || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0 ){
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

public void setBackground ( Color c ) {
	if ( c == background )
		return;

	if ( c != null ){
		flags |= IS_BG_COLORED;
	}
	else {
		flags &= ~IS_BG_COLORED;
		if ( parent != null )
			c = parent.background;
	}

	propagateBgClr( c);

	// we follow the "Java class libraries" description here (in favor of the Sun class docu), i.e.
	// clients have to explicitly force a repaint after changing colors. But - since many apps
	// rely in this unspec. behavior - we have to repaint automatically for native-likes
	if ( (flags & (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED | IS_NATIVE_LIKE))
	        == (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED) ) {
		repaint();
	}
    Color previous = background;
    background = c;
    firePropertyChange("background", previous, c);
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

  /**
   * Sets the font for this component to the specified font. This is a bound
   * property.
   *
   * @param newFont the new font for this component
   * 
   * @see #getFont()
   */
public void setFont ( Font newFont ) {
	if ( newFont == font )
		return;

	if ( newFont != null ){
		flags |= IS_FONTIFIED;
	}
	else {
		flags &= ~IS_FONTIFIED;
		if ( parent != null )
			newFont = parent.font;
	}

    	Font oldFont = font;
        font = newFont;
	propagateFont( font);

    	firePropertyChange("font", oldFont, newFont);
	// see setBackground for further details about why to repaint just visible native-likes
	if ( (flags & (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED | IS_NATIVE_LIKE))
	        == (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED) ) {
		repaint();
	}
}

  /**
   * Sets this component's foreground color to the specified color. This is a
   * bound property.
   *
   * @param c the new foreground color
   * @see #getForeground()
   */
public void setForeground ( Color c ) {
	if ( c == foreground )
		return;

	if ( c != null ){
		flags |= IS_FG_COLORED;
	}
	else {
		flags &= ~IS_FG_COLORED;
		if ( parent != null )
			c = parent.foreground;
	}

	propagateFgClr( c);

	// see setBackground for further details about why to repaint just visible native-likes
	if ( (flags & (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED | IS_NATIVE_LIKE))
	        == (IS_PARENT_SHOWING | IS_VISIBLE | IS_ADD_NOTIFIED) ) {
		repaint();
	}
    Color previous = foreground;
    foreground = c;
    firePropertyChange("foreground", previous, c);
}

  /**
   * Sets the locale for this component to the specified locale. This is a
   * bound property.
   *
   * @param newLocale the new locale for this component
   */
  public void setLocale(Locale newLocale)
  {
    if (locale == newLocale)
      return;

    Locale oldLocale = locale;
    locale = newLocale;
    firePropertyChange("locale", oldLocale, newLocale);
    // New writing/layout direction or more/less room for localized labels.
    invalidate();
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


/**
* Makes this component visible or invisible. Note that it wtill might
 * not show the component, if a parent is invisible.
 *
 * @param visible true to make this component visible
 *
 * @see #isVisible()
 *
 * @since 1.1
 */
public void setVisible(boolean visible)
{
    // Inspection by subclassing shows that Sun's implementation calls
    // show(boolean) which then calls show() or hide(). It is the show()
    // method that is overriden in subclasses like Window.
    show(visible);
}

/**
* Makes this component visible on the screen.
 *
 * @deprecated use {@link #setVisible(boolean)} instead
 */
public void show()
{
    // We must set visible before showing the peer.  Otherwise the
    // peer could post paint events before visible is true, in which
    // case lightweight components are not initially painted --
    // Container.paint first calls isShowing () before painting itself
    // and its children.

    // this is equivalent to isVisible()
    if ( (flags & IS_VISIBLE) == 0 ) {
        flags |= IS_VISIBLE;
        flags &= ~IS_TEMP_HIDDEN;

        // if we are a toplevel, the native window manager will take care
        // of repainting
        // Avoid NullPointerExceptions by creating a local reference.
        ComponentPeer currentPeer=peer;
        if (currentPeer != null)
            currentPeer.show();

        // The JDK repaints the component before invalidating the parent.
        // So do we.
        if (isShowing() && isLightweight())
            repaint();
        // Invalidate the parent if we have one. The component itself must
        // not be invalidated. We also avoid NullPointerException with
        // a local reference here.
        Container currentParent = parent;
        if (currentParent != null)
            currentParent.invalidate();

        if ( (componentListener != null) || (eventMask & AWTEvent.COMPONENT_EVENT_MASK) != 0 ){
            Toolkit.eventQueue.postEvent( ComponentEvt.getEvent( this, ComponentEvent.COMPONENT_SHOWN));
        }

        // update any resident graphics objects		
        if ( linkedGraphs != null )
            updateLinkedGraphics();
    }
}

/**
* Makes this component visible or invisible.
 *
 * @param visible true to make this component visible
 *
 * @deprecated use {@link #setVisible(boolean)} instead
 */
public void show(boolean visible)
{
    if (visible)
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

/**
* Updates this component. This is called in response to
 * <code>repaint</code>. This method fills the component with the
 * background color, then sets the foreground color of the specified
 * graphics context to the foreground color of this component and calls
 * the <code>paint()</code> method. The coordinates of the graphics are
 * relative to this component. Subclasses should call either
 * <code>super.update(g)</code> or <code>paint(g)</code>.
 *
 * @param g the graphics context for this update
 *
 * @see #paint(Graphics)
 * @see #repaint()
 *
 * @specnote In contrast to what the spec says, tests show that the exact
 *           behaviour is to clear the background on lightweight and
 *           top-level components only. Heavyweight components are not
 *           affected by this method and only call paint().
 */
public void update(Graphics g)
{
    // Tests show that the clearing of the background is only done in
    // two cases:
    // - If the component is lightweight (yes this is in contrast to the spec).
    // or
    // - If the component is a toplevel container.
    if (isLightweight() || getParent() == null)
    {
        Rectangle clip = g.getClipBounds();
        if (clip == null)
            g.clearRect(0, 0, width, height);
        else
            g.clearRect(clip.x, clip.y, clip.width, clip.height);
    }
    paint(g);
}

/**
* Sets the text layout orientation of this component. New components default
 * to UNKNOWN (which behaves like LEFT_TO_RIGHT). This method affects only
 * the current component, while
 * {@link #applyComponentOrientation(ComponentOrientation)} affects the
 * entire hierarchy.
 *
 * @param o the new orientation
 * @throws NullPointerException if o is null
 * @see #getComponentOrientation()
 */
public void setComponentOrientation(ComponentOrientation o)
{
    if (o == null)
        throw new NullPointerException();
    ComponentOrientation oldOrientation = orientation;
    orientation = o;
    firePropertyChange("componentOrientation", oldOrientation, o);
}

/**
* Determines the text layout orientation used by this component.
 *
 * @return the component orientation
 * @see #setComponentOrientation(ComponentOrientation)
 */
public ComponentOrientation getComponentOrientation()
{
    return orientation;
}

/**
* Sets the text layout orientation of this component. New components default
 * to UNKNOWN (which behaves like LEFT_TO_RIGHT). This method affects the
 * entire hierarchy, while
 * {@link #setComponentOrientation(ComponentOrientation)} affects only the
 * current component.
 *
 * @param o the new orientation
 * @throws NullPointerException if o is null
 * @see #getComponentOrientation()
 * @since 1.4
 */
public void applyComponentOrientation(ComponentOrientation o)
{
    setComponentOrientation(o);
}

/**
* Returns the accessibility framework context of this class. Component is
 * not accessible, so the default implementation returns null. Subclasses
 * must override this behavior, and return an appropriate subclass of
 * {@link AccessibleAWTComponent}.
 *
 * @return the accessibility context
 */
public AccessibleContext getAccessibleContext()
{
    return null;
}

/**
* Returns an array of all specified listeners registered on this component.
 *
 * @return an array of listeners
 * @see #addMouseMotionListener(MouseMotionListener)
 * @see #removeMouseMotionListener(MouseMotionListener)
 * @since 1.4
 */
public synchronized MouseMotionListener[] getMouseMotionListeners()
{
    return (MouseMotionListener[])
    AWTEventMulticaster.getListeners(mouseMotionListener,
                                     MouseMotionListener.class);
}

/**
* Adds the specified listener to this component. This is harmless if the
 * listener is null, but if the listener has already been registered, it
 * will now be registered twice.
 *
 * @param listener the new listener to add
 * @see MouseEvent
 * @see MouseWheelEvent
 * @see #removeMouseWheelListener(MouseWheelListener)
 * @see #getMouseWheelListeners()
 * @since 1.4
 */
public synchronized void addMouseWheelListener(MouseWheelListener listener)
{
    mouseWheelListener = AWTEventMulticaster.add(mouseWheelListener, listener);
    if (mouseWheelListener != null)
        enableEvents(AWTEvent.MOUSE_WHEEL_EVENT_MASK);
}

/**
* Removes the specified listener from the component. This is harmless if
 * the listener was not previously registered.
 *
 * @param listener the listener to remove
 * @see MouseEvent
 * @see MouseWheelEvent
 * @see #addMouseWheelListener(MouseWheelListener)
 * @see #getMouseWheelListeners()
 * @since 1.4
 */
public synchronized void removeMouseWheelListener(MouseWheelListener listener)
{
    mouseWheelListener = AWTEventMulticaster.remove(mouseWheelListener, listener);
}

/**
* Returns an array of all specified listeners registered on this component.
 *
 * @return an array of listeners
 * @see #addMouseWheelListener(MouseWheelListener)
 * @see #removeMouseWheelListener(MouseWheelListener)
 * @since 1.4
 */
public synchronized MouseWheelListener[] getMouseWheelListeners()
{
    return (MouseWheelListener[])
    AWTEventMulticaster.getListeners(mouseWheelListener,
                                     MouseWheelListener.class);
}

/**
* Adds the specified listener to this component. This is harmless if the
 * listener is null, but if the listener has already been registered, it
 * will now be registered twice.
 *
 * @param listener the new listener to add
 * @see InputMethodEvent
 * @see #removeInputMethodListener(InputMethodListener)
 * @see #getInputMethodListeners()
 * @see #getInputMethodRequests()
 * @since 1.2
 */
public synchronized void addInputMethodListener(InputMethodListener listener)
{
    inputMethodListener = AWTEventMulticaster.add(inputMethodListener, listener);
    if (inputMethodListener != null)
        enableEvents(AWTEvent.INPUT_METHOD_EVENT_MASK);
}

/**
* Removes the specified listener from the component. This is harmless if
 * the listener was not previously registered.
 *
 * @param listener the listener to remove
 * @see InputMethodEvent
 * @see #addInputMethodListener(InputMethodListener)
 * @see #getInputMethodRequests()
 * @since 1.2
 */
public synchronized void removeInputMethodListener(InputMethodListener listener)
{
    inputMethodListener = AWTEventMulticaster.remove(inputMethodListener, listener);
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

  /**
   * Adds the specified property listener to this component. This is harmless
   * if the listener is null, but if the listener has already been registered,
   * it will now be registered twice. The property listener ignores inherited
   * properties. Recognized properties include:<br>
   * <ul>
   * <li>the font (<code>"font"</code>)</li>
   * <li>the background color (<code>"background"</code>)</li>
   * <li>the foreground color (<code>"foreground"</code>)</li>
   * <li>the focusability (<code>"focusable"</code>)</li>
   * <li>the focus key traversal enabled state
   *     (<code>"focusTraversalKeysEnabled"</code>)</li>
   * <li>the set of forward traversal keys
   *     (<code>"forwardFocusTraversalKeys"</code>)</li>
   * <li>the set of backward traversal keys
   *     (<code>"backwardFocusTraversalKeys"</code>)</li>
   * <li>the set of up-cycle traversal keys
   *     (<code>"upCycleFocusTraversalKeys"</code>)</li>
   * </ul>
   *
   * @param listener the new listener to add
   * @see #removePropertyChangeListener(PropertyChangeListener)
   * @see #getPropertyChangeListeners()
   * @see #addPropertyChangeListener(String, PropertyChangeListener)
   * @since 1.1
   */
  public void addPropertyChangeListener(PropertyChangeListener listener)
  {
    if (changeSupport == null)
      changeSupport = new PropertyChangeSupport(this);
    changeSupport.addPropertyChangeListener(listener);
  }

  /**
   * Removes the specified property listener from the component. This is
   * harmless if the listener was not previously registered.
   *
   * @param listener the listener to remove
   * @see #addPropertyChangeListener(PropertyChangeListener)
   * @see #getPropertyChangeListeners()
   * @see #removePropertyChangeListener(String, PropertyChangeListener)
   * @since 1.1
   */
  public void removePropertyChangeListener(PropertyChangeListener listener)
  {
    if (changeSupport != null)
      changeSupport.removePropertyChangeListener(listener);
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

// Nested classes.

/**
* This class fixes the bounds for a Heavyweight component that
 * is placed inside a Lightweight container. When the lightweight is
 * moved or resized, setBounds for the lightweight peer does nothing.
 * Therefore, it was never moved on the screen. This class is
 * attached to the lightweight, and it adjusts the position and size
 * of the peer when notified.
 * This is the same for show and hide.
 */
class HeavyweightInLightweightListener
implements ComponentListener
{

    /**
    * Constructor. Adds component listener to lightweight parent.
     *
     * @param parent - the lightweight container.
     */
    public HeavyweightInLightweightListener(Container parent)
    {
        parent.addComponentListener(this);
    }

    /**
    * This method is called when the component is resized.
     *
     * @param event the <code>ComponentEvent</code> indicating the resize
     */
    public void componentResized(ComponentEvent event)
    {
        // Nothing to do here, componentMoved will be called.
    }

    /**
    * This method is called when the component is moved.
     *
     * @param event the <code>ComponentEvent</code> indicating the move
     */
    public void componentMoved(ComponentEvent event)
    {
        if (peer != null)
            peer.setBounds(x, y, width, height);
    }

    /**
    * This method is called when the component is made visible.
     *
     * @param event the <code>ComponentEvent</code> indicating the visibility
     */
    public void componentShown(ComponentEvent event)
    {
        if (isShowing())
            peer.show();
    }

    /**
    * This method is called when the component is hidden.
     *
     * @param event the <code>ComponentEvent</code> indicating the visibility
     */
    public void componentHidden(ComponentEvent event)
    {
        if (!isShowing())
            peer.hide();
    }
}

/**
* This class provides accessibility support for subclasses of container.
 *
 * @author Eric Blake (ebb9@email.byu.edu)
 * @since 1.3
 * @status updated to 1.4
 */
protected abstract class AccessibleAWTComponent extends AccessibleContext
implements Serializable, AccessibleComponent
{
    /**
    * Compatible with JDK 1.3+.
     */
    private static final long serialVersionUID = 642321655757800191L;

    /**
    * Converts show/hide events to PropertyChange events, and is registered
     * as a component listener on this component.
     *
     * @serial the component handler
     */
    protected ComponentListener accessibleAWTComponentHandler
        = new AccessibleAWTComponentHandler();

    /**
        * Converts focus events to PropertyChange events, and is registered
     * as a focus listener on this component.
     *
     * @serial the focus handler
     */
    protected FocusListener accessibleAWTFocusHandler
        = new AccessibleAWTFocusHandler();

    /**
        * The default constructor.
     */
    protected AccessibleAWTComponent()
    {
        Component.this.addComponentListener(accessibleAWTComponentHandler);
        Component.this.addFocusListener(accessibleAWTFocusHandler);
    }

    /**
        * Adds a global property change listener to the accessible component.
     *
     * @param l the listener to add
     * @see #ACCESSIBLE_NAME_PROPERTY
     * @see #ACCESSIBLE_DESCRIPTION_PROPERTY
     * @see #ACCESSIBLE_STATE_PROPERTY
     * @see #ACCESSIBLE_VALUE_PROPERTY
     * @see #ACCESSIBLE_SELECTION_PROPERTY
     * @see #ACCESSIBLE_TEXT_PROPERTY
     * @see #ACCESSIBLE_VISIBLE_DATA_PROPERTY
     */
    public void addPropertyChangeListener(PropertyChangeListener l)
    {
        Component.this.addPropertyChangeListener(l);
        super.addPropertyChangeListener(l);
    }

    /**
        * Removes a global property change listener from this accessible
     * component.
     *
     * @param l the listener to remove
     */
    public void removePropertyChangeListener(PropertyChangeListener l)
    {
        Component.this.removePropertyChangeListener(l);
        super.removePropertyChangeListener(l);
    }

    /**
        * Returns the accessible name of this component. It is almost always
     * wrong to return getName(), since it is not localized. In fact, for
     * things like buttons, this should be the text of the button, not the
     * name of the object. The tooltip text might also be appropriate.
     *
     * @return the name
     * @see #setAccessibleName(String)
     */
    public String getAccessibleName()
    {
        return accessibleName;
    }

    /**
        * Returns a brief description of this accessible context. This should
     * be localized.
     *
     * @return a description of this component
     * @see #setAccessibleDescription(String)
     */
    public String getAccessibleDescription()
    {
        return accessibleDescription;
    }

    /**
        * Returns the role of this component.
     *
     * @return the accessible role
     */
    public AccessibleRole getAccessibleRole()
    {
        return AccessibleRole.AWT_COMPONENT;
    }

    /**
        * Returns a state set describing this component's state.
     *
     * @return a new state set
     * @see AccessibleState
     */
    public AccessibleStateSet getAccessibleStateSet()
    {
        AccessibleStateSet s = new AccessibleStateSet();
        if (Component.this.isEnabled())
            s.add(AccessibleState.ENABLED);
        if (isFocusable())
            s.add(AccessibleState.FOCUSABLE);
        if (isFocusOwner())
            s.add(AccessibleState.FOCUSED);
        // Note: While the java.awt.Component has an 'opaque' property, it
        // seems that it is not added to the accessible state set here, even
        // if this property is true. However, it is handled for
        // javax.swing.JComponent, so we add it there.
        if (Component.this.isShowing())
            s.add(AccessibleState.SHOWING);
        if (Component.this.isVisible())
            s.add(AccessibleState.VISIBLE);
        return s;
    }

    /**
        * Returns the parent of this component, if it is accessible.
     *
     * @return the accessible parent
     */
    public Accessible getAccessibleParent()
    {
        if (accessibleParent == null)
        {
            Container parent = getParent();
            accessibleParent = parent instanceof Accessible
                ? (Accessible) parent : null;
        }
        return accessibleParent;
    }

    /**
        * Returns the index of this component in its accessible parent.
     *
     * @return the index, or -1 if the parent is not accessible
     * @see #getAccessibleParent()
     */
    public int getAccessibleIndexInParent()
    {
        if (getAccessibleParent() == null)
            return -1;
        AccessibleContext context
            = ((Component) accessibleParent).getAccessibleContext();
        if (context == null)
            return -1;
        for (int i = context.getAccessibleChildrenCount(); --i >= 0; )
            if (context.getAccessibleChild(i) == Component.this)
                return i;
        return -1;
    }

    /**
        * Returns the number of children of this component which implement
     * Accessible. Subclasses must override this if they can have children.
     *
     * @return the number of accessible children, default 0
     */
    public int getAccessibleChildrenCount()
    {
        return 0;
    }

    /**
        * Returns the ith accessible child. Subclasses must override this if
     * they can have children.
     *
     * @return the ith accessible child, or null
     * @see #getAccessibleChildrenCount()
     */
    public Accessible getAccessibleChild(int i)
    {
        return null;
    }

    /**
        * Returns the locale of this component.
     *
     * @return the locale
     * @throws IllegalComponentStateException if the locale is unknown
     */
    public Locale getLocale()
    {
        return Component.this.getLocale();
    }

    /**
        * Returns this, since it is an accessible component.
     *
     * @return the accessible component
     */
    public AccessibleComponent getAccessibleComponent()
    {
        return this;
    }

    /**
        * Gets the background color.
     *
     * @return the background color
     * @see #setBackground(Color)
     */
    public Color getBackground()
    {
        return Component.this.getBackground();
    }

    /**
        * Sets the background color.
     *
     * @param c the background color
     * @see #getBackground()
     * @see #isOpaque()
     */
    public void setBackground(Color c)
    {
        Component.this.setBackground(c);
    }

    /**
        * Gets the foreground color.
     *
     * @return the foreground color
     * @see #setForeground(Color)
     */
    public Color getForeground()
    {
        return Component.this.getForeground();
    }

    /**
        * Sets the foreground color.
     *
     * @param c the foreground color
     * @see #getForeground()
     */
    public void setForeground(Color c)
    {
        Component.this.setForeground(c);
    }

    /**
        * Gets the cursor.
     *
     * @return the cursor
     * @see #setCursor(Cursor)
     */
    public Cursor getCursor()
    {
        return Component.this.getCursor();
    }

    /**
        * Sets the cursor.
     *
     * @param cursor the cursor
     * @see #getCursor()
     */
    public void setCursor(Cursor cursor)
    {
        Component.this.setCursor(cursor);
    }

    /**
        * Gets the font.
     *
     * @return the font
     * @see #setFont(Font)
     */
    public Font getFont()
    {
        return Component.this.getFont();
    }

    /**
        * Sets the font.
     *
     * @param f the font
     * @see #getFont()
     */
    public void setFont(Font f)
    {
        Component.this.setFont(f);
    }

    /**
        * Gets the font metrics for a font.
     *
     * @param f the font to look up
     * @return its metrics
     * @throws NullPointerException if f is null
     * @see #getFont()
     */
    public FontMetrics getFontMetrics(Font f)
    {
        return Component.this.getFontMetrics(f);
    }

    /**
        * Tests if the component is enabled.
     *
     * @return true if the component is enabled
     * @see #setEnabled(boolean)
     * @see #getAccessibleStateSet()
     * @see AccessibleState#ENABLED
     */
    public boolean isEnabled()
    {
        return Component.this.isEnabled();
    }

    /**
        * Set whether the component is enabled.
     *
     * @param b the new enabled status
     * @see #isEnabled()
     */
    public void setEnabled(boolean b)
    {
        Component.this.setEnabled(b);
    }

    /**
        * Test whether the component is visible (not necesarily showing).
     *
     * @return true if it is visible
     * @see #setVisible(boolean)
     * @see #getAccessibleStateSet()
     * @see AccessibleState#VISIBLE
     */
    public boolean isVisible()
    {
        return Component.this.isVisible();
    }

    /**
        * Sets the visibility of this component.
     *
     * @param b the desired visibility
     * @see #isVisible()
     */
    public void setVisible(boolean b)
    {
        Component.this.setVisible(b);
    }

    /**
        * Tests if the component is showing.
     *
     * @return true if this is showing
     */
    public boolean isShowing()
    {
        return Component.this.isShowing();
    }

    /**
        * Tests if the point is contained in this component.
     *
     * @param p the point to check
     * @return true if it is contained
     * @throws NullPointerException if p is null
     */
    public boolean contains(Point p)
    {
        return Component.this.contains(p.x, p.y);
    }

    /**
        * Returns the location of this object on the screen, or null if it is
     * not showing.
     *
     * @return the location relative to screen coordinates, if showing
     * @see #getBounds()
     * @see #getLocation()
     */
    public Point getLocationOnScreen()
    {
        return Component.this.isShowing() ? Component.this.getLocationOnScreen()
        : null;
    }

    /**
        * Returns the location of this object relative to its parent's coordinate
     * system, or null if it is not showing.
     *
     * @return the location
     * @see #getBounds()
     * @see #getLocationOnScreen()
     */
    public Point getLocation()
    {
        return Component.this.getLocation();
    }

    /**
        * Sets the location of this relative to its parent's coordinate system.
     *
     * @param p the location
     * @throws NullPointerException if p is null
     * @see #getLocation()
     */
    public void setLocation(Point p)
    {
        Component.this.setLocation(p.x, p.y);
    }

    /**
        * Gets the bounds of this component, or null if it is not on screen.
     *
     * @return the bounds
     * @see #contains(Point)
     * @see #setBounds(Rectangle)
     */
    public Rectangle getBounds()
    {
        return Component.this.getBounds();
    }

    /**
        * Sets the bounds of this component.
     *
     * @param r the bounds
     * @throws NullPointerException if r is null
     * @see #getBounds()
     */
    public void setBounds(Rectangle r)
    {
        Component.this.setBounds(r.x, r.y, r.width, r.height);
    }

    /**
        * Gets the size of this component, or null if it is not showing.
     *
     * @return the size
     * @see #setSize(Dimension)
     */
    public Dimension getSize()
    {
        return Component.this.getSize();
    }

    /**
        * Sets the size of this component.
     *
     * @param d the size
     * @throws NullPointerException if d is null
     * @see #getSize()
     */
    public void setSize(Dimension d)
    {
        Component.this.setSize(d.width, d.height);
    }

    /**
        * Returns the Accessible child at a point relative to the coordinate
     * system of this component, if one exists, or null. Since components
     * have no children, subclasses must override this to get anything besides
     * null.
     *
     * @param p the point to check
     * @return the accessible child at that point
     * @throws NullPointerException if p is null
     */
    public Accessible getAccessibleAt(Point p)
    {
        return null;
    }

    /**
        * Tests whether this component can accept focus.
     *
     * @return true if this is focus traversable
     * @see #getAccessibleStateSet ()
     * @see AccessibleState#FOCUSABLE
     * @see AccessibleState#FOCUSED
     */
    public boolean isFocusTraversable ()
    {
        return Component.this.isFocusTraversable ();
    }

    /**
        * Requests focus for this component.
     *
     * @see #isFocusTraversable ()
     */
    public void requestFocus ()
    {
        Component.this.requestFocus ();
    }

    /**
        * Adds a focus listener.
     *
     * @param l the listener to add
     */
    public void addFocusListener(FocusListener l)
    {
        Component.this.addFocusListener(l);
    }

    /**
        * Removes a focus listener.
     *
     * @param l the listener to remove
     */
    public void removeFocusListener(FocusListener l)
    {
        Component.this.removeFocusListener(l);
    }

    /**
        * Converts component changes into property changes.
     *
     * @author Eric Blake (ebb9@email.byu.edu)
     * @since 1.3
     * @status updated to 1.4
     */
    protected class AccessibleAWTComponentHandler implements ComponentListener
    {
        /**
        * Default constructor.
         */
        protected AccessibleAWTComponentHandler()
    {
            // Nothing to do here.
    }

        /**
        * Convert a component hidden to a property change.
         *
         * @param e the event to convert
         */
        public void componentHidden(ComponentEvent e)
    {
            AccessibleAWTComponent.this.firePropertyChange
            (ACCESSIBLE_STATE_PROPERTY, AccessibleState.VISIBLE, null);
    }

        /**
        * Convert a component shown to a property change.
         *
         * @param e the event to convert
         */
        public void componentShown(ComponentEvent e)
    {
            AccessibleAWTComponent.this.firePropertyChange
            (ACCESSIBLE_STATE_PROPERTY, null, AccessibleState.VISIBLE);
    }

        /**
        * Moving a component does not affect properties.
         *
         * @param e ignored
         */
        public void componentMoved(ComponentEvent e)
    {
            // Nothing to do here.
    }

        /**
        * Resizing a component does not affect properties.
         *
         * @param e ignored
         */
        public void componentResized(ComponentEvent e)
    {
            // Nothing to do here.
    }
    } // class AccessibleAWTComponentHandler

    /**
        * Converts focus changes into property changes.
     *
     * @author Eric Blake (ebb9@email.byu.edu)
     * @since 1.3
     * @status updated to 1.4
     */
    protected class AccessibleAWTFocusHandler implements FocusListener
    {
        /**
        * Default constructor.
         */
        protected AccessibleAWTFocusHandler()
    {
            // Nothing to do here.
    }

        /**
        * Convert a focus gained to a property change.
         *
         * @param e the event to convert
         */
        public void focusGained(FocusEvent e)
    {
            AccessibleAWTComponent.this.firePropertyChange
            (ACCESSIBLE_STATE_PROPERTY, null, AccessibleState.FOCUSED);
    }

        /**
        * Convert a focus lost to a property change.
         *
         * @param e the event to convert
         */
        public void focusLost(FocusEvent e)
    {
            AccessibleAWTComponent.this.firePropertyChange
            (ACCESSIBLE_STATE_PROPERTY, AccessibleState.FOCUSED, null);
    }
    } // class AccessibleAWTComponentHandler
} // class AccessibleAWTComponent

/**
* This class provides support for blitting offscreen surfaces to a
 * component.
 *
 * @see BufferStrategy
 *
 * @since 1.4
 */
protected class BltBufferStrategy extends BufferStrategy
{
    /**
    * The capabilities of the image buffer.
     */
    protected BufferCapabilities caps;

    /**
    * The back buffers used in this strategy.
     */
    protected VolatileImage[] backBuffers;

    /**
        * Whether or not the image buffer resources are allocated and
     * ready to be drawn into.
     */
    protected boolean validatedContents;

    /**
        * The width of the back buffers.
     */
    protected int width;

    /**
        * The height of the back buffers.
     */
    protected int height;

    /**
        * The front buffer.
     */
    private VolatileImage frontBuffer;

    /**
        * Creates a blitting buffer strategy.
     *
     * @param numBuffers the number of buffers, including the front
     * buffer
     * @param caps the capabilities of this strategy
     */
    protected BltBufferStrategy(int numBuffers, BufferCapabilities caps)
    {
        this.caps = caps;
        createBackBuffers(numBuffers - 1);
        width = getWidth();
        height = getHeight();
    }

    /**
        * Initializes the backBuffers field with an array of numBuffers
     * VolatileImages.
     *
     * @param numBuffers the number of backbuffers to create
     */
    protected void createBackBuffers(int numBuffers)
    {
        GraphicsConfiguration c =
        GraphicsEnvironment.getLocalGraphicsEnvironment()
        .getDefaultScreenDevice().getDefaultConfiguration();

        backBuffers = new VolatileImage[numBuffers];

        for (int i = 0; i < numBuffers; i++)
            backBuffers[i] = c.createCompatibleVolatileImage(width, height);
    }

    /**
        * Retrieves the capabilities of this buffer strategy.
     *
     * @return the capabilities of this buffer strategy
     */
    public BufferCapabilities getCapabilities()
    {
        return caps;
    }

    /**
        * Retrieves a graphics object that can be used to draw into this
     * strategy's image buffer.
     *
     * @return a graphics object
     */
    public Graphics getDrawGraphics()
    {
        // Return the backmost buffer's graphics.
        return backBuffers[0].getGraphics();
    }

    /**
        * Bring the contents of the back buffer to the front buffer.
     */
    public void show()
    {
        GraphicsConfiguration c =
        GraphicsEnvironment.getLocalGraphicsEnvironment()
        .getDefaultScreenDevice().getDefaultConfiguration();

        // draw the front buffer.
        getGraphics().drawImage(backBuffers[backBuffers.length - 1],
                                width, height, null);

        BufferCapabilities.FlipContents f = getCapabilities().getFlipContents();

        // blit the back buffers.
        for (int i = backBuffers.length - 1; i > 0 ; i--)
            backBuffers[i] = backBuffers[i - 1];

        // create new backmost buffer.
        if (f == BufferCapabilities.FlipContents.UNDEFINED)
            backBuffers[0] = c.createCompatibleVolatileImage(width, height);

        // create new backmost buffer and clear it to the background
        // color.
        if (f == BufferCapabilities.FlipContents.BACKGROUND)
        {
            backBuffers[0] = c.createCompatibleVolatileImage(width, height);
            backBuffers[0].getGraphics().clearRect(0, 0, width, height);
        }

        // FIXME: set the backmost buffer to the prior contents of the
        // front buffer.  How do we retrieve the contents of the front
        // buffer?
        //
        //      if (f == BufferCapabilities.FlipContents.PRIOR)

        // set the backmost buffer to a copy of the new front buffer.
        if (f == BufferCapabilities.FlipContents.COPIED)
            backBuffers[0] = backBuffers[backBuffers.length - 1];
    }

    /**
        * Re-create the image buffer resources if they've been lost.
     */
    protected void revalidate()
    {
        GraphicsConfiguration c =
        GraphicsEnvironment.getLocalGraphicsEnvironment()
        .getDefaultScreenDevice().getDefaultConfiguration();

        for (int i = 0; i < backBuffers.length; i++)
        {
            int result = backBuffers[i].validate(c);
            if (result == VolatileImage.IMAGE_INCOMPATIBLE)
                backBuffers[i] = c.createCompatibleVolatileImage(width, height);
        }
        validatedContents = true;
    }

    /**
        * Returns whether or not the image buffer resources have been
     * lost.
     *
     * @return true if the resources have been lost, false otherwise
     */
    public boolean contentsLost()
    {
        for (int i = 0; i < backBuffers.length; i++)
        {
            if (backBuffers[i].contentsLost())
            {
                validatedContents = false;
                return true;
            }
        }
        // we know that the buffer resources are valid now because we
        // just checked them
        validatedContents = true;
        return false;
    }

    /**
        * Returns whether or not the image buffer resources have been
     * restored.
     *
     * @return true if the resources have been restored, false
     * otherwise
     */
    public boolean contentsRestored()
    {
        GraphicsConfiguration c =
        GraphicsEnvironment.getLocalGraphicsEnvironment()
        .getDefaultScreenDevice().getDefaultConfiguration();

        boolean imageRestored = false;

        for (int i = 0; i < backBuffers.length; i++)
        {
            int result = backBuffers[i].validate(c);
            if (result == VolatileImage.IMAGE_RESTORED)
                imageRestored = true;
            else if (result == VolatileImage.IMAGE_INCOMPATIBLE)
                return false;
        }
        // we know that the buffer resources are valid now because we
        // just checked them
        validatedContents = true;
        return imageRestored;
    }
}

/**
* This class provides support for flipping component buffers. It
 * can only be used on Canvases and Windows.
 *
 * @since 1.4
 */
protected class FlipBufferStrategy extends BufferStrategy
{
    /**
    * The number of buffers.
     */
    protected int numBuffers;

    /**
    * The capabilities of this buffering strategy.
     */
    protected BufferCapabilities caps;

    /**
        * An Image reference to the drawing buffer.
     */
    protected Image drawBuffer;

    /**
        * A VolatileImage reference to the drawing buffer.
     */
    protected VolatileImage drawVBuffer;

    /**
        * Whether or not the image buffer resources are allocated and
     * ready to be drawn into.
     */
    protected boolean validatedContents;

    /**
        * The width of the back buffer.
     */
    private int width;

    /**
        * The height of the back buffer.
     */
    private int height;

    /**
        * Creates a flipping buffer strategy.  The only supported
     * strategy for FlipBufferStrategy itself is a double-buffer page
     * flipping strategy.  It forms the basis for more complex derived
     * strategies.
     *
     * @param numBuffers the number of buffers
     * @param caps the capabilities of this buffering strategy
     *
     * @throws AWTException if the requested
     * number-of-buffers/capabilities combination is not supported
     */
    protected FlipBufferStrategy(int numBuffers, BufferCapabilities caps)
        throws AWTException
    {
        this.caps = caps;
        width = getWidth();
        height = getHeight();

        if (numBuffers > 1)
            createBuffers(numBuffers, caps);
        else
        {
            drawVBuffer = peer.createVolatileImage(width, height);
            drawBuffer = drawVBuffer;
        }
    }

    /**
        * Creates a multi-buffer flipping strategy.  The number of
     * buffers must be greater than one and the buffer capabilities
     * must specify page flipping.
     *
     * @param numBuffers the number of flipping buffers; must be
     * greater than one
     * @param caps the buffering capabilities; caps.isPageFlipping()
     * must return true
     *
     * @throws IllegalArgumentException if numBuffers is not greater
     * than one or if the page flipping capability is not requested
     *
     * @throws AWTException if the requested flipping strategy is not
     * supported
     */
    protected void createBuffers(int numBuffers, BufferCapabilities caps)
        throws AWTException
    {
        if (numBuffers <= 1)
            throw new IllegalArgumentException("FlipBufferStrategy.createBuffers:"
                                               + " numBuffers must be greater than"
                                               + " one.");

        if (!caps.isPageFlipping())
            throw new IllegalArgumentException("FlipBufferStrategy.createBuffers:"
                                               + " flipping must be a specified"
                                               + " capability.");

        peer.createBuffers(numBuffers, caps);
    }

    /**
        * Return a direct reference to the back buffer image.
     *
     * @return a direct reference to the back buffer image.
     */
    protected Image getBackBuffer()
    {
        return peer.getBackBuffer();
    }

    /**
        * Perform a flip operation to transfer the contents of the back
     * buffer to the front buffer.
     */
    protected void flip(BufferCapabilities.FlipContents flipAction)
    {
        peer.flip(flipAction);
    }

    /**
        * Release the back buffer's resources.
     */
    protected void destroyBuffers()
    {
        peer.destroyBuffers();
    }

    /**
        * Retrieves the capabilities of this buffer strategy.
     *
     * @return the capabilities of this buffer strategy
     */
    public BufferCapabilities getCapabilities()
    {
        return caps;
    }

    /**
        * Retrieves a graphics object that can be used to draw into this
     * strategy's image buffer.
     *
     * @return a graphics object
     */
    public Graphics getDrawGraphics()
    {
        return drawVBuffer.getGraphics();
    }

    /**
        * Re-create the image buffer resources if they've been lost.
     */
    protected void revalidate()
    {
        GraphicsConfiguration c =
        GraphicsEnvironment.getLocalGraphicsEnvironment()
        .getDefaultScreenDevice().getDefaultConfiguration();

        if (drawVBuffer.validate(c) == VolatileImage.IMAGE_INCOMPATIBLE)
            drawVBuffer = peer.createVolatileImage(width, height);
        validatedContents = true;
    }

    /**
        * Returns whether or not the image buffer resources have been
     * lost.
     *
     * @return true if the resources have been lost, false otherwise
     */
    public boolean contentsLost()
    {
        if (drawVBuffer.contentsLost())
        {
            validatedContents = false;
            return true;
        }
        // we know that the buffer resources are valid now because we
        // just checked them
        validatedContents = true;
        return false;
    }

    /**
        * Returns whether or not the image buffer resources have been
     * restored.
     *
     * @return true if the resources have been restored, false
     * otherwise
     */
    public boolean contentsRestored()
    {
        GraphicsConfiguration c =
        GraphicsEnvironment.getLocalGraphicsEnvironment()
        .getDefaultScreenDevice().getDefaultConfiguration();

        int result = drawVBuffer.validate(c);

        boolean imageRestored = false;

        if (result == VolatileImage.IMAGE_RESTORED)
            imageRestored = true;
        else if (result == VolatileImage.IMAGE_INCOMPATIBLE)
            return false;

        // we know that the buffer resources are valid now because we
        // just checked them
        validatedContents = true;
        return imageRestored;
    }

    /**
        * Bring the contents of the back buffer to the front buffer.
     */
    public void show()
    {
        flip(caps.getFlipContents());
    }
}

}
