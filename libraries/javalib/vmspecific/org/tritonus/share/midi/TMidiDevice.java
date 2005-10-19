/*
 *	TMidiDevice.java
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


package	org.tritonus.share.midi;


import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MetaMessage;
import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Receiver;
import javax.sound.midi.Transmitter;

import org.tritonus.share.TDebug;


/**	Base class for MidiDevice implementations.
 *	The goal of this class is to supply the common functionality for
 *	classes that implement the interface MidiDevice.
 */
public abstract class TMidiDevice
	implements	MidiDevice
{
	/**	The Info object for a certain instance of MidiDevice.
	 */
	private MidiDevice.Info		m_info;

	/**	A flag to store whether the device is "open".
	 */
	private boolean			m_bOpen;

	// TODO: rename to UseReceiver/UseTransmitter
	/**	Whether to handle input from the physical port
		and to allow Transmitters.
	 */
	private boolean		m_bUseIn;

	/**	Whether to handle output to the physical port
		and to allow Receivers.
	 */
	private boolean		m_bUseOut;

	/**	The number of Receivers that refer to this MidiDevice.
	 *	This is currently only maintained for information purposes.
	 *
	 *	@see #addReceiver
	 *	@see #removeReceiver
	 */
	private int			m_nNumReceivers;

	/**	The collection of Transmitter that refer to this MidiDevice.
	 */
	private List			m_transmitters;



	/**	Initialize this class.
	 *	This sets the info from the passed one, sets the open status
	 *	to false, the number of Receivers to zero and the collection
	 *	of Transmitters to be empty.
	 *
	 *	@param info	The info object that describes this instance.
	 */
	public TMidiDevice(MidiDevice.Info info)
	{
		this(info, true, true);
	}



	/**	Initialize this class.
	 *	This sets the info from the passed one, sets the open status
	 *	to false, the number of Receivers to zero and the collection
	 *	of Transmitters to be empty.
	 *
	 *	@param info	The info object that describes this instance.
	 */
	public TMidiDevice(MidiDevice.Info info,
			   boolean bUseIn,
			   boolean bUseOut)
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.<init>(): begin"); }
		m_info = info;
		m_bUseIn = bUseIn;
		m_bUseOut = bUseOut;
		m_bOpen = false;
		m_nNumReceivers = 0;
		m_transmitters = new ArrayList();
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.<init>(): end"); }
	}



	/**	Retrieves a description of this instance.
	 *	This returns the info object passed to the constructor.
	 *
	 *	@return the description
	 *
	 *	@see #TMidiDevice
	 */
	public MidiDevice.Info getDeviceInfo()
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.getDeviceInfo(): begin"); }
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.getDeviceInfo(): end"); }
		return m_info;
	}



	public void open()
		throws	MidiUnavailableException
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.open(): begin"); }
		if (! isOpen())
		{
			m_bOpen = true;
			openImpl();
		}
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.open(): end"); }
	}



	/**
	 *	Subclasses have to override this method to be notified of
	 *	opening.
	 */
	protected void openImpl()
		throws	MidiUnavailableException
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.openImpl(): begin"); }
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.openImpl(): end"); }
	}



	public void close()
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.close(): begin"); }
		if (isOpen())
		{
			closeImpl();
			// TODO: close all Receivers and Transmitters
			m_bOpen = false;
		}
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.close(): end"); }
	}



	/**
	 *	Subclasses have to override this method to be notified of
	 *	closeing.
	 */
	protected void closeImpl()
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.closeImpl(): begin"); }
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.closeImpl(): end"); }
	}



	public boolean isOpen()
	{
		return m_bOpen;
	}



	/**	Returns whether to handle input.
		If this is true, retrieving Transmitters is possible
		and input from the physical port is passed to them.

		@see #getUseOut
	 */
	protected boolean getUseIn()
	{
		return m_bUseIn;
	}



	/**	Returns whether to handle output.
		If this is true, retrieving Receivers is possible
		and output to them is passed to the physical port.

		@see #getUseIn
	 */
	protected boolean getUseOut()
	{
		return m_bUseOut;
	}



	/**	Returns the device time in microseconds.
		This is a default implementation, telling the application
		program that the device doesn't track time. If a device wants
		to give timing information, it has to override this method.
	*/
	public long getMicrosecondPosition()
	{
		return -1;
	}



	public int getMaxReceivers()
	{
		int	nMaxReceivers = 0;
		if (getUseOut())
		{
		/*
		 *	The value -1 means unlimited.
		 */
			nMaxReceivers = -1;
		}
		return nMaxReceivers;
	}



	public int getMaxTransmitters()
	{
		int	nMaxTransmitters = 0;
		if (getUseIn())
		{
		/*
		 *	The value -1 means unlimited.
		 */
			nMaxTransmitters = -1;
		}
		return nMaxTransmitters;
	}



	/**	Creates a new Receiver object associated with this instance.
	 *	In this implementation, an unlimited number of Receivers
	 *	per MidiDevice can be created.
	 */
	public Receiver getReceiver()
		throws	MidiUnavailableException
	{
		if (! getUseOut())
		{
			throw new MidiUnavailableException("Receivers are not supported by this device");
		}
		return new TReceiver();
	}



	/**	Creates a new Transmitter object associated with this instance.
	 *	In this implementation, an unlimited number of Transmitters
	 *	per MidiDevice can be created.
	 */
	public Transmitter getTransmitter()
		throws	MidiUnavailableException
	{
		if (! getUseIn())
		{
			throw new MidiUnavailableException("Transmitters are not supported by this device");
		}
		return new TTransmitter();
	}


	/*
	 *	Intended for overriding by subclasses to receive messages.
	 *	This method is called by TMidiDevice.Receiver object on
	 *	receipt of a MidiMessage.
	 */
	protected void receive(MidiMessage message, long lTimeStamp)
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("### [should be overridden] TMidiDevice.receive(): message " + message); }
	}



	private void addReceiver()
	{
		m_nNumReceivers++;
	}



	private void removeReceiver()
	{
		m_nNumReceivers--;
	}




	private void addTransmitter(Transmitter transmitter)
	{
		synchronized (m_transmitters)
		{
			m_transmitters.add(transmitter);
		}
	}


	private void removeTransmitter(Transmitter transmitter)
	{
		synchronized (m_transmitters)
		{
			m_transmitters.remove(transmitter);
		}
	}



	/**	Send a MidiMessage to all Transmitters.
	 *	This method should be called by subclasses when they get a
	 *	message from a physical MIDI port.
	 */
	protected void sendImpl(MidiMessage message, long lTimeStamp)
	{
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.sendImpl(): begin"); }
		Iterator	transmitters = m_transmitters.iterator();
		while (transmitters.hasNext())
		{
			TTransmitter	transmitter = (TTransmitter) transmitters.next();
			/* due to a bug in the Sun jdk1.3, we cannot use
			   clone() for MetaMessages. So we have to do the
			   equivalent ourselves.
			*/
			// MidiMessage	copiedMessage = (MidiMessage) message.clone();
			MidiMessage	copiedMessage = null;
			if (message instanceof MetaMessage)
			{
				MetaMessage	origMessage = (MetaMessage) message;
				MetaMessage	metaMessage = new MetaMessage();
				try
				{
					metaMessage.setMessage(origMessage.getType(), origMessage.getData(), origMessage.getData().length);
				}
				catch (InvalidMidiDataException e)
				{
					if (TDebug.TraceAllExceptions) { TDebug.out(e); }
				}
				copiedMessage = metaMessage;
			}
			else
			{
				copiedMessage = (MidiMessage) message.clone();
			}

			if (message instanceof MetaMessage)
			{
				if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.sendImpl(): MetaMessage.getData().length (original): " + ((MetaMessage) message).getData().length); }
				if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.sendImpl(): MetaMessage.getData().length (cloned): " + ((MetaMessage) copiedMessage).getData().length); }
			}
			transmitter.send(copiedMessage, lTimeStamp);
		}
		if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.sendImpl(): end"); }
	}




/////////////////// INNER CLASSES //////////////////////////////////////


	/**	Receiver proxy class.
	 *	This class' objects are handed out on calls to
	 *	TMidiDevice.getReceiver(). 
	 */
	public class TReceiver
		implements	Receiver
	{
		private boolean		m_bOpen;



		public TReceiver()
		{
			TMidiDevice.this.addReceiver();
			m_bOpen = true;
		}



		protected boolean isOpen()
		{
			return m_bOpen;
		}



		/**	Receive a MidiMessage.
		 *
		 */
		public void send(MidiMessage message, long lTimeStamp)
		{
			if (TDebug.TraceMidiDevice) { TDebug.out("TMidiDevice.TReceiver.send(): message " + message); }
			if (m_bOpen)
			{
				TMidiDevice.this.receive(message, lTimeStamp);
			}
			else
			{
				throw new IllegalStateException("receiver is not open");
			}
		}



		/**	Closes the receiver.
		 *	After a receiver has been closed, it does no longer
		 *	propagate MidiMessages to its associated MidiDevice.
		 */
		public void close()
		{
			TMidiDevice.this.removeReceiver();
			m_bOpen = false;
		}
	}




	public class TTransmitter
		implements	Transmitter
	{
		private Receiver	m_receiver;



		public TTransmitter()
		{
			TMidiDevice.this.addTransmitter(this);
		}



		public void setReceiver(Receiver receiver)
		{
			synchronized (this)
			{
				m_receiver = receiver;
			}
		}



		public Receiver getReceiver()
		{
			return m_receiver;
		}



		public void send(MidiMessage message, long lTimeStamp)
		{
			if (getReceiver() != null)
			{
				getReceiver().send(message, lTimeStamp);
			}
		}



		/**	Closes the transmitter.
		 *	After a transmitter has been closed, it no longer
		 *	passes MidiMessages to a Receiver previously set for
		 *	it.
		 */
		public void close()
		{
			TMidiDevice.this.removeTransmitter(this);
			synchronized (this)
			{
				m_receiver = null;
			}
		}
	}



	/*
	 *	This is needed only because MidiDevice.Info's
	 *	constructor is protected (in the Sun jdk1.3).
	 */
	public static class Info
		extends MidiDevice.Info
	{
		public Info(String a, String b, String c, String d)
		{
			super(a, b, c, d);
		}
	}

}



/*** TMidiDevice.java ***/

