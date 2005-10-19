/*
 *	AlsaReceiver.java
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


package	org.tritonus.midi.device.alsa;


import javax.sound.midi.Receiver;



/**	A Receiver that is based on an ALSA client.
	This interface is used to facilitate subscriptions between
	Transmitters and Receivers. This subscription is used to
	transfer events directely inside the ALSA sequencer, instead of
	passing them by Java.
 */
public interface AlsaReceiver
	extends Receiver
{
	/**	Establish the subscription.
		Calling this method establishes a subscription between a
		AlsaTransmitters' ALSA client and port, which are passed as
		parameters here, and this Receiver's ALSA client and port.
		This method is typically called by an AlsaTransmitter that
		got an AlsaReceiver as its Receiver.

		@param nClient The ALSA client number of the Transmitter that
		a read subscription should established to.

		@param nPort The ALSA port number of the Transmitter that a
		read subscription should established to.

		@return true, if the subscription was established, false
		otherwise.
	 */
	public boolean subscribeTo(int nClient, int nPort);
}



/*** AlsaReceiver.java ***/

