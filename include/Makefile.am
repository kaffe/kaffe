# Library support
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

KLASSES_JAR = $(top_srcdir)/libraries/javalib/Klasses.jar

INSTALL_DERIVED_HDRS = \
	java_lang_Throwable.h \
	java_lang_Thread.h \
	java_lang_ThreadGroup.h

NOINSTALL_DERIVED_HDRS = \
	java_io_File.h \
	java_io_FileDescriptor.h \
	java_io_FileInputStream.h \
	java_io_FileOutputStream.h \
	java_io_InputStream.h \
	java_io_ObjectInputStream.h \
	java_io_ObjectOutputStream.h \
	kaffe_io_ObjectStreamClassImpl.h \
	java_io_PrintStream.h \
	java_io_RandomAccessFile.h \
	java_lang_Boolean.h \
	java_lang_Byte.h \
	java_lang_Character.h \
	java_lang_Cloneable.h \
	java_lang_Compiler.h \
	java_lang_Double.h \
	java_lang_Float.h \
	java_lang_Integer.h \
	java_lang_Long.h \
	java_lang_Math.h \
	java_lang_Runtime.h \
	java_lang_SecurityManager.h \
	java_lang_Short.h \
	java_lang_System.h \
	java_lang_reflect_Array.h \
	java_lang_reflect_Constructor.h \
	java_lang_reflect_Field.h \
	java_lang_reflect_Method.h \
	java_net_DatagramPacket.h \
	java_net_InetAddress.h \
	java_net_InetAddressImpl.h \
	java_net_PlainDatagramSocketImpl.h \
	java_net_PlainSocketImpl.h \
	java_net_SocketImpl.h \
	java_net_SocketOptions.h \
	java_util_Vector.h \
	java_util_zip_Adler32.h \
	java_util_zip_CRC32.h \
	java_util_zip_Deflater.h \
	java_util_zip_Inflater.h \
	java_util_zip_ZipEntry.h \
	kaffe_lang_UNIXProcess.h

DERIVED_HDRS = $(INSTALL_DERIVED_HDRS) $(NOINSTALL_DERIVED_HDRS)

noinst_HEADERS = $(NOINSTALL_DERIVED_HDRS) \
	defs.h \
	files.h \
	nets.h \
	system.h

include_HEADERS = $(INSTALL_DERIVED_HDRS) \
	jtypes.h \
	native.h \
	jmalloc.h \
	jsyscall.h \
	jni.h \
	jni_cpp.h \
	Arrays.h \
	java_lang_Object.h \
	java_lang_String.h

$(DERIVED_HDRS): stamp-h0all
	@if test -f $@; then :; else \
## If this file is removed after stamp-h0all is built, force its rebuild.
	  rm -f stamp-h0all; $(MAKE) DERIVED_HDRS=$@ stamp-h0all; \
	fi

stamp-h0all: $(KLASSES_JAR)
## If KAFFEH does not exist, build it
	@test -x $(KAFFEH) || ($(MAKE_KAFFEH))
## Then, generate each header file,
## but if it does not change, do not touch it
	@for f in $(DERIVED_HDRS); do \
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

CLEANFILES = $(DERIVED_HDRS)
