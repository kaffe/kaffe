native_LTLIBRARIES = libgtkpeer.la

# Gtk/Cairo JNI sources.
if GTK_CAIRO
gtk_cairo_c_source_files = \
	gnu_java_awt_peer_gtk_GdkGlyphVector.c \
	gnu_java_awt_peer_gtk_GdkGraphics2D.c
endif

libgtkpeer_la_SOURCES = \
	$(gtk_cairo_c_source_files) \
	gdkfont.h \
	gnu_java_awt_peer_gtk_GThreadNativeMethodRunner.c \
	gnu_java_awt_peer_gtk_GThreadNativeMethodRunner.h \
	gnu_java_awt_peer_gtk_GdkFontMetrics.c \
	gnu_java_awt_peer_gtk_GdkFontPeer.c \
	gnu_java_awt_peer_gtk_GdkGraphics.c \
	gnu_java_awt_peer_gtk_GdkGraphicsEnvironment.c \
	gnu_java_awt_peer_gtk_GdkPixbufDecoder.c \
	gnu_java_awt_peer_gtk_GdkRobotPeer.c \
	gnu_java_awt_peer_gtk_GdkTextLayout.c \
	gnu_java_awt_peer_gtk_GtkFileDialogPeer.c \
	gnu_java_awt_peer_gtk_GtkButtonPeer.c \
	gnu_java_awt_peer_gtk_GtkCanvasPeer.c \
	gnu_java_awt_peer_gtk_GtkCheckboxGroupPeer.c \
	gnu_java_awt_peer_gtk_GtkCheckboxMenuItemPeer.c \
	gnu_java_awt_peer_gtk_GtkCheckboxPeer.c \
	gnu_java_awt_peer_gtk_GtkChoicePeer.c \
	gnu_java_awt_peer_gtk_GtkClipboard.c \
	gnu_java_awt_peer_gtk_GtkComponentPeer.c \
	gnu_java_awt_peer_gtk_GtkEvents.c \
	gnu_java_awt_peer_gtk_GtkEmbeddedWindowPeer.c \
	gnu_java_awt_peer_gtk_GtkFramePeer.c \
	gnu_java_awt_peer_gtk_GtkGenericPeer.c	\
	gnu_java_awt_peer_gtk_GtkImagePainter.c \
	gnu_java_awt_peer_gtk_GtkLabelPeer.c \
	gnu_java_awt_peer_gtk_GtkListPeer.c	\
	gnu_java_awt_peer_gtk_GtkMainThread.c \
	gnu_java_awt_peer_gtk_GtkMenuBarPeer.c \
        gnu_java_awt_peer_gtk_GtkMenuComponentPeer.c \
	gnu_java_awt_peer_gtk_GtkMenuItemPeer.c \
	gnu_java_awt_peer_gtk_GtkMenuPeer.c \
	gnu_java_awt_peer_gtk_GtkPanelPeer.c \
	gnu_java_awt_peer_gtk_GtkPopupMenuPeer.c \
	gnu_java_awt_peer_gtk_GtkScrollBarPeer.c \
	gnu_java_awt_peer_gtk_GtkScrollPanePeer.c \
	gnu_java_awt_peer_gtk_GtkTextAreaPeer.c \
	gnu_java_awt_peer_gtk_GtkTextComponentPeer.c \
	gnu_java_awt_peer_gtk_GtkTextFieldPeer.c \
	gnu_java_awt_peer_gtk_GtkToolkit.c \
	gnu_java_awt_peer_gtk_GtkWindowPeer.c \
	gthread-jni.c \
	gthread-jni.h \
	gtkpeer.h 

libgtkpeer_la_CFLAGS = \
	$(CFLAGS_PG)

libgtkpeer_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	@GTK_CFLAGS@ \
	@CAIRO_CFLAGS@ \
	@PANGOFT2_CFLAGS@ \
	-I$(top_srcdir)/libraries/clib/classpath/ \
	-I$(top_builddir)/include/

libgtkpeer_la_DEPENDENCIES = \
	$(top_builddir)/libraries/clib/classpath/libclasspath.la

libgtkpeer_la_LIBADD = \
	$(libgtkpeer_la_DEPENDENCIES)

libgtkpeer_la_LDFLAGS = \
	@CLASSPATH_MODULE@ \
	@GTK_LIBS@ \
	@CAIRO_LIBS@ \
	@PANGOFT2_LIBS@ \
	@X_LIBS@ \
	-lXtst

$(libgtkpeer_la_SOURCES): libgtkpeer_headers

.PHONY:libgtkpeer_headers
libgtkpeer_headers:
	@$(MAKE) -C $(top_builddir)/include libgtkpeer_headers
