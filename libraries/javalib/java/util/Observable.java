
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

public class Observable {
	private final List observers;
	private boolean changed;

	public Observable() {
		observers = new ArrayList();
		changed = false;
	}

	public synchronized void addObserver(Observer o) {
		observers.add(o);
	}

	public synchronized void deleteObserver(Observer o) {
		int index = observers.indexOf(o);
		if (index != -1) {
			observers.remove(index);
		}
	}

	public void notifyObservers() {
		notifyObservers(null);
	}

	public synchronized void notifyObservers(Object arg) {
		if (changed) {
			for (Iterator i = observers.listIterator();
			    i.hasNext(); ) {
				((Observer)i.next()).update(this, arg);
			}
			clearChanged();
		}
	}

	public synchronized void deleteObservers() {
		observers.clear();
	}

	protected void setChanged() {
		changed = true;
	}

	protected void clearChanged() {
		changed = false;
	}

	public boolean hasChanged() {
		return changed;
	}

	public synchronized int countObservers() {
		return observers.size();
	}
}

