# Alsa files for Tritonus javax.sound implementation
#
# Copyright (c) 2002, 2003, 2004
#	Dalibor Topic <robilad@kaffe.org>
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

AM_CPPFLAGS = -I$(top_builddir)/include

native_LTLIBRARIES = libtritonusalsa.la

libtritonusalsa_la_CFLAGS = \
	$(CFLAGS_PG)

libtritonusalsa_la_LDFLAGS = \
	$(KLIBFLAGS) \
	-module \
	-release $(PACKAGE_VERSION) \
	-export-symbols-regex "^([Jj]ava|org)_"

libtritonusalsa_la_LIBADD = -lasound

libtritonusalsa_la_SOURCES = \
	common.c \
	init.c \
	org_tritonus_lowlevel_alsa_Alsa.c \
	org_tritonus_lowlevel_alsa_AlsaCtl.c \
	org_tritonus_lowlevel_alsa_AlsaCtlCardInfo.c \
	org_tritonus_lowlevel_alsa_AlsaMixer.c \
	org_tritonus_lowlevel_alsa_AlsaMixerElement.c \
	org_tritonus_lowlevel_alsa_AlsaPcm.c \
	org_tritonus_lowlevel_alsa_AlsaPcmHWParams.c \
	org_tritonus_lowlevel_alsa_AlsaPcmHWParamsFormatMask.c \
	org_tritonus_lowlevel_alsa_AlsaPcmSWParams.c \
	org_tritonus_lowlevel_alsa_AlsaSeq.c \
	org_tritonus_lowlevel_alsa_AlsaSeqClientInfo.c \
	org_tritonus_lowlevel_alsa_AlsaSeqEvent.c \
	org_tritonus_lowlevel_alsa_AlsaSeqPortInfo.c \
	org_tritonus_lowlevel_alsa_AlsaSeqPortSubscribe.c \
	org_tritonus_lowlevel_alsa_AlsaSeqQueueInfo.c \
	org_tritonus_lowlevel_alsa_AlsaSeqQueueStatus.c \
	org_tritonus_lowlevel_alsa_AlsaSeqQueueTempo.c \
	org_tritonus_lowlevel_alsa_AlsaSeqRemoveEvents.c \
	org_tritonus_lowlevel_alsa_AlsaSeqSystemInfo.c

noinst_HEADERS = \
	common.h \
	constants_check.h

CLEANFILES = so_locations
