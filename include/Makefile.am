# Library support
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
# Copyright (c) 2003
#	Mark Wielaard <mark@klomp.org>
# Copyright (c) 2004
#	Kaffe.org contributors. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

KLASSES_JAR = $(top_builddir)/libraries/javalib/external/classpath/lib/glibj.zip


EXTRA_DIST = distclean-hack \
		stamp-h2.in

# INSTALL_ means: these headers get installed,
# NOINSTALL_ means: they don't get installed.
# _DERIVED_ means: these headers are derived (using
# kaffeh, from glibj.zip). 
# _JNI_DERIVED_ means: these headers are derived using kaffeh
# from glibj.zip *and* the -jni option is
# set, resulting in JNI-ish method names.
#
# So if your class uses JNI to call its native methods
# (like tritonus), then it goes into the JNI set,
# otherwise it goes into the other one.

INSTALL_DERIVED_HDRS = 


NOINSTALL_DERIVED_HDRS = \
	java_lang_Object.h \
	java_lang_VMObject.h \
	java_lang_VMClass.h \
	java_lang_VMClassLoader.h \
	java_lang_VMRuntime.h \
	java_lang_String.h \
	java_lang_StackTraceElement.h \
	java_lang_Thread.h \
	java_lang_VMThread.h \
	java_lang_ThreadGroup.h \
	java_lang_Throwable.h \
	java_lang_VMThrowable.h \
	java_io_InterruptedIOException.h \
	java_io_VMFile.h \
	java_lang_ClassLoader.h \
	java_lang_Cloneable.h \
	java_lang_Integer.h \
	java_lang_Math.h \
	java_lang_reflect_Array.h \
	java_lang_reflect_Constructor.h \
	java_lang_reflect_Field.h \
	java_lang_reflect_Method.h \
	java_net_DatagramPacket.h \
	java_net_InetAddress.h \
	java_net_VMInetAddress.h \
	java_net_InetSocketAddress.h \
	java_net_NetworkInterface.h \
	java_net_VMNetworkInterface.h \
	gnu_java_net_PlainDatagramSocketImpl.h \
	gnu_java_net_PlainSocketImpl.h \
	gnu_java_net_SysInetAddressImpl.h \
	java_net_SocketAddress.h \
	java_net_SocketImpl.h \
	java_net_SocketOptions.h \
	java_security_VMAccessController.h \
	java_util_Comparator.h \
	java_util_VMTimeZone.h \
	java_util_Vector.h \
	java_util_zip_Adler32.h \
	java_util_zip_CRC32.h \
	java_util_zip_Deflater.h \
	java_util_zip_Inflater.h \
	java_util_zip_ZipEntry.h \
	java_util_zip_ZipFile.h \
	org_kaffe_management_Classpath.h \
	gnu_classpath_VMStackWalker.h

DERIVED_HDRS = $(INSTALL_DERIVED_HDRS) $(NOINSTALL_DERIVED_HDRS)

INSTALL_JNI_DERIVED_HDRS =

LIBTRITONUSALSA_JNI_HDRS = \
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


LIBTRITONUSESD_JNI_HDRS = \
	org_tritonus_lowlevel_esd_Esd.h \
	org_tritonus_lowlevel_esd_EsdRecordingStream.h \
	org_tritonus_lowlevel_esd_EsdSample.h \
	org_tritonus_lowlevel_esd_EsdStream.h


# Only create headers for sound if they are going to be built.

if COND_NATIVE_SOUND
if COND_SOUND_ALSA
  MAYBE_LIBTRITONUSALSA_JNI_HDRS = $(LIBTRITONUSALSA_JNI_HDRS)
endif
if COND_SOUND_ESD
  MAYBE_LIBTRITONUSESD_JNI_HDRS = $(LIBTRITONUSESD_JNI_HDRS)
endif
endif

NOINSTALL_JNI_DERIVED_HDRS = \
	java_math_BigInteger.h \
	java_lang_VMProcess.h \
	java_lang_ref_Reference.h \
	gnu_classpath_VMSystemProperties.h \
	gnu_java_nio_SelectorImpl.h \
	java_lang_VMSystem.h \
	java_nio_VMDirectByteBuffer.h \
	java_nio_MappedByteBufferImpl.h \
	org_kaffe_lang_UNIXProcess.h \
	org_kaffe_management_JIT.h \
	org_kaffe_management_Debug.h \
	org_kaffe_management_XProfiler.h \
	org_kaffe_security_provider_MD2.h \
	org_kaffe_security_provider_MD4.h \
	org_kaffe_security_provider_MD5.h \
	org_kaffe_security_provider_SHA.h \
	$(MAYBE_LIBTRITONUSALSA_JNI_HDRS) \
	$(MAYBE_LIBTRITONUSESD_JNI_HDRS) 

JNI_DERIVED_HDRS = \
	$(INSTALL_JNI_DERIVED_HDRS) \
	$(NOINSTALL_JNI_DERIVED_HDRS)

if USE_COMPAT_IFADDRS
ifaddrs_compat = ifaddrs.h

# Always link, ignore failures.
ifaddrs.h: ifaddrs_compat.h
	$(LN_S) $(top_srcdir)/include/ifaddrs_compat.h ifaddrs.h || true
else
ifaddrs_compat = #
endif

nodist_noinst_HEADERS = \
	$(ifaddrs_compat) \
	$(NOINSTALL_DERIVED_HDRS) \
	$(NOINSTALL_JNI_DERIVED_HDRS)

noinst_HEADERS = \
	ifaddrs_compat.h \
	defs.h \
	dummyin6.h \
	files.h \
	gccbuiltin.h \
	getaddrinfo.h \
	gettext.h \
	nets.h \
	system.h \
	Arrays.h \
	errors.h \
	jsyscall.h \
	native.h 

include_HEADERS = \
	config-int.h \
	jni.h \
	jni_md.h \
	jni_cpp.h \
	jvmpi.h \
	kaffe_jni.h \
	$(JAWT_HDRS)

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
	kaffe/trampoline.h

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


.PHONY: rebuild-jni-hdrs
rebuild-jni-hdrs: stamp-kaffeh $(KLASSES_JAR)
## Then, generate each header file,
## but if it does not change, do not touch it
	@set -e; for f in $(HDRS); do \
	  class=`echo $$f | sed -e 's%.*/%%g' -e 's%\.h$$%%' -e 's%_%/%g'`; \
	  echo "$(KAFFEH) -jni -classpath $(top_builddir)/libraries/javalib/gmp-math/lib:$(KLASSES_JAR):$(top_builddir)/libraries/javalib/lib:$(top_builddir)/libraries/javalib/external/classpath/lib/glibj.zip -o $$f $$class"; \
	  $(KAFFEH) -jni -classpath $(top_builddir)/libraries/javalib/gmp-math/lib:$(KLASSES_JAR):$(top_builddir)/libraries/javalib/lib:$(top_builddir)/libraries/javalib/external/classpath/lib/glibj.zip -o stamp-h1$$f $$class; \
	  if test -f $$f && cmp $$f stamp-h1$$f >/dev/null; then \
	    echo "$$f is already up-to-date"; \
	    rm -f stamp-h1$$f; \
	  else \
	    rm -f $$f; mv stamp-h1$$f $$f; \
	  fi; \
	done


#
# jni headers of the core libraries
#
$(JNI_DERIVED_HDRS): stamp-h1all
	@if test -f $@; then :; else \
	  rm -f stamp-h1all; $(MAKE) JNI_DERIVED_HDRS=$@ stamp-h1all; \
	fi

stamp-h1all: HDRS = $(JNI_DERIVED_HDRS)
stamp-h1all: rebuild-jni-hdrs
	@echo timestamp > $@

CLEANFILES = \
	$(DERIVED_HDRS) \
	$(JNI_DERIVED_HDRS) \
	stamp-kaffeh

DISTCLEANFILES = \
	$(DERIVED_HDRS) \
	$(JNI_DERIVED_HDRS) \
	config-int.h \
	stamp-h0all \
	stamp-h1all \
	stamp-libtritonusalsa \
	stamp-libtritonusesd \
	stamp-libgtkpeer

dist-hook:
	@for file in $(DISTCLEANFILES); do \
	  rm -f $(distdir)/$$file; \
	done
