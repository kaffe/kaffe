/*
 *	EnumControl.java
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


package	javax.sound.sampled;

import java.util.Arrays;
import java.util.Collection;

import org.tritonus.share.TDebug;



public abstract class EnumControl
	extends		Control
{
	private static final Object[]	EMPTY_OBJECT_ARRAY = new Object[0];



	private Object		m_value;
	private Collection	m_values;



	protected EnumControl(Type type,
			      Object[]	aValues,
			      Object value)
	{
		super(type);
		if (TDebug.TraceControl)
		{
			TDebug.out("EnumControl.<init>: begin");
		}
		m_values = Arrays.asList(aValues);
		setValue(value);
		if (TDebug.TraceControl)
		{
			TDebug.out("EnumControl.<init>: end");
		}
	}



	public void setValue(Object value)
	{
		if (m_values.contains(value))
		{
			m_value = value;
		}
		else
		{
			throw new IllegalArgumentException("illegal value " + value);
		}
	}



	public Object getValue()
	{
		return m_value;
	}



	public Object[] getValues()
	{
		return (Object[]) m_values.toArray(EMPTY_OBJECT_ARRAY);
	}



	public String toString()
	{
		return super.toString() + " [value = " + getValue() + "]";
	}




	public static class Type
		extends	Control.Type
	{
		public static final Type	REVERB = new Type("REVERB");



		protected Type(String strName)
		{
			super(strName);
		}
	}
}



/*** EnumControl.java ***/

