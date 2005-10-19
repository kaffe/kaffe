/*
 *	TCompoundControl.java
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


// import	java.util.Collection;
// import	java.util.ArrayList;
// import	java.util.HashSet;
// import	java.util.Iterator;
// import	java.util.List;
// import	java.util.Set;

// import	javax.sound.sampled.AudioSystem;
import javax.sound.sampled.CompoundControl;
import javax.sound.sampled.Control;

import org.tritonus.share.TDebug;




/**	Base class for classes implementing Line.
 */
public class TCompoundControl
	extends CompoundControl
	implements TControllable
{
	private TControlController	m_controller;



	public TCompoundControl(CompoundControl.Type type,
				Control[] aMemberControls)
	{
		super(type, aMemberControls);
		if (TDebug.TraceControl)
		{
			TDebug.out("TCompoundControl.<init>: begin");
		}
		m_controller = new TControlController();
		if (TDebug.TraceControl)
		{
			TDebug.out("TCompoundControl.<init>: end");
		}
	}



	public void setParentControl(TCompoundControl compoundControl)
	{
		m_controller.setParentControl(compoundControl);
	}



	public TCompoundControl getParentControl()
	{
		return m_controller.getParentControl();
	}



	public void commit()
	{
		m_controller.commit();
	}
}



/*** TCompoundControl.java ***/
