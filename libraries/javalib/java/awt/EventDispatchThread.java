package java.awt;

import java.awt.event.PaintEvent;
import java.awt.event.WindowEvent;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
public class EventDispatchThread
  extends Thread
{
	boolean stop;
	EventQueue queue;

EventDispatchThread ( EventQueue queue ) {
	super( "AWT-EventQueue-0" ); // some apps depend on this JDK thread name

	this.queue = queue;

	setPriority( Thread.NORM_PRIORITY + 1);
}

public void run () {
	AWTEvent e;

	while ( !stop ) {
		// the inner loop protects us from being disrupted by
		// an exception (we should continue to dispatch as long as possible)
		try {
			while ( !stop ) {			
				if ( (e = queue.getNextEvent()) != null ){
					e.dispatch();
				}
			}
		}
		catch ( SecurityException sx ) {
			if ( "system_exit".equals( sx.getMessage()) ) {
				// this is from our KaffeServer SecurityManager, ignore
			}
			else {
				sx.printStackTrace( System.err);
			}
		}
		catch ( Throwable x ) {
			x.printStackTrace( System.err);
		}
	}
}

void run ( Window modalWindow ) {
	AWTEvent e;

	// check if we are outside of the dispatcher thread (to prevent 
	// race conditions)
	if ( Thread.currentThread() != Toolkit.eventThread ) {
		e = new WMEvent( modalWindow, WMEvent.WM_DISPATCH_MODAL);
		Toolkit.eventQueue.postEvent( e);
		synchronized ( e ) { // no need to loop, nobody else knows 'e'
			try { e.wait(); } catch ( InterruptedException x ) {}
		}
	}
	else {
		while ( !stop ) {
			// the inner loop protects us from being disrupted by
			// an exception (we should continue to dispatch as long as possible)
			try {
				while ( !stop ) {			
					if ( (e = queue.getNextEvent()) != null ){
						if ( (e.id == WindowEvent.WINDOW_CLOSED) &&
						     (AWTEvent.getSource( e) == modalWindow) ){
							e.dispatch();      // e.source is nulled after dispatch() !
							return;
						}
						else
							e.dispatch();
					}
				}
			}
			catch ( SecurityException sx ) {
				if ( "system_exit".equals( sx.getMessage()) ) {
					// this is from our KaffeServer SecurityManager, ignore
				}
				else {
					sx.printStackTrace( System.err);
				}
			}
			catch ( Throwable x ) {
				x.printStackTrace( System.err);
			}
		}
	}
}

void show ( Window window ) {
	AWTEvent e;

	// check if we are outside of the dispatcher thread (to prevent 
	// race conditions)
	if ( Thread.currentThread() != Toolkit.eventThread ) {
		e = new WMEvent( window, WMEvent.WM_SHOW);
		Toolkit.eventQueue.postEvent( e);
		synchronized ( e ) { // no need to loop, nobody else knows 'e'
			try { e.wait(); } catch ( InterruptedException x ) {}
		}
	}
	else {
		// this has to be done here to make sure we are in the right thread
		// (otherwise there is a good chance that we get a paint in the dispatcher
		// thread before we start to wait for it)
		Toolkit.wndSetVisible( window.nativeData, true);
	
		while ( !stop ) {
			// the inner loop protects us from being disrupted by
			// an exception (we should continue to dispatch as long as possible)
			try {
				while ( !stop ) {			
					if ( (e = queue.getNextEvent()) != null ){
						if ( (e.id == PaintEvent.PAINT)  &&
						     (AWTEvent.getSource( e) == window)  ){
							e.dispatch();      // e.source is nulled after dispatch() !
							return;
						}
						else
							e.dispatch();
					}
				}
			}
			catch ( Throwable x ) {
				x.printStackTrace( System.err);
			}
		}
	}
}

public void stopDispatching () {
	stop = true;
}
}
