package java.awt;

import java.awt.event.FocusEvent;
import java.awt.event.PaintEvent;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
public class EventQueue
{
	AWTEvent localQueue;
	AWTEvent localEnd;

static {
	// force static init of AWTEvent (need that for native event system
	// initialization)
	Class c = AWTEvent.class;
}

public EventQueue () {
}

synchronized void dropAll ( Object source ) {
	AWTEvent e = localQueue;
	AWTEvent del, last = null;

	while ( e != null ) {
		if ( e.getSource() == source ) {
			if ( localEnd == e )
				localEnd = last;

			if ( last == null )
				localQueue = e.next;
			else
				last.next = e.next;

			del = e;
			e = e.next;
			del.recycle();  // watch out - recycle mutates the 'next' field
		}
		else {
			last = e;
			e = e.next;
		}
	}
}

synchronized void dropLiveEvents ( Object source ) {
	AWTEvent e = localQueue;
	AWTEvent del, last = null;

	while ( e != null ) {
		if ( e.isLiveEventFor( source) ) {
			if ( localEnd == e )
				localEnd = last;

			if ( last == null )
				localQueue = e.next;
			else
				last.next = e.next;

			del = e;
			e = e.next;
			del.recycle();  // watch out - recycle mutates the 'next' field
		}
		else {
			last = e;
			e = e.next;
		}
	}
}

synchronized void dropPaintEvents ( Object source, int x, int y, int w, int h ) {
	AWTEvent e = localQueue;
	AWTEvent del, last = null;

	while ( e != null ) {
		if ( (e.id == PaintEvent.UPDATE) && e.isObsoletePaint( source, x, y, w, h) ) {
			if ( localEnd == e )
				localEnd = last;

	 		if ( last == null )
				localQueue = e.next;
			else
				last.next = e.next;

			del = e;
			e = e.next;
			del.recycle();  // watch out - recycle mutates the 'next' field
		}
		else {
			last = e;
			e = e.next;
		}
	}
}

synchronized void dropPendingEvents ( Component src, int id ) {
	AWTEvent e = localQueue;
	AWTEvent del, last = null;

	while ( e != null ) {
		if ( ((id != 0) && (e.id != id)) ||
		     ((src != null) && (e.getSource() != src)) ) {
			last = e;
			e = e.next;
			continue;
		}
	
		if ( localEnd == e )
			localEnd = last;

 		if ( last == null )
			localQueue = e.next;
		else
			last.next = e.next;

		del = e;
		e = e.next;
		del.recycle();  // watch out - recycle mutates the 'next' field
	}
}

public AWTEvent getNextEvent () throws InterruptedException {
	AWTEvent e;

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// We only have one source of Java events - our localQueue (every
		// native event is posted to this queue by the native layer). If
		// it is empty, we can't do anything else than go sleeping
		synchronized ( this) {
			// block until we get something in
			while ( localQueue == null ) {
				wait();
			}

			e = localQueue;
			localQueue = e.next;
			e.next = null;
			if ( e == localEnd )
				localEnd = null; // just to avoid a temp mem leak

			return e;
		}
	}
	else {
		// Slightly more complex - we have two sources of events: our local queue
		// and some native mechanism accessed via evtGetNextEvent(). We start with
		// our local queue, but don't block in case it is empty. Blocking (if any) is done
		// in evtGetNextEvent(), which means we have to create some native event traffic
		// (getting back as a null event) in case we subsequently post a event to
		// the localQueue
		while ( true ) {
			synchronized ( this ) {
				if ( localQueue != null ) {
					e = localQueue;
					localQueue = e.next;
					e.next = null;
					if ( e == localEnd )
						localEnd = null; // just to avoid a temp mem leak

					return e;
				}
			}

			// this is the sync point in case we have a blocking AWT (suspending
			// the dispatcher thread until the next event becomes available)
			if ( (e = Toolkit.evtGetNextEvent()) != null ) {
				e.next = null;
				return e;
			}
			// we don't have to check Toolkit.IS_BLOCKING here, since we reach
			// this point only in case it is not blocked, or evtGetNextEvent()
			// returned 'null'
			Thread.sleep( Defaults.EventPollingRate);
		}
	}
}

synchronized boolean hasPendingEvents ( Component c, int id ) {
	AWTEvent e;

	for ( e=localQueue; e != null; e = e.next ) {
		if ( (c != null) && (c != e.getSource()) )
			continue;
	
		if ( (id != 0) && (id != e.id) )
			continue;
	
		return true;
	}

	return false;
}

public static boolean isDispatchThread() {
	return Thread.currentThread() == Toolkit.eventThread;
}

public synchronized AWTEvent peekEvent () {
	if ( localQueue != null ){
		return localQueue;
	}
	else if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) == 0 ) {
		return Toolkit.evtPeekEvent();
	}

	return null;
}

public synchronized AWTEvent peekEvent ( int id ) {
	for ( AWTEvent e=localQueue; e != null; e = e.next ) {
		if ( e.id == id )
			return e;
	}
	
	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) == 0 )
		return Toolkit.evtPeekEventId( id);
		
	return null;
}

public synchronized void postEvent ( AWTEvent e ) {
	if ( localQueue == null ) {
		localQueue = localEnd = e;

		if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
			notify();  // wake up any waiter
		}
		else {
			// If we use blocked IO, and this is not the eventThread, wake it up by creating
			// some IO traffic. No need to do that if we have a native dispatcher loop
			if ( ((Toolkit.flags & Toolkit.IS_BLOCKING) != 0)
			     && !isDispatchThread()){
				Toolkit.evtWakeup();
			}
		}
	}
	else {
		localEnd.next = e;
		localEnd = e;
				
		// there is no need to wakeup the eventThread, since local events are
		// always processed *before* blocking on a native event inquiry (and
		// the localQueue isn't empty)
	}
}

void postFocusEvent ( FocusEvent evt ) {
	if ( evt.id == FocusEvent.FOCUS_GAINED ) {
		dropPendingEvents( null, FocusEvent.FOCUS_GAINED);
	}
	
	postEvent( evt);
}

synchronized void postPaintEvent ( int id, Component c, int x, int y, int width, int height ) {
	AWTEvent e = localQueue;

	// OK, this is pretty redundant to postEvent, but we don't want to
	// scan the localQueue twice (it might get large)
	if ( e != null ) {
		do {
			if ( (e.id == id) && ((PaintEvt)e).solicitRepaint( c, x, y, width, height) ){
				return;
			}
			if ( e.next == null ) {
				break;
			}
			else {
				e = e.next;
			}
		} while ( true );
		
		e.next = localEnd = PaintEvt.getEvent( c, id, 0, x, y, width, height);
	}
	else {
		localQueue = localEnd = PaintEvt.getEvent( c, id, 0, x, y, width, height);

		if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
			notify();  // wake up any waiter
		}
		else if ( ((Toolkit.flags & Toolkit.IS_BLOCKING) != 0) &&
			  !isDispatchThread()){
			Toolkit.evtWakeup();
		}
	}
}

  /* taken from GNU Classpath */
  public static long getMostRecentEventTime()
  {
    // XXX For now, this ONLY does the current time.
    return System.currentTimeMillis();
  }
}
