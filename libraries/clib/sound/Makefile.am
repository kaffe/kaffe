# Base level Makefile.am for kaffe extensions
#
# Copyright (c) 2002, 2003
#	Dalibor Topic <robilad@yahoo.com>
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

## Pick sound backends
if COND_SOUND_ALSA
  MAYBE_ALSA_DIR = alsa
endif

if COND_SOUND_ESD
  MAYBE_ESD_DIR = esd
endif

SUBDIRS = common $(MAYBE_ALSA_DIR) $(MAYBE_ESD_DIR)

