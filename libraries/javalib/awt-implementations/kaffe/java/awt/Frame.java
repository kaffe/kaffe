package java.awt;

import java.awt.event.WindowEvent;

/**
 * Frame - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */
public class Frame
  extends Window
{
	String title;
	Image icon;
	BarMenu bMenu;
	static Insets frameInsets;
	static Insets menuFrameInsets;
	static Rectangle frameDeco;
	private static int counter;
	final private static long serialVersionUID = 2673458971256075116L;
	final public static int CROSSHAIR_CURSOR = Cursor.CROSSHAIR_CURSOR;
	final public static int DEFAULT_CURSOR = Cursor.DEFAULT_CURSOR;
	final public static int HAND_CURSOR = Cursor.HAND_CURSOR;
	final public static int TEXT_CURSOR = Cursor.TEXT_CURSOR;
	final public static int WAIT_CURSOR = Cursor.WAIT_CURSOR;
	final public static int E_RESIZE_CURSOR = Cursor.E_RESIZE_CURSOR;
	final public static int N_RESIZE_CURSOR = Cursor.N_RESIZE_CURSOR;
	final public static int NE_RESIZE_CURSOR = Cursor.NE_RESIZE_CURSOR;
	final public static int NW_RESIZE_CURSOR = Cursor.NW_RESIZE_CURSOR;
	final public static int S_RESIZE_CURSOR = Cursor.S_RESIZE_CURSOR;
	final public static int SE_RESIZE_CURSOR = Cursor.SE_RESIZE_CURSOR;
	final public static int SW_RESIZE_CURSOR = Cursor.SW_RESIZE_CURSOR;
	final public static int W_RESIZE_CURSOR = Cursor.W_RESIZE_CURSOR;
	final public static int MOVE_CURSOR = Cursor.MOVE_CURSOR;

static {
	// these are just the guesses (to be overwritten by a subsequent
	// setFrameInsets() from the native layer)
	Insets guess = Defaults.FrameInsets;
	
	frameInsets = new Insets( guess.top, guess.left, guess.bottom, guess.right);
	menuFrameInsets = new Insets( guess.top + Defaults.MenuBarHeight, guess.left,
	                              guess.bottom, guess.right);	
	frameDeco = new Rectangle( guess.left, guess.top,
	                           (guess.left+guess.right), (guess.top+guess.bottom));
	
	Toolkit.wndSetFrameInsets( guess.top, guess.left, guess.bottom, guess.right);
}

public Frame () {
	this( null, null);
}

Frame ( Frame owner, String title ) {
	super( owner);
	
	flags |= IS_RESIZABLE;
	this.title = (title == null) ? "" : title;	
	deco = frameDeco;
	setName("frame" + counter++);
}

public Frame ( String title ) {
	this( null, title);
}

public int countComponents() {
	// DEP - should be in getComponentCount()

	if ( bMenu == null )
		return nChildren;
	
	return nChildren-1;
}

void createNative () {
	// This is the terminal class addNotify() part. DANGER: ptr isn't a real object
	int u = x;
	int v = y;
	int w = width;
	int h = height;

	// Insets seem to be set by the JDK during addNotify
	// (no need to create fresh objects since they are insets()-copied anyway <sigh>).
	// Note that the global inset objects might be changed if the native layer
	// has to compute the real deco offsets during window creation
	insets = (bMenu != null) ? menuFrameInsets : frameInsets;

	if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ) {
		// we just pretend to own the deco space, subtract it before going native
		u += deco.x;
		v += deco.y;
		w -= deco.width;
		h -= deco.height;
	}

	nativeData = Toolkit.wndCreateFrame( title, u, v, w, h,
	                               cursor.type, bgClr.getNativeValue(), ((flags & IS_RESIZABLE) != 0));
}

public Component getComponent( int index) {
	if ( children == null )
		return null;
	if ( bMenu == null )
		return children[index];
	
	return children[index+1];
}

public Component[] getComponents() {
	if ( bMenu == null )
		return super.getComponents();
	
	Component ca[] = new Component[nChildren-1];
	
	if ( ca.length > 0 )
		System.arraycopy( children, 1, ca, 0, nChildren-1);
	
	return ca;
}

/**
 * @deprecated, use Component.getCursor()
 */
public int getCursorType() {
	return (getCursor().getType());
}

public Image getIconImage() {
	return icon;
}

public MenuBar getMenuBar () {
	return (bMenu != null) ? bMenu.mb : null;
}

public String getTitle() {
	return title;
}

public boolean isResizable () {
	return ((flags & IS_RESIZABLE) != 0);
}

public void layout () {
	// DEP - should be in doLayout()

	super.layout();

	if ( bMenu != null){
		bMenu.setBounds( frameDeco.x, frameDeco.y,
		                 width-(frameDeco.width), Defaults.MenuBarHeight);
  }		
}

public void paint ( Graphics g ) {
//synchronized ( treeLock ) {
		int n = 0;

		// we should treat the bMenu special because (a) it avoids flicker, and (b)
		// some careless apps have components painting over the menu (we have to clip)
		if ( ((flags & IS_IN_UPDATE) == 0) && (bMenu != null) ) {
			// otherwise the menu has already been drawn by update()
			g.paintChild( bMenu, false);
			n = 1;
			g.clipRect( deco.x, insets.top, width - (deco.width), height - (deco.height));
		}

		for ( int i=nChildren-1; i>=n; i-- ) {
			Component c = children[i];

			if ( (c.flags & IS_VISIBLE) != 0 ) {
				g.paintChild( c, (flags & IS_IN_UPDATE) != 0);
			}
		}
//}
}

protected String paramString() {
	return super.paramString()
		+ ",title=" + getTitle() 
		+ (isResizable() ? ",resizable" : ",fixed");
}

/**
 * @deprecated, use getPreferredSize()
 */
public Dimension preferredSize () {
	Insets    in = Defaults.FrameInsets;
	Dimension d = super.preferredSize();
	
	d.width  += (in.left + in.right);
	d.height += (in.top + in.bottom);
	
	if ( bMenu != null ) {
		d.height += Defaults.MenuBarHeight;
	}
	return (d);
}

protected void processWindowEvent ( WindowEvent event ) {
	// should be in Windows, but the JDK docu says Windows don't understand
	// all of the WindowEvents (probably because they didn't implement popups
	// w/o titlebar shading on Solaris)
	if ( wndListener != null ) {
		switch ( event.id ) {
		case WindowEvent.WINDOW_OPENED:
			wndListener.windowOpened( event);
			break;
		case WindowEvent.WINDOW_CLOSING:
			wndListener.windowClosing( event);
			break;
		case WindowEvent.WINDOW_CLOSED:
			wndListener.windowClosed( event);
			break;
		case WindowEvent.WINDOW_ICONIFIED:
			wndListener.windowIconified( event);
			break;
		case WindowEvent.WINDOW_DEICONIFIED:
			wndListener.windowDeiconified( event);
			break;
		case WindowEvent.WINDOW_ACTIVATED:
				wndListener.windowActivated( event);
			break;
		case WindowEvent.WINDOW_DEACTIVATED:
				wndListener.windowDeactivated( event);
			break;
		}
	}
}

public void remove ( MenuComponent mc ) {
}

/**
 * @deprecated, use Component.setCursor()
 */
public void setCursor(int cursorType) {
	setCursor(Cursor.getPredefinedCursor(cursorType));
}

static void setDecoInsets ( int top, int left, int bottom, int right, int srcIdx ){
	// this is the native callBack to set exact (calculated) Frame deco extends

	frameInsets.top    = top;
	frameInsets.left   = left;
	frameInsets.bottom = bottom;
	frameInsets.right  = right;
	
	menuFrameInsets.top  = top + Defaults.MenuBarHeight;
	menuFrameInsets.left = left;
	menuFrameInsets.bottom = bottom;
	menuFrameInsets.right = right;
	
	frameDeco.x = left;
	frameDeco.y = top;
	frameDeco.width = left + right;
	frameDeco.height = top + bottom;
	
	// if we got the correction in the context of a initial Window positioning
	// we have to make sure a subsequent ComponentEvt.getEvent() invalidates
	// this instance (which wouldn't be the case if we let its (faked) dimension alone)
	if ( srcIdx != -1 ) {
		Component src = AWTEvent.sources[srcIdx];
		src.width = src.height = 0;
	}
}

public void setIconImage ( Image icon ) {
	this.icon = icon;
	
	if ( nativeData != null )
		Toolkit.wndSetIcon( nativeData, icon.nativeData);
}

public void setMenuBar ( MenuBar mb ) {

	bMenu = new BarMenu( mb);
	
	// add BarMenu to children at position 0 ( without LayoutManager registration )
	if ( children == null )
		children= new Component[3];
	else if ( nChildren == children.length) {
		Component[] old = children;
		children = new Component[nChildren*2];
		System.arraycopy( old, 0, children, 1, nChildren);
	}
	else
		System.arraycopy( children, 0, children, 1, nChildren);
	
	children[0] = bMenu;
	nChildren++;
	bMenu.parent = this;

	if ( nativeData != null ) {
		insets = menuFrameInsets;
		bMenu.addNotify();
		doLayout();
	}
}

public void setResizable ( boolean isResizable ) {
	if ( isResizable )
		flags |= IS_RESIZABLE;
	else
		flags &= ~IS_RESIZABLE;
	
	if ( nativeData != null )
		Toolkit.wndSetResizable( nativeData, isResizable, x, y, width, height);
}

public void setTitle ( String newTitle ) {
	title = newTitle;
	
	if ( nativeData != null )
		Toolkit.wndSetTitle( nativeData, newTitle);
}

public void update ( Graphics g ) {
	int w = width - deco.width;
	int h = height - (insets.top + insets.bottom);
	
	flags |= IS_IN_UPDATE;

	// we should treat the bMenu special because (a) it avoids flicker, and (b)
	// some careless apps have components painting over the menu (we have to clip)
	if ( bMenu != null ) {
		g.paintChild( bMenu, false);
		g.clipRect( deco.x, insets.top, w, h);
	}

	g.clearRect( deco.x, insets.top, w, h);
	paint( g);

	flags &= ~IS_IN_UPDATE;
}
}
