
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

public class Timer {
	private final TimerThread thread;

	public Timer() {
		this(false);
	}

	public Timer(boolean isDaemon) {
		thread = new TimerThread();
		thread.setDaemon(isDaemon);
	}

	public void schedule(TimerTask task, long delay) {
		scheduleDelay(task, delay, false, 0, false);
	}

	public void schedule(TimerTask task, Date time) {
		scheduleTime(task, time.getTime(), false, 0, false);
	}

	public void schedule(TimerTask task, long delay, long period) {
		scheduleDelay(task, delay, true, period, false);
	}

	public void schedule(TimerTask task, Date time, long period) {
		scheduleTime(task, time.getTime(), true, period, false);
	}

	public void scheduleAtFixedRate(TimerTask task,
			long delay, long period) {
		scheduleDelay(task, delay, true, period, true);
	}

	public void scheduleAtFixedRate(TimerTask task,
			Date time, long period) {
		scheduleTime(task, time.getTime(), true, period, true);
	}

	public void scheduleDelay(TimerTask task, long delay,
			boolean repeat, long period, boolean fixedRate) {
		if (delay < 0) {
			throw new IllegalArgumentException("delay < 0");
		}
		scheduleTime(task, System.currentTimeMillis() + delay,
			repeat, period, fixedRate);
	}

	private void scheduleTime(TimerTask task, long time,
			boolean repeat, long period, boolean fixedRate) {
		if (time < 0) {
			throw new IllegalArgumentException("time < 0");
		}
		if (period < 0) {
			throw new IllegalArgumentException("period < 0");
		}
		thread.schedule(task, this, time,
			repeat ? period : -1, fixedRate);
	}

	// This is called by TimerTask.cancel()
	boolean unschedule(TimerTask task) {
		return thread.unschedule(task);
	}

	public void cancel() {
		thread.cancel();
	}

    // When finalize() is called, we know that no TimerTasks are scheduled,
    // because if they were, they would be holding a reference to this Timer
    // object, and the thread would be holding a reference to the TimerTask,
    // and that would be keeping this object alive. So thread.cancel() is
    // appropriate to call here.

	protected void finalize() throws Throwable {
		thread.cancel();
		super.finalize();
	}

    // This inner class must not retain any references to the Timer object,
    // because that would prevent the Timer object from ever getting garbage
    // collected (unless timer.cancel() was explicitly called). When there are
    // no more references to the Timer and no tasks scheduled, the Timer is
    // supposed to become eligible for garbage collection.

	private static class TimerThread extends Thread {
		private final Comparator TCOMP = new Comparator() {
			public int compare(Object x, Object y) {
				long diff = ((TimerTask)x).time
						- ((TimerTask)y).time;
				return diff < 0 ? -1 : diff > 0 ? 1 : 0;
			}
		};
		private SortedSet tasks = new TreeSet(TCOMP);
		private boolean canceled = false;

		// Schedule a task
		public synchronized void schedule(TimerTask task, Timer timer,
				long time, long period, boolean fixedRate) {

			// Check that timer has not been canceled
			if (canceled) {
				throw new IllegalStateException(
					"timer has been canceled");
			}

			// Check that task is not already scheduled
			if (tasks.contains(task)) {
				throw new IllegalStateException(
					"task is already scheduled");
			}

			// Initialize task
			task.timer = timer;
			task.time = time;
			task.period = period;
			task.fixedRate = fixedRate;

			// Start thread if necessary
			if (!isAlive()) {
				start();
			}

			// Add task to queue
			tasks.add(task);
			task.changed = true;

			// Wake up thread if necessary
			boolean wake = false;
			try {
				wake = TCOMP.compare(task, tasks.first()) < 0;
			} catch (NoSuchElementException e) {
				wake = true;
			}
			if (wake) {
				notify();
			}
		}

		// Unschedule a task; returns true if task was scheduled
		public synchronized boolean unschedule(TimerTask task) {
			boolean removed;

			if ((removed = tasks.remove(task)) == true) {
				task.timer = null;
				task.changed = true;
			}
			return removed;
		}

		// Unschedule all tasks and cease operation
		public synchronized void cancel() {
			for (Iterator i = tasks.iterator(); i.hasNext(); ) {
				TimerTask task = (TimerTask)i.next();

				task.timer = null;	// this is important!
				task.changed = true;
			}
			tasks.clear();
			canceled = true;
			notify();
		}

		// Main thread loop
		public synchronized void run() {
			while (true) {
				long sleep;

				// Calculate how long to sleep
				try {
					sleep = ((TimerTask)tasks.first()).time
						- System.currentTimeMillis();
					if (sleep <= 0) {
						sleep = -1;	// don't sleep
					}
				} catch (NoSuchElementException e) {
					if (canceled) {
						return;		// we're done
					}
					sleep = 0;		// sleep forever
				}

				// Sleep until next task expires
				try {
					wait(sleep);
				} catch (IllegalArgumentException e) {
				} catch (InterruptedException e) {
				}

				// Service expired timers
				while (true) {
					long now = System.currentTimeMillis();
					TimerTask task;

					// Get next expiring task
					try {
						task = (TimerTask)tasks.first();
					} catch (NoSuchElementException e) {
						break;
					}
					if (task.time > now) {
						break;
					}

					// Run task and reschedule it as needed
					if (task.period == -1) {
						tasks.remove(task);
						task.timer = null;
					}
					task.changed = false;
					try {
						task.run();
					} catch (Throwable t) {
					}
					if (!task.changed
					    && task.period != -1) {
						if (!task.fixedRate) {
							task.time =
						    System.currentTimeMillis();
						}
						task.time += task.period;
						unschedule(task);
						schedule(task, task.timer,
							task.time, task.period,
							task.fixedRate);
					}
				}
			}
		}
	}
}

