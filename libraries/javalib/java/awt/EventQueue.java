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
public class EventQueue
{
	AWTEvent localQueue;

public EventQueue () {
}

public AWTEvent getNextEvent () {
	AWTEvent e;

	while ( true ) {
		synchronized ( this ) {
			if ( localQueue != null ) {
				e = localQueue;
				localQueue = e.next;
				e.next = null;
				return e;
			}
		}

		// this is the sync point in case we have a blocking AWT (suspending
		// the dispatcher thread until the next event becomes available)
		if ( (e = Toolkit.evtGetNextEvent()) != null ) {
			e.next = null;
			return e;
		}

		// we don't have to check Toolkit.isBlocking here, since we reach
		// this point only in case it is not blocked, or evtGetNextEvent()
		// returned 'null'
		if ( !AWTEvent.accelHint ) {
			try {
				Thread.sleep( Defaults.EventPollingRate);
			}
			catch ( InterruptedException x ) {
			}
		}
	}
}

public synchronized AWTEvent peekEvent () {
	if ( localQueue != null )
		return localQueue;
	else
		return Toolkit.evtPeekEvent();
}

public synchronized AWTEvent peekEvent ( int id ) {
	AWTEvent e = null;

	if ( localQueue != null ) {
		for ( e=localQueue; (e != null) && (e.id != id); e = e.next );
	}
	
	if ( e == null )
		e = Toolkit.evtPeekEventId( id);

	return e;
}

public synchronized void postEvent ( AWTEvent e ) {
	if ( localQueue == null ) {
		localQueue = e;

		// if we use blocked IO, and this is not the eventThread, wake it
		// by creating some IO traffic
		if ( Toolkit.isBlocking && (Thread.currentThread() != Toolkit.eventThread) )
			Toolkit.evtWakeup();
	}
	else {
		AWTEvent q;
		for ( q=localQueue; q.next != null; q = q.next );
		q.next = e;
		
		// there is no need to wakeup the eventThread, since local events are
		// always processed *before* blocking on a native event inquiry (and
		// the localQueue isn't empty)
	}
}

synchronized void repaint ( Component c, int x, int y, int width, int height ) {
	AWTEvent e = localQueue;

	// OK, this is pretty redundant to postEvent, but we don't want to
	// scan the localQueue twice (it might get long)
	if ( e != null ) {
		do {
			if ( (e.id == PaintEvt.REPAINT) &&
           ((PaintEvt)e).solicitRepaint( c, x, y, width, height) )
				return;
			if ( e.next == null )
				break;
			else
				e = e.next;
		} while ( true );
		
		e.next = PaintEvt.getEvent( c, PaintEvt.REPAINT, 0, x, y, width, height);
	}
	else {
		localQueue = PaintEvt.getEvent( c, PaintEvt.REPAINT, 0, x, y, width, height);
		if ( Toolkit.isBlocking && (Thread.currentThread() != Toolkit.eventThread) )
			Toolkit.evtWakeup();
	}
}
}
