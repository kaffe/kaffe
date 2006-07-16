/*
 *	GlobalInfo.java
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


package	org.tritonus.share;





public class GlobalInfo
{
	private static final String	VENDOR = "Tritonus is free software. See http://www.tritonus.org/";
	private static final String	VERSION = "0.3.1";



	public static String getVendor()
	{
		return VENDOR;
	}



	public static String getVersion()
	{
		return VERSION;
	}
}



/*** GlobalInfo.java ***/

