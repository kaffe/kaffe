/*
 *	TSettings.java
 */

/*
 *  Copyright (c) 1999 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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


import java.security.AccessControlException;


public class TSettings
{
	public static boolean		SHOW_ACCESS_CONTROL_EXCEPTIONS = false;
	private static final String	PROPERTY_PREFIX = "tritonus.";


	public static boolean	AlsaUsePlughw = getBooleanProperty("AlsaUsePlughw");



	private static boolean getBooleanProperty(String strName)
	{
		String	strPropertyName = PROPERTY_PREFIX + strName;
		String	strValue = "false";
		try
		{
			strValue = System.getProperty(strPropertyName, "false");
		}
		catch (AccessControlException e)
		{
			if (SHOW_ACCESS_CONTROL_EXCEPTIONS)
			{
				TDebug.out(e);
			}
		}
		// TDebug.out("property: " + strPropertyName + "=" + strValue);
		boolean	bValue = strValue.toLowerCase().equals("true");
		// TDebug.out("bValue: " + bValue);
		return bValue;
	}
}



/*** TSettings.java ***/

