/*
 *	AlsaPcmHWParams.java
 */

/*
 *  Copyright (c) 2000 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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
 */

package	org.tritonus.lowlevel.alsa;


import org.tritonus.share.TDebug;


/** TODO:
 */
public class AlsaPcmHWParams
{
	/**
	 *	Holds the pointer to snd_pcm_hw_params_t
	 *	for the native code.
	 *	This must be long to be 64bit-clean.
	 */
	private long	m_lNativeHandle;



	public AlsaPcmHWParams()
	{
		if (TDebug.TraceAlsaPcmNative) { TDebug.out("AlsaPcmHWParams.<init>(): begin"); }
		int	nReturn = malloc();
		if (nReturn < 0)
		{
			throw new RuntimeException("malloc of hw_params failed");
		}
		if (TDebug.TraceAlsaPcmNative) { TDebug.out("AlsaPcmHWParams.<init>(): end"); }
	}



	public void finalize()
	{
		// TODO: call free()
		// call super.finalize() first or last?
		// and introduce a flag if free() has already been called?
	}



	private native int malloc();
	public native void free();

	/**
	 *	Calls snd_pcm_hw_params_get_rate_numden().
	 *
	 *	alValues[0]:	numerator
	 *	alValues[1]:	denominator
	 */
	public native int getRate(long[] alValues);
	public double getRate()
	{
		long[]	alValues = new long[2];
		int	nReturn;

		nReturn = getRate(alValues);
		double	dRate = (double) -1;
		if (nReturn >= 0)
		{
			dRate = (double) alValues[0] / (double) alValues[1];
		}
		return dRate;
	}

	public native int getSBits();
	public native int getFifoSize();
	public native int getAccess();
	public native int getFormat();
	public native void getFormatMask(AlsaPcmHWParamsFormatMask mask);
	public native int getSubformat();
	public native int getChannels();
	public native int getChannelsMin();
	public native int getChannelsMax();


	/**	Gets approximate rate.
	 *	Calls snd_pcm_hw_params_get_rate().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getRate(int[] anValues);

	/**	Gets approximate minimum rate.
	 *	Calls snd_pcm_hw_params_get_rate_min().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getRateMin(int[] anValues);

	/**	Gets approximate maximum rate.
	 *	Calls snd_pcm_hw_params_get_rate_max().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getRateMax(int[] anValues);

	/**	Gets approximate period time.
	 *	Calls snd_pcm_hw_params_get_period_time().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getPeriodTime(int[] anValues);

	/**	Gets approximate minimum period time.
	 *	Calls snd_pcm_hw_params_get_period_time_min().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getPeriodTimeMin(int[] anValues);

	/**	Gets approximate maximum period time.
	 *	Calls snd_pcm_hw_params_get_period_time_max().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getPeriodTimeMax(int[] anValues);


	/**	Gets approximate period size.
	 *	Calls snd_pcm_hw_params_get_period_size().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getPeriodSize(int[] anValues);

	/**	Gets approximate minimum period size.
	 *	Calls snd_pcm_hw_params_get_period_size_min().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getPeriodSizeMin(int[] anValues);

	/**	Gets approximate maximum period size.
	 *	Calls snd_pcm_hw_params_get_period_size_max().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getPeriodSizeMax(int[] anValues);


	/**	Gets approximate periods.
	 *	Calls snd_pcm_hw_params_get_periods().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getPeriods(int[] anValues);

	/**	Gets approximate minimum periods.
	 *	Calls snd_pcm_hw_params_get_periods_min().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getPeriodsMin(int[] anValues);

	/**	Gets approximate maximum periods.
	 *	Calls snd_pcm_hw_params_get_periods_max().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getPeriodsMax(int[] anValues);

	/**	Gets approximate buffer time.
	 *	Calls snd_pcm_hw_params_get_buffer_time().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getBufferTime(int[] anValues);

	/**	Gets approximate minimum buffer time.
	 *	Calls snd_pcm_hw_params_get_buffer_time_min().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getBufferTimeMin(int[] anValues);

	/**	Gets approximate maximum buffer time.
	 *	Calls snd_pcm_hw_params_get_buffer_time_max().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getBufferTimeMax(int[] anValues);


	/**	Gets approximate buffer size.
	 *	Calls snd_pcm_hw_params_get_buffer_size().
	 */
	public native int getBufferSize();

	/**	Gets approximate minimum buffer size.
	 *	Calls snd_pcm_hw_params_get_buffer_size_min().
	 */
	public native int getBufferSizeMin();

	/**	Gets approximate maximum buffer size.
	 *	Calls snd_pcm_hw_params_get_buffer_size_max().
	 */
	public native int getBufferSizeMax();


	/**	Gets approximate tick time.
	 *	Calls snd_pcm_hw_params_get_tick_time().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getTickTime(int[] anValues);

	/**	Gets approximate minimum tick time.
	 *	Calls snd_pcm_hw_params_get_tick_time_min().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getTickTimeMin(int[] anValues);

	/**	Gets approximate maximum tick time.
	 *	Calls snd_pcm_hw_params_get_tick_time_max().
	 *	anValues[0]:	-1, 0 or +1, depending on the direction the exact rate differs from the returned value.
	 */
	public native int getTickTimeMax(int[] anValues);
}





/*** AlsaPcmHWParams.java ***/
