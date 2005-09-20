# Qt AWT backend for Classpath
#
# Copyright (c) 2005
#	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

native_LTLIBRARIES = libqtpeer.la

QT_CXXFLAGS=`echo $(QT_CFLAGS) | sed 's/include/include\/Qt/'`

AM_CPPFLAGS = \
	-I$(top_builddir)/include

libqtpeer_la_CXXFLAGS = \
	$(QT_CFLAGS) \
	$(QT_CXXFLAGS)

libqtpeer_la_LDFLAGS = \
	$(QT_LIBS) \
	-no-undefined \
	-module \
	-release $(PACKAGE_VERSION)

libqtpeer_la_MOC = \
	slotcallbacks.moc.h

slotcallbacks.moc.h: slotcallbacks.cpp
	$(MOC) -o slotcallbacks.moc.h $(srcdir)/slotcallbacks.cpp

nodist_libqtpeer_la_SOURCES = \
	$(libqtpeer_la_MOC) 

libqtpeer_la_SOURCES = \
        buttonevent.h \
        componentevent.cpp \
        componentevent.h \
        containers.h \
        eventmethods.h \
        keybindings.cpp \
        keybindings.h \
        mainqtthread.cpp \
        mainthreadinterface.cpp \
        mainthreadinterface.h \
        nativewrapper.cpp \
        nativewrapper.h \
        qmatrix.cpp \
        qpainterpath.cpp \
        qpen.cpp \
        qtaudioclip.cpp \
        qtbuttonpeer.cpp \
        qtcanvaspeer.cpp \
        qtcheckboxpeer.cpp \
        qtchoicepeer.cpp \
        qtcomponent.cpp \
        qtcomponent.h \
        qtcomponentpeer.cpp \
        qtdialogpeer.cpp \
        qtfiledialogpeer.cpp \
        qtfont.h \
        qtfontmetrics.cpp \
        qtfontpeer.cpp \
        qtframepeer.cpp \
        qtgraphics.cpp \
        qtgraphics.h \
        qtimage.cpp \
        qtimage.h \
        qtlabelpeer.cpp \
        qtlistpeer.cpp \
        qtmenubarpeer.cpp \
        qtmenucomponentpeer.cpp \
        qtmenuitempeer.cpp \
        qtmenupeer.cpp \
        qtpanelpeer.cpp \
        qtpopupmenupeer.cpp \
        qtscreendevice.cpp \
        qtscrollbarpeer.cpp \
        qtscrollpanepeer.cpp \
        qtstrings.cpp \
        qtstrings.h \
        qttextareapeer.cpp \
        qttextfieldpeer.cpp \
        qttoolkit.cpp \
        qtvolatileimage.cpp \
        qtwindowpeer.cpp \
        slotcallbacks.cpp \
        slotcallbacks.h 

# 	qtembeddedwindowpeer.cpp 

BUILT_SOURCES = $(libqtpeer_la_MOC)

CLEANFILES = so_locations $(BUILT_SOURCES)
