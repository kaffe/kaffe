package java.awt;

import gnu.classpath.RawData;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.Transferable;
import java.awt.dnd.DragGestureListener;
import java.awt.dnd.DragGestureRecognizer;
import java.awt.dnd.DragSource;
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

import kaffe.awt.DoNothingPeer;
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
	static ColorModel colorModel;
	static WindowPeer windowPeer = new DoNothingPeer();
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
	final String AWT_NATIVE_LIBRARY = System.getProperty("kaffe.awt.nativelib");
	System.loadLibrary(AWT_NATIVE_LIBRARY);
	flags = tlkProperties();

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
				while ( singleton == null )
					Toolkit.class.wait();
			}
		}
		catch ( Exception x ) {
			x.printStackTrace();
		}
	}
}

public Toolkit () {
}

public void beep () {
	tlkBeep();
}

native static synchronized void cbdFreeClipboard ( RawData cbdData );

native static synchronized Transferable cbdGetContents( RawData cbdData);

native static synchronized RawData cbdInitClipboard ();

native static synchronized boolean cbdSetOwner ( RawData cbdData );

public int checkImage(Image image, int width, int height, ImageObserver observer) {
	return (image.checkImage( width, height, observer, false));
}

native static synchronized long clrBright ( int rgbValue );

native static synchronized long clrDark ( int rgbValue );

native static synchronized ColorModel clrGetColorModel();

native static synchronized int clrGetPixelValue ( int rgb );

native static synchronized int clrSetSystemColors ( int[] sysClrs );

public Image createImage ( ImageProducer producer ) {
	return new Image( producer);
}

public Image createImage ( byte[] imageData ) {
	return createImage( imageData, 0, imageData.length);
}

public Image createImage ( byte[] imagedata, int imageoffset, int imagelength ) {
	return new Image( imagedata, imageoffset, imagelength);
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
			if ( !EventQueue.isDispatchThread()) {
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
					try { e.wait(); } catch ( InterruptedException x ) {}
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
			if ( !EventQueue.isDispatchThread()) {
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

native static synchronized AWTEvent evtGetNextEvent ();

native static synchronized Component[] evtInit ();

native static synchronized AWTEvent evtPeekEvent ();

native static synchronized AWTEvent evtPeekEventId ( int eventId );

native static synchronized int evtRegisterSource ( RawData wndData );

native static synchronized void evtSendWMEvent ( WMEvent e );

native static synchronized int evtUnregisterSource ( RawData wndData );

native static synchronized void evtWakeup ();

native static synchronized int fntBytesWidth ( RawData fmData, byte[] data, int off, int len );

native static synchronized int fntCharWidth ( RawData fmData, char c );

native static synchronized int fntCharsWidth ( RawData fmData, char[] data, int off, int len );

native static synchronized void fntFreeFont ( RawData fontData );

native static synchronized void fntFreeFontMetrics ( RawData fmData );

native static synchronized int fntGetAscent ( RawData fmData);

native static synchronized int fntGetDescent ( RawData fmData);

native static synchronized int fntGetFixedWidth ( RawData fmData);

native static synchronized int fntGetHeight ( RawData fmData);

native static synchronized int fntGetLeading ( RawData fmData);

native static synchronized int fntGetMaxAdvance ( RawData fmData);

native static synchronized int fntGetMaxAscent ( RawData fmData);

native static synchronized int fntGetMaxDescent ( RawData fmData);

native static synchronized int[] fntGetWidths ( RawData fmData );

native static synchronized RawData fntInitFont ( String fntSpec, int style, int size );

native static synchronized RawData fntInitFontMetrics ( RawData fontData);

native static synchronized boolean fntIsWideFont ( RawData fmData);

native static synchronized int fntStringWidth ( RawData fmData, String s );

public ColorModel getColorModel () {
	if ( colorModel == null ){
		colorModel = clrGetColorModel();
	}

	return colorModel;
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
	File f = new File( filename);
	
	// Hmm, that's a inconsistency with getImage(URL). The doc isn't very
	// helpful, here (class doc says nothing, book tells us it should return
	// null in case it's not there - but that is not known a priori for URLs).
	// JDK never returns a null object, so we don't do, either
	return (f.exists()) ? new Image( f) : Image.getUnknownImage();
}

public Image getImage ( URL url ) {
	// how can we return 'null' before we start to produce this thing (which
	// isn't supposed to happen here)? We are NOT going to open the URLConnection
	// twice just to check if it is there
	return new Image( url);
}

public int getMenuShortcutKeyMask() {
	return InputEvent.CTRL_MASK;
}

public PrintJob getPrintJob ( Frame frame, String jobtitle, Properties props ) {
	return new PSPrintJob( frame, jobtitle, props);
}

public static String getProperty ( String key, String defaultValue ) {
	return null;
}

public int getScreenResolution() {
	return resolution;
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

native static synchronized void graAddClip ( RawData grData, int xClip, int yClip, int wClip, int hClip );

native static synchronized void graClearRect ( RawData grData, int x, int y, int width, int height );

native static synchronized void graCopyArea ( RawData grData, int x, int y, int width, int height,
					   int xDelta, int yDelta );

native static synchronized void graDraw3DRect ( RawData grData, int x, int y, int width, int height,
		     boolean raised, int rgbValue );

native static synchronized void graDrawArc ( RawData grData, int x, int y, int width, int height, int startAngle, int arcAngle );

native static synchronized void graDrawBytes ( RawData grData, byte[] data, int offset, int length, int x, int y );

native static synchronized void graDrawChars ( RawData grData, char[] data, int offset, int length, int x, int y );

native static synchronized void graDrawImage( RawData grData, RawData imgData, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int bgval );

native static synchronized void graDrawImageScaled ( RawData grData, RawData imgData,
			  int dx0, int dy0, int dx1, int dy1,
			  int sx0, int sy0, int sx1, int sy1, int bgval );

native static synchronized void graDrawLine ( RawData grData, int x1, int y1, int x2, int y2 );

native static synchronized void graDrawOval ( RawData grData, int x, int y, int width, int height );

native static synchronized void graDrawPolygon ( RawData grData, int[] xPoints, int[] yPoints, int nPoints );

native static synchronized void graDrawPolyline ( RawData grData, int[] xPoints, int[] yPoints, int nPoints );

native static synchronized void graDrawRect ( RawData grData, int x, int y, int width, int height );

native static synchronized void graDrawRoundRect ( RawData grData, int x, int y, int width, int height,
						int arcWidth, int arcHeight );

native static synchronized void graDrawString ( RawData grData, String s, int x, int y );

native static synchronized void graFill3DRect ( RawData grData, int x, int y, int width, int height,
		     boolean raised, int rgbValue );

native static synchronized void graFillArc ( RawData grData, int x, int y, int width, int height, int startAngle, int arcAngle );

native static synchronized void graFillOval ( RawData grData, int x, int y, int width, int height );

native static synchronized void graFillPolygon ( RawData grData, int[] xPoints, int[] yPoints, int nPoints );

native static synchronized void graFillRect ( RawData grData, int x, int y, int width, int height );

native static synchronized void graFillRoundRect ( RawData grData, int x, int y, int width, int height,
						int arcWidth, int arcHeight );

native static synchronized void graFreeGraphics ( RawData grData );

native static synchronized RawData graInitGraphics ( RawData grData, RawData tgtData, int tgtType,
					      int xOffset, int yOffset,
					      int xClip, int yClip, int wClip, int hClip,
					      RawData fontData, int fgClr, int bgClr, boolean blank );

native static synchronized void graSetBackColor ( RawData grData, int pixelValue );

native static synchronized void graSetClip ( RawData grData, int xClip, int yClip, int wClip, int hClip );

native static synchronized void graSetColor ( RawData grData, int pixelValue );

native static synchronized void graSetFont ( RawData grData, RawData fontData );

native static synchronized void graSetOffset ( RawData grData, int xOffset, int yOffset );

native static synchronized void graSetPaintMode ( RawData grData );

native static synchronized void graSetVisible ( RawData grData, boolean isVisible );

native static synchronized void graSetXORMode ( RawData grData, int xClr );

native static synchronized void imgComplete ( RawData imgData, int status );

native static synchronized RawData imgCreateFromData( byte[] buf, int offset, int len);

native static synchronized RawData imgCreateFromFile( String gifPath);

native static synchronized RawData imgCreateImage( int w, int h);

native static synchronized RawData imgCreateScaledImage( RawData imgData, int w, int h);

native static synchronized RawData imgCreateScreenImage( int w, int h);

native static synchronized void imgFreeImage ( RawData imgData );

native static synchronized int imgGetHeight( RawData imgData);

native static synchronized int imgGetLatency ( RawData imgData );

native static synchronized RawData imgGetNextFrame ( RawData imgData );

native static synchronized int imgGetWidth( RawData imgData);

native static synchronized boolean imgIsMultiFrame( RawData imgData);

native static synchronized void imgProduceImage( ImageNativeProducer prod, RawData imgData);

native static synchronized RawData imgSetFrame( RawData imgData, int frame);

native static synchronized void imgSetIdxPels( RawData imgData, int x, int y, int w, int h, int[] rgbs, byte[] pels, int trans, int off, int scans);

native static synchronized void imgSetRGBPels( RawData imgData, int x, int y, int w, int h, int[] rgbs, int off, int scans);

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

protected void loadSystemColors ( int[] sysColors ) {
	clrSetSystemColors( sysColors);
}

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

  /* taken from GNU Classpath */

  public DragGestureRecognizer
    createDragGestureRecognizer(Class recognizer, DragSource ds,
                                Component comp, int actions,
                                DragGestureListener l)
  {
    return null;
  }

native static synchronized void tlkBeep ();

native static synchronized void tlkDisplayBanner ( String banner );

native static synchronized void tlkFlush ();

native static synchronized int tlkGetResolution ();

native static synchronized int tlkGetScreenHeight ();

native static synchronized int tlkGetScreenWidth ();

native static boolean tlkInit ( String displayName );

native static synchronized int tlkProperties();

native static synchronized void tlkSync ();

native static synchronized void tlkTerminate ();

native static synchronized String tlkVersion ();

native static synchronized RawData wndCreateDialog ( RawData ownerData, String title, int x, int y, int width, int height,
		      int cursorType, int bgColor, boolean isResizable );

native static synchronized RawData wndCreateFrame ( String title, int x, int y, int width, int height,
		     int cursorType, int bgColor, boolean isResizable );

native static synchronized RawData wndCreateWindow ( RawData ownerData, int x, int y, int width, int height,
		      int cursorType, int bgColor );

native static synchronized void wndDestroyWindow ( RawData wndData );

native static synchronized void wndRepaint ( RawData wndData, int x, int y, int width, int height );

native static synchronized void wndRequestFocus ( RawData wndData );

native static synchronized void wndSetBounds ( RawData wndData, int x, int y, int width, int height, boolean isResizable );

native static synchronized void wndSetCursor ( RawData wndData, int cursorType );

native static synchronized int wndSetDialogInsets ( int top, int left, int bottom, int right);

native static synchronized int wndSetFrameInsets ( int top, int left, int bottom, int right);

native static synchronized void wndSetIcon ( RawData wndData, RawData iconData );

native static synchronized void wndSetResizable ( RawData wndData, boolean isResizable,
		       int x, int y, int width, int height );

native static synchronized void wndSetTitle ( RawData wndData, String title );

native static synchronized void wndSetVisible ( RawData wndData, boolean showIt );

native static synchronized void wndToBack ( RawData wndData );

native static synchronized void wndToFront ( RawData wndData );
}
