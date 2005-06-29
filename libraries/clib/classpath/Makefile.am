noinst_LTLIBRARIES = libclasspath.la

AM_CPPFLAGS = \
        -I$(top_srcdir)/kaffe/jvmpi -I$(top_builddir)/include \
        -I$(top_srcdir)/kaffe/kaffevm/jni

libclasspath_la_SOURCES = \
			jcl.c jcl.h \
			jnilink.c jnilink.h \
			native_state.c native_state.h \
			classpath_jawt.h

libclasspath_la_LDFLAGS = \
	$(KLIBFLAGS) \
        -no-undefined 
