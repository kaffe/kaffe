package java.awt;

import java.awt.AWTError;
import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Frame;
import java.awt.Image;
import java.awt.MenuComponent;
import java.awt.MenuItem;
import java.awt.PSPrintJob;
import java.awt.PrintJob;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.Transferable;
import java.awt.event.InputEvent;
import java.awt.image.ColorModel;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.awt.peer.ComponentPeer;
import java.awt.peer.LightweightPeer;
import java.awt.peer.WindowPeer;
import java.io.File;
import java.io.PrintStream;
import java.net.URL;
import java.util.Properties;

import kaffe.util.Ptr;
import kaffe.util.log.LogClient;
import kaffe.util.log.LogStream;

/**
 * Toolkit - used to be an abstract factory for peers, but since we don't have
 * peers, it is just a simple anchor for central mechanisms (like System-
 * EventQueue etc.) and a wrapper for all native methods. Of course, it is a
 * singleton.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */
class FlushThread
  extends Thread
{
	boolean stop;
	int flushInterval;

FlushThread ( int interval ) {
	super( "AWT-Flusher");

	flushInterval = interval;
	setPriority( Thread.MIN_PRIORITY + 1);
}

public void run () {
	while ( !stop ) {
		Toolkit.tlkFlush();
		try {
			Thread.sleep( flushInterval);
		}
		catch ( Exception x ) {}
	}
}

void stopFlushing () {
	stop = true;
}
}

class NativeCollector
  extends Thread
{
NativeCollector () {
	super( "AWT-native");
}

public void run () {        
	// this does not return until the we shut down the system, since it
	// consititutes the native dispatcher loop. Don't be confused about
	// tlkInit being a sync method. It gives up the lock in the native
	// layer before falling into its dispatcher loop

	try {
		if ( !Toolkit.tlkInit( System.getProperty( "awt.display")) ) {
			throw new AWTError( "native layer init failed");
		}
	} catch ( Throwable x ) {
		x.printStackTrace();
	}
}
}

public class Toolkit
{
	static Toolkit singleton;
	static Dimension screenSize;
	static int resolution;
	static EventQueue eventQueue;
	static EventDispatchThread eventThread;
	static NativeClipboard clipboard;
	static LightweightPeer lightweightPeer = new LightweightPeer() {};
	static ComponentPeer componentPeer = new ComponentPeer() {};
	static WindowPeer windowPeer = new WindowPeer() {};
	static FlushThread flushThread;
	static NativeCollector collectorThread;
	static int flags;
	final static int FAILED = -1;
	final static int IS_BLOCKING = 1;
	final static int IS_DISPATCH_EXCLUSIVE = 2;
	final static int NEEDS_FLUSH = 4;
	final static int NATIVE_DISPATCHER_LOOP = 8;
	final static int EXTERNAL_DECO = 16;

static {
	System.loadLibrary( "awt");
	flags = tlkProperties();
	
	// hack to please swings default look and feel init
	// ( no utsname support for System.c , so default is 'Unknown' )
	
	Properties p = System.getProperties();
	String osn = p.getProperty( "os.name");
	if ( (osn == null) || !(osn.startsWith("Windows"))) {
		p.put( "os.name", "Windows" );
	}

	if ( (flags & NATIVE_DISPATCHER_LOOP) == 0 ) {
		if ( !tlkInit( System.getProperty( "awt.display")) ) {
			throw new AWTError( "native layer initialization failed");
		}
		initToolkit();
	}
	else {
		// Not much we can do here, we have to delegate the native init
		// to a own thread since tlkInit() doesn't return. Wait for this
		// thread to flag that initialization has been completed
		collectorThread = new NativeCollector();
		collectorThread.start();

		try {
			synchronized ( Toolkit.class ) {
				while ( singleton == null ) {
					Toolkit.class.wait();
				}
			}
		}
		catch ( Exception x ) {
			x.printStackTrace();
		}
	}
}

Toolkit () {
}

public void beep () {
	tlkBeep();
}

native static synchronized Ptr btnCreateCheckbox ( Ptr parent, String label );

native static synchronized Ptr btnCreatePushbutton ( Ptr parent, String label );

native static synchronized Ptr btnCreateRadiobutton ( Ptr parent, String label );

native static synchronized boolean btnGetCheck ( Ptr data);

native static synchronized void btnSetCheck ( Ptr data, boolean check );

native static synchronized void btnSetGrouped ( Ptr data, boolean grouped);

native static synchronized void btnSetLabel ( Ptr data, String label );

native static synchronized void cbdFreeClipboard ( Ptr cbdData );

native static synchronized Transferable cbdGetContents( Ptr cbdData);

native static synchronized Ptr cbdInitClipboard ();

native static synchronized boolean cbdSetOwner ( Ptr cbdData );

public int checkImage(Image image, int width, int height, ImageObserver observer) {
	return (image.checkImage( width, height, observer, false));
}

native static synchronized void choiceAppendItem( Ptr data, String item);

native static synchronized Ptr choiceCreateChoice( Ptr parent);

native static synchronized void choiceInsertItem( Ptr data, String item, int idx);

native static synchronized void choiceRemoveAll( Ptr data);

native static synchronized void choiceRemoveItem( Ptr data, int idx);

native static synchronized void choiceSelectItem( Ptr data, int idx);

native static synchronized long clrBright ( int rgbValue );

native static synchronized long clrDark ( int rgbValue );

native static synchronized int clrGetPixelValue ( int rgb );

native static synchronized int clrSetSystemColors ( int[] sysClrs );

native static synchronized void cmnDestroyWindow ( Ptr wndData );

native static synchronized Dimension cmnGetPreferredSize ( Ptr data);

native static synchronized void cmnInvalidate ( Ptr data, int x, int y, int w, int h );

native static synchronized void cmnSetBackground ( Ptr data, int clr );

native static synchronized void cmnSetBounds ( Ptr data, int x, int y, int w, int h);

native static synchronized void cmnSetCursor ( Ptr data, int type);

native static synchronized void cmnSetEnabled ( Ptr data, boolean en );

native static synchronized void cmnSetFocus ( Ptr data);

native static synchronized void cmnSetFont ( Ptr data, Ptr fntData );

native static synchronized void cmnSetForeground ( Ptr data, int clr );

native static synchronized void cmnSetVisible ( Ptr data, boolean vis );

native static synchronized void cmnValidate ( Ptr data );

public Image createImage ( ImageProducer producer ) {
	return new Image( producer);
}

public Image createImage ( byte[] imageData ) {
	return createImage( imageData, 0, imageData.length);
}

public Image createImage ( byte[] imagedata, int imageoffset, int imagelength ) {
	return new Image( imagedata, imageoffset, imagelength);
}

ComponentPeer createLightweight ( Component c ) {
	// WARNING! this is just a dummy to enable checks like
	// "..getPeer() != null.. or ..peer instanceof LightweightPeer..
	// see createWindow()
	return lightweightPeer;
}

static void createNative ( Component c ) {
	WMEvent e = null;

	synchronized ( Toolkit.class ) {
		// even if this could be done in a central location, we defer this
		// as much as possible because it might involve polling (for non-threaded
		// AWTs), slowing down the startup time
		if ( eventThread == null ) {
			startDispatch();
		}
	}

	// do we need some kind of a context switch ?
	if ( (flags & IS_DISPATCH_EXCLUSIVE) != 0 ){
		if ( (flags & NATIVE_DISPATCHER_LOOP) != 0 ){
			if ( Thread.currentThread() != collectorThread ){
				// this is beyond our capabilities (there is no Java message entry we can call
				// in the native collector), we have to revert to some native mechanism
				e =  WMEvent.getEvent( c, WMEvent.WM_CREATE);
				evtSendWMEvent( e);
			}
		}
		else {
			if ( Thread.currentThread() != eventThread ){
				// we can force the context switch by ourselves, no need to go native
				e =  WMEvent.getEvent( c, WMEvent.WM_CREATE);
				eventQueue.postEvent( e);
			}
		}
			
		// Ok, we have a request out there, wait for it to be served
		if ( e != null ) {
			// we should check for nativeData because the event might
			// already be processed (depending on the thread system)
			while ( c.getNativeData() == null ) {
				synchronized ( e ) {
					try { e.wait(); } 
					catch ( InterruptedException x ) {}
				} 
			}
				
			return;
		}
	}
	else {
		// no need to switch threads, go native right away
		c.createNative();
	}
}

protected WindowPeer createWindow ( Window w ) {
	// WARNING! this is just a dummy to enable checks like
	// "..getPeer() != null.. or ..peer instanceof LightweightPeer..
	// it is NOT a real peer support. The peer field just exists to
	// check if a Component already passed its addNotify()/removeNotify().
	// This most probably will be removed once the "isLightweightComponent()"
	// method gets official (1.2?)

	return windowPeer;
}

static void destroyNative ( Component c ) {
	WMEvent e = null;

	// do we need some kind of a context switch ?
	if ( (flags & IS_DISPATCH_EXCLUSIVE) != 0 ){
		if ( (flags & NATIVE_DISPATCHER_LOOP) != 0 ){
			if ( Thread.currentThread() != collectorThread ){
				// this is beyond our capabilities (there is no Java message entry we can call
				// in the native collector), we have to revert to some native mechanism
				e =  WMEvent.getEvent( c, WMEvent.WM_DESTROY);
				evtSendWMEvent( e);
			}
		}
		else {
			if ( Thread.currentThread() != eventThread ){
				// we can force the context switch by ourselves, no need to go native
				e =  WMEvent.getEvent( c, WMEvent.WM_DESTROY);
				eventQueue.postEvent( e);
			}
		}
			
		// Ok, we have a request out there, wait for it to be served
		if ( e != null ) {
			// we should check for nativeData because the event might
			// already be processed (depending on the thread system)
			while ( c.getNativeData() != null ) {
				synchronized ( e ) {
					try { e.wait(); } catch ( InterruptedException x ) {}
				} 
			}
				
			return;
		}
	}
	else {
		// no need to switch threads, go native right away
		c.destroyNative();
	}
}

native static synchronized void editAppend( Ptr data, String txt);

native static synchronized Ptr editCreateArea( Ptr parent, String txt, int style);

native static synchronized Ptr editCreateField( Ptr parent, String txt);

native static synchronized int editGetCaretPosition( Ptr data);

native static synchronized String editGetSelectedText( Ptr data);

native static synchronized int editGetSelectionEnd( Ptr data);

native static synchronized int editGetSelectionStart( Ptr data);

native static synchronized String editGetText( Ptr data);

native static synchronized void editInsert( Ptr data, String txt, int idx);

native static synchronized void editReplaceRange( Ptr data, String txt, int start, int end);

native static synchronized void editSelect( Ptr data, int start, int end);

native static synchronized void editSelectAll( Ptr data);

native static synchronized void editSetCaretPosition( Ptr data, int idx);

native static synchronized int editSetEchoChar( Ptr data, char c);

native static synchronized void editSetEditable( Ptr data, boolean edit);

native static synchronized void editSetSelectionEnd( Ptr data, int idx);

native static synchronized void editSetSelectionStart( Ptr data, int idx);

native static synchronized void editSetText( Ptr data, String txt);

native static synchronized AWTEvent evtGetNextEvent ();

native static synchronized Component[] evtInit ();

native static synchronized AWTEvent evtPeekEvent ();

native static synchronized AWTEvent evtPeekEventId ( int eventId );

native static synchronized int evtRegisterSource ( Ptr wndData );

native static synchronized void evtSendWMEvent ( WMEvent e );

native static synchronized int evtUnregisterSource ( Ptr wndData );

native static synchronized void evtWakeup ();

native static synchronized String fdlgLoad ( Ptr owner, String title, String dir, String file, String filter );

native static synchronized String fdlgSave ( Ptr owner, String title, String dir, String file, String filter );

native static synchronized int fntBytesWidth ( Ptr fmData, byte[] data, int off, int len );

native static synchronized int fntCharWidth ( Ptr fmData, char c );

native static synchronized int fntCharsWidth ( Ptr fmData, char[] data, int off, int len );

native static synchronized void fntFreeFont ( Ptr fontData );

native static synchronized void fntFreeFontMetrics ( Ptr fmData );

native static synchronized int fntGetAscent ( Ptr fmData);

native static synchronized int fntGetDescent ( Ptr fmData);

native static synchronized int fntGetFixedWidth ( Ptr fmData);

native static synchronized int fntGetHeight ( Ptr fmData);

native static synchronized int fntGetLeading ( Ptr fmData);

native static synchronized int fntGetMaxAdvance ( Ptr fmData);

native static synchronized int fntGetMaxAscent ( Ptr fmData);

native static synchronized int fntGetMaxDescent ( Ptr fmData);

native static synchronized int[] fntGetWidths ( Ptr fmData );

native static synchronized Ptr fntInitFont ( String fntSpec, int style, int size );

native static synchronized Ptr fntInitFontMetrics ( Ptr fontData);

native static synchronized boolean fntIsWideFont ( Ptr fmData);

native static synchronized int fntStringWidth ( Ptr fmData, String s );

public ColorModel getColorModel () {
	return null;
}

public static Toolkit getDefaultToolkit () {
	return singleton;
}

public String[] getFontList () {
	String[] list = { "Default", "Monospaced",
		"SansSerif", "Serif",
		"Dialog", "DialogInput", "ZapfDingbats" };
	return list;
}

public FontMetrics getFontMetrics ( Font font ) {
	return FontMetrics.getFontMetrics( font);
}

public Image getImage ( String filename ) {
	return new Image( new File( filename));
}

public Image getImage ( URL url ) {
	return new Image( url);
}

public int getMenuShortcutKeyMask() {
	return InputEvent.CTRL_MASK;
}

static Ptr getNativeData( Component c) {
	if ( c instanceof NativeComponent )
		return ( (NativeComponent)c).nativeData;
	else if ( c instanceof NativeContainer )
		return ( (NativeContainer)c).nativeData;
	else
		return null;
}

public PrintJob getPrintJob ( Frame frame, String jobtitle, Properties props ) {
	return new PSPrintJob( frame, jobtitle, props);
}

public static String getProperty ( String key, String defaultValue ) {
	return null;
}

public Dimension getScreenSize () {
	return screenSize;
}

public Clipboard getSystemClipboard () {
	if ( clipboard == null )
		clipboard = new NativeClipboard();

	return clipboard;
}

public EventQueue getSystemEventQueue () {
	return eventQueue;
}

native static synchronized void graCleanUpGraphics ( Ptr grData );

native static synchronized void graClearRect ( Ptr grData, int x, int y, int width, int height );

native static synchronized void graCopyArea ( Ptr grData, int x, int y, int width, int height,
                   int xDelta, int yDelta );

native static synchronized void graDraw3DRect ( Ptr grData, int x, int y, int width, int height,
                     boolean raised, int rgbValue );

native static synchronized void graDrawArc ( Ptr grData, int x, int y, int width, int height, int startAngle, int arcAngle );

native static synchronized void graDrawBytes ( Ptr grData, byte[] data, int offset, int length, int x, int y );

native static synchronized void graDrawChars ( Ptr grData, char[] data, int offset, int length, int x, int y );

native static synchronized void graDrawImage( Ptr grData, Ptr imgData, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int bgval );

native static synchronized void graDrawImageScaled ( Ptr grData, Ptr imgData,
                          int dx0, int dy0, int dx1, int dy1,
                          int sx0, int sy0, int sx1, int sy1, int bgval );

native static synchronized void graDrawLine ( Ptr grData, int x1, int y1, int x2, int y2 );

native static synchronized void graDrawOval ( Ptr grData, int x, int y, int width, int height );

native static synchronized void graDrawPolygon ( Ptr grData, int[] xPoints, int[] yPoints, int nPoints );

native static synchronized void graDrawPolyline ( Ptr grData, int[] xPoints, int[] yPoints, int nPoints );

native static synchronized void graDrawRect ( Ptr grData, int x, int y, int width, int height );

native static synchronized void graDrawRoundRect ( Ptr grData, int x, int y, int width, int height,
                        int arcWidth, int arcHeight );

native static synchronized void graDrawString ( Ptr grData, String s, int x, int y );

native static synchronized void graFill3DRect ( Ptr grData, int x, int y, int width, int height,
                     boolean raised, int rgbValue );

native static synchronized void graFillArc ( Ptr grData, int x, int y, int width, int height, int startAngle, int arcAngle );

native static synchronized void graFillOval ( Ptr grData, int x, int y, int width, int height );

native static synchronized void graFillPolygon ( Ptr grData, int[] xPoints, int[] yPoints, int nPoints );

native static synchronized void graFillRect ( Ptr grData, int x, int y, int width, int height );

native static synchronized void graFillRoundRect ( Ptr grData, int x, int y, int width, int height,
                        int arcWidth, int arcHeight );

native static synchronized void graFreeGraphics ( Ptr grData );

native static synchronized Ptr graInitGraphics ( Ptr grData, Ptr tgtData, int tgtType,
                      int xOffset, int yOffset,
                      int xClip, int yClip, int wClip, int hClip,
                      Ptr fontData, int fgClr, int bgClr, boolean blank );

native static synchronized void graSetBackColor ( Ptr grData, int pixelValue );

native static synchronized void graSetClip ( Ptr grData, int xClip, int yClip, int wClip, int hClip );

native static synchronized void graSetColor ( Ptr grData, int pixelValue );

native static synchronized void graSetFont ( Ptr grData, Ptr fontData );

native static synchronized void graSetOffset ( Ptr grData, int xOffset, int yOffset );

native static synchronized void graSetPaintMode ( Ptr grData );

native static synchronized void graSetVisible ( Ptr grData, boolean isVisible );

native static synchronized void graSetXORMode ( Ptr grData, int xClr );

native static synchronized void imgComplete ( Ptr imgData, int status );

native static synchronized Ptr imgCreateFromData( byte[] buf, int offset, int len);

native static synchronized Ptr imgCreateFromFile( String gifPath);

native static synchronized Ptr imgCreateImage( int w, int h);

native static synchronized Ptr imgCreateScaledImage( Ptr imgData, int w, int h);

native static synchronized Ptr imgCreateScreenImage( int w, int h);

native static synchronized void imgFreeImage ( Ptr imgData );

native static synchronized int imgGetHeight( Ptr imgData);

native static synchronized int imgGetLatency ( Ptr imgData );

native static synchronized Ptr imgGetNextFrame ( Ptr imgData );

native static synchronized int imgGetWidth( Ptr imgData);

native static synchronized boolean imgIsMultiFrame( Ptr imgData);

native static synchronized void imgProduceImage( ImageNativeProducer prod, Ptr imgData);

native static synchronized Ptr imgSetFrame( Ptr imgData, int frame);

native static synchronized void imgSetIdxPels( Ptr imgData, int x, int y, int w, int h, int[] rgbs, byte[] pels, int trans, int off, int scans);

native static synchronized void imgSetRGBPels( Ptr imgData, int x, int y, int w, int h, int[] rgbs, int off, int scans);

static synchronized void initToolkit () {
	// this is called when the native layer has been initialized, and it is safe
	// to query native settings / rely on native functionality

	screenSize = new Dimension( tlkGetScreenWidth(), tlkGetScreenHeight());
	resolution = tlkGetResolution();

	// we do this here to keep the getDefaultToolkit() method as simple
	// as possible (since it might be called frequently). This is a
	// deviation from the normal Singleton (which initializes the singleton
	// instance upon request)
	singleton = new Toolkit();
	eventQueue = new EventQueue();

	/**
	  if ( Defaults.ConsoleClass != null ){
	  // since we have to defer the ConsoleWindow until the native Toolkit is propperly
	  // initialized, it seems to be a good idea to defuse any output to the standard streams
	  // (which might cause SEGFAULTS on some systems (e.g. DOS)
	  System.setOut( new PrintStream( NullOutputStream.singleton));
	  System.setErr( System.out);
	  }
	 **/

	if ( (flags & NATIVE_DISPATCHER_LOOP)	!= 0 ) {
		// let the world know we are ready to take over, native-wise
		Toolkit.class.notify();
	}
}

static boolean isInCreateThread () {
	if ( (flags & IS_DISPATCH_EXCLUSIVE) != 0 ){
		if ( (flags & NATIVE_DISPATCHER_LOOP) != 0 ){
			if ( Thread.currentThread() != collectorThread )
				return false;
		}
		else {
			if ( Thread.currentThread() != eventThread )
				return false;
		}
	}

	return true;
}

native static synchronized Ptr lblCreateLabel( Ptr parent, String text);

native static synchronized void lblSetJustify( Ptr data, int adjust);

native static synchronized void lblSetText( Ptr data, String text);

protected void loadSystemColors ( int[] sysColors ) {
	clrSetSystemColors( sysColors);
}

native static synchronized void lstAppendItem( Ptr data, String item);

native static synchronized Ptr lstCreateList( Ptr parent, boolean multipleMode);

native static synchronized void lstFreeze( Ptr data);

native static synchronized int[] lstGetSelectionIdxs( Ptr data);

native static synchronized int lstGetVisibleIndex( Ptr data);

native static synchronized void lstInsertItem( Ptr data, String item, int idx);

native static synchronized void lstRemoveAll( Ptr data);

native static synchronized void lstRemoveItem( Ptr data, int idx);

native static synchronized void lstReplaceItem( Ptr data, String item, int idx);

native static synchronized void lstSelectItem( Ptr data, int idx);

native static synchronized void lstSetMultipleMode( Ptr data, boolean mul);

native static synchronized void lstThaw( Ptr data);

native static synchronized void lstUnselectItem( Ptr data, int idx);

native static synchronized void menuAssign ( Ptr owner, Ptr data, int x, int y );

native static synchronized void menuCheckItem( Ptr data, MenuItem mi, boolean state );

native static synchronized Ptr menuCreateMenu( boolean popup);

native static synchronized void menuDeleteMenu ( Ptr data);

native static synchronized void menuEnableItem( Ptr data, MenuItem mi, boolean state );

native static synchronized void menuInsertItem( Ptr data, Ptr subMenu, String item, int pos, MenuItem mi);

native static synchronized void menuRemoveItem( Ptr data, MenuComponent mc );

public boolean prepareImage ( Image image, int width, int height, ImageObserver observer ) {
	return (Image.loadImage( image, width, height, observer));
}

static void redirectStreams () {
	try {
		LogClient lv = (LogClient) Class.forName( Defaults.ConsoleClass).newInstance();
		LogStream ls = new LogStream( 30, lv);
		lv.enable();

		System.setOut( new PrintStream( ls) );
		System.setErr( System.out);

		System.out.println( "Java console enabled");
	}
	catch ( Exception x ) {
		System.err.println( "unable to redirect out, err");
		x.printStackTrace();
	}
}

native static synchronized Ptr scrollCreateScrollbar( Ptr parent, boolean vert);

native static synchronized void scrollSetOrientation( Ptr data, boolean vert);

native static synchronized void scrollSetPage( Ptr data, int page);

native static synchronized void scrollSetPos( Ptr data, int pos);

native static synchronized void scrollSetRange( Ptr data, int min, int max);

native static synchronized void scrollSetValues( Ptr data, int pos, int min, int max, int page);

static synchronized void startDispatch () {
	if ( eventThread == null ) {
		eventThread = new EventDispatchThread( eventQueue);
		eventThread.start();

		// we defer the Console creation / output redirection up to this point, since we otherwise
		// might get all sort of trouble because of a incompletely initialized native layer / Toolkit
		if ( Defaults.ConsoleClass != null )
			redirectStreams();
	}

	if ( ((flags & NEEDS_FLUSH) != 0) && (flushThread == null) ){
		flushThread = new FlushThread( Defaults.GraFlushRate);
		flushThread.start();
	}
}

static synchronized void stopDispatch () {
	if ( eventThread != null ) {
		eventThread.stopDispatching();
		eventThread = null;
	}

	if ( flushThread != null ){
		flushThread.stopFlushing();
		flushThread = null;
	}
}

public void sync () {
	tlkSync();
}

static void terminate () {
	if ( clipboard != null )
		clipboard.dispose();

	stopDispatch();
	tlkTerminate();
}

native static synchronized void tlkBeep ();

native static synchronized void tlkDisplayBanner ( String banner );

native static synchronized void tlkFlush ();

native static synchronized int tlkGetMenubarHeight ();

native static synchronized int tlkGetResolution ();

native static synchronized int tlkGetScreenHeight ();

native static synchronized int tlkGetScreenWidth ();

native static boolean tlkInit ( String displayName );

native static synchronized int tlkProperties();

native static synchronized void tlkSync ();

native static synchronized void tlkTerminate ();

native static synchronized String tlkVersion ();

native static synchronized Ptr widgetCreateWidget( Ptr parent);

native static synchronized Ptr wndCreateDialog ( Ptr ownerData, String title, int x, int y, int width, int height,
                      int cursorType, int bgColor, boolean isResizable );

native static synchronized Ptr wndCreateFrame ( String title, int x, int y, int width, int height,
                     int cursorType, int bgColor, boolean isResizable );

native static synchronized Ptr wndCreateWindow ( Ptr ownerData, int x, int y, int width, int height,
                      int cursorType, int bgColor );

native static synchronized void wndDestroyWindow ( Ptr wndData );

native static synchronized void wndRepaint ( Ptr wndData, int x, int y, int width, int height );

native static synchronized void wndRequestFocus ( Ptr wndData );

native static synchronized void wndSetBounds ( Ptr wndData, int x, int y, int width, int height, boolean isResizable );

native static synchronized void wndSetCursor ( Ptr wndData, int cursorType );

native static synchronized int wndSetDialogInsets ( int top, int left, int bottom, int right);

native static synchronized int wndSetFrameInsets ( int top, int left, int bottom, int right);

native static synchronized void wndSetIcon ( Ptr wndData, Ptr iconData );

native static synchronized void wndSetResizable ( Ptr wndData, boolean isResizable,
                       int x, int y, int width, int height );

native static synchronized void wndSetTitle ( Ptr wndData, String title );

native static synchronized void wndSetVisible ( Ptr wndData, boolean showIt );

native static synchronized void wndToBack ( Ptr wndData );

native static synchronized void wndToFront ( Ptr wndData );
}
