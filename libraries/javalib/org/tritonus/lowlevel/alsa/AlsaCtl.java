/*
 *	AlsaCtl.java

 for the following, see 'man ident'.

$Id: AlsaCtl.java,v 1.1 2003/01/06 17:14:34 dalibor Exp $
$Log: AlsaCtl.java,v $
Revision 1.1  2003/01/06 17:14:34  dalibor
2003-01-06 Dalibor Topic <robilad@yahoo.com>

        Merged in java.awt.image.renderable from GNU Classpath.
        Merged in java.awt.Graphics2D from GNU Classpath.

        * WHATSNEW:
        Added java.awt.image.renderable to the new package list.

        * libraries/javalib/java/awt/image/ColorModel.java:
        Replaced by version from GNU Classpath.

        * libraries/javalib/java/awt/image/DirectColorModel.java:
        (DirectColorModel) Added missing constructor.

        * libraries/javalib/gnu/java/awt/ComponentDataBlitOp.java,
        libraries/javalib/java/awt/Graphics2D.java,
        libraries/javalib/java/awt/GraphicsConfigTemplate.java,
        libraries/javalib/java/awt/GraphicsConfiguration.java,
        libraries/javalib/java/awt/GraphicsDevice.java,
        libraries/javalib/java/awt/image/BufferedImage.java,
        libraries/javalib/java/awt/image/BufferedImageOp.java,
        libraries/javalib/java/awt/image/ComponentColorModel.java,
        libraries/javalib/java/awt/image/RasterOp.java,
        libraries/javalib/java/awt/image/RenderedImage.java,
        libraries/javalib/java/awt/image/VolatileImage.java,
        libraries/javalib/java/awt/image/renderable/ContextualRenderedImageFactory.java,
        libraries/javalib/java/awt/image/renderable/ParameterBlock.java,
        libraries/javalib/java/awt/image/renderable/RenderableImage.java,
        libraries/javalib/java/awt/image/renderable/RenderableImageOp.java,
        libraries/javalib/java/awt/image/renderable/RenderableImageProducer.java,
        libraries/javalib/java/awt/image/renderable/RenderContext.java,
        libraries/javalib/java/awt/image/renderable/RenderedImageFactory.java:
        New files, taken from GNU Classpath.

        * libraries/javalib/Makefile.am:
        Added new files.

        * libraries/javalib/Makefile.in:
        regenerated.

2003-01-04 Dalibor Topic <robilad@yahoo.com>

        Fixed some gcc 3.2 warnings with different warning switches.

        * kaffe/kaffevm/jar.c,
        kaffe/kaffevm/classMethod.h,
        kaffe/kaffevm/constants.c,
        kaffe/kaffevm/utf8const.c,
        kaffe/kaffeh/support.c,
        libraries/clib/native/ObjectStreamClassImpl.c,
        libraries/clib/awt/X/clr.c,
        libraries/clib/awt/X/evt.c,
        libraries/clib/awt/X/tlk.c,
        kaffe/kaffevm/jit3/labels.c,
        kaffe/kaffe/version.c:
        Turned C++ comments into C comments.

        * kaffe/kaffevm/jar.h,
        kaffe/kaffevm/jit3/basecode.h,
        kaffe/xprof/mangle.h,
        kaffe/xprof/fileSections.h,
        kaffe/xprof/sectionFile.h,
        kaffe/xprof/debugFile.h:
        Removed comma at end of enumerator lists.

        * kaffe/kaffevm/systems/unix-jthread.h:
        Cast void pointers to char * before using them in arithmetic.

        * kaffe/kaffevm/jar.h:
        (jarFile) Changed type of 'size' to size_t. Chaged type of 'error'
        to const char *. Chaged type of data to uint8*.

        * kaffe/kaffevm/jar.c:
        Use verbosely named string constants for error messages.
        (JAR_ERROR_BAD_CENTRAL_RECORD_SIGNATURE, JAR_ERROR_BAD_SIGNATURE,
        JAR_ERROR_DECOMPRESSION_FAILED, JAR_ERROR_ENTRY_COUNT_MISMATCH,
        JAR_ERROR_IMPOSSIBLY_LARGE_DIRECTORY, JAR_ERROR_IO,
        JAR_ERROR_NO_END, JAR_ERROR_OUT_OF_MEMORY,
        JAR_ERROR_TRUNCATED_FILE, JAR_ERROR_UNSUPPORTED_COMPRESSION) Added
        constants.
        (findJarFile, cacheJarFile, removeJarFile) removed trivially true
        asserts.
        (openJarFile) Added braces around an if statement.
        (jarRead) Changed type of 'len' to size_t. Changed type of buf to
        uint8*.
        (readJarHeader) Changed type of 'len' to size_t.

        * libraries/clib/native/ZipFile.c:
        (java_util_zip_ZipFile_getZipEntries0) declared j as unsigned int
        to avoid comparison between signed and unsigned

        * libraries/clib/zip/CRC32.c:
        (crc_table) shuffled 'static' to beginning of declaration.

2003-01-03 Dalibor Topic <robilad@yahoo.com>

        * kaffe/kaffevm/jar.c:
        (getDataJarFile) removed unused label 'data'.

        * kaffe/kaffevm/readClass.h:
        [KAFFEH] Added prototype for finishMethods.

        * kaffe/kaffevm/exception.h:
        (vmExcept_setIntrpFrame) Only define if INTERPRETER is defined.

        * kaffe/kaffevm/external.c:
        (loadNativeLibrary2) Removed unused variable status.

        * kaffe/kaffevm/gcRefs.c:
        (TwalkThread) use %p for jthread_current in dprintf.

        * include/nets.h:
        [h_errno] Turned h_errno after #endif into a comment.

        * config/i386/trampolines.c:
        Fixed gcc warnings about deprecated multi-line strings.

        * config/i386/jit3-i386.def,
        kaffe/kaffevm/jit3/labels.c,
        kaffe/kaffevm/jit3/labels.h:
        Merged in jit3 changes from JanosVM 0.8.0.

        2002-07-27 Saturday 15:47  stack

        * config/i386/jit3-i386.def: eliminate noop branch to epilogue,
        switch around requests for float values to avoid extra
        loads/stores, updated debugging printfs to print out label names

        2002-02-20 Wednesday 14:00  stack

        * config/i386/jit3-i386.def: add a debug printf for the method name

        2002-07-27 Saturday 16:08  stack

        * kaffe/kaffevm/jit3/: labels.c, labels.h: better debugging
        branches to the epilogue

2003-01-03 Timothy S Stack <stack@cs.utah.edu>

        * kaffe/kaffevm/jit3/machine.h: get rid of ## in
        canCatch/willCatch, theres no need for them, there can be space
        between the '.' token and the field name token.

2003-01-03 Dan Kegel <dank@kegel.com>

        * kaffe/kaffe/Makefile.am:
        (kaffe_bin_LDADD) Added @LIBLTDL@, fixes linking with CVS version
        of libtool.

2003-01-03 Dylan Schell <dylans@xs4all.nl>
        Ported kaffe to Playstation 2.

        * config/mips/common.h:
        Added specific settings for R5900, the CPU in Playstation 2.

        * config/mips/linux/md.h:
        (FPOFFSET) Set for Playstation 2.

        * config/mips/o32-sysdepCallMethod.h:
        (split) new struct used to fake a double on Playstation 2.
        Use split for dealing with doubles on Playstation 2.

2003-01-02 Helmer Kraemer <hkraemer@freenet.de>

        * kaffe/kaffevm/external.c:
        (LIBRARYLOAD) use lt_dlopenext instead of lt_dlopen. Let Libtool
        append the extensions itself.
        (LTDL_ARCHIVE_EXT) removed.
        (libSuffixes) removed.
        (TRY_LOAD_FOUND, TRY_LOAD_NOT_FOUND, TRY_LOAD_ERROR) removed.
        (loadNativeLibrary2) just use LBRARYLOAD.

        * libltdl/ltdl.c:
        (lt_dlopenext) contained a bug that prevented it
        from trying to load  non-libtool libraries.

        Reported by: Richard Stahl <richard.stahl@imec.be>

2003-01-01 Dalibor Topic <robilad@yahoo.com>

        Merged in functional support for javax.sound from
        LGPL-d implementation from tritonus.org.

        Many thanks to Matthias Pfisterer <Matthias.Pfisterer@gmx.de>, who
        made it all possible, and to Helmer Kraemer <hkraemer@freenet.de>,
        who fixed the bugs in kaffeh quickly.

        * FAQ/FAQ.sound: new file.

        * configure.in:
        Added sqaure braces around 'X', in order to indicate it's the
        default choice for an AWT backend. Added configuration machinery
        to select the sound backend, adapted from tritonus.org sources.
        (SOUND_DIR) new variable for the selected sound backend.

        * include/Makefile.am:
        Added headers for org.tritonus files.

        * include/jmalloc.h:
        include <stdlib.h> in order to define size_t.

        * kaffe/kaffeh/support.c:
        (setFieldValue) print constants as defines in -jni mode, too.

        * kaffe/kaffevm/classMethod.h:
        (METHOD_IS_NATIVE) new convenience macro.

        * libraries/extensions/Makefile.am:
        (SUBDIRS) Added sound directory.

        * libraries/javalib/Makefile.am:
        Added tritonus sources, and separated GNU JAXP sompilation into
        its own pass.
        (JAXP_SRCS) new variable for GNU JAXP sources.
        (SOUND_SRCS) new variable for tritonus sources.

        * libraries/javalib/bootstrap.classlist:
        Added classes from tritonus necessary for native sound libraries.

        * libraries/javalib/Klasses.jar.bootstrap:
        regenerated.

        * Makefile.in,
        config/Makefile.in,
        configure,
        include/Makefile.in,
        kaffe/Makefile.in,
        kaffe/kaffe/Makefile.in,
        kaffe/kaffeh/Makefile.in,
        kaffe/kaffevm/Makefile.in,
        kaffe/kaffevm/gcj/Makefile.in,
        kaffe/kaffevm/intrp/Makefile.in,
        kaffe/kaffevm/jit/Makefile.in,
        kaffe/kaffevm/jit3/Makefile.in,
        kaffe/kaffevm/systems/Makefile.in,
        kaffe/kaffevm/systems/beos-native/Makefile.in,
        kaffe/kaffevm/systems/oskit-pthreads/Makefile.in,
        kaffe/kaffevm/systems/unix-jthreads/Makefile.in,
        kaffe/kaffevm/systems/unix-pthreads/Makefile.in,
        kaffe/man/Makefile.in,
        kaffe/scripts/Makefile.in,
        kaffe/scripts/bat/Makefile.in,
        kaffe/scripts/compat/Makefile.in,
        kaffe/xprof/Makefile.in,
        libraries/Makefile.in,
        libraries/clib/Makefile.in,
        libraries/clib/awt/Makefile.in,
        libraries/clib/awt/X/Makefile.in,
        libraries/clib/awt/qt/Makefile.in,
        libraries/clib/io/Makefile.in,
        libraries/clib/management/Makefile.in,
        libraries/clib/math/Makefile.in,
        libraries/clib/native/Makefile.in,
        libraries/clib/net/Makefile.in,
        libraries/clib/security/Makefile.in,
        libraries/clib/zip/Makefile.in,
        libraries/extensions/Makefile.in,
        libraries/extensions/comm/Makefile.in,
        libraries/extensions/comm/javalib/Makefile.in,
        libraries/extensions/microsoft/Makefile.in,
        libraries/extensions/microsoft/clib/Makefile.in,
        libraries/extensions/microsoft/javalib/Makefile.in,
        libraries/extensions/pjava/Makefile.in,
        libraries/extensions/pjava/javalib/Makefile.in,
        libraries/extensions/servlet/Makefile.in,
        libraries/extensions/servlet/javalib/Makefile.in,
        libraries/javalib/Makefile.in,
        test/Makefile.in,
        test/regression/Makefile.in:
        regenerated.

        libraries/extensions/sound/Makefile.am,
        libraries/extensions/sound/Makefile.in,
        libraries/extensions/sound/alsa/.cvsignore,
        libraries/extensions/sound/alsa/Makefile.am,
        libraries/extensions/sound/alsa/Makefile.in,
        libraries/extensions/sound/alsa/common.h,
        libraries/extensions/sound/alsa/constants_check.h,
        libraries/extensions/sound/alsa/init.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_Alsa.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaCtl.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaCtlCardInfo.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaMixer.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaMixerElement.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaPcm.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaPcmHWParams.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaPcmHWParamsFormatMask.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaPcmSWParams.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaSeq.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaSeqClientInfo.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaSeqEvent.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaSeqPortInfo.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaSeqQueueInfo.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaSeqQueueStatus.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents.c,
        libraries/extensions/sound/alsa/org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo.c,
        libraries/extensions/sound/common/.cvsignore,
        libraries/extensions/sound/common/HandleFieldHandler.h,
        libraries/extensions/sound/common/Makefile.am,
        libraries/extensions/sound/common/Makefile.in,
        libraries/extensions/sound/common/common.c,
        libraries/extensions/sound/common/common.h,
        libraries/extensions/sound/common/debug.h,
        libraries/extensions/sound/esd/.cvsignore,
        libraries/extensions/sound/esd/Makefile.am,
        libraries/extensions/sound/esd/Makefile.in,
        libraries/extensions/sound/esd/common.h,
        libraries/extensions/sound/esd/org_tritonus_lowlevel_esd_EsdRecordingStream.c,
        libraries/extensions/sound/esd/org_tritonus_lowlevel_esd_EsdStream.c,
        libraries/javalib/META-INF/services/javax.sound.midi.spi.MidiDeviceProvider,
        libraries/javalib/META-INF/services/javax.sound.midi.spi.MidiFileReader,
        libraries/javalib/META-INF/services/javax.sound.midi.spi.MidiFileWriter,
        libraries/javalib/META-INF/services/javax.sound.midi.spi.SoundbankReader,
        libraries/javalib/META-INF/services/javax.sound.sampled.spi.AudioFileReader,
        libraries/javalib/META-INF/services/javax.sound.sampled.spi.AudioFileWriter,
        libraries/javalib/META-INF/services/javax.sound.sampled.spi.FormatConversionProvider,
        libraries/javalib/META-INF/services/javax.sound.sampled.spi.MixerProvider,
        libraries/javalib/javax/sound/midi/ControllerEventListener.java,
        libraries/javalib/javax/sound/midi/Instrument.java,
        libraries/javalib/javax/sound/midi/InvalidMidiDataException.java,
        libraries/javalib/javax/sound/midi/MetaEventListener.java,
        libraries/javalib/javax/sound/midi/MetaMessage.java,
        libraries/javalib/javax/sound/midi/MidiChannel.java,
        libraries/javalib/javax/sound/midi/MidiDevice.java,
        libraries/javalib/javax/sound/midi/MidiEvent.java,
        libraries/javalib/javax/sound/midi/MidiFileFormat.java,
        libraries/javalib/javax/sound/midi/MidiMessage.java,
        libraries/javalib/javax/sound/midi/MidiSystem.java,
        libraries/javalib/javax/sound/midi/MidiUnavailableException.java,
        libraries/javalib/javax/sound/midi/Patch.java,
        libraries/javalib/javax/sound/midi/Receiver.java,
        libraries/javalib/javax/sound/midi/Sequence.java,
        libraries/javalib/javax/sound/midi/Sequencer.java,
        libraries/javalib/javax/sound/midi/ShortMessage.java,
        libraries/javalib/javax/sound/midi/Soundbank.java,
        libraries/javalib/javax/sound/midi/SoundbankResource.java,
        libraries/javalib/javax/sound/midi/Synthesizer.java,
        libraries/javalib/javax/sysexMessage.java,
        libraries/javalib/javax/sound/midi/Track.java,
        libraries/javalib/javax/sound/midi/Transmitter.java,
        libraries/javalib/javax/sound/midi/VoiceStatus.java,
        libraries/javalib/javax/sound/midi/spi/MidiDeviceProvider.java,
        libraries/javalib/javax/sound/midi/spi/MidiFileReader.java,
        libraries/javalib/javax/sound/midi/spi/MidiFileWriter.java,
        libraries/javalib/javax/sound/midi/spi/SoundbankReader.java,
        libraries/javalib/javax/sound/sampled/AudioFileFormat.java,
        libraries/javalib/javax/sound/sampled/AudioFormat.java,
        libraries/javalib/javax/sound/sampled/AudioInputStream.java,
        libraries/javalib/javax/sound/sampled/AudioPermission.java,
        libraries/javalib/javax/sound/sampled/AudioSystem.java,
        libraries/javalib/javax/sound/sampled/BooleanControl.java,
        libraries/javalib/javax/sound/sampled/Clip.java,
        libraries/javalib/javax/sound/sampled/CompoundControl.java,
        libraries/javalib/javax/sound/sampled/Control.java,
        libraries/javalib/javax/sound/sampled/DataLine.java,
        libraries/javalib/javax/sound/sampled/EnumControl.java,
        libraries/javalib/javax/sound/sampled/FloatControl.java,
        libraries/javalib/javax/sound/sampled/Line.java,
        libraries/javalib/javax/sound/sampled/LineEvent.java,
        libraries/javalib/javax/sound/sampled/LineListener.java,
        libraries/javalib/javax/sound/sampled/LineUnavailableException.java,
        libraries/javalib/javax/sound/sampled/Mixer.java,
        libraries/javalib/javax/sound/sampled/Port.java,
        libraries/javalib/javax/sound/sampled/ReverbType.java,
        libraries/javalib/javax/sound/sampled/SourceDataLine.java,
        libraries/javalib/javax/sound/sampled/TargetDataLine.java,
        libraries/javalib/javax/sound/sampled/UnsupportedAudioFileException.java,
        libraries/javalib/javax/sound/sampled/spi/AudioFileReader.java,
        libraries/javalib/javax/sound/sampled/spi/AudioFileWriter.java,
        libraries/javalib/javax/sound/sampled/spi/FormatConversionProvider.java,
        libraries/javalib/javax/sound/sampled/spi/MixerProvider.java,
        libraries/javalib/org/tritonus/core/Service.java,
        libraries/javalib/org/tritonus/core/TAudioConfig.java,
        libraries/javalib/org/tritonus/core/TInit.java,
        libraries/javalib/org/tritonus/core/TMidiConfig.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/Alsa.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaCtl.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaCtlCardInfo.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaMixer.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaMixerElement.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaPcm.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaPcmHWParams.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaPcmHWParamsFormatMask.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaPcmSWParams.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeq.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeqClientInfo.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeqEvent.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeqPortInfo.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeqPortSubscribe.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeqQueueInfo.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeqQueueStatus.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeqQueueTempo.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeqQueueTimer.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeqRemoveEvents.java,
        libraries/javalib/org/tritonus/lowlevel/alsa/AlsaSeqSystemInfo.java,
        libraries/javalib/org/tritonus/lowlevel/esd/Esd.java,
        libraries/javalib/org/tritonus/lowlevel/esd/EsdRecordingStream.java,
        libraries/javalib/org/tritonus/lowlevel/esd/EsdSample.java,
        libraries/javalib/org/tritonus/lowlevel/esd/EsdStream.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaMidiChannel.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaMidiDevice.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaMidiDeviceProvider.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaMidiIn.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaMidiOut.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaQueueHolder.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaReceiver.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaSequencer.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaSequencerProvider.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaSequencerReceiver.java,
        libraries/javalib/org/tritonus/midi/device/alsa/AlsaSynthesizer.java,
        libraries/javalib/org/tritonus/midi/file/MidiConstants.java,
        libraries/javalib/org/tritonus/midi/file/StandardMidiFileReader.java,
        libraries/javalib/org/tritonus/midi/file/StandardMidiFileWriter.java,
        libraries/javalib/org/tritonus/sampled/convert/AlawFormatConversionProvider.java,
        libraries/javalib/org/tritonus/sampled/convert/PCM2PCMConversionProvider.java,
        libraries/javalib/org/tritonus/sampled/convert/SampleRateConversionProvider.java,
        libraries/javalib/org/tritonus/sampled/convert/SmartFormatConversionProvider.java,
        libraries/javalib/org/tritonus/sampled/convert/UlawFormatConversionProvider.java,
        libraries/javalib/org/tritonus/sampled/file/AiffAudioFileReader.java,
        libraries/javalib/org/tritonus/sampled/file/AiffAudioFileWriter.java,
        libraries/javalib/org/tritonus/sampled/file/AiffAudioOutputStream.java,
        libraries/javalib/org/tritonus/sampled/file/AiffTool.java,
        libraries/javalib/org/tritonus/sampled/file/AuAudioFileReader.java,
        libraries/javalib/org/tritonus/sampled/file/AuAudioFileWriter.java,
        libraries/javalib/org/tritonus/sampled/file/AuAudioOutputStream.java,
        libraries/javalib/org/tritonus/sampled/file/AuTool.java,
        libraries/javalib/org/tritonus/sampled/file/WaveAudioFileReader.java,
        libraries/javalib/org/tritonus/sampled/file/WaveAudioFileWriter.java,
        libraries/javalib/org/tritonus/sampled/file/WaveAudioOutputStream.java,
        libraries/javalib/org/tritonus/sampled/file/WaveTool.java,
        libraries/javalib/org/tritonus/sampled/mixer/alsa/AlsaBaseDataLine.java,
        libraries/javalib/org/tritonus/sampled/mixer/alsa/AlsaDataLineMixer.java,
        libraries/javalib/org/tritonus/sampled/mixer/alsa/AlsaDataLineMixerProvider.java,
        libraries/javalib/org/tritonus/sampled/mixer/alsa/AlsaPortMixer.java,
        libraries/javalib/org/tritonus/sampled/mixer/alsa/AlsaPortMixerProvider.java,
        libraries/javalib/org/tritonus/sampled/mixer/alsa/AlsaSourceDataLine.java,
        libraries/javalib/org/tritonus/sampled/mixer/alsa/AlsaTargetDataLine.java,
        libraries/javalib/org/tritonus/sampled/mixer/alsa/AlsaUtils.java,
        libraries/javalib/org/tritonus/sampled/mixer/esd/EsdClip.java,
        libraries/javalib/org/tritonus/sampled/mixer/esd/EsdMixer.java,
        libraries/javalib/org/tritonus/sampled/mixer/esd/EsdMixerProvider.java,
        libraries/javalib/org/tritonus/sampled/mixer/esd/EsdSourceDataLine.java,
        libraries/javalib/org/tritonus/sampled/mixer/esd/EsdTargetDataLine.java,
        libraries/javalib/org/tritonus/sampled/mixer/esd/EsdUtils.java,
        libraries/javalib/org/tritonus/share/ArraySet.java,
        libraries/javalib/org/tritonus/share/GlobalInfo.java,
        libraries/javalib/org/tritonus/share/StringHashedSet.java,
        libraries/javalib/org/tritonus/share/TCircularBuffer.java,
        libraries/javalib/org/tritonus/share/TDebug.java,
        libraries/javalib/org/tritonus/share/TNotifier.java,
        libraries/javalib/org/tritonus/share/TSettings.java,
        libraries/javalib/org/tritonus/share/midi/MidiUtils.java,
        libraries/javalib/org/tritonus/share/midi/TMidiDevice.java,
        libraries/javalib/org/tritonus/share/midi/TMidiFileFormat.java,
        libraries/javalib/org/tritonus/share/midi/TSequencer.java,
        libraries/javalib/org/tritonus/share/sampled/AudioFileTypes.java,
        libraries/javalib/org/tritonus/share/sampled/AudioFormatSet.java,
        libraries/javalib/org/tritonus/share/sampled/AudioFormats.java,
        libraries/javalib/org/tritonus/share/sampled/AudioSystemShadow.java,
        libraries/javalib/org/tritonus/share/sampled/AudioUtils.java,
        libraries/javalib/org/tritonus/share/sampled/Encodings.java,
        libraries/javalib/org/tritonus/share/sampled/FloatSampleBuffer.java,
        libraries/javalib/org/tritonus/share/sampled/TConversionTool.java,
        libraries/javalib/org/tritonus/share/sampled/TVolumeUtils.java,
        libraries/javalib/org/tritonus/share/sampled/convert/TAsynchronousFilteredAudioInputStream.java,
        libraries/javalib/org/tritonus/share/sampled/convert/TEncodingFormatConversionProvider.java,
        libraries/javalib/org/tritonus/share/sampled/convert/TFormatConversionProvider.java,
        libraries/javalib/org/tritonus/share/sampled/convert/TMatrixFormatConversionProvider.java,
        libraries/javalib/org/tritonus/share/sampled/convert/TSimpleFormatConversionProvider.java,
        libraries/javalib/org/tritonus/share/sampled/convert/TSynchronousFilteredAudioInputStream.java,
        libraries/javalib/org/tritonus/share/sampled/file/AudioOutputStream.java,
        libraries/javalib/org/tritonus/share/sampled/file/HeaderlessAudioOutputStream.java,
        libraries/javalib/org/tritonus/share/sampled/file/TAudioFileFormat.java,
        libraries/javalib/org/tritonus/share/sampled/file/TAudioFileReader.java,
        libraries/javalib/org/tritonus/share/sampled/file/TAudioFileWriter.java,
        libraries/javalib/org/tritonus/share/sampled/file/TAudioOutputStream.java,
        libraries/javalib/org/tritonus/share/sampled/file/TDataOutputStream.java,
        libraries/javalib/org/tritonus/share/sampled/file/THeaderlessAudioFileWriter.java,
        libraries/javalib/org/tritonus/share/sampled/file/TNonSeekableDataOutputStream.java,
        libraries/javalib/org/tritonus/share/sampled/file/TSeekableDataOutputStream.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TBaseDataLine.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TBooleanControl.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TClip.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TCompoundControl.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TCompoundControlType.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TControlController.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TControllable.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TDataLine.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TEnumControl.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TFloatControl.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TLine.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TMixer.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TMixerInfo.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TMixerProvider.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TPort.java,
        libraries/javalib/org/tritonus/share/sampled/mixer/TSoftClip.java:
        New files, merged in from tritonus.org.

2002-12-29  Helmer Kraemer  <hkraemer@freenet.de>

        * kaffe/kaffevm/jni.c (getMethodFunc): new Method.
        [JNI_METHOD_CODE]: removed.
        (Kaffe_CallObjectMethod*), (Kaffe_CallBooleanMethod*),
        (Kaffe_CallByteMethod*), (Kaffe_CallCharMethod*),
        (Kaffe_CallShortMethod*), (Kaffe_CallIntMethod*),
        (Kaffe_CallLongMethod*), (Kaffe_CallFloatMethod*),
        (Kaffe_CallDoubleMethod*), (Kaffe_CallVoidMethod*):
        properly handle interface methods.
        Replaced JNI_METHOD_CODE with METHOD_INDIRECTMETHOD.

        * kaffe/kaffevm/support.c (callMethodV) [TRANSLATOR]:
        removed bogus assertion.

        * test/regression/ReflectInvoke.java: added tests for
        invocation of interface and superclass methods.

        Reported by: Benja Fallenstein <b.fallenstein@gmx.de>

2002-12-29  Helmer Kraemer  <hkraemer@freenet.de>

       * kaffe/kaffevm/readClass.c (readMethods) [KAFFEH]: call
       finishMethods after reading all methods from a class file

       * kaffe/kaffeh/support.c (fprintfJni): new method
       (addMethod): don't generate c declarations for native methods
       but collect them in a list
       (finishMethods): new method that traverses the list of native
       methods and generates appropriate c declarations

Revision 1.8  2002/08/04 06:57:00  pfisterer
Introduced card info object instead of single method with strange parameters; removed obsolete methods; cleanup

Revision 1.7  2001/07/02 18:41:44  pfisterer
reorganized native library loading and tracing

Revision 1.6  2001/05/30 09:25:03  pfisterer
intermediate development state, mainly related to ALSA

Revision 1.5  2001/05/11 08:53:45  pfisterer
rcs keywords test


 */

/*
 *  Copyright (c) 2000 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@web.de>
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

import	org.tritonus.share.TDebug;



/**	TODO:
 */
public class AlsaCtl
{
	/** contains a pointer to snd_ctl_t
	 */
	private long	m_lNativeHandle;



	static
	{
		Alsa.loadNativeLibrary();
		if (TDebug.TraceAlsaCtlNative)
		{
			setTrace(true);
		}
	}



	public static native int loadCard(int nCard);
	// this encapsulates snd_card_next()
	public static native int[] getCards();
	public static native int getCardIndex(String strName);
	public static native String getCardName(int nCard);
	public static native String getCardLongName(int nCard);



	/**	Open a ctl.

		Objects created with this constructor have to be
		closed by calling {@link #close() close()}. This is
		necessary to free native resources.

		@param strName The name of the sound card. For
		instance, "hw:0", or an identifier you gave the
		card ("CARD1").

		@param nMode Special modes for the low-level opening
		like SND_CTL_NONBLOCK, SND_CTL_ASYNC. Normally, set
		this to 0.

	*/
	public AlsaCtl(String strName, int nMode)
		throws	Exception
	{
		if (open(strName, nMode) < 0)
		{
			throw new Exception();
		}
	}



	public AlsaCtl(int nCard)
		throws	Exception
	{
		this("hw:" + nCard, 0);
	}


	/**	Calls snd_ctl_open().
	 */
	private native int open(String strName, int nMode);

	/**	Calls snd_ctl_close().
	 */
	public native int close();

	/**	Calls snd_ctl_card_info().
	 */
	public native int getCardInfo(AlsaCtlCardInfo cardInfo);


	// TODO: ??
	public native int[] getPcmDevices();

	// TODO: remove
	/**
	 *	anValues[0]	device (inout)
	 *	anValues[1]	subdevice (inout)
	 *	anValues[2]	stream (inout)
	 *	anValues[3]	card (out)
	 *	anValues[4]	class (out)
	 *	anValues[5]	subclass (out)
	 *	anValues[6]	subdevice count (out)
	 *	anValues[7]	subdevice available (out)
	 *
	 *	astrValues[0]	id (out)
	 *	astrValues[1]	name (out)
	 *	astrValues[2]	subdevice name (out)
	 */
	public native int getPcmInfo(int[] anValues, String[] astrValues);


	private static native void setTrace(boolean bTrace);
}



/*** AlsaCtl.java ***/
