/*
 *	Instrument.java
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


package	javax.sound.midi;




public abstract class Instrument
	extends	SoundbankResource
{
	private Patch		m_patch;



	protected Instrument(Soundbank soundbank, Patch patch, String strName, Class dataClass)
	{
		super(soundbank, strName, dataClass);
		m_patch = patch;
	}



	public Patch getPatch()
	{
		return m_patch;
	}
}



/*** Instrument.java ***/
