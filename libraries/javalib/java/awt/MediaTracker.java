package java.awt;

import java.awt.image.ImageObserver;
import java.io.Serializable;

/**
 * MediaTracker - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
public class MediaTracker
  implements Serializable
{
	private MediaTrackerEntry images;
	final public static int LOADING = 1;
	final public static int ABORTED = 2;
	final public static int ERRORED = 4;
	final public static int COMPLETE = 8;
	final static int LOADED = ImageObserver.ALLBITS | ImageObserver.ABORT;

public MediaTracker(Component comp)
{
}

public void addImage(Image image, int id)
{
	addImage(image, id, -1, -1);
}

public synchronized void addImage(Image image, int id, int w, int h)
{
	MediaTrackerEntry entry = new MediaTrackerEntry(image, id, w, h);
	entry.next = images;
	images = entry;
}

public boolean checkAll()
{
	return (checkAll(false));
}

public synchronized boolean checkAll ( boolean load ) {
	for ( MediaTrackerEntry e = images; e != null; e = e.next) {
		if ((e.img.checkImage( e.w, e.h, e, load) & (ImageObserver.ALLBITS|ImageObserver.FRAMEBITS)) == 0) {
			return (false);
		}
	}
	return (true);
}

public boolean checkID(int id)
{
	return (checkID(id, false));
}

public synchronized boolean checkID ( int id, boolean load ) {
	for ( MediaTrackerEntry e = getNextEntry( id, null); e != null; e= getNextEntry( id, e) ) {
		if ( (e.img.checkImage( e.w, e.h, e, load) & (ImageObserver.ALLBITS|ImageObserver.FRAMEBITS)) == 0)
			return false;
	}
	return true;
}

public synchronized Object[] getErrorsAny()
{
	int used = 0;
	int size = 1;
	int ic;	
	Object[] array;
	final boolean load = false;

	array = new Object[size];

	for ( MediaTrackerEntry e = images; e != null; e = e.next ) {
		ic = e.img.checkImage( e.w, e.h, e, load);
			
		if ( (ic & ImageObserver.ERROR) != 0 ) {
		        // Expand the byte array if we need to make more room
		        if (used >= size) {
			    Object[] oldArray = array;
			    int new_size = size * 2;
			    
			    array = new Object[new_size];
			    System.arraycopy(oldArray, 0, array, 0, used);
			    oldArray = null;
			    
			    size = new_size;
			}
		        // Add this object to our object array
			array[used++] = e.img;
		}
	}

	if (used == 0)
	    return null;
	
	Object[] ret = new Object[used];
	System.arraycopy(ret, 0, array, 0, used);
	return ret;
}

public synchronized Object[] getErrorsID(int id)
{
    	int used = 0;
	int size = 1;
	int ic;	
	Object[] array;
	final boolean load = false;

	array = new Object[size];
	
	for ( MediaTrackerEntry e = getNextEntry( id, null); e != null; e = getNextEntry( id, e) ) {
		ic = e.img.checkImage( e.w, e.h, e, load);
			
		if ( (ic & ImageObserver.ERROR) != 0 ) {
		        // Expand the byte array if we need to make more room
		        if (used >= size) {
			    Object[] oldArray = array;
			    int new_size = size * 2;
			    
			    array = new Object[new_size];
			    System.arraycopy(oldArray, 0, array, 0, used);
			    oldArray = null;
			    
			    size = new_size;
			}

			// Add this object to our object array
			array[used++] = e.img;
		}
	}

	if (used == 0)
	    return null;
	
	Object[] ret = new Object[used];
	System.arraycopy(ret, 0, array, 0, used);
	return ret;
}

MediaTrackerEntry getNextEntry ( int id, MediaTrackerEntry prev ) {
	MediaTrackerEntry e = (prev != null) ? prev.next : images;

	for ( ; e != null; e = e.next ) {
		if ( e.id == id )
			return e;
	}
	
	return null;
}

public synchronized boolean isErrorAny()
{
	return ((statusAll(false) & ERRORED) != 0);
}

public synchronized boolean isErrorID(int id)
{
	return ((statusID(id,false) & ERRORED) != 0);	
}

public synchronized void removeImage(Image image)
{
	MediaTrackerEntry prev = null;
	MediaTrackerEntry curr = images;

	while (curr != null) {
		if (curr.img == image) {
			if (prev != null) {
				prev.next = curr.next;
			}
			else {
				images = curr.next;
			}
		}
		else {
			prev = curr;
		}
		curr = curr.next;
	}
}

public synchronized void removeImage(Image image, int id)
{
	MediaTrackerEntry prev = null;
	MediaTrackerEntry curr = images;

	while (curr != null) {
		if (curr.img == image && curr.id == id) {
			if (prev != null) {
				prev.next = curr.next;
			}
			else {
				images = curr.next;
			}
		}
		else {
			prev = curr;
		}
		curr = curr.next;
	}
}

public synchronized void removeImage(Image image, int id, int w, int h)
{
	MediaTrackerEntry prev = null;
	MediaTrackerEntry curr = images;

	while (curr != null) {
		if (curr.img == image && curr.id == id && curr.w == w && curr.h == h) {
			if (prev != null) {
				prev.next = curr.next;
			}
			else {
				images = curr.next;
			}
		}
		else {
			prev = curr;
		}
		curr = curr.next;
	}
}

public synchronized int statusAll ( boolean load ) {
	int ic, ret = 0;

	for ( MediaTrackerEntry e = images; e != null; e = e.next ) {
		ic = e.img.checkImage( e.w, e.h, e, load);

		if ( (ic & ImageObserver.ALLBITS) != 0 )
			ret |= COMPLETE;
		else if ( (ic & ImageObserver.ABORT) != 0 )
			ret |= ABORTED;
		else if ( (ic & Image.PRODUCING) != 0 )
			ret |= LOADING;
			
		if ( (ic & ImageObserver.ERROR) != 0 )
			ret |= ERRORED;
	}
	
	return ret;
}

public synchronized int statusID ( int id, boolean load ) {
	int ic, ret = 0;

	for ( MediaTrackerEntry e = getNextEntry( id, null); e != null; e = getNextEntry( id, e) ) {
		ic = e.img.checkImage( e.w, e.h, e, load);

		if ( (ic & (ImageObserver.ALLBITS|ImageObserver.FRAMEBITS)) != 0 )
			ret |= COMPLETE;
		else if ( (ic & ImageObserver.ABORT) != 0 )
			ret |= ABORTED;
		else if ( (ic & Image.PRODUCING) != 0 )
			ret |= LOADING;
			
		if ( (ic & ImageObserver.ERROR) != 0 )
			ret |= ERRORED;
	}

	return ret;

}

public synchronized void waitForAll() throws InterruptedException {
	for ( MediaTrackerEntry e = images; e != null; e = e.next ) {
		if ((e.img.checkImage( e.w, e.h, e, true) & LOADED) == 0) {
			synchronized ( e ) {
				e.wait();
			}
		}
	}
}

public synchronized boolean waitForAll ( long ms ) throws InterruptedException {
	long now = System.currentTimeMillis();
	long end = now + ms;
	
	for ( MediaTrackerEntry e = images; e != null; e = e.next ) {
		if ( now > end )
			return false;
	
		if ((e.img.checkImage( e.w, e.h, e, true) & LOADED) == 0) {
			synchronized ( e ) {
				e.wait( ms);
			}
		}
		now = System.currentTimeMillis();
	}

	return true;
}

public synchronized void waitForID ( int id ) throws InterruptedException {
	for ( MediaTrackerEntry e = getNextEntry( id, null); e != null; e = getNextEntry( id, e) ) {
		if ((e.img.checkImage( e.w, e.h, e, true) & LOADED) == 0) {
			synchronized ( e ) {
				e.wait();
			}
		}
	}
}

public synchronized boolean waitForID ( int id, long ms) throws InterruptedException {
	long now = System.currentTimeMillis();
	long end = now + ms;
	
	for (MediaTrackerEntry e = getNextEntry( id, null); e != null; e = getNextEntry( id, e) ) {
		if ( now > end ) {
			return false;
		}
	
		if ((e.img.checkImage(e.w, e.h, e, true) & (ImageObserver.FRAMEBITS|ImageObserver.ALLBITS)) == 0) {
			synchronized ( e ) {
				e.wait( ms);
			}
		}
		now = System.currentTimeMillis();
	}

	return true;
}
}

class MediaTrackerEntry
  implements ImageObserver
{
	Image img;
	int id;
	int w;
	int h;
	MediaTrackerEntry next;

MediaTrackerEntry(Image img, int id, int w, int h)
{
	this.img = img;
	this.id = id;
	this.w = w;
	this.h = h;
}

public synchronized boolean imageUpdate ( Image img, int infoflags, int x, int y, int width, int height ) {
	if ( (infoflags & (ImageObserver.WIDTH | ImageObserver.HEIGHT )) != 0 ) {
		w = width;
		h = height;
	}
	if ( (infoflags & (ALLBITS | FRAMEBITS | ABORT)) != 0 ) {
		notify();
		return (false);
	}
	
	return (true);
}
}
