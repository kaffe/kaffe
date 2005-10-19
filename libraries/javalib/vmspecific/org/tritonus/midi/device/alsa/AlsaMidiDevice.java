/*
 *	AlsaMidiDevice.java
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


package	org.tritonus.midi.device.alsa;


import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Receiver;
import javax.sound.midi.Transmitter;

import org.tritonus.lowlevel.alsa.AlsaSeq;
import org.tritonus.lowlevel.alsa.AlsaSeqEvent;
import org.tritonus.lowlevel.alsa.AlsaSeqPortSubscribe;
import org.tritonus.lowlevel.alsa.AlsaSeqQueueStatus;
import org.tritonus.share.GlobalInfo;
import org.tritonus.share.TDebug;
import org.tritonus.share.midi.TMidiDevice;



/**	A representation of a physical MIDI port based on the ALSA sequencer.
 */
public class AlsaMidiDevice
	extends		TMidiDevice
	implements	AlsaMidiIn.AlsaMidiInListener
{
	/**	ALSA client id of the physical port.
	 */
	private int			m_nPhysicalClient;

	/**	ALSA port id of the physical port.
	 */
	private int			m_nPhysicalPort;

	/**	The object interfacing to the ALSA sequencer.
	 */
	private AlsaSeq			m_alsaSeq;

	/**	The object used for getting timestamps.
	 */
	private AlsaSeqQueueStatus	m_queueStatus;

	/**	The ALSA port id of the handler.
	 *	This is used by m_alsaSeq.
	 */
	private int			m_nOwnPort;

	/**	Handler for input from the physical MIDI port.
	 */
	private AlsaMidiIn		m_alsaMidiIn;

	/**	Handler for output to the physical MIDI port.
	 */
	private AlsaMidiOut		m_alsaMidiOut;

	/**	ALSA queue number used to timestamp incoming events.
	 */
	private int			m_nTimestampingQueue;

	/**	The event used for starting and stopping the queue.
	 */
	private AlsaSeqEvent		m_event = new AlsaSeqEvent();




	public AlsaMidiDevice(int nClient, int nPort, boolean bUseIn, boolean bUseOut)
	{
		this(new TMidiDevice.Info("ALSA MIDI port (" + nClient + ":" + nPort + ")",
					  GlobalInfo.getVendor(),
					  "ALSA MIDI port (" + nClient + ":" + nPort + ")",
					  GlobalInfo.getVersion()),
		     nClient, nPort, bUseIn, bUseOut);
	}



	protected AlsaMidiDevice(MidiDevice.Info info, int nClient, int nPort, boolean bUseIn, boolean bUseOut)
	{
		super(info, bUseIn, bUseOut);
		if (TDebug.TraceMidiDevice) { TDebug.out("AlsaMidiDevice.<init>(): begin"); }
		m_nPhysicalClient = nClient;
		m_nPhysicalPort = nPort;
		if (TDebug.TraceMidiDevice) { TDebug.out("AlsaMidiDevice.<init>(): end"); }
	}



	private AlsaSeq getAlsaSeq()
	{
		return m_alsaSeq;
	}



	private int getOwnPort()
	{
		return m_nOwnPort;
	}



	private int getPhysicalClient()
	{
		return m_nPhysicalClient;
	}




	private int getPhysicalPort()
	{
		return m_nPhysicalPort;
	}



	private AlsaSeqQueueStatus getQueueStatus()
	{
		return m_queueStatus;
	}



	protected void openImpl()
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("AlsaMidiDevice.openImpl(): begin"); }
		// create an ALSA client...
		m_alsaSeq = new AlsaSeq("Tritonus Midi port handler");
		// ...and an ALSA port
		m_nOwnPort = getAlsaSeq().createPort(
			"handler port",
			AlsaSeq.SND_SEQ_PORT_CAP_WRITE | AlsaSeq.SND_SEQ_PORT_CAP_SUBS_WRITE | AlsaSeq.SND_SEQ_PORT_CAP_READ | AlsaSeq.SND_SEQ_PORT_CAP_SUBS_READ,
			0,
			AlsaSeq.SND_SEQ_PORT_TYPE_APPLICATION,
			0, 0, 0);
		if (getUseIn())
		{
			/*
			 *	AlsaMidiIn listens to incoming event on the
			 *	MIDI port.
			 *	It calls this.dequeueEvent() if
			 *	it receives an event.
			 */
			m_nTimestampingQueue = getAlsaSeq().allocQueue();
			m_queueStatus = new AlsaSeqQueueStatus();
			// TODO: stop queue
			startQueue();
			m_alsaMidiIn = new AlsaMidiIn(
				getAlsaSeq(), getOwnPort(),
				getPhysicalClient(), getPhysicalPort(),
				getTimestampingQueue(), true,
				this);
			m_alsaMidiIn.start();
		}
		if (getUseOut())
		{
			// uses subscribers, immediately
			m_alsaMidiOut = new AlsaMidiOut(getAlsaSeq(), getOwnPort());
			AlsaSeqPortSubscribe	portSubscribe = new AlsaSeqPortSubscribe();
			portSubscribe.setSender(getAlsaSeq().getClientId(), getOwnPort());
			portSubscribe.setDest(getPhysicalClient(), getPhysicalPort());
			getAlsaSeq().subscribePort(portSubscribe);
			portSubscribe.free();
		}
		if (TDebug.TraceMidiDevice) { TDebug.out("AlsaMidiDevice.openImpl(): end"); }
	}



	protected void closeImpl()
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("AlsaMidiDevice.closeImpl(): begin"); }
		if (getUseIn())
		{
			m_alsaMidiIn.interrupt();
			m_alsaMidiIn = null;
			stopQueue();
			// TODO: release timestamping queue
			m_queueStatus.free();
			m_queueStatus = null;
		}
		// TODO:
		// getAlsaSeq().destroyPort(getOwnPort());
		getAlsaSeq().close();
		m_alsaSeq = null;
		if (TDebug.TraceMidiDevice) { TDebug.out("AlsaMidiDevice.closeImpl(): end"); }
	}



	public long getMicroSecondPosition()
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("AlsaMidiDevice.getMicroSecondPosition(): begin"); }
		long	lPosition = 0;
		if (m_queueStatus != null)
		{
			getAlsaSeq().getQueueStatus(getTimestampingQueue(), getQueueStatus());
			long	lNanoSeconds = getQueueStatus().getRealTime();
			lPosition = lNanoSeconds / 1000;
		}
		if (TDebug.TraceMidiDevice) { TDebug.out("AlsaMidiDevice.getMicroSecondPosition(): end"); }
		return lPosition;
	}



	private void startQueue()
	{
		controlQueue(AlsaSeq.SND_SEQ_EVENT_START);
	}



	private void stopQueue()
	{
		controlQueue(AlsaSeq.SND_SEQ_EVENT_STOP);
	}



	private void controlQueue(int nType)
	{
		m_event.setCommon(nType,
				  AlsaSeq.SND_SEQ_TIME_STAMP_REAL | AlsaSeq.SND_SEQ_TIME_MODE_REL,
				  0, AlsaSeq.SND_SEQ_QUEUE_DIRECT, 0L,
				  0, getOwnPort(),
				  AlsaSeq.SND_SEQ_CLIENT_SYSTEM, AlsaSeq.SND_SEQ_PORT_SYSTEM_TIMER);
		m_event.setQueueControl(getTimestampingQueue(), 0, 0);
		getAlsaSeq().eventOutputDirect(m_event);
	}



	/**	Pass MidiMessage from Receivers to physical MIDI port.
	 */
	protected void receive(MidiMessage message, long lTimeStamp)
	{
		if (isOpen())
		{
			m_alsaMidiOut.enqueueMessage(message, lTimeStamp);
		}
	}



	// for AlsaMidiInListener
	// passes events read from the device to the Transmitters
	public void dequeueEvent(MidiMessage message, long lTimestamp)
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("AlsaMidiDevice.dequeueEvent(): message: " + message); }
		if (TDebug.TraceMidiDevice) { TDebug.out("AlsaMidiDevice.dequeueEvent(): tick: " + lTimestamp); }
		// send via superclass method
		sendImpl(message, lTimestamp);
	}



	private int getTimestampingQueue()
	{
		return m_nTimestampingQueue;
	}



	public Receiver getReceiver()
		throws	MidiUnavailableException
	{
		if (! getUseOut())
		{
			throw new MidiUnavailableException("Receivers are not supported by this device");
		}
		return new AlsaMidiDeviceReceiver();
	}



	public Transmitter getTransmitter()
		throws	MidiUnavailableException
	{
		if (! getUseIn())
		{
			throw new MidiUnavailableException("Transmitters are not supported by this device");
		}
		return new AlsaMidiDeviceTransmitter();
	}


/////////////////// INNER CLASSES //////////////////////////////////////

	private class AlsaMidiDeviceReceiver
		extends		TReceiver
		implements	AlsaReceiver
	{



		public AlsaMidiDeviceReceiver()
		{
			super();
		}



		/**	Subscribe to the passed port.
		 *	This establishes a subscription in the ALSA sequencer
		 *	so that the device this Receiver belongs to receives
		 *	event from the client:port passed as parameters.
		 *
		 *	@return true if subscription was established,
		 *		false otherwise
		 */
		public boolean subscribeTo(int nClient, int nPort)
		{
			try
			{
				AlsaSeqPortSubscribe	portSubscribe = new AlsaSeqPortSubscribe();
				portSubscribe.setSender(nClient, nPort);
				portSubscribe.setDest(AlsaMidiDevice.this.getPhysicalClient(), AlsaMidiDevice.this.getPhysicalPort());
				AlsaMidiDevice.this.getAlsaSeq().subscribePort(portSubscribe);
				portSubscribe.free();
				return true;
			}
			catch (RuntimeException e)
			{
				if (TDebug.TraceAllExceptions) { TDebug.out(e); }
				return false;
			}
		}


	}




	private class AlsaMidiDeviceTransmitter
		extends		TTransmitter
	{
		private boolean		m_bReceiverSubscribed;



		public AlsaMidiDeviceTransmitter()
		{
			super();
			m_bReceiverSubscribed = false;
		}



		public void setReceiver(Receiver receiver)
		{
			super.setReceiver(receiver);
			/*
			 *	Try to establish a subscription of the Receiver
			 *	to the ALSA seqencer client of the device this
			 *	Transmitter belongs to.
			 */
			if (receiver instanceof AlsaReceiver)
			{
				m_bReceiverSubscribed = ((AlsaReceiver) receiver).subscribeTo(getPhysicalClient(), getPhysicalPort());
			}
		}



		public void send(MidiMessage message, long lTimeStamp)
		{
			/*
			 *	Send message via Java methods only if no
			 *	subscription was established. If there is a
			 *	subscription, the message is routed inside of
			 *	the ALSA sequencer.
			 */
			if (! m_bReceiverSubscribed)
			{
				super.send(message, lTimeStamp);
			}
		}



		public void close()
		{
			super.close();
			// TODO: remove subscription
		}
	}
}



/*** AlsaMidiDevice.java ***/

