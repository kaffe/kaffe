/*
 *	PCM2PCMConversionProvider.java
 */

/*
 *  Copyright (c) 2000 by Florian Bomers <florian@bome.com>
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

import java.util.Arrays;
import java.util.Iterator;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;

import org.tritonus.share.ArraySet;
import org.tritonus.share.TDebug;
import org.tritonus.share.sampled.AudioFormats;
import org.tritonus.share.sampled.Encodings;
import org.tritonus.share.sampled.FloatSampleBuffer;
import org.tritonus.share.sampled.TConversionTool;
import org.tritonus.share.sampled.convert.TSimpleFormatConversionProvider;
import org.tritonus.share.sampled.convert.TSynchronousFilteredAudioInputStream;

/**
 * This provider supports these PCM conversions (<--> meaning both directions):
 * <ul><li>8 Signed <-> 8 unsigned
 * <li>16/24/32 Signed little endian <-> 16/24/32 Signed big endian
 * <li>arbitrary conversion between 8/16/24/32 bit sample width<BR>
 *     (up-conversion is done by adding low-byte zero(s)).
 * <li>1 channel <-> x channels
 * </ul>
 * The class uses 2 different approaches for conversion:
 * <ol><li>Simple, often used conversions with performance-optimized methods.<br>
 *         These are the following conversions:<br>
 *         <ul><li>8 Signed <-> 8 unsigned
 *             <li>16 signed little endian <--> 16 signed big endian
 *             <li>24 signed little endian <--> 24 signed big endian
 *             <li>32 signed little endian <--> 32 signed big endian
 *             <li>16 signed little endian <--> 8 signed
 *             <li>16 signed big endian    <--> 8 signed
 *             <li>16 signed little endian <--> 8 unsigned
 *             <li>16 signed big endian    <--> 8 unsigned
 *         </ul><br>
 *         Downsampling from 16bit to 8bit is currently done
 *         using the float conversion (see next point), in order
 *         to profit of dithering.
 *     <li>All other conversions are done using the FloatSampleBuffer.<br>
 *         Mixdown of channels (x channels -> 1 channel) is done by 
 *         plainly adding all channels together.
 *         Thus, up mixing and down mixing will not result in the same audio, 
 *         as downmixing does NOT lower the volume and clippings are very 
 *         probable. To avoid that, the volume of the channels
 *         should be lowered before using this converter for down mixing.
 *     <li>All conversions support upmixing of channels: 
 *         1 channel -> x channels. This is done by
 *         copying the channel to the other channels <b>after</b>
 *         conversion of the format (if necessary).
 * </ol>
 * <p>SampleRate CANNOT be converted.
 *
 * @author Florian Bomers
 * @see org.tritonus.share.sampled.FloatSampleBuffer
 * @see org.tritonus.share.sampled.TConversionTool
 */

public class PCM2PCMConversionProvider
	extends TSimpleFormatConversionProvider {

	public static AudioFormat.Encoding PCM_SIGNED=Encodings.getEncoding("PCM_SIGNED");
	public static AudioFormat.Encoding PCM_UNSIGNED=Encodings.getEncoding("PCM_UNSIGNED");

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
	public PCM2PCMConversionProvider() {
		super(Arrays.asList(OUTPUT_FORMATS),
		      Arrays.asList(OUTPUT_FORMATS));
	}

	// formatType constants
	private static final int UNSIGNED8=1;
	private static final int SIGNED8=2;
	private static final int BIG_ENDIAN16=3;
	private static final int LITTLE_ENDIAN16=4;
	private static final int BIG_ENDIAN24=5;
	private static final int LITTLE_ENDIAN24=6;
	private static final int BIG_ENDIAN32=7;
	private static final int LITTLE_ENDIAN32=8;

	// conversionType
	private static final int CONVERT_NOT_POSSIBLE=0;
	private static final int CONVERT_SIGN=1;
	private static final int CONVERT_BYTE_ORDER16=2;
	private static final int CONVERT_BYTE_ORDER24=3;
	private static final int CONVERT_BYTE_ORDER32=4;
	private static final int CONVERT_16LTO8S=5;
	private static final int CONVERT_16LTO8U=6;
	private static final int CONVERT_16BTO8S=7;
	private static final int CONVERT_16BTO8U=8;
	private static final int CONVERT_8STO16L=9;
	private static final int CONVERT_8STO16B=10;
	private static final int CONVERT_8UTO16L=11;
	private static final int CONVERT_8UTO16B=12;
	private static final int CONVERT_ONLY_EXPAND_CHANNELS=13;
	private static final int CONVERT_FLOAT=100;   // all other conversions
	private static final int CONVERT_NONE=101;    // no conversion necessary

	public AudioInputStream getAudioInputStream(AudioFormat targetFormat, AudioInputStream sourceStream) {
		AudioFormat sourceFormat=sourceStream.getFormat();

		// the non-conversion case
		if (AudioFormats.matches(sourceFormat, targetFormat)) {
			return sourceStream;
		}
		if (doMatch(targetFormat.getFrameRate(), sourceFormat.getFrameRate())
		        && doMatch(targetFormat.getSampleRate(), sourceFormat.getSampleRate())) {

			targetFormat=replaceNotSpecified(sourceFormat, targetFormat);

			int sourceType=getFormatType(sourceFormat);
			int targetType=getFormatType(targetFormat);
			int conversionType=getConversionType(sourceType, sourceFormat.getChannels(),
			                                     targetType, targetFormat.getChannels());

			if (TDebug.TraceAudioConverter) {
				TDebug.out("PCM2PCM: sourceType="+formatType2Str(sourceType)+", "
				           +sourceFormat.getChannels()+"ch"
				           +" targetType="+formatType2Str(targetType)+", "
				           +targetFormat.getChannels()+"ch"
				           +" conversionType="+conversionType2Str(conversionType));
			}
			if (conversionType==CONVERT_NOT_POSSIBLE) {
				throw new IllegalArgumentException("format conversion not supported");
			}
			return new PCM2PCMStream(sourceStream, targetFormat,
			                         sourceType, targetType, conversionType);
		}

		throw new IllegalArgumentException("format conversion not supported");
	}

	public AudioFormat[] getTargetFormats(AudioFormat.Encoding targetEncoding,
	                                      AudioFormat sourceFormat) {
		if (TDebug.TraceAudioConverter) {
			TDebug.out(">PCM2PCMFormatConversionProvider.getTargetFormats(AudioFormat.Encoding, AudioFormat):");
			TDebug.out("checking out possible target formats");
			TDebug.out("from: " + sourceFormat);
			TDebug.out("to  : " + targetEncoding);
		}
		if (isConversionSupported(targetEncoding, sourceFormat)) {
			// TODO: check that no duplicates may occur...
			ArraySet result=new ArraySet();
			Iterator iterator=getCollectionTargetFormats().iterator();
			while (iterator.hasNext()) {
				AudioFormat targetFormat = (AudioFormat) iterator.next();
				targetFormat=replaceNotSpecified(sourceFormat, targetFormat);
				if (isConversionSupported(targetFormat, sourceFormat)) {
					result.add(targetFormat);
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


	/** method overidden due to the difficult situation with the channel count
	 * and the possible conversions possible.
	 */
	public boolean isConversionSupported(AudioFormat targetFormat,
	                                     AudioFormat sourceFormat) {
		targetFormat=replaceNotSpecified(sourceFormat, targetFormat);

		boolean res=AudioFormats.matches(sourceFormat, targetFormat)
		            || (
		                doMatch(targetFormat.getFrameRate(), sourceFormat.getFrameRate())
		                && doMatch(targetFormat.getSampleRate(), sourceFormat.getSampleRate())
		                && getConversionType(getFormatType(sourceFormat),
		                                     sourceFormat.getChannels(),
		                                     getFormatType(targetFormat),
		                                     targetFormat.getChannels())!=CONVERT_NOT_POSSIBLE);

		if (TDebug.TraceAudioConverter) {
			TDebug.out(">PCM2PCM: isConversionSupported(AudioFormat, AudioFormat):");
			TDebug.out("checking if conversion possible");
			TDebug.out("from: " + sourceFormat);
			TDebug.out("to  : " + targetFormat);
			TDebug.out("< result : " + res);
		}

		return res;
	}

	private int getFormatType(AudioFormat af) {
		int result=0;
		AudioFormat.Encoding encoding=af.getEncoding();
		boolean bigEndian=af.isBigEndian();
		int ssib=af.getSampleSizeInBits();
		// now set up the convert type
		if (encoding.equals(PCM_SIGNED)) {
			if (ssib==32) {
				if (bigEndian) {
					result=BIG_ENDIAN32;
				} else {
					result=LITTLE_ENDIAN32;
				}
			} else if (ssib==24) {
				if (bigEndian) {
					result=BIG_ENDIAN24;
				} else {
					result=LITTLE_ENDIAN24;
				}
			} else if (ssib==16) {
				if (bigEndian) {
					result=BIG_ENDIAN16;
				} else {
					result=LITTLE_ENDIAN16;
				}
			} else if (ssib==8) {
				result=SIGNED8;
			}
		} else if (encoding.equals(PCM_UNSIGNED)) {
			if (ssib==8) {
				result=UNSIGNED8;
			}
		}
		return result;
	}

	private int getConversionType(int sourceType, int sourceChannels,
	                              int targetType, int targetChannels) {
		if (sourceType==0 || targetType==0 ||
		        (sourceChannels!=1 && targetChannels!=1 && targetChannels!=sourceChannels)) {
			return CONVERT_NOT_POSSIBLE;
		}
		if (sourceType==targetType) {
			if (sourceChannels==targetChannels) {
				return CONVERT_NONE;
			} else if (sourceChannels==1 && targetChannels>1) {
				return CONVERT_ONLY_EXPAND_CHANNELS;
			}
		}
		if (sourceChannels==1 && targetChannels>=1 || sourceChannels==targetChannels) {
			// when channels only have to be duplicated, direct conversions can be done
			if ((sourceType==UNSIGNED8 && targetType==SIGNED8) ||
			        (sourceType==SIGNED8 && targetType==UNSIGNED8)) {
				return CONVERT_SIGN;
			} else if ((sourceType==BIG_ENDIAN16 && targetType==LITTLE_ENDIAN16) ||
			           (sourceType==LITTLE_ENDIAN16 && targetType==BIG_ENDIAN16)) {
				return CONVERT_BYTE_ORDER16;
			} else if ((sourceType==BIG_ENDIAN24 && targetType==LITTLE_ENDIAN24) ||
			           (sourceType==LITTLE_ENDIAN24 && targetType==BIG_ENDIAN24)) {
				return CONVERT_BYTE_ORDER24;
			} else if ((sourceType==BIG_ENDIAN32 && targetType==LITTLE_ENDIAN32) ||
			           (sourceType==LITTLE_ENDIAN32 && targetType==BIG_ENDIAN32)) {
				return CONVERT_BYTE_ORDER32;
				/*  downsampling is better handled with Float conversion -> dithering
				    } else 
				    if (sourceType==LITTLE_ENDIAN16 && targetType==SIGNED8) {
				    return CONVERT_16LTO8S;
				    } else if (sourceType==LITTLE_ENDIAN16 && targetType==UNSIGNED8) {
				    return CONVERT_16LTO8U;
				    } else if (sourceType==BIG_ENDIAN16 && targetType==SIGNED8) {
				    return CONVERT_16BTO8S;
				    } else if (sourceType==BIG_ENDIAN16 && targetType==UNSIGNED8) {
				    return CONVERT_16BTO8U;
				*/
			} else if (sourceType==SIGNED8 && targetType==LITTLE_ENDIAN16) {
				return CONVERT_8STO16L;
			} else if (sourceType==SIGNED8 && targetType==BIG_ENDIAN16) {
				return CONVERT_8STO16B;
			} else if (sourceType==UNSIGNED8 && targetType==LITTLE_ENDIAN16) {
				return CONVERT_8UTO16L;
			} else if (sourceType==UNSIGNED8 && targetType==BIG_ENDIAN16) {
				return CONVERT_8UTO16B;
			}
		}
		return CONVERT_FLOAT;
	}

	/**
	 * Debugging functions
	 */
	private static String formatType2Str(int formatType) {
		switch (formatType) {
		case 0:
			return "unsupported";
		case UNSIGNED8:
			return "UNSIGNED8";
		case SIGNED8:
			return "SIGNED8";
		case BIG_ENDIAN16:
			return "BIG_ENDIAN16";
		case LITTLE_ENDIAN16:
			return "LITTLE_ENDIAN16";
		case BIG_ENDIAN24:
			return "BIG_ENDIAN24";
		case LITTLE_ENDIAN24:
			return "LITTLE_ENDIAN24";
		case BIG_ENDIAN32:
			return "BIG_ENDIAN32";
		case LITTLE_ENDIAN32:
			return "LITTLE_ENDIAN32";
		}
		return "unknown";
	}

	private static String conversionType2Str(int conversionType) {
		switch (conversionType) {
		case CONVERT_NOT_POSSIBLE:
			return "CONVERT_NOT_POSSIBLE";
		case CONVERT_SIGN:
			return "CONVERT_SIGN";
		case CONVERT_BYTE_ORDER16:
			return "CONVERT_BYTE_ORDER16";
		case CONVERT_BYTE_ORDER24:
			return "CONVERT_BYTE_ORDER24";
		case CONVERT_BYTE_ORDER32:
			return "CONVERT_BYTE_ORDER32";
		case CONVERT_16LTO8S:
			return "CONVERT_16LTO8S";
		case CONVERT_16LTO8U:
			return "CONVERT_16LTO8U";
		case CONVERT_16BTO8S:
			return "CONVERT_16BTO8S";
		case CONVERT_16BTO8U:
			return "CONVERT_16BTO8U";
		case CONVERT_8STO16L:
			return "CONVERT_8STO16L";
		case CONVERT_8STO16B:
			return "CONVERT_8STO16B";
		case CONVERT_8UTO16L:
			return "CONVERT_8UTO16L";
		case CONVERT_8UTO16B:
			return "CONVERT_8UTO16B";
		case CONVERT_ONLY_EXPAND_CHANNELS:
			return "CONVERT_ONLY_EXPAND_CHANNELS";
		case CONVERT_FLOAT:
			return "CONVERT_FLOAT";
		case CONVERT_NONE:
			return "CONVERT_NONE";
		}
		return "unknown";
	}


	/**
	 * PCM2PCMStream
	 * Provides direct conversion of some selected formats and rxpanding of channels.
	 */

	class PCM2PCMStream extends TSynchronousFilteredAudioInputStream {
		private int conversionType;
		private int sourceType;
		private int targetType;
		private boolean needExpandChannels;
		private boolean needMixDown;

		private AudioFormat intermediateFloatBufferFormat;
		private FloatSampleBuffer floatBuffer=null;

		public PCM2PCMStream(AudioInputStream sourceStream, AudioFormat targetFormat,
		                     int sourceType, int targetType, int conversionType) {
			// transform the targetFormat so that
			// FrameRate, and SampleRate match the sourceFormat
			super (sourceStream, new AudioFormat(
			           targetFormat.getEncoding(),
			           sourceStream.getFormat().getSampleRate(),
			           targetFormat.getSampleSizeInBits(),
			           targetFormat.getChannels(),
			           targetFormat.getChannels()*targetFormat.getSampleSizeInBits()/8,
			           sourceStream.getFormat().getFrameRate(),
			           targetFormat.isBigEndian()));
			if (TDebug.TraceAudioConverter) {
				TDebug.out("PCM2PCMStream: constructor. ConversionType="
				           +conversionType2Str(conversionType));
			}
			this.conversionType=conversionType;
			this.sourceType=sourceType;
			this.targetType=targetType;
			needExpandChannels=sourceStream.getFormat().getChannels()<targetFormat.getChannels();
			needMixDown=sourceStream.getFormat().getChannels()>targetFormat.getChannels();

			// some sanity tests. These can be dropped when this converter has been tested enough...
			if (needMixDown && conversionType!=CONVERT_FLOAT) {
				throw new IllegalArgumentException(
				    "PCM2PCMStream: MixDown only possible with CONVERT_FLOAT");
			}
			if (needMixDown && targetFormat.getChannels()!=1) {
				throw new IllegalArgumentException(
				    "PCM2PCMStream: MixDown only possible with target channel count=1");
			}
			if (needExpandChannels && sourceStream.getFormat().getChannels()!=1) {
				throw new IllegalArgumentException(
				    "PCM2PCMStream: Expanding channels only possible with source channel count=1");
			}
			// end sanity

			if (conversionType==CONVERT_FLOAT) {
				int floatChannels=needExpandChannels?1:targetFormat.getChannels();
				intermediateFloatBufferFormat=new AudioFormat(
				                                  targetFormat.getEncoding(),
				                                  sourceStream.getFormat().getSampleRate(),
				                                  targetFormat.getSampleSizeInBits(),
				                                  floatChannels,
				                                  floatChannels*targetFormat.getSampleSizeInBits()/8,
				                                  sourceStream.getFormat().getFrameRate(),
				                                  targetFormat.isBigEndian());
				// with floatBuffer we need to copy anyway, so enable in-place conversion
				enableConvertInPlace();
			}

			if (!needExpandChannels &&
			        (conversionType==CONVERT_SIGN ||
			         conversionType==CONVERT_BYTE_ORDER16 ||
			         conversionType==CONVERT_BYTE_ORDER24 ||
			         conversionType==CONVERT_BYTE_ORDER32)) {
				enableConvertInPlace();
			}
		}

		// these functions only treat the highbyte of 16bit samples
		// obsolete: is handled with FloatBuffer because of dithering
		private void do16BTO8S(byte[] inBuffer, int inCounter,
		                       byte[] outBuffer, int outByteOffset, int sampleCount) {
			for (; sampleCount>0; sampleCount--, inCounter++) {
				outBuffer[outByteOffset++]=inBuffer[inCounter++];
			}
		}

		// obsolete: is handled with FloatBuffer because of dithering
		private void do16BTO8U(byte[] inBuffer, int inCounter,
		                       byte[] outBuffer, int outByteOffset, int sampleCount) {
			for (; sampleCount>0; sampleCount--, inCounter++) {
				outBuffer[outByteOffset++]=(byte)(inBuffer[inCounter++]+128);
			}
		}

		private void do8STO16L(byte[] inBuffer, byte[] outBuffer,
		                       int outByteOffset, int sampleCount) {
			for (int inCounter=0; sampleCount>0; sampleCount--) {
				outBuffer[outByteOffset++]=0;
				outBuffer[outByteOffset++]=inBuffer[inCounter++];
			}
		}

		private void do8UTO16L(byte[] inBuffer, byte[] outBuffer,
		                       int outByteOffset, int sampleCount) {
			for (int inCounter=0; sampleCount>0; sampleCount--) {
				outBuffer[outByteOffset++]=0;
				outBuffer[outByteOffset++]=(byte)(inBuffer[inCounter++]+128);
			}
		}

		private void do8STO16B(byte[] inBuffer, byte[] outBuffer,
		                       int outByteOffset, int sampleCount) {
			for (int inCounter=0; sampleCount>0; sampleCount--) {
				outBuffer[outByteOffset++]=inBuffer[inCounter++];
				outBuffer[outByteOffset++]=0;
			}
		}

		private void do8UTO16B(byte[] inBuffer, byte[] outBuffer,
		                       int outByteOffset, int sampleCount) {
			for (int inCounter=0; sampleCount>0; sampleCount--) {
				outBuffer[outByteOffset++]=(byte)(inBuffer[inCounter++]+128);
				outBuffer[outByteOffset++]=0;
			}
		}

		// copies the channels: in the buffer there is only one channel
		private void expandChannels(byte[] buffer, int offset, int frameCount, int bytesPerFrame, int channels) {
			int inOffset=offset+bytesPerFrame*frameCount;
			int outOffset=offset+bytesPerFrame*channels*frameCount;
			switch (bytesPerFrame) {
			case 1:
				if (channels==2) {
					for (;frameCount>0; frameCount--) {
						buffer[--outOffset]=buffer[--inOffset];
						buffer[--outOffset]=buffer[inOffset];
					}
				} else {
					for (;frameCount>0; frameCount--) {
						inOffset--;
						for (int channel=0; channel<channels; channel++) {
							buffer[--outOffset]=buffer[inOffset];
						}
					}
				}
				break;
			case 2:
				if (channels==2) {
					for (;frameCount>0; frameCount--) {
						buffer[--outOffset]=buffer[--inOffset];
						buffer[--outOffset]=buffer[inOffset-1];
						buffer[--outOffset]=buffer[inOffset];
						buffer[--outOffset]=buffer[--inOffset];
					}
				} else {
					for (;frameCount>0; frameCount--) {
						inOffset--;
						for (int channel=0; channel<channels; channel++) {
							buffer[--outOffset]=buffer[inOffset];
							buffer[--outOffset]=buffer[inOffset-1];
						}
						inOffset--;
					}
				}
				break;
			default:
				for (;frameCount>0; frameCount--) {
					for (int channel=0; channel<channels; channel++) {
						for (int by=1; by<=bytesPerFrame; by++) {
							buffer[--outOffset]=buffer[inOffset-by];
						}
					}
					inOffset-=bytesPerFrame;
				}
				break;
			}
		}

		private void doFloatConversion(FloatSampleBuffer buffer, boolean expandChannels) {
			if (needMixDown) {
				buffer.mixDownChannels();
			}
			if (expandChannels) {
				buffer.expandChannel(getFormat().getChannels());
			}
		}

		private void doFloatConversion(byte[] inBuffer, int inByteOffset,
		                               byte[] outBuffer, int outByteOffset, int sampleCount) {
			int byteCount=sampleCount*(getOriginalStream().getFormat().getSampleSizeInBits()/8);
			if (floatBuffer==null) {
				floatBuffer=new FloatSampleBuffer();
			}
			floatBuffer.initFromByteArray(inBuffer, inByteOffset, byteCount, getOriginalStream().getFormat());
			doFloatConversion(floatBuffer, false); // expansion is done on byte array
			floatBuffer.convertToByteArray(outBuffer, outByteOffset, intermediateFloatBufferFormat);
		}

		protected int convert(byte[] inBuffer, byte[] outBuffer, int outByteOffset, int inFrameCount) {
			int sampleCount=inFrameCount*getOriginalStream().getFormat().getChannels();
			switch (conversionType) {
			case CONVERT_SIGN:
				TConversionTool.convertSign8(
				    inBuffer, 0, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_BYTE_ORDER16:
				TConversionTool.swapOrder16(
				    inBuffer, 0, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_BYTE_ORDER24:
				TConversionTool.swapOrder24(
				    inBuffer, 0, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_BYTE_ORDER32:
				TConversionTool.swapOrder32(
				    inBuffer, 0, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_16LTO8S:
				do16BTO8S(inBuffer, 1, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_16LTO8U:
				do16BTO8U(inBuffer, 1, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_16BTO8S:
				do16BTO8S(inBuffer, 0, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_16BTO8U:
				do16BTO8U(inBuffer, 0, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_8STO16L:
				do8STO16L(inBuffer, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_8STO16B:
				do8STO16B(inBuffer, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_8UTO16L:
				do8UTO16L(inBuffer, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_8UTO16B:
				do8UTO16B(inBuffer, outBuffer, outByteOffset, sampleCount);
				break;
			case CONVERT_ONLY_EXPAND_CHANNELS:
				// implicit: channelCount in inBuffer=1
				System.arraycopy(inBuffer, 0, outBuffer, outByteOffset,
				                 inFrameCount*getOriginalStream().getFormat().getFrameSize());
				break;
			case CONVERT_FLOAT:
				doFloatConversion(inBuffer, 0, outBuffer, outByteOffset, sampleCount);
				break;
			default:
				throw new RuntimeException(
				    "PCM2PCMStream: Call to convert with unknown conversionType.");
			}
			if (needExpandChannels) {
				expandChannels(outBuffer, outByteOffset, inFrameCount,
				               getFormat().getSampleSizeInBits()/8,
				               getFormat().getChannels());
			}
			return inFrameCount;
		}

		protected void convertInPlace(byte[] buffer, int byteOffset, int frameCount) {
			int sampleCount=frameCount*getOriginalStream().getFormat().getChannels();
			switch (conversionType) {
			case CONVERT_SIGN:
				TConversionTool.convertSign8(buffer, byteOffset, sampleCount);
				break;
			case CONVERT_BYTE_ORDER16:
				TConversionTool.swapOrder16(buffer, byteOffset, sampleCount);
				break;
			case CONVERT_BYTE_ORDER24:
				TConversionTool.swapOrder24(buffer, byteOffset, sampleCount);
				break;
			case CONVERT_BYTE_ORDER32:
				TConversionTool.swapOrder32(buffer, byteOffset, sampleCount);
				break;
			case CONVERT_FLOAT:
				doFloatConversion(buffer, byteOffset, buffer, byteOffset, sampleCount);
				if (needExpandChannels) {
					expandChannels(buffer, byteOffset, frameCount,
					               getFormat().getSampleSizeInBits()/8,
					               getFormat().getChannels());
				}
				break;
			default:
				throw new RuntimeException(
				    "PCM2PCMStream: Call to convertInPlace, but it cannot convert in place.");
			}
		}
	}

}

/*** PCM2PCMFormatConversionProvider.java ***/
