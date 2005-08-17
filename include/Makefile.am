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

KLASSES_JAR = $(top_builddir)/libraries/javalib/rt.jar

EXTRA_DIST = distclean-hack \
		stamp-h2.in

# INSTALL_ means: these headers get installed,
# NOINSTALL_ means: they don't get installed.
# _DERIVED_ means: these headers are derived (using
# kaffeh, from rt.jar). 
# _JNI_DERIVED_ means: these headers are derived using kaffeh
# from rt.jar *and* the -jni option is
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
	java_io_File.h \
	java_io_FileInputStream.h \
	java_io_FileOutputStream.h \
	java_io_InputStream.h \
	java_io_InterruptedIOException.h \
	java_io_PrintStream.h \
	java_io_RandomAccessFile.h \
	java_io_VMFile.h \
	java_lang_Boolean.h \
	java_lang_Byte.h \
	java_lang_Character.h \
	java_lang_ClassLoader.h \
	java_lang_Cloneable.h \
	java_lang_Integer.h \
	java_lang_Long.h \
	java_lang_Math.h \
	java_lang_Short.h \
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
	kaffe_management_Classpath.h \
	gnu_java_nio_VMSelector.h \
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


LIBXMLJ_JNI_HDRS = \
	gnu_xml_libxmlj_dom_GnomeAttr.h \
	gnu_xml_libxmlj_dom_GnomeDocument.h \
	gnu_xml_libxmlj_dom_GnomeDocumentBuilder.h \
	gnu_xml_libxmlj_dom_GnomeDocumentType.h	\
	gnu_xml_libxmlj_dom_GnomeElement.h \
	gnu_xml_libxmlj_dom_GnomeEntity.h \
	gnu_xml_libxmlj_dom_GnomeNamedNodeMap.h \
	gnu_xml_libxmlj_dom_GnomeNode.h \
	gnu_xml_libxmlj_dom_GnomeNodeList.h \
	gnu_xml_libxmlj_dom_GnomeNotation.h \
	gnu_xml_libxmlj_dom_GnomeProcessingInstruction.h \
	gnu_xml_libxmlj_dom_GnomeTypeInfo.h \
	gnu_xml_libxmlj_dom_GnomeXPathExpression.h \
	gnu_xml_libxmlj_dom_GnomeXPathNodeList.h \
	gnu_xml_libxmlj_dom_GnomeXPathNSResolver.h \
	gnu_xml_libxmlj_dom_GnomeXPathResult.h \
	gnu_xml_libxmlj_sax_GnomeLocator.h \
	gnu_xml_libxmlj_sax_GnomeXMLReader.h \
	gnu_xml_libxmlj_transform_GnomeTransformer.h \
	gnu_xml_libxmlj_transform_GnomeTransformerFactory.h

LIBGTKPEER_JNI_HDRS = \
	gnu_java_awt_peer_gtk_GdkFontPeer.h \
	gnu_java_awt_peer_gtk_GdkGraphics.h \
	gnu_java_awt_peer_gtk_GdkGraphics2D.h \
	gnu_java_awt_peer_gtk_GdkGraphicsEnvironment.h \
	gnu_java_awt_peer_gtk_GdkPixbufDecoder.h \
	gnu_java_awt_peer_gtk_GdkRobotPeer.h \
	gnu_java_awt_peer_gtk_GdkTextLayout.h \
	gnu_java_awt_peer_gtk_GtkButtonPeer.h \
	gnu_java_awt_peer_gtk_GtkCanvasPeer.h \
	gnu_java_awt_peer_gtk_GtkCheckboxGroupPeer.h \
	gnu_java_awt_peer_gtk_GtkCheckboxMenuItemPeer.h \
	gnu_java_awt_peer_gtk_GtkCheckboxPeer.h \
	gnu_java_awt_peer_gtk_GtkChoicePeer.h \
	gnu_java_awt_peer_gtk_GtkClipboard.h \
	gnu_java_awt_peer_gtk_GtkComponentPeer.h \
	gnu_java_awt_peer_gtk_GtkEmbeddedWindowPeer.h \
	gnu_java_awt_peer_gtk_GtkFileDialogPeer.h \
	gnu_java_awt_peer_gtk_GtkFramePeer.h \
	gnu_java_awt_peer_gtk_GtkGenericPeer.h \
	gnu_java_awt_peer_gtk_GtkImage.h \
	gnu_java_awt_peer_gtk_GtkLabelPeer.h \
	gnu_java_awt_peer_gtk_GtkListPeer.h \
	gnu_java_awt_peer_gtk_GtkMenuBarPeer.h \
	gnu_java_awt_peer_gtk_GtkMenuComponentPeer.h \
	gnu_java_awt_peer_gtk_GtkMenuItemPeer.h \
	gnu_java_awt_peer_gtk_GtkMenuPeer.h \
	gnu_java_awt_peer_gtk_GtkPanelPeer.h \
	gnu_java_awt_peer_gtk_GtkPopupMenuPeer.h \
	gnu_java_awt_peer_gtk_GtkScrollbarPeer.h \
	gnu_java_awt_peer_gtk_GtkScrollPanePeer.h \
	gnu_java_awt_peer_gtk_GtkSelection.h \
	gnu_java_awt_peer_gtk_GtkTextAreaPeer.h \
	gnu_java_awt_peer_gtk_GtkTextFieldPeer.h \
	gnu_java_awt_peer_gtk_GtkToolkit.h \
	gnu_java_awt_peer_gtk_GtkWindowPeer.h \
	gnu_java_awt_peer_gtk_GThreadNativeMethodRunner.h

LIBQTPEER_JNI_HDRS = \
        gnu_java_awt_peer_qt_MainQtThread.h \
        gnu_java_awt_peer_qt_NativeWrapper.h \
        gnu_java_awt_peer_qt_QMatrix.h \
        gnu_java_awt_peer_qt_QPainterPath.h \
        gnu_java_awt_peer_qt_QPen.h \
        gnu_java_awt_peer_qt_QtAudioClip.h \
        gnu_java_awt_peer_qt_QtButtonPeer.h \
        gnu_java_awt_peer_qt_QtCanvasPeer.h \
        gnu_java_awt_peer_qt_QtCheckboxPeer.h \
        gnu_java_awt_peer_qt_QtChoicePeer.h \
        gnu_java_awt_peer_qt_QtComponentGraphics.h \
        gnu_java_awt_peer_qt_QtComponentPeer.h \
        gnu_java_awt_peer_qt_QtContainerPeer.h \
        gnu_java_awt_peer_qt_QtDialogPeer.h \
	gnu_java_awt_peer_qt_QtEmbeddedWindowPeer.h \
        gnu_java_awt_peer_qt_QtFileDialogPeer.h \
        gnu_java_awt_peer_qt_QtFontMetrics.h \
        gnu_java_awt_peer_qt_QtFontPeer.h \
        gnu_java_awt_peer_qt_QtFramePeer.h \
        gnu_java_awt_peer_qt_QtGraphics.h \
        gnu_java_awt_peer_qt_QtGraphicsEnvironment.h \
        gnu_java_awt_peer_qt_QtImage.h \
        gnu_java_awt_peer_qt_QtImageConsumer.h \
        gnu_java_awt_peer_qt_QtImageGraphics.h \
        gnu_java_awt_peer_qt_QtLabelPeer.h \
        gnu_java_awt_peer_qt_QtListPeer.h \
        gnu_java_awt_peer_qt_QtMenuBarPeer.h \
        gnu_java_awt_peer_qt_QtMenuComponentPeer.h \
        gnu_java_awt_peer_qt_QtMenuItemPeer.h \
        gnu_java_awt_peer_qt_QtMenuPeer.h \
        gnu_java_awt_peer_qt_QtPanelPeer.h \
        gnu_java_awt_peer_qt_QtPopupMenuPeer.h \
        gnu_java_awt_peer_qt_QtScreenDevice.h \
        gnu_java_awt_peer_qt_QtScreenDeviceConfiguration.h \
        gnu_java_awt_peer_qt_QtScrollPanePeer.h \
        gnu_java_awt_peer_qt_QtScrollbarPeer.h \
        gnu_java_awt_peer_qt_QtTextAreaPeer.h \
        gnu_java_awt_peer_qt_QtTextFieldPeer.h \
        gnu_java_awt_peer_qt_QtToolkit.h \
        gnu_java_awt_peer_qt_QtVolatileImage.h \
        gnu_java_awt_peer_qt_QtWindowPeer.h 

# Only create headers for AWT, Sound and libxmlj if they are going to be built.

if COND_LIBXMLJ
  MAYBE_LIBXMLJ_JNI_HDRS = $(LIBXMLJ_JNI_HDRS)
endif

if COND_NATIVE_SOUND
if COND_SOUND_ALSA
  MAYBE_LIBTRITONUSALSA_JNI_HDRS = $(LIBTRITONUSALSA_JNI_HDRS)
endif
if COND_SOUND_ESD
  MAYBE_LIBTRITONUSESD_JNI_HDRS = $(LIBTRITONUSESD_JNI_HDRS)
endif
endif

if COND_NATIVE_AWT
if COND_CLASSPATH_GTK_AWT
  MAYBE_LIBGTKPEER_JNI_HDRS = $(LIBGTKPEER_JNI_HDRS)
endif

if COND_CLASSPATH_QT_AWT
  MAYBE_LIBQTPEER_JNI_HDRS = $(LIBQTPEER_JNI_HDRS)
endif
endif

if COND_CLASSPATH_GTK_AWT
JAWT_HDRS = \
	jawt.h \
	jawt_md.h
endif

NOINSTALL_JNI_DERIVED_HDRS = \
	java_math_BigInteger.h \
	java_lang_VMDouble.h \
	java_lang_VMProcess.h \
	java_lang_VMFloat.h \
	java_lang_ref_Reference.h \
	gnu_classpath_VMSystemProperties.h \
	gnu_java_nio_charset_iconv_IconvDecoder.h \
	gnu_java_nio_charset_iconv_IconvEncoder.h \
	gnu_java_nio_channels_FileChannelImpl.h \
	gnu_java_nio_SelectorImpl.h \
	java_lang_VMSystem.h \
	java_io_VMObjectInputStream.h \
	java_io_VMObjectStreamClass.h \
	java_nio_VMDirectByteBuffer.h \
	java_nio_MappedByteBufferImpl.h \
	kaffe_lang_UNIXProcess.h \
	kaffe_management_JIT.h \
	kaffe_management_Debug.h \
	kaffe_management_XProfiler.h \
	kaffe_security_provider_MD2.h \
	kaffe_security_provider_MD4.h \
	kaffe_security_provider_MD5.h \
	kaffe_security_provider_SHA.h \
	$(MAYBE_LIBTRITONUSALSA_JNI_HDRS) \
	$(MAYBE_LIBTRITONUSESD_JNI_HDRS) \
	$(MAYBE_LIBXMLJ_JNI_HDRS) \
	$(MAYBE_LIBGTKPEER_JNI_HDRS) \
	$(MAYBE_LIBQTPEER_JNI_HDRS)

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
	  echo "$(KAFFEH) -jni -classpath $(top_builddir)/libraries/javalib/gmp-math/lib:$(KLASSES_JAR):$(top_builddir)/libraries/javalib/lib:$(top_builddir)/libraries/javalib/rt.jar -o $$f $$class"; \
	  $(KAFFEH) -jni -classpath $(top_builddir)/libraries/javalib/gmp-math/lib:$(KLASSES_JAR):$(top_builddir)/libraries/javalib/lib:$(top_builddir)/libraries/javalib/rt.jar -o stamp-h1$$f $$class; \
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
	stamp-libxmlj \
	stamp-libgtkpeer

dist-hook:
	@for file in $(DISTCLEANFILES); do \
	  rm -f $(distdir)/$$file; \
	done
