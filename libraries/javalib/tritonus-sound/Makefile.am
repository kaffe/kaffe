CLASSFILE = kaffe-tritonus.jar
SRCDIRS = org
jrelib_DATA = $(CLASSFILE)
LIBDIR = lib
JAR_CMD1 = ($(FASTJAR) cf $(CLASSFILE) -C $(LIBDIR) $(SRCDIRS))
JAR_CMD2 = ($(FASTJAR) uf $(CLASSFILE) -C $(srcdir) META-INF)

CPATH =  .:$(GLIBJ_ZIP)
AM_JAVACFLAGS = -bootclasspath $(CPATH)
JAVAC = $(JAVA_COMPILER)
JAVAROOT = $(LIBDIR)

$(CLASSFILE): classdist_tritonus.stamp
	rm -f $(CLASSFILE)
	$(JAR_CMD1)
	$(JAR_CMD2)

clean-local:
	rm -rf $(LIBDIR) $(CLASSFILE)

EXTRA_DIST = \
	META-INF

# Use a fake install class and dir to keep automake's JAVA (un)install tasks happy.
FAKE=fakeinstall
install-data-local: 
	touch $(FAKE).class
uninstall-local: 
	rm $(FAKE).class
tritonusdir = $(FAKE)

dist_tritonus_JAVA = \
	org/tritonus/core/Service.java \
	org/tritonus/core/TAudioConfig.java \
	org/tritonus/core/TInit.java \
	org/tritonus/core/TMidiConfig.java \
	org/tritonus/lowlevel/alsa/Alsa.java \
	org/tritonus/lowlevel/alsa/AlsaCtl.java \
	org/tritonus/lowlevel/alsa/AlsaCtlCardInfo.java \
	org/tritonus/lowlevel/alsa/AlsaMixer.java \
	org/tritonus/lowlevel/alsa/AlsaMixerElement.java \
	org/tritonus/lowlevel/alsa/AlsaPcm.java \
	org/tritonus/lowlevel/alsa/AlsaPcmHWParams.java \
	org/tritonus/lowlevel/alsa/AlsaPcmHWParamsFormatMask.java \
	org/tritonus/lowlevel/alsa/AlsaPcmSWParams.java \
	org/tritonus/lowlevel/alsa/AlsaSeq.java \
	org/tritonus/lowlevel/alsa/AlsaSeqClientInfo.java \
	org/tritonus/lowlevel/alsa/AlsaSeqEvent.java \
	org/tritonus/lowlevel/alsa/AlsaSeqPortInfo.java \
	org/tritonus/lowlevel/alsa/AlsaSeqPortSubscribe.java \
	org/tritonus/lowlevel/alsa/AlsaSeqQueueInfo.java \
	org/tritonus/lowlevel/alsa/AlsaSeqQueueStatus.java \
	org/tritonus/lowlevel/alsa/AlsaSeqQueueTempo.java \
	org/tritonus/lowlevel/alsa/AlsaSeqQueueTimer.java \
	org/tritonus/lowlevel/alsa/AlsaSeqRemoveEvents.java \
	org/tritonus/lowlevel/alsa/AlsaSeqSystemInfo.java \
	org/tritonus/lowlevel/esd/Esd.java \
	org/tritonus/lowlevel/esd/EsdRecordingStream.java \
	org/tritonus/lowlevel/esd/EsdSample.java \
	org/tritonus/lowlevel/esd/EsdStream.java \
	org/tritonus/midi/device/alsa/AlsaMidiChannel.java \
	org/tritonus/midi/device/alsa/AlsaMidiDevice.java \
	org/tritonus/midi/device/alsa/AlsaMidiDeviceProvider.java \
	org/tritonus/midi/device/alsa/AlsaMidiIn.java \
	org/tritonus/midi/device/alsa/AlsaMidiOut.java \
	org/tritonus/midi/device/alsa/AlsaQueueHolder.java \
	org/tritonus/midi/device/alsa/AlsaReceiver.java \
	org/tritonus/midi/device/alsa/AlsaSequencer.java \
	org/tritonus/midi/device/alsa/AlsaSequencerProvider.java \
	org/tritonus/midi/device/alsa/AlsaSequencerReceiver.java \
	org/tritonus/midi/device/alsa/AlsaSynthesizer.java \
	org/tritonus/midi/file/MidiConstants.java \
	org/tritonus/midi/file/StandardMidiFileReader.java \
	org/tritonus/midi/file/StandardMidiFileWriter.java \
	org/tritonus/sampled/convert/AlawFormatConversionProvider.java \
	org/tritonus/sampled/convert/PCM2PCMConversionProvider.java \
	org/tritonus/sampled/convert/SampleRateConversionProvider.java \
	org/tritonus/sampled/convert/SmartFormatConversionProvider.java \
	org/tritonus/sampled/convert/UlawFormatConversionProvider.java \
	org/tritonus/sampled/file/AiffAudioFileReader.java \
	org/tritonus/sampled/file/AiffAudioFileWriter.java \
	org/tritonus/sampled/file/AiffAudioOutputStream.java \
	org/tritonus/sampled/file/AiffTool.java \
	org/tritonus/sampled/file/AuAudioFileReader.java \
	org/tritonus/sampled/file/AuAudioFileWriter.java \
	org/tritonus/sampled/file/AuAudioOutputStream.java \
	org/tritonus/sampled/file/AuTool.java \
	org/tritonus/sampled/file/WaveAudioFileReader.java \
	org/tritonus/sampled/file/WaveAudioFileWriter.java \
	org/tritonus/sampled/file/WaveAudioOutputStream.java \
	org/tritonus/sampled/file/WaveTool.java \
	org/tritonus/sampled/mixer/alsa/AlsaBaseDataLine.java \
	org/tritonus/sampled/mixer/alsa/AlsaDataLineMixer.java \
	org/tritonus/sampled/mixer/alsa/AlsaDataLineMixerProvider.java \
	org/tritonus/sampled/mixer/alsa/AlsaPortMixer.java \
	org/tritonus/sampled/mixer/alsa/AlsaPortMixerProvider.java \
	org/tritonus/sampled/mixer/alsa/AlsaSourceDataLine.java \
	org/tritonus/sampled/mixer/alsa/AlsaTargetDataLine.java \
	org/tritonus/sampled/mixer/alsa/AlsaUtils.java \
	org/tritonus/sampled/mixer/esd/EsdClip.java \
	org/tritonus/sampled/mixer/esd/EsdMixer.java \
	org/tritonus/sampled/mixer/esd/EsdMixerProvider.java \
	org/tritonus/sampled/mixer/esd/EsdSourceDataLine.java \
	org/tritonus/sampled/mixer/esd/EsdTargetDataLine.java \
	org/tritonus/sampled/mixer/esd/EsdUtils.java \
	org/tritonus/share/midi/MidiUtils.java \
	org/tritonus/share/midi/TMidiDevice.java \
	org/tritonus/share/midi/TMidiFileFormat.java \
	org/tritonus/share/midi/TSequencer.java \
	org/tritonus/share/ArraySet.java \
	org/tritonus/share/GlobalInfo.java \
	org/tritonus/share/StringHashedSet.java \
	org/tritonus/share/TCircularBuffer.java \
	org/tritonus/share/TDebug.java \
	org/tritonus/share/TNotifier.java \
	org/tritonus/share/TSettings.java \
	org/tritonus/share/sampled/convert/TAsynchronousFilteredAudioInputStream.java \
	org/tritonus/share/sampled/convert/TAudioInputStream.java \
	org/tritonus/share/sampled/convert/TEncodingFormatConversionProvider.java \
	org/tritonus/share/sampled/convert/TFormatConversionProvider.java \
	org/tritonus/share/sampled/convert/TMatrixFormatConversionProvider.java \
	org/tritonus/share/sampled/convert/TSimpleFormatConversionProvider.java \
	org/tritonus/share/sampled/convert/TSynchronousFilteredAudioInputStream.java \
	org/tritonus/share/sampled/AudioFileTypes.java \
	org/tritonus/share/sampled/AudioFormatSet.java \
	org/tritonus/share/sampled/AudioFormats.java \
	org/tritonus/share/sampled/AudioSystemShadow.java \
	org/tritonus/share/sampled/AudioUtils.java \
	org/tritonus/share/sampled/Encodings.java \
	org/tritonus/share/sampled/FloatSampleBuffer.java \
	org/tritonus/share/sampled/TConversionTool.java \
	org/tritonus/share/sampled/TVolumeUtils.java \
	org/tritonus/share/sampled/file/AudioOutputStream.java \
	org/tritonus/share/sampled/file/HeaderlessAudioOutputStream.java \
	org/tritonus/share/sampled/file/TAudioFileFormat.java \
	org/tritonus/share/sampled/file/TAudioFileReader.java \
	org/tritonus/share/sampled/file/TAudioFileWriter.java \
	org/tritonus/share/sampled/file/TAudioOutputStream.java \
	org/tritonus/share/sampled/file/TDataOutputStream.java \
	org/tritonus/share/sampled/file/THeaderlessAudioFileWriter.java \
	org/tritonus/share/sampled/file/TNonSeekableDataOutputStream.java \
	org/tritonus/share/sampled/file/TSeekableDataOutputStream.java \
	org/tritonus/share/sampled/mixer/TBaseDataLine.java \
	org/tritonus/share/sampled/mixer/TBooleanControl.java \
	org/tritonus/share/sampled/mixer/TClip.java \
	org/tritonus/share/sampled/mixer/TCompoundControl.java \
	org/tritonus/share/sampled/mixer/TCompoundControlType.java \
	org/tritonus/share/sampled/mixer/TControlController.java \
	org/tritonus/share/sampled/mixer/TControllable.java \
	org/tritonus/share/sampled/mixer/TDataLine.java \
	org/tritonus/share/sampled/mixer/TEnumControl.java \
	org/tritonus/share/sampled/mixer/TFloatControl.java \
	org/tritonus/share/sampled/mixer/TLine.java \
	org/tritonus/share/sampled/mixer/TMixer.java \
	org/tritonus/share/sampled/mixer/TMixerInfo.java \
	org/tritonus/share/sampled/mixer/TMixerProvider.java \
	org/tritonus/share/sampled/mixer/TPort.java \
	org/tritonus/share/sampled/mixer/TSoftClip.java 

