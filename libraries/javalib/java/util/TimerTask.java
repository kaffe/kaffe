
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.util;

public abstract class TimerTask implements Runnable {
	Timer timer;			// non-null IFF currently scheduled
	boolean fixedRate;		// fixed-rate or fixed-delay
	boolean changed;		// true if run() changes timer or task
	long period;			// repeat period (or -1 for none)
	long time;			// time of next expiration

	public TimerTask() {
	}

	public abstract void run();

	public boolean cancel() {
		try {
			return timer.unschedule(this);
		} catch (NullPointerException e) {
			return false;
		}
	}

}

