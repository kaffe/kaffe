nativeexeclib_LTLIBRARIES = libgconfpeer.la

libgconfpeer_la_SOURCES = GConfNativePeer.c

libgconfpeer_la_LIBADD = $(top_builddir)/native/jni/classpath/native_state.lo \
                       $(top_builddir)/native/jni/classpath/jcl.lo

AM_LDFLAGS = @CLASSPATH_MODULE@ @GCONF_LIBS@ 
#@CLASSPATH_MODULE@ @GCONF_LIBS@ @CAIRO_LIBS@ @FREETYPE2_LIBS@ \
#             @PANGOFT2_LIBS@ @X_PRE_LIBS@ @X_LIBS@ @X_EXTRA_LIBS@ -lX11 -lXtst

AM_CPPFLAGS = @CLASSPATH_INCLUDES@

AM_CFLAGS = @WARNING_CFLAGS@ @ERROR_CFLAGS@ @GCONF_CFLAGS@
