/*
 *	Sequence.java
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


package	javax.sound.midi;

import java.util.Iterator;
import java.util.Vector;


// TODO: some synchronization now obsolete? (after change from ArrayList to Vector)
public class Sequence
{
	public static final float	PPQ = 0.0F;
	public static final float	SMPTE_24 = 24.0F;
	public static final float	SMPTE_25 = 25.0F;
	public static final float	SMPTE_30DROP = 29.97F;
	public static final float	SMPTE_30 = 30.0F;

	private static final Track[]	EMPTY_TRACK_ARRAY = new Track[0];


	protected float		divisionType;
	protected int		resolution;
	protected Vector		tracks;




	public Sequence(float fDivisionType,
			int nResolution)
		throws	InvalidMidiDataException
	{
		this(fDivisionType, nResolution, 0);
	}



	public Sequence(float fDivisionType,
			int nResolution,
			int nNumTracks)
		throws	InvalidMidiDataException
	{
		if (fDivisionType == PPQ || fDivisionType == SMPTE_24 || fDivisionType == SMPTE_25 || fDivisionType == SMPTE_30DROP || fDivisionType == SMPTE_30)
		{
			divisionType = fDivisionType;
		}
		else
		{
			throw new InvalidMidiDataException("Invalid division type: " + fDivisionType);
		}
		resolution = nResolution;
		tracks = new Vector();
		for (int i = 0; i < nNumTracks; i++)
		{
			createTrack();
		}
	}



	public float getDivisionType()
	{
		return divisionType;
	}



	public int getResolution()
	{
		return resolution;
	}



	public Track createTrack()
	{
		Track	track = new Track();
		synchronized (tracks)
		{
			tracks.add(track);
		}
		return track;
	}


	public boolean deleteTrack(Track track)
	{
		synchronized (tracks)
		{
			return tracks.remove(track);
		}
	}



	public Track[] getTracks()
	{
		synchronized (tracks)
		{
			return (Track[]) tracks.toArray(EMPTY_TRACK_ARRAY);
		}
	}



	public long getMicrosecondLength()
	{
		if (getDivisionType() != PPQ)
		{
			return (long) (getTickLength() * 1000000 / (getDivisionType() * getResolution()));
		}
		else
		{
			// TODO: find all tempo change events and calculate length according to them.
			return -1;
		}
	}



	public long getTickLength()
	{
		long	lLength = 0;
		Iterator	tracksIterator = tracks.iterator();
		while (tracksIterator.hasNext())
		{
			Track	track = (Track) tracksIterator.next();
			lLength = Math.max(lLength, track.ticks());
		}
		return lLength;
	}


	// make abstract ??
	public Patch[] getPatchList()
	{
		// TODO:
		// not implemented  in sun version, too
		return new Patch[0];
	}
}



/*** Sequence.java ***/
