/* Buffer.java -- 
   Copyright (C) 2002 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

package java.nio;


public abstract class Buffer
{
    int cap, limit, pos, mark;

    Buffer()
    {
    }

    public final int capacity()
    {
	return cap;
    }

    public final int capacity(int c)
    {
	int old = cap;
	cap = c;
	return old;
    }

    public final Buffer clear()
    {
	limit = cap;
	mark = 0;
	pos = 0;
	return this;
    }
    
    public final Buffer flip()
    {
	limit = pos;
	pos = 0;

	mark = 0;

	return this;
    }
    
    public final boolean hasRemaining()
    {
	return limit > pos;
    }

    public abstract  boolean isReadOnly();    
    
    
    public final int limit()
    {
	return limit;
    }

    public final Buffer limit(int newLimit)
    {
	if (newLimit <= mark)
	    mark = 0;

	if (pos > newLimit)
	    pos = newLimit - 1;

	limit = newLimit;
	return this;
    }

    public final Buffer mark()
    {
	mark = pos;
	return this;
    }

    public final int position()
    {
	return pos;
    }
    

    public final Buffer position(int newPosition)
    {
	/// If the mark is defined and larger than the new 

	if (newPosition <= mark)
	    mark = 0;

	pos = newPosition;
	return this;
    }

    public final int remaining()
    {
	return limit - pos;
    }

    public final Buffer reset()
    {
	pos = mark;
	return this;
    }

    public final Buffer rewind()
    {
	mark = pos = 0;
	return this;
    }
}
