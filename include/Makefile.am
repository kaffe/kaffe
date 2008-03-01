# Library support
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
# Copyright (c) 2003
#	Mark Wielaard <mark@klomp.org>
# Copyright (c) 2004, 2006, 2008
#	Kaffe.org contributors. All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

KAFFEH_CLASSPATH = $(top_builddir)/libraries/javalib/vmspecific/kaffe-vminterface.jar:$(top_builddir)/libraries/javalib/zlib-zip/kaffe-zlib.jar:$(top_builddir)/libraries/javalib/gmp-math/gmpjavamath.jar:$(GLIBJ_ZIP)

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

if COND_ZLIB_ZIP
MAYBE_ZLIB_ZIP_HDRS = \
	java_util_zip_ZipEntry.h \
	java_util_zip_ZipFile.h 
endif

NOINSTALL_DERIVED_HDRS = \
	java_lang_Object.h \
	java_lang_VMObject.h \
	java_lang_VMClass.h \
	java_lang_VMRuntime.h \
	java_lang_VMClassLoader.h \
	java_lang_String.h \
	java_lang_StackTraceElement.h \
	java_lang_Thread.h \
	java_lang_ThreadGroup.h \
	java_lang_VMThread.h \
	java_lang_Throwable.h \
	java_lang_VMThrowable.h \
	java_lang_ClassLoader.h \
	java_lang_Cloneable.h \
	java_lang_reflect_Constructor.h \
	java_lang_reflect_Field.h \
	java_lang_reflect_Method.h \
	java_security_VMAccessController.h \
	java_util_Vector.h \
	gnu_classpath_VMStackWalker.h \
	$(MAYBE_ZLIB_ZIP_HDRS)

DERIVED_HDRS = $(INSTALL_DERIVED_HDRS) $(NOINSTALL_DERIVED_HDRS)

INSTALL_JNI_DERIVED_HDRS =

NOINSTALL_JNI_DERIVED_HDRS = \
	sun_misc_Unsafe.h \
	java_math_BigInteger.h \
	java_lang_VMProcess.h \
	java_lang_ref_Reference.h \
	gnu_classpath_VMSystemProperties.h \
	gnu_java_nio_SelectorImpl.h \
	java_lang_VMSystem.h \
	java_nio_VMDirectByteBuffer.h \
	java_nio_MappedByteBufferImpl.h 

JNI_DERIVED_HDRS = \
	$(INSTALL_JNI_DERIVED_HDRS) \
	$(NOINSTALL_JNI_DERIVED_HDRS)

nodist_noinst_HEADERS = \
	$(NOINSTALL_DERIVED_HDRS) \
	$(NOINSTALL_JNI_DERIVED_HDRS)

noinst_HEADERS = \
	defs.h \
	files.h \
	gccbuiltin.h \
	gettext.h \
	system.h \
	Arrays.h \
	errors.h \
	jsyscall.h \
	native.h 

include_HEADERS = \
	jni.h \
	jni_md.h \
	jni_cpp.h \
	jvmpi.h \
	kaffe_jni.h

nodist_pkginclude_HEADERS = \
	$(INSTALL_DERIVED_HDRS) \
	$(INSTALL_JNI_DERIVED_HDRS)

pkginclude_HEADERS = \
	java_lang_Object.h \
	java_lang_String.h \
	java_lang_Thread.h \
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

stamp-h0all: stamp-kaffeh $(GLIBJ_ZIP)
## Then, generate each header file,
## but if it does not change, do not touch it
	@set -e; for f in $(DERIVED_HDRS); do \
	  class=`echo $$f | $(SED) -e 's%.*/%%g' -e 's%\.h$$%%' -e 's%_%/%g'`; \
	  echo "$(KAFFEH) -bootclasspath $(KAFFEH_CLASSPATH) -o $$f $$class"; \
	  $(KAFFEH) -bootclasspath $(KAFFEH_CLASSPATH) -o stamp-h0$$f $$class; \
	  if test -f $$f && cmp $$f stamp-h0$$f >/dev/null; then \
	    echo "$$f is already up-to-date"; \
	    rm -f stamp-h0$$f; \
	  else \
	    rm -f $$f; mv stamp-h0$$f $$f; \
	  fi; \
	done
	@echo timestamp > $@


stamp-h1all: stamp-kaffeh $(GLIBJ_ZIP)
## Then, generate each header file,
## but if it does not change, do not touch it
	@set -e; for f in $(JNI_DERIVED_HDRS); do \
	  class=`echo $$f | $(SED) -e 's%.*/%%g' -e 's%\.h$$%%' -e 's%_%/%g'`; \
	  echo "$(KAFFEH) -jni -bootclasspath $(KAFFEH_CLASSPATH) -o $$f $$class"; \
	  $(KAFFEH) -jni -bootclasspath $(KAFFEH_CLASSPATH) -o stamp-h1$$f $$class; \
	  if test -f $$f && cmp $$f stamp-h1$$f >/dev/null; then \
	    echo "$$f is already up-to-date"; \
	    rm -f stamp-h1$$f; \
	  else \
	    rm -f $$f; mv stamp-h1$$f $$f; \
	  fi; \
	done
	@echo timestamp > $@

#
# jni headers of the core libraries
#
$(JNI_DERIVED_HDRS): stamp-h1all
	@if test -f $@; then :; else \
	  rm -f stamp-h1all; $(MAKE) JNI_DERIVED_HDRS=$@ stamp-h1all; \
	fi

CLEANFILES = \
	$(DERIVED_HDRS) \
	$(JNI_DERIVED_HDRS) \
	stamp-kaffeh

DISTCLEANFILES = \
	$(DERIVED_HDRS) \
	$(JNI_DERIVED_HDRS) \
	stamp-h0all \
	stamp-h1all \
	stamp-libtritonusalsa \
	stamp-libtritonusesd \
	stamp-libgtkpeer

dist-hook:
	@for file in $(DISTCLEANFILES); do \
	  rm -f $(distdir)/$$file; \
	done
