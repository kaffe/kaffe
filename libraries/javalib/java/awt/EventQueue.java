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
		if ( (e = Toolkit.evtGetNextEvent()) != null ) {
			e.next = null;
			return e;
		}

		if ( !AWTEvent.accelHint && !Toolkit.isMultiThreaded ) {
			try {
				Thread.sleep( Defaults.EventPollingRate);
			}
			catch ( InterruptedException x ) {
			}
		}
	}
}

public AWTEvent peekEvent () {
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
	if ( localQueue == null )
		localQueue = e;
	else {
		AWTEvent q;
		for ( q=localQueue; q.next != null; q = q.next );
		q.next = e;
	}
		
	if ( Toolkit.isWrongThread() )
		Toolkit.evtWakeup();
}
}
