# Library support
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
# Copyright (c) 2003
#	Mark Wielaard <mark@klomp.org>
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

KLASSES_JAR = $(top_srcdir)/libraries/javalib/Klasses.jar.bootstrap

EXTRA_DIST = distclean-hack \
		stamp-h2.in

# INSTALL_ means: these headers get installed,
# NOINSTALL_ means: they don't get installed.
# _DERIVED_ means: these headers are derived (using
# kaffeh, from Klasses.jar.bootstrap). 
# _JNI_DERIVED_ means: these headers are derived using kaffeh
# from Klasses.jar.bootstrap *and* the -jni option is
# set, resulting in JNI-ish method names.
#
# So if your class uses JNI to call its native methods
# (like tritonus), then it goes into the JNI set,
# otherwise it goes into the other one.
#
# In libraries/javalib, Klasses.jar.bootstrap contains
# beside classes necesary for kjc to run, also all the
# classes necessary to generate headers from. They are
# at the end of bootstrap.classlist.

INSTALL_DERIVED_HDRS = 


NOINSTALL_DERIVED_HDRS = \
	java_lang_Object.h \
	java_lang_String.h \
	java_lang_StackTraceElement.h \
	java_lang_Thread.h \
	java_lang_ThreadGroup.h \
	java_lang_Throwable.h \
	java_lang_VMThrowable.h \
	java_lang_SecurityManager.h \
	java_io_File.h \
	java_io_FileDescriptor.h \
	java_io_FileInputStream.h \
	java_io_FileOutputStream.h \
	java_io_InputStream.h \
	java_io_InterruptedIOException.h \
	java_io_ObjectInputStream.h \
	java_io_PrintStream.h \
	java_io_RandomAccessFile.h \
	java_lang_Boolean.h \
	java_lang_Byte.h \
	java_lang_Character.h \
	java_lang_Class.h \
	java_lang_ClassLoader.h \
	java_lang_Cloneable.h \
	java_lang_Compiler.h \
	java_lang_Double.h \
	java_lang_Float.h \
	java_lang_Integer.h \
	java_lang_Long.h \
	java_lang_Math.h \
	java_lang_NativeLibrary.h \
	java_lang_Runtime.h \
	java_lang_Short.h \
	java_lang_System.h \
	java_lang_reflect_Array.h \
	java_lang_reflect_Constructor.h \
	java_lang_reflect_Field.h \
	java_lang_reflect_Method.h \
	java_net_DatagramPacket.h \
	java_net_InetAddress.h \
	java_net_InetSocketAddress.h \
	java_net_NetworkInterface.h \
	gnu_java_net_PlainDatagramSocketImpl.h \
	gnu_java_net_PlainSocketImpl.h \
	java_net_SocketAddress.h \
	java_net_SocketImpl.h \
	java_net_SocketOptions.h \
	java_text_DateFormat.h \
	java_util_Arrays.h \
	java_util_Comparator.h \
	java_util_TimeZone.h \
	java_util_Vector.h \
	java_util_zip_Adler32.h \
	java_util_zip_CRC32.h \
	java_util_zip_Deflater.h \
	java_util_zip_Inflater.h \
	java_util_zip_ZipEntry.h \
	java_util_zip_ZipFile.h \
	kaffe_applet_AudioPlayer.h \
	kaffe_lang_PrimordialClassLoader.h \
	kaffe_lang_ThreadStack.h \
	kaffe_management_Classpath.h

DERIVED_HDRS = $(INSTALL_DERIVED_HDRS) $(NOINSTALL_DERIVED_HDRS)

INSTALL_JNI_DERIVED_HDRS =

NOINSTALL_JNI_DERIVED_HDRS = \
	java_math_BigInteger.h \
	kaffe_io_ByteToCharDefault.h \
	kaffe_io_ByteToCharIconv.h \
	kaffe_io_CharToByteDefault.h \
	kaffe_io_CharToByteIconv.h \
	kaffe_lang_MemoryAdvice.h \
	kaffe_lang_UNIXProcess.h \
	kaffe_management_JIT.h \
	kaffe_management_Debug.h \
	kaffe_management_XProfiler.h \
	kaffe_security_provider_MD2.h \
	kaffe_security_provider_MD4.h \
	kaffe_security_provider_MD5.h \
	kaffe_security_provider_SHA.h \
	org_tritonus_lowlevel_esd_Esd.h \
	org_tritonus_lowlevel_esd_EsdRecordingStream.h \
	org_tritonus_lowlevel_esd_EsdSample.h \
	org_tritonus_lowlevel_esd_EsdStream.h \
	org_tritonus_lowlevel_alsa_AlsaCtlCardInfo.h \
	org_tritonus_lowlevel_alsa_AlsaCtl.h \
	org_tritonus_lowlevel_alsa_Alsa.h \
	org_tritonus_lowlevel_alsa_AlsaMixerElement.h \
	org_tritonus_lowlevel_alsa_AlsaMixer.h \
	org_tritonus_lowlevel_alsa_AlsaPcmHWParamsFormatMask.h \
	org_tritonus_lowlevel_alsa_AlsaPcmHWParams.h \
	org_tritonus_lowlevel_alsa_AlsaPcm.h \
	org_tritonus_lowlevel_alsa_AlsaPcmSWParams.h \
	org_tritonus_lowlevel_alsa_AlsaSeqClientInfo.h \
	org_tritonus_lowlevel_alsa_AlsaSeqEvent.h \
	org_tritonus_lowlevel_alsa_AlsaSeq.h \
	org_tritonus_lowlevel_alsa_AlsaSeqPortInfo.h \
	org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe.h \
	org_tritonus_lowlevel_alsa_AlsaSeqQueueInfo.h \
	org_tritonus_lowlevel_alsa_AlsaSeqQueueStatus.h \
	org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo.h \
	org_tritonus_lowlevel_alsa_AlsaSeqQueueTimer.h \
	org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents.h \
	org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo.h

JNI_DERIVED_HDRS = $(INSTALL_JNI_DERIVED_HDRS) $(NOINSTALL_JNI_DERIVED_HDRS)

if USE_COMPAT_IFADDRS
ifaddrs_compat = ifaddrs.h
ifaddrs.h: ifaddrs_compat.h
	$(LN_S) $< $@
else
ifaddrs_compat = #
endif

nodist_noinst_HEADERS = \
	$(NOINSTALL_DERIVED_HDRS) \
	$(NOINSTALL_JNI_DERIVED_HDRS)

noinst_HEADERS = \
	ifaddrs_compat.h \
	$(ifaddrs_compat) \
	defs.h \
	dummyin6.h \
	files.h \
	getaddrinfo.h \
	nets.h \
	system.h \
	Arrays.h \
	errors.h \
	jsyscall.h \
	native.h 

include_HEADERS = \
	jni.h \
	jni_cpp.h \
	jvmpi.h 


nodist_pkginclude_HEADERS = \
	$(INSTALL_DERIVED_HDRS) \
	$(INSTALL_JNI_DERIVED_HDRS)

pkginclude_HEADERS = \
	java_lang_Object.h \
	java_lang_String.h \
	java_lang_StackTraceElement.h \
	java_lang_Thread.h \
	java_lang_ThreadGroup.h \
	java_lang_Throwable.h \
	java_lang_VMThrowable.h \
	kaffe/jmalloc.h \
	kaffe/jtypes.h 

stamp-kaffeh:
## If KAFFEH does not exist, build it
	@test -x $(KAFFEH) || ($(MAKE_KAFFEH))
	@echo timestamp > $@

$(DERIVED_HDRS): stamp-h0all
	@if test -f $@; then :; else \
## If this file is removed after stamp-h0all is built, force its rebuild.
	  rm -f stamp-h0all; $(MAKE) DERIVED_HDRS=$@ stamp-h0all; \
	fi

stamp-h0all: stamp-kaffeh $(KLASSES_JAR)
## Then, generate each header file,
## but if it does not change, do not touch it
	@set -e; for f in $(DERIVED_HDRS); do \
	  class=`echo $$f | sed -e 's%.*/%%g' -e 's%\.h$$%%' -e 's%_%/%g'`; \
	  echo "$(KAFFEH) -classpath $(KLASSES_JAR) -o $$f $$class"; \
	  $(KAFFEH) -classpath $(KLASSES_JAR) -o stamp-h0$$f $$class; \
	  if test -f $$f && cmp $$f stamp-h0$$f >/dev/null; then \
	    echo "$$f is already up-to-date"; \
	    rm -f stamp-h0$$f; \
	  else \
	    rm -f $$f; mv stamp-h0$$f $$f; \
	  fi; \
	done
	@echo timestamp > $@

$(JNI_DERIVED_HDRS): stamp-h1all
	@if test -f $@; then :; else \
## If this file is removed after stamp-h1all is built, force its rebuild.
	  rm -f stamp-h1all; $(MAKE) JNI_DERIVED_HDRS=$@ stamp-h1all; \
	fi

stamp-h1all: stamp-kaffeh $(KLASSES_JAR)
## Then, generate each header file,
## but if it does not change, do not touch it
	@set -e; for f in $(JNI_DERIVED_HDRS); do \
	  class=`echo $$f | sed -e 's%.*/%%g' -e 's%\.h$$%%' -e 's%_%/%g'`; \
	  echo "$(KAFFEH) -jni -classpath $(KLASSES_JAR) -o $$f $$class"; \
	  $(KAFFEH) -jni -classpath $(KLASSES_JAR) -o stamp-h1$$f $$class; \
	  if test -f $$f && cmp $$f stamp-h1$$f >/dev/null; then \
	    echo "$$f is already up-to-date"; \
	    rm -f stamp-h1$$f; \
	  else \
	    rm -f $$f; mv stamp-h1$$f $$f; \
	  fi; \
	done
	@echo timestamp > $@

CLEANFILES = $(DERIVED_HDRS) $(JNI_DERIVED_HDRS) stamp-kaffeh

DISTCLEANFILES = \
	$(DERIVED_HDRS) \
	$(JNI_DERIVED_HDRS) \
	kaffe/jtypes.h \
	stamp-h0all \
	stamp-h1all

dist-hook:
	@for file in $(DISTCLEANFILES); do \
	  rm -f $(distdir)/$$file; \
	done
