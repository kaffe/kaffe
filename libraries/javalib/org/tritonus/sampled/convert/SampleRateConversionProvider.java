/*
 *	SampleRateConversionProvider.java
 */

/*
 *  Copyright (c) 2001 by Florian Bomers <florian@bome.com>
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

package	org.tritonus.sampled.convert;

import java.io.IOException;
import java.util.Arrays;
import java.util.Iterator;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;

import org.tritonus.share.ArraySet;
import org.tritonus.share.TDebug;
import org.tritonus.share.sampled.AudioFormats;
import org.tritonus.share.sampled.AudioUtils;
import org.tritonus.share.sampled.FloatSampleBuffer;
import org.tritonus.share.sampled.convert.TSimpleFormatConversionProvider;

/**
 * This provider converts sample rate of 2 PCM streams. <br>
 * It does:
 * <ul><li>conversion of different sample rates
 * <li>conversion of unsigned/signed (only 8bit unsigned supported)
 * <li>conversion of big/small endian
 * <li>8,16,24,32 bit conversion
 * </ul>
 * It does NOT:
 * <ul><li>change channel count
 * <li>accept a stream where the sample rates are equal. This case should be handled
 * by the PCM2PCM converter
 * </ul>
 *
 * @author Florian Bomers
 */

public class SampleRateConversionProvider
	extends TSimpleFormatConversionProvider {

	// only used as abbreviation
	public static AudioFormat.Encoding PCM_SIGNED=AudioFormat.Encoding.PCM_SIGNED;
	public static AudioFormat.Encoding PCM_UNSIGNED=AudioFormat.Encoding.PCM_UNSIGNED;

	private static final boolean DEBUG_STREAM=false;
	private static final boolean DEBUG_STREAM_PROBLEMS=false;

	private static final int ALL=AudioSystem.NOT_SPECIFIED;
	private static final AudioFormat[]	OUTPUT_FORMATS = {
	    // Encoding, SampleRate, sampleSizeInBits, channels, frameSize, frameRate, bigEndian
	    new AudioFormat(PCM_SIGNED, ALL, 8, ALL, ALL, ALL, false),
	    new AudioFormat(PCM_SIGNED, ALL, 8, ALL, ALL, ALL, true),
	    new AudioFormat(PCM_UNSIGNED, ALL, 8, ALL, ALL, ALL, false),
	    new AudioFormat(PCM_UNSIGNED, ALL, 8, ALL, ALL, ALL, true),
	    new AudioFormat(PCM_SIGNED, ALL, 16, ALL, ALL, ALL, false),
	    new AudioFormat(PCM_SIGNED, ALL, 16, ALL, ALL, ALL, true),
	    new AudioFormat(PCM_SIGNED, ALL, 24, ALL, ALL, ALL, false),
	    new AudioFormat(PCM_SIGNED, ALL, 24, ALL, ALL, ALL, true),
	    new AudioFormat(PCM_SIGNED, ALL, 32, ALL, ALL, ALL, false),
	    new AudioFormat(PCM_SIGNED, ALL, 32, ALL, ALL, ALL, true),
	};

	/**	Constructor.
	 */
	public SampleRateConversionProvider() {
		super(Arrays.asList(OUTPUT_FORMATS),
		      Arrays.asList(OUTPUT_FORMATS));
	}

	public AudioInputStream getAudioInputStream(AudioFormat targetFormat, AudioInputStream sourceStream) {
		AudioFormat sourceFormat=sourceStream.getFormat();

		// the non-conversion case
		if (AudioFormats.matches(sourceFormat, targetFormat)) {
			return sourceStream;
		}

		targetFormat=replaceNotSpecified(sourceFormat, targetFormat);
		// do not support NOT_SPECIFIED as sample rates
		if (targetFormat.getSampleRate()!=AudioSystem.NOT_SPECIFIED
			&& sourceFormat.getSampleRate()!=AudioSystem.NOT_SPECIFIED
			&& targetFormat.getChannels()!=AudioSystem.NOT_SPECIFIED
			&& sourceFormat.getChannels()!=AudioSystem.NOT_SPECIFIED
			&& targetFormat.getSampleSizeInBits()!=AudioSystem.NOT_SPECIFIED
			&& sourceFormat.getSampleSizeInBits()!=AudioSystem.NOT_SPECIFIED
			&& isConversionSupported(sourceFormat, targetFormat)) {
			return new SampleRateConverterStream(sourceStream, targetFormat);
		}
		throw new IllegalArgumentException("format conversion not supported");
	}
	
	// replaces the sample rate and frame rate. 
	// Should only be used with PCM_SIGNED or PCM_UNSIGNED
	private static AudioFormat replaceSampleRate(AudioFormat format, float newSampleRate) {
		if (format.getSampleRate()==newSampleRate) {
			return format;
		}
		return new AudioFormat(format.getEncoding(), newSampleRate, format.getSampleSizeInBits(), 
			format.getChannels(), format.getFrameSize(), newSampleRate, format.isBigEndian());
	}

	private static final float[] commonSampleRates={
		8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 64000, 88200, 96000
	};

	public AudioFormat[] getTargetFormats(AudioFormat.Encoding targetEncoding,
	                                      AudioFormat sourceFormat) {
		if (TDebug.TraceAudioConverter) {
			TDebug.out(">SampleRateConversionProvider.getTargetFormats(AudioFormat.Encoding, AudioFormat):");
			TDebug.out("checking out possible target formats");
			TDebug.out("from: " + sourceFormat);
			TDebug.out("to  : " + targetEncoding);
		}
		float sourceSampleRate=sourceFormat.getSampleRate();
		// a trick: set sourceFormat's sample rate to -1 so that
		//          replaceNotSpecified does not replace the sample rate.
		//          we want to convert that !
		sourceFormat=replaceSampleRate(sourceFormat, AudioSystem.NOT_SPECIFIED);
		if (isConversionSupported(targetEncoding, sourceFormat)) {
			ArraySet result=new ArraySet();
			Iterator iterator=getCollectionTargetFormats().iterator();
			while (iterator.hasNext()) {
				AudioFormat targetFormat = (AudioFormat) iterator.next();
				targetFormat=replaceNotSpecified(sourceFormat, targetFormat);
				if (isConversionSupported(targetFormat, sourceFormat)) {
					result.add(targetFormat);
				}
			}
			// for convenience, add some often used sample rates as output
			// this may help applications that do not handle NOT_SPECIFIED
			if (result.size()>0 
				&& sourceSampleRate!=AudioSystem.NOT_SPECIFIED) {
				int count=result.size();
				for (int i=0; i<count; i++) {
					AudioFormat format=(AudioFormat) result.get(i);
					for (int j=0; j<commonSampleRates.length; j++) {
						if (!doMatch(sourceSampleRate, commonSampleRates[j])) {
							result.add(replaceSampleRate(format, commonSampleRates[j]));
						}
					}
				}
			}
			if (TDebug.TraceAudioConverter) {
				TDebug.out("<found "+result.size()+" matching formats.");
			}
			return (AudioFormat[]) result.toArray(EMPTY_FORMAT_ARRAY);
		} else {
			if (TDebug.TraceAudioConverter) {
				TDebug.out("<returning empty array.");
			}
			return EMPTY_FORMAT_ARRAY;
		}
	}

	// overriden
	public boolean isConversionSupported(AudioFormat targetFormat,
	                                     AudioFormat sourceFormat) {
		// do not match when targetSampleRate set and sourceSamplerate set and NOT both the same
		boolean result=(targetFormat.getSampleRate()==AudioSystem.NOT_SPECIFIED
				|| targetFormat.getSampleRate()==AudioSystem.NOT_SPECIFIED
				|| !doMatch(targetFormat.getSampleRate(), sourceFormat.getSampleRate())
			&& doMatch(targetFormat.getChannels(), sourceFormat.getChannels()))
			&& AudioUtils.containsFormat(sourceFormat, getCollectionSourceFormats().iterator())
			&& AudioUtils.containsFormat(targetFormat, getCollectionTargetFormats().iterator());
		if (TDebug.TraceAudioConverter) {
			TDebug.out(">SampleRateConverter: isConversionSupported(AudioFormat, AudioFormat):");
			TDebug.out("checking if conversion possible");
			TDebug.out("from: " + sourceFormat);
			TDebug.out("to  : " + targetFormat);
			TDebug.out("< result : " + result);
		}
		return result;
	}
	
	private static long convertLength(AudioFormat sourceFormat, AudioFormat targetFormat, long sourceLength) {
		if (sourceLength==AudioSystem.NOT_SPECIFIED) {
			return sourceLength;
		}
		return Math.round(targetFormat.getSampleRate()/sourceFormat.getSampleRate()*sourceLength);
		//return (long) (targetFormat.getSampleRate()/sourceFormat.getSampleRate()*sourceLength);
	}


	/**
	 * SampleRateConverterStream
	 */
	// at the moment, there are so many special things to care
	// about, and new things in an AIS, that I derive directly from
	// AudioInputStream.
	// I cannot use TAsynchronousFilteredAudioInputStream because
	// - it doesn't allow convenient use of a history. The history will be needed
	//   especially when performing filtering
	// - it doesn't work on FloatSampleBuffer (yet)
	// - each sample must be calculated one-by-one. The asynchronous
	//   difficulty isn't overcome by using a TCircularBuffer
	// I cannot use TSynchronousFilteredAudioInputStream because
	// - it doesn't handle different sample rates
	// Later we can make a base class for this, e.g. THistoryAudioInputStream
	
	// TODO: when target sample rate is < source sample rate (or only slightly above),
	// this stream calculates ONE sample too much.
	public static class SampleRateConverterStream extends AudioInputStream {

		/** the current working buffer with samples of the sourceStream */
		private FloatSampleBuffer thisBuffer=null;
		/** used when read(byte[],int,int) is called */
		private FloatSampleBuffer writeBuffer=null;
		private byte[] byteBuffer; // used for reading samples of sourceStream
		private AudioInputStream sourceStream;
		private float sourceSampleRate;
		private float targetSampleRate;
		/** index in thisBuffer */
		private double dPos;
		/** Conversion algorithm */
		public static final int SAMPLE_AND_HOLD=1;
		/** Conversion algorithm */
		public static final int LINEAR_INTERPOLATION=2;
		/** Conversion algorithm */
		public static final int RESAMPLE=3;
		
		/** source stream is read in buffers of this size - in milliseconds */
		private int sourceBufferTime;

		/** the current conversion algorithm */
		private int conversionAlgorithm=LINEAR_INTERPOLATION;
		//private int conversionAlgorithm=SAMPLE_AND_HOLD;
		
		// History support
		/** the buffer with history samples */
		private FloatSampleBuffer historyBuffer=null;
		/** the minimum number of samples that must be present in the history buffer */
		private int minimumSamplesInHistory=1;
		
		/** force to discard current contents in thisBuffer if true */
		private boolean thisBufferValid=false;

		public SampleRateConverterStream(AudioInputStream sourceStream, AudioFormat targetFormat) {
			// clean up targetFormat:
			// - ignore frame rate totally
			// - recalculate frame size
			super (sourceStream, new SRCAudioFormat(targetFormat), 
			           convertLength(sourceStream.getFormat(), targetFormat, sourceStream.getFrameLength()));
			if (TDebug.TraceAudioConverter) {
				TDebug.out("SampleRateConverterStream: <init>");
			}
			this.sourceStream=sourceStream;
			sourceSampleRate=sourceStream.getFormat().getSampleRate();
			targetSampleRate=targetFormat.getSampleRate();
			dPos=0;
			// use a buffer size of 100ms
			sourceBufferTime=100;
			resizeBuffers();
			flush(); // force read of source stream next time read is called
		}
	
	
		/**
		 * Assures that both historyBuffer and working buffer
		 * <ul><li>exist
		 * <li>have about <code>sourceBufferTime</code> ms samples
		 * <li>that both have at least <code>minimumSamplesInHistory</code>
		 * samples
		 * </ul>
		 * This method must be called when anything is changed that
		 * may change the size of the buffers.
		 */
		private synchronized void resizeBuffers() {
			int bufferSize=(int) AudioUtils.millis2Frames(
				(long) sourceBufferTime, sourceSampleRate);
			if (bufferSize<minimumSamplesInHistory) {
				bufferSize=minimumSamplesInHistory;
			}
			// we must be able to calculate at least one output sample from
			// one input buffer block
			if (bufferSize<outSamples2inSamples(1)) {
				bufferSize=roundUp(outSamples2inSamples(1));
			}
			if (historyBuffer==null) {
				historyBuffer=new FloatSampleBuffer(sourceStream.getFormat().getChannels(),
					bufferSize, sourceSampleRate);
				historyBuffer.makeSilence();
			}
			// TODO: retain last samples !
			historyBuffer.changeSampleCount(bufferSize, true);
			if (thisBuffer==null) {
				thisBuffer=new FloatSampleBuffer(sourceStream.getFormat().getChannels(),
					bufferSize, sourceSampleRate);
			}
			// TODO: retain last samples and adjust dPos
			thisBuffer.changeSampleCount(bufferSize, true);
		}
		
		/**
		 * Reads from a source stream that cannot handle float buffers.
		 * After this method has been called, it is to be checked whether
		 * we are closed !
		 */
		private synchronized void readFromByteSourceStream() throws IOException {
			int byteCount=thisBuffer.getByteArrayBufferSize(getFormat());
			if (byteBuffer==null || byteBuffer.length<byteCount) {
				byteBuffer=new byte[byteCount];
			}
			// finally read it
			int bytesRead=0;
			int thisRead;
			do {
				thisRead=sourceStream.read(byteBuffer, bytesRead, byteCount-bytesRead);
				if (thisRead>0) {
					bytesRead+=thisRead;
				}
			} while (bytesRead<byteCount && thisRead>0);
			if (bytesRead==0) {
				// sourceStream is closed. We don't accept 0 bytes read from source stream
				close();
			} else {
				thisBuffer.initFromByteArray(byteBuffer, 0, bytesRead, sourceStream.getFormat());
			}
		}
		
		private synchronized void readFromFloatSourceStream() throws IOException {
			//((FloatSampleInput) sourceStream).read(thisBuffer);
		}
		
private long testInFramesRead=0;
private long testOutFramesReturned=0;

		/**
		 * fills thisBuffer with new samples.
		 * It sets the history buffer to the last buffer.
		 * thisBuffer's sampleCount will be the number of samples read.
		 * Calling methods MUST check whether this stream is closed upon
		 * completion of this method. If the stream is closed, the contents
		 * of <code>thisBuffer</code> are not valid.
		 */
		private synchronized void readFromSourceStream() throws IOException {
			if (isClosed()) {
				return;
			}
			int oldSampleCount=thisBuffer.getSampleCount();
			// reuse history buffer
			FloatSampleBuffer newBuffer=historyBuffer;
			historyBuffer=thisBuffer;
			thisBuffer=newBuffer;
			if (sourceStream.getFrameLength()!=AudioSystem.NOT_SPECIFIED 
				&& thisBuffer.getSampleCount()+testInFramesRead>sourceStream.getFrameLength()) {
					if (sourceStream.getFrameLength()-testInFramesRead<=0) {
						close();
						return;
					}
					thisBuffer.changeSampleCount((int) (sourceStream.getFrameLength()-testInFramesRead), false);
			}
					
			// if (sourceStream instanceof FloatSampleInput) {
			//	readFromFloatSourceStream();
			// } else {
				readFromByteSourceStream();
			// }
			if (TDebug.TraceAudioConverter) {
				testInFramesRead+=thisBuffer.getSampleCount();
				if (DEBUG_STREAM) {
					TDebug.out("Read "+thisBuffer.getSampleCount()+" frames from source stream. Total="+testInFramesRead);
				}
			}
			double inc=outSamples2inSamples(1.0);
			if (!thisBufferValid) {
				thisBufferValid=true;
				//dPos=inc/2;
				dPos=0.0;
			} else {
				double temp=dPos;
				dPos-=(double) oldSampleCount;
				if (DEBUG_STREAM) {
					TDebug.out("new dPos: "+temp+" - "+oldSampleCount+" = "+dPos);
				}
				if ((dPos>inc || dPos<-inc) && roundDown(dPos)!=0) {
					// hard-reset dPos if - why ever - it got out of bounds
					if (DEBUG_STREAM_PROBLEMS) {
						TDebug.out("Need to hard reset dPos="+dPos+" !!!!!!!!!!!!!!!!!!!!!!!");
					}
					//dPos=inc/2;
					dPos=0.0;
				}
			}
		}
		
		private int roundDown(double a) {
			//return a<0?((int) (a-0.5f)):((int) (a+0.5f));
			//return a<0?(-((int) -a)):((int) a);
			//return (int) a;
			return (int) Math.floor(a);
		}
		
		private int roundUp(double a) {
			//return a<0?((int) (a-0.5f)):((int) (a+0.5f));
			//return a<0?(-((int) -a)):((int) a);
			return (int) Math.ceil(a);
		}

		private void convertSampleAndHold(
				float[] inSamples, double inSampleOffset, int inSampleCount, double increment, 
				float[] outSamples, int outSampleOffset, int outSampleCount, float[] history, int historyLength) {
			if (DEBUG_STREAM) {
				TDebug.out("convertSampleAndHold(inSamples["+inSamples.length+"], "
					+roundDown(inSampleOffset)+" to "+roundDown(inSampleOffset+increment*(outSampleCount-1))+", "
					+"outSamples["+outSamples.length+"], "+outSampleOffset+" to "+(outSampleOffset+outSampleCount-1)+")");
				System.out.flush();
			}
			for (int i=0; i<outSampleCount; i++) {
				int iInIndex=roundDown(inSampleOffset+increment*i);
				if (iInIndex<0) {
					outSamples[i+outSampleOffset]=history[iInIndex+historyLength];
					if (DEBUG_STREAM) {
						TDebug.out("convertSampleAndHold: using history["+(iInIndex+historyLength)+" because inIndex="+iInIndex);
					}
				} 
				else if (iInIndex>=inSampleCount) {
					if (DEBUG_STREAM_PROBLEMS) {
						TDebug.out("convertSampleAndHold: INDEX OUT OF BOUNDS outSamples["+i+"]=inSamples[roundDown("+inSampleOffset+")="+iInIndex+"];");
					}
				} else {
					outSamples[i+outSampleOffset]=inSamples[iInIndex];
					//outSamples[i]=inSamples[roundDown(inSampleOffset)];
				}
				//inSampleOffset+=increment; <- this produces too much rounding errors...
			}
		}

		private void convertLinearInterpolation(
				float[] inSamples, double inSampleOffset, int inSampleCount, double increment, 
				float[] outSamples, int outSampleOffset, int outSampleCount, float[] history, int historyLength) {
			if (DEBUG_STREAM) {
				TDebug.out("convertLinearInterpolate(inSamples["+inSamples.length+"], "
					+roundDown(inSampleOffset)+" to "+roundDown(inSampleOffset+increment*(outSampleCount-1))+", "
					+"outSamples["+outSamples.length+"], "+outSampleOffset+" to "+(outSampleOffset+outSampleCount-1)+")");
				System.out.flush();
			}
			for (int i=0; i<outSampleCount; i++) {
				try {
					double dInIndex=inSampleOffset+increment*i-1;
					int iInIndex=(int) Math.floor(dInIndex);
					double factor=1.0d-(dInIndex-iInIndex);
					float value=0;
					for (int x=0; x<2; x++) {
						if (iInIndex>=inSampleCount) {
							// we clearly need more samples !
							if (DEBUG_STREAM_PROBLEMS) {
								TDebug.out("linear interpolation: INDEX OUT OF BOUNDS iInIndex="+iInIndex+" inSampleCount="+inSampleCount);
							}
						} 
						else if (iInIndex<0) {
							int histIndex=iInIndex+historyLength;
							if (histIndex>=0) {
								value+=history[histIndex]*factor;
								if (DEBUG_STREAM) {
									TDebug.out("linear interpolation: using history["+iInIndex+"]");
								}
							} 
							else if (DEBUG_STREAM_PROBLEMS) {
								TDebug.out("linear interpolation: history INDEX OUT OF BOUNDS iInIndex="+iInIndex+" histIndex="+histIndex+" history length="+historyLength);
							}
						} else {
							value+=inSamples[iInIndex]*factor;
						}
						factor=1-factor;
						iInIndex++;
					}
					outSamples[i+outSampleOffset]=value;
					//outSamples[i]=inSamples[roundDown(inSampleOffset)];
				} catch (ArrayIndexOutOfBoundsException aioobe) {  
					if (DEBUG_STREAM_PROBLEMS) {
						TDebug.out("**** REAL INDEX OUT OF BOUNDS ****** outSamples["+i+"]=inSamples[roundDown("+inSampleOffset+")="+roundDown(inSampleOffset)+"];");
					}
					//throw aioobe;
				}
				//inSampleOffset+=increment; <- this produces too much rounding errors...
			}
		}

		private double inSamples2outSamples(double inSamples) {
			return inSamples*targetSampleRate/sourceSampleRate;
		}

		private double outSamples2inSamples(double outSamples) {
			return outSamples*sourceSampleRate/targetSampleRate;
		}

		/**
		 * Main read method. It blocks until all samples are converted or
		 * the source stream is at its end or closed.<br>
		 * The sourceStream's sample rate is converted following
		 * the current setting of <code>conversionAlgorithm</code>.
		 * At most outBuffer.getSampleCount() are converted. In general,
		 * if the return value (and outBuffer.getSampleCount()) is less
		 * after processing this function, then it is an indicator
		 * that it was the last block to be processed.
		 * 
		 * @see #setConversionAlgorithm(int)
		 * @param outBuffer the buffer that the converted samples will be written to.
		 * @throws IllegalArgumentException when outBuffer's channel count does not match
		 * @return number of samples in outBuffer ( == outBuffer.getSampleCount())
		 *         or -1. A return value of 0 is only possible when outBuffer has 0 samples.
		 */
		public synchronized int read(FloatSampleBuffer outBuffer) throws IOException {
			if (isClosed()) {
				return -1;
			}
			if (outBuffer.getChannelCount()!=thisBuffer.getChannelCount()) {
				throw new IllegalArgumentException("passed buffer has different channel count");
			}
			if (outBuffer.getSampleCount()==0) {
				return 0;
			}
			if (TDebug.TraceAudioConverter) {
				TDebug.out(">SamplerateConverterStream.read("+outBuffer.getSampleCount()+"frames)");
			}
			float[] outSamples;
			float[] inSamples;
			float[] history;
			double increment=outSamples2inSamples(1.0);
			int writtenSamples=0;
			do {
				// check thisBuffer with samples of source stream
				int inSampleCount=thisBuffer.getSampleCount();
				if (roundDown(dPos)>=inSampleCount || !thisBufferValid) {
					// need to load new data of sourceStream
					readFromSourceStream();
					if (isClosed()) {
						break;
					}
					inSampleCount=thisBuffer.getSampleCount();
				}
				// calculate number of samples to write
				int writeCount=outBuffer.getSampleCount()-writtenSamples;
				// check whether this exceeds the current in-buffer
				if (roundDown(outSamples2inSamples((double) writeCount)+dPos)>=inSampleCount) {
					int lastOutIndex=roundUp(inSamples2outSamples(((double) inSampleCount)-dPos));
					// normally, the above formula gives the exact writeCount.
					// but due to rounding issues, sometimes it has to be decremented once.
					// so we need to iterate to get the last index and then increment it once to make 
					// it the writeCount (=the number of samples to write)
					while (roundDown(outSamples2inSamples((double) lastOutIndex)+dPos)>=inSampleCount) {
						lastOutIndex--;
						if (DEBUG_STREAM) {
							TDebug.out("--------- Decremented lastOutIndex="+lastOutIndex);
						}
					}
					if (DEBUG_STREAM_PROBLEMS) {
						int testLastOutIndex=writeCount-1;
						if (DEBUG_STREAM_PROBLEMS) {
							while (roundDown(outSamples2inSamples((double) testLastOutIndex)+dPos)>=inSampleCount) {
								testLastOutIndex--;
							}
						}
						if (testLastOutIndex!=lastOutIndex) {
							TDebug.out("lastOutIndex wrong: lastOutIndex="+lastOutIndex+" testLastOutIndex="+testLastOutIndex+" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
						}
					}
					writeCount=lastOutIndex+1;
				}
				// finally do the actual conversion - separated per channel
				for (int channel=0; channel<outBuffer.getChannelCount(); channel++) {
					inSamples=thisBuffer.getChannel(channel);
					outSamples=outBuffer.getChannel(channel);
					history=historyBuffer.getChannel(channel);
					switch (conversionAlgorithm) {
						case SAMPLE_AND_HOLD: convertSampleAndHold(inSamples, dPos, inSampleCount, increment, 
												outSamples, writtenSamples, writeCount, history, historyBuffer.getSampleCount()); break;
						case LINEAR_INTERPOLATION: convertLinearInterpolation(inSamples, dPos, inSampleCount, increment, 
												outSamples, writtenSamples, writeCount, history, historyBuffer.getSampleCount()); break;
					}
				}
				writtenSamples+=writeCount;
				// adjust new position
				dPos+=outSamples2inSamples((double) writeCount);
			} while (!isClosed() && writtenSamples<outBuffer.getSampleCount());
			if (writtenSamples<outBuffer.getSampleCount()) {
				outBuffer.changeSampleCount(writtenSamples, true);
			}
			if (TDebug.TraceAudioConverter) {
				testOutFramesReturned+=outBuffer.getSampleCount();
				TDebug.out("< return "+outBuffer.getSampleCount()+"frames. Total="+testOutFramesReturned+" frames. Read total "+testInFramesRead+" frames from source stream");
			}
			return outBuffer.getSampleCount();
		}
		

		//////////////////// utility methods ////////////////////////
		
		protected double sourceFrames2targetFrames(double sourceFrames) {
			return targetSampleRate/sourceSampleRate*sourceFrames;
		}

		protected double targetFrames2sourceFrames(double targetFrames) {
			return sourceSampleRate/targetSampleRate*targetFrames;
		}

		protected long sourceBytes2targetBytes(long sourceBytes) {
			long sourceFrames=sourceBytes/getSourceFrameSize();
			long targetFrames=(long) sourceFrames2targetFrames(sourceFrames);
			return targetFrames*getFrameSize();
		}

		protected long targetBytes2sourceBytes(long targetBytes) {
			long targetFrames=targetBytes/getFrameSize();
			long sourceFrames=(long) targetFrames2sourceFrames(targetFrames);
			return sourceFrames*getSourceFrameSize();
		}
		
		public int getFrameSize() {
			return getFormat().getFrameSize();
		}

		public int getSourceFrameSize() {
			return sourceStream.getFormat().getFrameSize();
		}

		//////////////////// methods overwritten of AudioInputStream ////////////////////////
		
		public int read() throws IOException {
			if (getFormat().getFrameSize() != 1) {
				throw new IOException("frame size must be 1 to read a single byte");
			}
			// very ugly, but efficient. Who uses this method anyway ?
			byte[] temp = new byte[1];
			int result = read(temp);
			if (result <= 0) {
				return -1;
			}
			return temp[0] & 0xFF;
		}
		
		/**
		 * @see #read(byte[], int, int)
		 */
		public int read(byte[] abData) throws IOException {
			return read(abData, 0, abData.length);
		}

		/**
		 * Read nLength bytes that will be the converted samples
		 * of the original inputStream.
		 * When nLength is not an integral number of frames,
		 * this method may read less than nLength bytes.
		 */
		public int read(byte[] abData, int nOffset, int nLength)
		throws	IOException {
			if (isClosed()) {
				return -1;
			}
			int frameCount=nLength/getFrameSize();
			if (writeBuffer==null) {
				writeBuffer=new FloatSampleBuffer(getFormat().getChannels(), frameCount, getFormat().getSampleRate());
			} else {
				writeBuffer.changeSampleCount(frameCount, false);
			}
			int writtenSamples=read(writeBuffer);
			if (writtenSamples==-1) {
				return -1;
			}
			int written=writeBuffer.convertToByteArray(abData, nOffset, getFormat());
			return written;
		}
	
		public synchronized long skip(long nSkip) throws IOException {
			// only returns integral frames
			long sourceSkip = targetBytes2sourceBytes(nSkip);
			long sourceSkipped = sourceStream.skip(sourceSkip);
			flush();
			return sourceBytes2targetBytes(sourceSkipped);
		}
	
	
		public int available() throws IOException {
			return (int) sourceBytes2targetBytes(sourceStream.available());
		}
	
	
		public void mark(int readlimit) {
			sourceStream.mark((int) targetBytes2sourceBytes(readlimit));
		}

		public synchronized void reset() throws IOException {
			sourceStream.reset();
			flush();
		}
	
		public boolean markSupported() {
			return sourceStream.markSupported();
		}
	
		public void close() throws IOException {
			if (isClosed()) {
				return;
			}
			sourceStream.close();
			// clean memory, this will also be an indicator that
			// the stream is closed
			thisBuffer=null;
			historyBuffer=null;
			byteBuffer=null;
		}
		
		///////////////////////////// additional methods /////////////////////////////

		public boolean isClosed() {
			return thisBuffer==null;
		}
		
		/**
		 * Flushes the internal buffers
		 */
		public synchronized void flush() {
			if (!isClosed()) {
				thisBufferValid=false;
				historyBuffer.makeSilence();
			}
		}
		
		/////////////////////////// Properties ///////////////////////////////////////
		
		public synchronized void setTargetSampleRate(float sr) {
			if (sr>0) {
				targetSampleRate=sr;
				//((SRCAudioFormat) getFormat()).setSampleRate(sr);
				resizeBuffers();
			}
		}
		
		public synchronized void setConversionAlgorithm(int algo) {
			if ((algo==SAMPLE_AND_HOLD || algo==LINEAR_INTERPOLATION) 
				&& (algo!=conversionAlgorithm)) {
					conversionAlgorithm=algo;
					resizeBuffers();
			}
		}

		public synchronized float getTargetSampleRate() {
			return targetSampleRate;
		}
		
		public synchronized int getConversionAlgorithm() {
			return conversionAlgorithm; 
		}

	}


	/**
	   Obviously, this class is used to be able to set the
	   frame rate/sample rate after the AudioFormat object
	   has been created. It assumes the PCM case where the
	   frame rate is always in sync with the sample rate.
	   (MP)
	*/
	public static class SRCAudioFormat extends AudioFormat {
		private float sampleRate;
		
		public SRCAudioFormat(AudioFormat targetFormat) {
			super(targetFormat.getEncoding(),
				targetFormat.getSampleRate(),
				targetFormat.getSampleSizeInBits(),
				targetFormat.getChannels(),
				targetFormat.getChannels()*targetFormat.getSampleSizeInBits()/8,
				targetFormat.getSampleRate(),
				targetFormat.isBigEndian());
			this.sampleRate=targetFormat.getSampleRate();
		}
	
		public void setSampleRate(float sr) {
			if (sr>0) {
				this.sampleRate=sr;
			}
		}
		
		public float getSampleRate() {
			return this.sampleRate;
		}
		
		public float getFrameRate() {
			return this.sampleRate;
		}
	}
}

/*** SampleRateConversionProvider.java ***/
