/*
 *	Patch.java
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




public class Patch
{
	private int		m_nBank;
	private int		m_nProgram;



	public Patch(int nBank, int nProgram)
	{
		m_nBank = nBank;
		m_nProgram = nProgram;
	}



	public int getBank()
	{
		return m_nBank;
	}



	public int getProgram()
	{
		return m_nProgram;
	}



}



/*** Patch.java ***/
