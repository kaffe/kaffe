/*
 *	AlsaPortMixer.java
 */

/*
 *  Copyright (c) 1999 - 2002 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
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

package	org.tritonus.sampled.mixer.alsa;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.sound.sampled.BooleanControl;
import javax.sound.sampled.CompoundControl;
import javax.sound.sampled.Control;
import javax.sound.sampled.FloatControl;
import javax.sound.sampled.Line;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.Mixer;
import javax.sound.sampled.Port;

import org.tritonus.lowlevel.alsa.AlsaMixer;
import org.tritonus.lowlevel.alsa.AlsaMixerElement;
import org.tritonus.share.GlobalInfo;
import org.tritonus.share.TDebug;
import org.tritonus.share.sampled.mixer.TCompoundControlType;
import org.tritonus.share.sampled.mixer.TMixer;
import org.tritonus.share.sampled.mixer.TMixerInfo;
import org.tritonus.share.sampled.mixer.TPort;


/**	TODO:
 */
public class AlsaPortMixer
extends TMixer
{
	/**	Used to signal an illegal value for direction.
	 */
	public static final int		DIRECTION_NONE = -1;

	/**	Used to signal common volume or common switch.
	 */
	public static final int		DIRECTION_COMMON = 0;

	/**	Used to signal playback volume or playback switch.
	 */
	public static final int		DIRECTION_PLAYBACK = 1;

	/**	Used to signal capture volume or capture switch.
	 */
	public static final int		DIRECTION_CAPTURE = 2;


	/*
	  For the first shot, we try to create one port line per mixer
	  element. For now, the following two lists should have the same size
	  and related elements at the same index position.
	*/
	private List			m_mixerElements;

	/*
	  Port.Infos are keys, Port instances are values.
	*/
	private Map			m_portMap;

	/*
	  Port.Infos are keys, AlsaMixerElement instances are values.
	*/
	private Map			m_mixerElementMap;



	public AlsaPortMixer(int nCard)
	{
		this("hw:" + nCard);
	}



	public AlsaPortMixer(String strDeviceName)
	{
		super(new TMixerInfo(
			      "Alsa Port Mixer (" + strDeviceName + ")",
			      GlobalInfo.getVendor(),
			      "System Mixer for the Advanced Linux Sound System (card " + strDeviceName + ")",
			      GlobalInfo.getVersion()),
		      new Line.Info(Mixer.class));
		if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.<init>: begin"); }
		m_mixerElements = new ArrayList();
		m_mixerElementMap = new HashMap();
		m_portMap = new HashMap();
		AlsaMixer	alsaMixer = null;
		try
		{
			alsaMixer = new AlsaMixer(strDeviceName);
			if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.<init>: successfully created AlsaMixer instance"); }
		}
		catch (Exception e)
		{
			if (TDebug.TraceMixer || TDebug.TraceAllExceptions) { TDebug.out(e); }
		}
		int		nArraySize = 150;	// TODO: original value: 128. With this value, a vm crash occurs
		int[]		anIndices = null;
		String[]	astrNames = null;
		int		nControlCount = 0;
		while (true)
		{
			anIndices = new int[nArraySize];
			astrNames = new String[nArraySize];
			nControlCount = alsaMixer.readControlList(anIndices, astrNames);
			if (nControlCount >= 0)
			{
				break;
			}
			if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.<init>: increating array size for AlsaMixer.readControlList(): now " + nArraySize * 2); }
			nArraySize *= 2;
		}
		List	sourcePortInfos = new ArrayList();
		List	targetPortInfos = new ArrayList();
		for (int i = 0; i < nControlCount; i++)
		{
			if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.<init>(): control " + i + ": " + anIndices[i] + " " + astrNames[i]); }
			AlsaMixerElement	element = new AlsaMixerElement(alsaMixer, anIndices[i], astrNames[i]);
			if (element.isActive())
			{
				m_mixerElements.add(element);
				if (hasPlaybackChannels(element))
				{
					Port.Info	info = new Port.Info(Port.class,
									     element.getName(), true);
					sourcePortInfos.add(info);
					m_mixerElementMap.put(info, element);
				}
				if (hasCaptureChannels(element))
				{
					Port.Info	info = new Port.Info(Port.class,
									     element.getName(), false);
					targetPortInfos.add(info);
					m_mixerElementMap.put(info, element);
				}
			}
		}
		setSupportInformation(
			new ArrayList(),
			new ArrayList(),
			sourcePortInfos,
			targetPortInfos);
		if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.<init>: end."); }
	}



	private static boolean hasPlaybackChannels(AlsaMixerElement element)
	{
		boolean	bHasChannels = false;
		for (int nChannel = AlsaMixerElement.SND_MIXER_SCHN_FRONT_LEFT;
		     nChannel <= AlsaMixerElement.SND_MIXER_SCHN_WOOFER;
		     nChannel++)
		{
			bHasChannels |= element.hasPlaybackChannel(nChannel);
		}
		return bHasChannels;
	}


	private static boolean hasCaptureChannels(AlsaMixerElement element)
	{
		boolean	bHasChannels = false;
		for (int nChannel = AlsaMixerElement.SND_MIXER_SCHN_FRONT_LEFT;
		     nChannel <= AlsaMixerElement.SND_MIXER_SCHN_WOOFER;
		     nChannel++)
		{
			bHasChannels |= element.hasCaptureChannel(nChannel);
		}
		return bHasChannels;
	}


	//////////////// Line //////////////////////////////////////


	// TODO: allow real close and reopen of mixer
	public void open()
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.open(): begin"); }

		// currently does nothing

		if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.open(): end"); }
	}



	public void close()
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.close(): begin"); }

		// currently does nothing

		if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.close(): end"); }
	}





	//////////////// Mixer //////////////////////////////////////






/*
  public Line.Info getLineInfo(Line.Info info)
  {
  // TODO:
  return null;
  }
*/

	// TODO:
	public int getMaxLines(Line.Info info)
	{
		// TODO:
		return 0;
	}




	protected Port getPort(Port.Info info)
		throws LineUnavailableException
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.getPort(): begin"); }
		Port	port = null;
		port = (Port) m_portMap.get(info);
		if (port == null)
		{
			port = createPort(info);
			m_portMap.put(info, port);
		}
		if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.getPort(): end"); }
		return port;
	}


	private Port createPort(Port.Info info)
	{
		if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.createPort(): begin"); }
		AlsaMixerElement	element = (AlsaMixerElement) m_mixerElementMap.get(info);
		if (element == null)
		{
			throw new IllegalArgumentException("no port for this info");
		}
		List	controls;
		Control	c;
		int	nDirection;
		if (info.isSource())
		{
			controls = createSourcePortControls(element);
		}
		else
		{
			//nDirection = DIRECTION_CAPTURE;
			// controls = createTargetPortControls(element);
			controls = new ArrayList();
		}
		Port	port = new TPort(this, info, controls);
		if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.createPort(): end"); }
		return port;
	}


	/**	TODO:
	 */
	private List createSourcePortControls(AlsaMixerElement element)
	{
		int	nDirection = DIRECTION_PLAYBACK;
		List	controls = new ArrayList();
		Control	c;
		if (element.hasPlaybackVolume() ||
		    element.hasCommonVolume())
		{
			if (element.isPlaybackMono() ||
			    element.hasPlaybackVolumeJoined())
			{
				c = createVolumeControl(element, AlsaMixerElement.SND_MIXER_SCHN_FRONT_LEFT, nDirection);
				controls.add(c);
			}
			else
			{
				List	volumeControls = new ArrayList();
				for (int nChannel = AlsaMixerElement.SND_MIXER_SCHN_FRONT_LEFT; nChannel < AlsaMixerElement.SND_MIXER_SCHN_LAST; nChannel++)
				{
					if (element.hasPlaybackChannel(nChannel))
					{
						// TDebug.out("adding channel " + nChannel);
						c = createVolumeControl(element, nChannel, nDirection);
						// System.out.println("control to add: " + c);
						volumeControls.add(c);
					}
				}
				// list should not be empty
				CompoundControl.Type	type = new TCompoundControlType("test");
				Control[]	aMemberControls = (Control[]) volumeControls.toArray(new Control[0]);
				// System.out.println("member controls: " + aMemberControls);
				// System.out.println("# member controls: " + aMemberControls.length);
				c = new AlsaCompoundControl(type, aMemberControls);
				controls.add(c);
			}
		}
		if (element.hasPlaybackSwitch() ||
		    element.hasCommonSwitch())
		{
			if (element.isPlaybackMono() ||
			    element.hasPlaybackSwitchJoined())
			{
				c = createSwitchControl(element, AlsaMixerElement.SND_MIXER_SCHN_FRONT_LEFT, nDirection);
				controls.add(c);
			}
			else
			{
				List	volumeControls = new ArrayList();
				for (int nChannel = AlsaMixerElement.SND_MIXER_SCHN_FRONT_LEFT; nChannel < AlsaMixerElement.SND_MIXER_SCHN_LAST; nChannel++)
				{
					if (element.hasPlaybackChannel(nChannel))
					{
						// TDebug.out("adding channel " + nChannel);
						c = createSwitchControl(element, nChannel, nDirection);
						// System.out.println("control to add: " + c);
						volumeControls.add(c);
					}
				}
				// list should not be empty
				CompoundControl.Type	type = new TCompoundControlType("test");
				Control[]	aMemberControls = (Control[]) volumeControls.toArray(new Control[0]);
				// System.out.println("member controls: " + aMemberControls);
				// System.out.println("# member controls: " + aMemberControls.length);
				c = new AlsaCompoundControl(type, aMemberControls);
				controls.add(c);
			}
		}
		return controls;
	}



	/**	TODO:
	 */
	private FloatControl createVolumeControl(AlsaMixerElement element,
						 int nChannel,
						 int nDirection)
	{
		int[]	anValues = new int[2];
		switch (nDirection)
		{
		case DIRECTION_COMMON:
		case DIRECTION_PLAYBACK:
			element.getPlaybackVolumeRange(anValues);
			break;

		case DIRECTION_CAPTURE:
			element.getCaptureVolumeRange(anValues);
			break;
		}
		FloatControl	control = new AlsaVolumeControl(
			FloatControl.Type.VOLUME,
			anValues[0],
			anValues[1],
			1.0F,
			-1,
			anValues[0],
			"", "", "", "",
			element,
			nChannel,
			nDirection);
		return control;
	}


	/**	TODO:
	 */
	private BooleanControl createSwitchControl(AlsaMixerElement element,
						   int nChannel,
						   int nDirection)
	{
		BooleanControl	control = new AlsaSwitchControl(
			BooleanControl.Type.MUTE,
			false,
			"", "",
			element,
			nChannel,
			nDirection);
		return control;
	}


	//////////////// inner classes //////////////////////////////////////


	private class AlsaVolumeControl
	extends FloatControl
	{
		private AlsaMixerElement	m_element;
		private int			m_nChannel;

		/**	One of the constants DIRECTION_*.
		 */
		private int			m_nDirection;


		/**
		   @param nDirection One of the constants DIRECTION_*.
		 */
		public AlsaVolumeControl(Type type,
					float fMinimum,
					float fMaximum,
					float fPrecision,
					int nUpdatePeriod,
					float fInitialValue,
					String strUnits,
					String strMinLabel,
					String strMidLabel,
					String strMaxLabel,
					AlsaMixerElement element,
					int nChannel,
					int nDirection)
		{
			super(type,
			      fMinimum,
			      fMaximum,
			      fPrecision,
			      nUpdatePeriod,
			      fInitialValue,
			      strUnits,
			      strMinLabel,
			      strMidLabel,
			      strMaxLabel);
			if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.AlsaFloatControl.<init>(): begin"); }
			m_element = element;
			m_nChannel = nChannel;
			m_nDirection = nDirection;
			setValue(getValueImpl());
			if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.AlsaFloatControl.<init>(): end"); }
		}


		private AlsaMixerElement getElement()
		{
			return m_element;
		}



		private int getChannel()
		{
			return m_nChannel;
		}



		private int getDirection()
		{
			return m_nDirection;
		}



		// TODO: respect channels
		public void setValue(float fValue)
		{
			super.setValue(fValue);
			int	nValue = (int) fValue;
			switch (getDirection())
			{
			case DIRECTION_COMMON:
			case DIRECTION_PLAYBACK:
				getElement().setPlaybackVolumeAll(nValue);
				break;

			case DIRECTION_CAPTURE:
				getElement().setCaptureVolumeAll(nValue);
				break;
			}
		}



		private float getValueImpl()
		{
			int	nChannel = getChannel();
			float	fValue = 0.0F;
			switch (getDirection())
			{
			case DIRECTION_COMMON:
			case DIRECTION_PLAYBACK:
				fValue = getElement().getPlaybackVolume(nChannel);
				break;

			case DIRECTION_CAPTURE:
				fValue = getElement().getCaptureVolume(nChannel);
				break;
			}
			return fValue;
		}
	}



	private class AlsaSwitchControl
	extends BooleanControl
	{
		private AlsaMixerElement	m_element;
		private int			m_nChannel;

		/**	One of the constants DIRECTION_*.
		 */
		private int			m_nDirection;


		/**
		   @param nDirection One of the constants DIRECTION_*.
		 */
		public AlsaSwitchControl(Type type,
					boolean bInitialValue,
					String strTrueLabel,
					String strFalseLabel,
					AlsaMixerElement element,
					int nChannel,
					int nDirection)
		{
			super(type,
			      bInitialValue,
			      strTrueLabel,
			      strFalseLabel);
			if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.AlsaFloatControl.<init>(): begin"); }
			m_element = element;
			m_nChannel = nChannel;
			m_nDirection = nDirection;
			if (TDebug.TraceMixer) { TDebug.out("AlsaPortMixer.AlsaFloatControl.<init>(): end"); }
		}


		private AlsaMixerElement getElement()
		{
			return m_element;
		}



		private int getChannel()
		{
			return m_nChannel;
		}



		private int getDirection()
		{
			return m_nDirection;
		}



		// TODO: respect channels
		public void setValue(boolean bValue)
		{
			super.setValue(bValue);
			switch (getDirection())
			{
			case DIRECTION_COMMON:
			case DIRECTION_PLAYBACK:
				getElement().setPlaybackSwitchAll(bValue);
				break;

			case DIRECTION_CAPTURE:
				getElement().setCaptureSwitchAll(bValue);
				break;
			}
		}
	}


	/**	CompoundControl class.
		This class is only needed to provide a public
		constructor.
	 */
	public class AlsaCompoundControl
	extends CompoundControl
	{
		public AlsaCompoundControl(CompoundControl.Type type,
					   Control[] aMemberControls)
		{
			super(type, aMemberControls);
		}
	}
}



/*** AlsaPortMixer.java ***/
