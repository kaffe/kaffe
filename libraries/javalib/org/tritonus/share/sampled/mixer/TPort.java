/*
 *	TPort.java
 *
 *	This file is part of Tritonus,
 *	an implementation of the Java Sound API.
 */

/*
 *  Copyright (c) 1999, 2000 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


import java.util.Collection;

import javax.sound.sampled.Line;
import javax.sound.sampled.Port;




/**	Base class for Ports.
 */
public class TPort
	extends TLine
	implements	Port
{
	public TPort(TMixer mixer,
			Line.Info info)
	{
		super(mixer, info);
	}



	public TPort(TMixer mixer,
			Line.Info info,
			Collection controls)
	{
		super(mixer, info, controls);
	}
}



/*** TPort.java ***/
