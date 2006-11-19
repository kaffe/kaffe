package java.awt;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 * @author Bryce McKinlay
 * @author Riccardo Mottola
 */
class EventDispatchThread
  extends Thread
{
  /**
   * The default priority when no property has been set.
   */
  private static final int DEFAULT_PRIORITY = NORM_PRIORITY + 1;

  private static int dispatchThreadNum;

  private EventQueue queue;
  
  boolean stop;

  EventDispatchThread(EventQueue queue)
  {
    super();
    setName("AWT-EventQueue-" + ++dispatchThreadNum);
    this.queue = queue;

    int priority = DEFAULT_PRIORITY;
    try
      {
        String priorityString =
          System.getProperty("gnu.awt.dispatchthread.priority");
        if (priorityString != null)
          {
            priority = Integer.parseInt(priorityString); 
          }      
      }
    catch (NumberFormatException ex)
      {
        // Ignore and use default.
      }
    setPriority(priority);
  }

public void run () {
	AWTEvent e;

	while ( !stop ) {
		// the inner loop protects us from being disrupted by
		// an exception (we should continue to dispatch as long as possible)
        try
	{
	  AWTEvent evt = queue.getNextEvent();

          KeyboardFocusManager manager;
          manager = KeyboardFocusManager.getCurrentKeyboardFocusManager ();

          // Try to dispatch this event to the current keyboard focus
          // manager.  It will dispatch all FocusEvents, all
          // WindowEvents related to focus, and all KeyEvents,
          // returning true.  Otherwise, it returns false and we
          // dispatch the event normally.
          if (!manager.dispatchEvent (evt))
	    queue.dispatchEvent(evt);
	}
        catch (ThreadDeath death)
        {
          // If someone wants to kill us, let them.
          return;
        }
	catch (InterruptedException ie)
	{
	  // We are interrupted when we should finish executing
	  return;
	}
	catch (Throwable x)
	{
	  System.err.println("Exception during event dispatch:");
	  x.printStackTrace(System.err);
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
			        try
	{
	  AWTEvent evt = queue.getNextEvent();

          KeyboardFocusManager manager;
          manager = KeyboardFocusManager.getCurrentKeyboardFocusManager ();

          // Try to dispatch this event to the current keyboard focus
          // manager.  It will dispatch all FocusEvents, all
          // WindowEvents related to focus, and all KeyEvents,
          // returning true.  Otherwise, it returns false and we
          // dispatch the event normally.
          if (!manager.dispatchEvent (evt))
	    queue.dispatchEvent(evt);
	}
        catch (ThreadDeath death)
        {
          // If someone wants to kill us, let them.
          return;
        }
	catch (InterruptedException ie)
	{
	  // We are interrupted when we should finish executing
	  return;
	}
	catch (Throwable x)
	{
	  System.err.println("Exception during event dispatch:");
	  x.printStackTrace(System.err);
	}

}
}
}

public void stopDispatching () {
	stop = true;
}
}
