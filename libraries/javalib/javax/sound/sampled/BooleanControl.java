/*
 *	BooleanControl.java
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


import org.tritonus.share.TDebug;



public abstract class BooleanControl
	extends		Control
{
	private static final String	DEFAULT_TRUE_LABEL = "true";
	private static final String	DEFAULT_FALSE_LABEL = "false";



	private boolean		m_bValue;
	private String		m_strTrueLabel;
	private String		m_strFalseLabel;



	protected BooleanControl(Type type,
				 boolean bInitialValue,
				 String strTrueLabel,
				 String strFalseLabel)
	{
		super(type);

		if (TDebug.TraceControl)
		{
			TDebug.out("BooleanControl.<init>: begin");
		}
		/* $$mp 2001-02-06: TODO: what's the matter with this?
		 */
		//$$fb 2000-12-02: incompatible with Sun implementation...
		//setValue(bInitialValue);
		m_bValue = bInitialValue;

		m_strTrueLabel = strTrueLabel;
		m_strFalseLabel = strFalseLabel;
		if (TDebug.TraceControl)
		{
			TDebug.out("BooleanControl.<init>: end");
		}
	}



	protected BooleanControl(Type type,
				 boolean bInitialValue)
	{
		this(type,
		     bInitialValue,
		     DEFAULT_TRUE_LABEL,
		     DEFAULT_FALSE_LABEL);
	}



	public void setValue(boolean bValue)
	{
		m_bValue = bValue;
	}



	public boolean getValue()
	{
		return m_bValue;
	}



	public String getStateLabel(boolean bState)
	{
		if (bState)
		{
			return m_strTrueLabel;
		}
		else
		{
			return m_strFalseLabel;
		}
	}



	public String toString()
	{
		return super.toString() + " state = " + getStateLabel(getValue());
	}





	public static class Type
		extends	Control.Type
	{
		public static final Type	MUTE = new Type("MUTE");
		public static final Type	APPLY_REVERB = new Type("APPLY_REVERB");



		protected Type(String strName)
		{
			super(strName);
		}
	}
}



/*** BooleanControl.java ***/

