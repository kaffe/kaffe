/*
 *	SoundbankResource.java
 */

/*
 *  Copyright (c) 2000 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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



public abstract class SoundbankResource
{
	private Soundbank	m_soundbank;
	private String		m_strName;
	private Class		m_dataClass;



	protected SoundbankResource(Soundbank soundbank,
				    String strName,
				    Class dataClass)
	{
		m_soundbank = soundbank;
		m_strName = strName;
	}


	public Soundbank getSoundbank()
	{
		return m_soundbank;
	}



	public String getName()
	{
		return m_strName;
	}



	public Class getDataClass()
	{
		return m_dataClass;
	}



	public abstract Object getData();
}



/*** SoundbankResource.java ***/
