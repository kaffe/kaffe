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
	java_lang_VMObject.h \
	java_lang_String.h \
	java_lang_StackTraceElement.h \
	java_lang_Thread.h \
	java_lang_VMThread.h \
	java_lang_ThreadGroup.h \
	java_lang_Throwable.h \
	java_lang_VMThrowable.h \
	java_lang_SecurityManager.h \
	java_io_File.h \
	java_io_FileInputStream.h \
	java_io_FileOutputStream.h \
	java_io_InputStream.h \
	java_io_InterruptedIOException.h \
	java_io_ObjectInputStream.h \
	java_io_PrintStream.h \
	java_io_RandomAccessFile.h \
	java_io_VMFile.h \
	java_io_VMObjectStreamClass.h \
	java_lang_Boolean.h \
	java_lang_Byte.h \
	java_lang_Character.h \
	java_lang_Class.h \
	java_lang_ClassLoader.h \
	java_lang_Cloneable.h \
	java_lang_Double.h \
	java_lang_Float.h \
	java_lang_Integer.h \
	java_lang_Long.h \
	java_lang_Math.h \
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
	gnu_java_net_SysInetAddressImpl.h \
	java_net_SocketAddress.h \
	java_net_SocketImpl.h \
	java_net_SocketOptions.h \
	java_security_VMAccessController.h \
	java_text_DateFormat.h \
	java_util_Arrays.h \
	java_util_Comparator.h \
	java_util_VMTimeZone.h \
	java_util_Vector.h \
	java_util_zip_Adler32.h \
	java_util_zip_CRC32.h \
	java_util_zip_Deflater.h \
	java_util_zip_Inflater.h \
	java_util_zip_ZipEntry.h \
	java_util_zip_ZipFile.h \
	kaffe_lang_PrimordialClassLoader.h \
	kaffe_lang_ThreadStack.h \
	kaffe_management_Classpath.h \
	gnu_java_nio_VMSelector.h

DERIVED_HDRS = $(INSTALL_DERIVED_HDRS) $(NOINSTALL_DERIVED_HDRS)

INSTALL_JNI_DERIVED_HDRS =

NOINSTALL_JNI_DERIVED_HDRS = \
	java_math_BigInteger.h \
	gnu_classpath_VMSystemProperties.h \
	gnu_java_nio_channels_FileChannelImpl.h \
	gnu_java_nio_SelectorImpl.h \
	java_nio_channels_Channels.h \
	java_nio_VMDirectByteBuffer.h \
	java_nio_MappedByteBufferImpl.h \
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
	kaffe_security_provider_SHA.h


JNI_DERIVED_HDRS = $(INSTALL_JNI_DERIVED_HDRS) $(NOINSTALL_JNI_DERIVED_HDRS)


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
	gnu_java_awt_peer_gtk_GdkFontMetrics.h \
	gnu_java_awt_peer_gtk_GdkFontPeer.h \
	gnu_java_awt_peer_gtk_GdkGlyphVector.h \
	gnu_java_awt_peer_gtk_GdkGraphics.h \
	gnu_java_awt_peer_gtk_GdkGraphics2D.h \
	gnu_java_awt_peer_gtk_GdkGraphicsEnvironment.h \
	gnu_java_awt_peer_gtk_GdkPixbufDecoder.h \
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
	gnu_java_awt_peer_gtk_GtkImagePainter.h \
	gnu_java_awt_peer_gtk_GtkLabelPeer.h \
	gnu_java_awt_peer_gtk_GtkListPeer.h \
	gnu_java_awt_peer_gtk_GtkMainThread.h \
	gnu_java_awt_peer_gtk_GtkMenuBarPeer.h \
	gnu_java_awt_peer_gtk_GtkMenuComponentPeer.h \
	gnu_java_awt_peer_gtk_GtkMenuItemPeer.h \
	gnu_java_awt_peer_gtk_GtkMenuPeer.h \
	gnu_java_awt_peer_gtk_GtkPanelPeer.h \
	gnu_java_awt_peer_gtk_GtkPopupMenuPeer.h \
	gnu_java_awt_peer_gtk_GtkScrollbarPeer.h \
	gnu_java_awt_peer_gtk_GtkScrollPanePeer.h \
	gnu_java_awt_peer_gtk_GtkTextAreaPeer.h \
	gnu_java_awt_peer_gtk_GtkTextComponentPeer.h \
	gnu_java_awt_peer_gtk_GtkTextFieldPeer.h \
	gnu_java_awt_peer_gtk_GtkToolkit.h \
	gnu_java_awt_peer_gtk_GtkWindowPeer.h \
	gnu_java_awt_peer_gtk_GThreadNativeMethodRunner.h

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
	getaddrinfo.h \
	gettext.h \
	nets.h \
	system.h \
	Arrays.h \
	errors.h \
	jsyscall.h \
	native.h 

include_HEADERS = \
	jni.h \
	jni_cpp.h \
	jvmpi.h \
	kaffe_jni.h


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
	kaffe/jni_md.h

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
	  echo "$(KAFFEH) -jni -classpath $(KLASSES_JAR):$(top_builddir)/libraries/javalib/lib::$(top_builddir)/libraries/javalib/rt.jar -o $$f $$class"; \
	  $(KAFFEH) -jni -classpath $(KLASSES_JAR):$(top_builddir)/libraries/javalib/lib::$(top_builddir)/libraries/javalib/rt.jar -o stamp-h1$$f $$class; \
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

#
# header files for alsa backend of tritonus
#
.PHONY:libtritonusalsa_headers
libtritonusalsa_headers: $(LIBTRITONUSALSA_JNI_HDRS)

$(LIBTRITONUSALSA_JNI_HDRS): stamp-libtritonusalsa
	@if test -f $@; then :; else \
	  rm -f stamp-libtritonusalsa; $(MAKE) stamp-libtritonusalsa; \
	fi

stamp-libtritonusalsa: HDRS = $(LIBTRITONUSALSA_JNI_HDRS)
stamp-libtritonusalsa: rebuild-jni-hdrs
	@echo timestamp > $@

#
# header files for esd backend of tritonus
#
.PHONY:libtritonusesd_headers
libtritonusesd_headers: $(LIBTRITONUSESD_JNI_HDRS)

$(LIBTRITONUSESD_JNI_HDRS): stamp-libtritonusesd
	@if test -f $@; then :; else \
	  rm -f stamp-libtritonusesd; $(MAKE) stamp-libtritonusesd; \
	fi

stamp-libtritonusesd: HDRS = $(LIBTRITONUSESD_JNI_HDRS)
stamp-libtritonusesd: rebuild-jni-hdrs
	@echo timestamp > $@

#
# header files for libxmlj
#
.PHONY:libxmlj_headers
libxmlj_headers: $(LIBXMLJ_JNI_HDRS)

$(LIBXMLJ_JNI_HDRS): stamp-libxmlj
	@if test -f $@; then :; else \
	  rm -f stamp-libxmlj; $(MAKE) stamp-libxmlj; \
	fi

stamp-libxmlj: HDRS = $(LIBXMLJ_JNI_HDRS)
stamp-libxmlj: rebuild-jni-hdrs
	@echo timestamp > $@

#
# header files for classpath's gtk awt backend
#
.PHONY:libgtkpeer_headers
libgtkpeer_headers: $(LIBGTKPEER_JNI_HDRS)

$(LIBGTKPEER_JNI_HDRS): stamp-libgtkpeer
	@if test -f $@; then :; else \
	  rm -f stamp-libgtkpeer; $(MAKE) stamp-libgtkpeer; \
	fi

stamp-libgtkpeer: HDRS = $(LIBGTKPEER_JNI_HDRS)
stamp-libgtkpeer: rebuild-jni-hdrs
	@echo timestamp > $@


CLEANFILES = \
	$(DERIVED_HDRS) \
	$(JNI_DERIVED_HDRS) \
	$(LIBTRITONUSALSA_JNI_HDRS) \
	$(LIBTRITONUSESD_JNI_HDRS) \
	$(LIBXMLJ_JNI_HDRS) \
	stamp-kaffeh

DISTCLEANFILES = \
	$(DERIVED_HDRS) \
	$(JNI_DERIVED_HDRS) \
	kaffe/jni_md.h \
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
