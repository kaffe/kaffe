/*
 *	ArraySet.java
 */

/*
 *  Copyright (c) 1999 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published
 *   by the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


package	org.tritonus.share;


import java.util.ArrayList;
import java.util.Collection;
import java.util.Set;



public class ArraySet
	extends		ArrayList
	implements	Set
{
	public ArraySet()
	{
		super();
	}



	public ArraySet(Collection c)
	{
		this();
		addAll(c);
	}



	public boolean add(Object element)
	{
		if (!contains(element))
		{
			super.add(element);
			return true;
		}
		else
		{
			return false;
		}
	}



	public void add(int index, Object element)
	{
		throw new UnsupportedOperationException("ArraySet.add(int index, Object element) unsupported");
	}

	public Object set(int index, Object element)
	{
		throw new UnsupportedOperationException("ArraySet.set(int index, Object element) unsupported");
	}

}



/*** ArraySet.java ***/
