/*
 *	Alsa.java
 */

/*
 *  Copyright (c) 2000 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@web.de>
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


package	org.tritonus.lowlevel.alsa;


import org.tritonus.share.TDebug;


/**	Common ALSA functions.
	Used only for the functions that do neither belong to the
	ctl sections nor to any specific interface section
	(like pcm, rawmidi, etc.).
	Currently, there is only one function remaining.
 */
public class Alsa
{
	private static boolean	sm_bIsLibraryAvailable = false;



	static
	{
		Alsa.loadNativeLibrary();
	}



	public static void loadNativeLibrary()
	{
		if (TDebug.TraceAlsaNative) { TDebug.out("Alsa.loadNativeLibrary(): begin"); }

		if (! isLibraryAvailable())
		{
			loadNativeLibraryImpl();
		}
		if (TDebug.TraceAlsaNative) { TDebug.out("Alsa.loadNativeLibrary(): end"); }
	}



	/** Load the native library for alsa.

	This method actually does the loading of the library.  Unlike
	{@link loadNativeLibrary() loadNativeLibrary()}, it does not
	check if the library is already loaded.

	 */
	private static void loadNativeLibraryImpl()
	{
		if (TDebug.TraceAlsaNative) { TDebug.out("Alsa.loadNativeLibraryImpl(): loading native library tritonusalsa"); }
		try
		{
			System.loadLibrary("tritonusalsa");
			// only reached if no exception occures
			sm_bIsLibraryAvailable = true;
		}
		catch (Error e)
		{
			if (TDebug.TraceAlsaNative ||
			    TDebug.TraceAllExceptions)
			{
				TDebug.out(e);
			}
			// throw e;
		}
		if (TDebug.TraceAlsaNative) { TDebug.out("Alsa.loadNativeLibraryImpl(): loaded"); }
	}



	/**	Returns whether the libraries are installed correctly.
	 */
	public static boolean isLibraryAvailable()
	{
		return sm_bIsLibraryAvailable;
	}



	public static native String getStringError(int nErrnum);
}



/*** Alsa.java ***/
