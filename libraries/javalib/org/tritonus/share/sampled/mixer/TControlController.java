/*
 *	TControlController.java
 *
 *	This file is part of Tritonus,
 *	an implementation of the Java Sound API.
 */

/*
 *  Copyright (c) 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


package	org.tritonus.share.sampled.mixer;


import org.tritonus.share.TDebug;




/**	Base class for classes implementing Line.
 */
public class TControlController
implements TControllable
{
	/**	The parent (compound) control.
		In case this control is part of a compound control, the parentControl
		property is set to a value other than null.
	 */
	private TCompoundControl	m_parentControl;


	public TControlController()
	{
	}



	public void setParentControl(TCompoundControl compoundControl)
	{
		m_parentControl = compoundControl;
	}


	public TCompoundControl getParentControl()
	{
		return m_parentControl;
	}


	public void commit()
	{
		if (TDebug.TraceControl)
		{
			TDebug.out("TControlController.commit(): called [" + this.getClass().getName() + "]");
		}
		if (getParentControl() != null)
		{
			getParentControl().commit();
		}
	}
}



/*** TControlController.java ***/
