/*
 *	ReverbType.java
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


package	javax.sound.sampled;





public class ReverbType
{
	private String		m_strName;
	private int		m_nEarlyReflectionDelay;
	private float		m_fEarlyReflectionIntensity;
	private int		m_nLateReflectionDelay;
	private float		m_fLateReflectionIntensity;
	private int		m_nDecayTime;



	protected ReverbType(String strName,
			     int nEarlyReflectionDelay,
			     float fEarlyReflectionIntensity,
			     int nLateReflectionDelay,
			     float fLateReflectionIntensity,
			     int nDecayTime)
	{
		m_strName = strName;
		m_nEarlyReflectionDelay = nEarlyReflectionDelay;
		m_fEarlyReflectionIntensity = fEarlyReflectionIntensity;
		m_nLateReflectionDelay = nLateReflectionDelay;
		m_fLateReflectionIntensity = fLateReflectionIntensity;
		m_nDecayTime = nDecayTime;
	}



	public String getName()
	{
		return m_strName;
	}



	public int getEarlyReflectionDelay()
	{
		return m_nEarlyReflectionDelay;
	}



	public float getEarlyReflectionIntensity()
	{
		return m_fEarlyReflectionIntensity;
	}



	public int getLateReflectionDelay()
	{
		return m_nLateReflectionDelay;
	}



	public float getLateReflectionIntensity()
	{
		return m_fLateReflectionIntensity;
	}



	public int getDecayTime()
	{
		return m_nDecayTime;
	}



	public final boolean equals(Object obj)
	{
		return super.equals(obj);
	}



	public final int hashCode()
	{
		return hashCode();
	}



	public final String toString()
	{
		return super.toString() +
			"[name=" + getName() +
			", earlyReflectionDelay = " + getEarlyReflectionDelay() +
			", earlyReflectionIntensity = " + getEarlyReflectionIntensity() +
			", lateReflectionDelay = " + getLateReflectionDelay() +
			", lateReflectionIntensity = " + getLateReflectionIntensity() +
			", decayTime = " + getDecayTime() + "]";
	}
}



/*** ReverbType.java ***/
