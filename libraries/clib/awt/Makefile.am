# generic AWT support for standard Java libraries
#
# Copyright (c) 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# Copyright (c) 2004
#	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

# This section used to be a simple series of if conditionals, 
# but automake 1.8.5 and 1.8d decided to mangle it to death.
# so, you get this undebuggable, redundancy-ridden nested 
# conditionals that will hopefully just plain work with automake.
#
#

if COND_KAFFE_X_AWT
MAYBE_KAFFE_X_AWT = X
endif

if COND_KAFFE_QT_AWT
MAYBE_KAFFE_QT_AWT = qt
endif

if COND_KAFFE_NANOX_AWT
MAYBE_KAFFE_NANOX_AWT = nano-X
endif

if COND_CLASSPATH_GTK_AWT
MAYBE_CLASSPATH_GTK_AWT = classpath-gtk
endif

if COND_CLASSPATH_QT_AWT
MAYBE_CLASSPATH_QT_AWT = classpath-qt
endif

SUBDIRS = \
	$(MAYBE_KAFFE_X_AWT) \
	$(MAYBE_KAFFE_QT_AWT) \
	$(MAYBE_KAFFE_NANOX_AWT) \
	$(MAYBE_CLASSPATH_GTK_AWT) \
	$(MAYBE_CLASSPATH_QT_AWT)

EXTRA_DIST = \
	win32/btn.cpp \
	win32/cbd.cpp \
	win32/choice.cpp \
	win32/clr.cpp \
	win32/cmnwnd.cpp \
	win32/cmnwnd.hpp \
	win32/edit.cpp \
	win32/evt.cpp \
	win32/filedlg.cpp \
	win32/fnt.cpp \
	win32/gra.cpp \
	win32/img.cpp \
	win32/imggif.cpp \
	win32/imgjpeg.cpp \
	win32/imgpng.cpp \
	win32/lbl.cpp \
	win32/lst.cpp \
	win32/menu.cpp \
	win32/scroll.cpp \
	win32/tlk.cpp \
	win32/tlkprops.hpp \
	win32/toolkit.hpp \
	win32/widget.cpp \
	win32/wnd.cpp
