package java.awt;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
class EventDispatchThread
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
				Toolkit.tlkBeep();
				sx.printStackTrace( System.err);
			}
		}
		catch ( Throwable x ) {
			Toolkit.tlkBeep();
			x.printStackTrace( System.err);
		}
	}
}

void run ( Component modalWindow ) {
	AWTEvent e;

	// check if we are outside of the dispatcher thread (to prevent 
	// race conditions)
	if ( !EventQueue.isDispatchThread()) {
		e = WMEvent.getEvent( modalWindow, WMEvent.WM_DISPATCH_MODAL);
		Toolkit.eventQueue.postEvent( e);
		synchronized ( e ) {
			while ( !e.consumed ) {
				try { e.wait(); } catch ( InterruptedException x ) {}
			}
		}
	}
	else {
		while ( !stop ) {
			// the inner loop protects us from being disrupted by
			// an exception (we should continue to dispatch as long as possible)
			try {
				while ( !stop ) {			
					if ( (e = queue.getNextEvent()) != null ){
						e.dispatch();
						// this is better than to rely on a WINDOW_CLOSED, since we can
						// save postEvents AND make dispatching faster
						if ( (modalWindow.flags & Component.IS_ADD_NOTIFIED) == 0 ){
							return;
						}
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

public void stopDispatching () {
	stop = true;
}
}
