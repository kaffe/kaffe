/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.lang;

import java.util.Vector;
import java.util.Enumeration;

public class MemoryAdvice implements Runnable {

public static final int GREEN = java.lang.Runtime.MemoryAdvice.GREEN;
public static final int YELLOW = java.lang.Runtime.MemoryAdvice.YELLOW;
public static final int ORANGE = java.lang.Runtime.MemoryAdvice.ORANGE;
public static final int RED = java.lang.Runtime.MemoryAdvice.RED;

public static MemoryAdvice advice = new MemoryAdvice();

private int color = GREEN;
private int hi_yellow;
private int lo_yellow;
private int hi_orange;
private int lo_orange;
private int hi_red;
private int lo_red;

private Vector callbacks;

public MemoryAdvice() {
	this(50, 75, 90, 40, 65, 80);
}

public MemoryAdvice(int yh, int oh, int rh, int yl, int ol, int rl) {
	hi_yellow = yh;
	hi_orange = oh;
	hi_red = rh;
	lo_yellow = yl;
	lo_orange = ol;
	lo_red = rl;
	register0();
}

public static MemoryAdvice getInstance() {
	return (advice);
}

public int getColor() {
	return (color);
}

public synchronized int waitForColor(int col) throws InterruptedException {
	while (col != color) {
		wait();
	}
	return (color);
}

public synchronized int waitForOtherColor(int col) throws InterruptedException {
	while (col == color) {
		wait();
	}
	return (color);
}

public synchronized void addCallback(MemoryAdviceCallback call) {
	if (callbacks == null) {
		callbacks = new Vector();
		Thread d = new Thread(this, "Memory advice dispatcher");
		d.setDaemon(true);
		d.start();
	}
	callbacks.addElement(call);
}

public synchronized void removeCallback(MemoryAdviceCallback call) {
	callbacks.removeElement(call);
}

public void run() {
	Enumeration e;
	int col;

	for (;;) {

		synchronized(this) {
			try {
				waitForOtherColor(color);
			}
			catch (InterruptedException _) {
				continue;
			}
			e = ((Vector)callbacks.clone()).elements();
			col = color;
		}
		while (e.hasMoreElements()) {
			MemoryAdviceCallback call = (MemoryAdviceCallback)e.nextElement();
			call.memoryAdvice(col);
		}
	}
}

private native void register0();

}
