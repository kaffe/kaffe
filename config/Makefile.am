# Configure makefile.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

if USE_JIT
ENGINE_BUILT_SRCS = jit-md.h
else
ENGINE_BUILT_SRCS =
endif

BUILT_SOURCES = md.h $(ENGINE_BUILT_SRCS)

noinst_HEADERS = config-hacks.h config-io.h config-math.h config-mem.h \
	config-net.h config-setjmp.h config-signal.h config-std.h

EXTRA_DIST = config.alias \
	*/common.h */jit-*.def */jit-icode.h */jit.h */*/jit.h */*/jit-md.h \
	*/threads.h */trampolines.c 
	*/*/Make.frag */*/config.frag */*/md.c */*/md.h \
	m68k/amigaos/REPORT.amiga m68k/linux/README mips/mips.c 

jit-md.h: stamp-h01
	@if test ! -f $@; then rm -f stamp-h01; $(MAKE) stamp-h01; fi
stamp-h01: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(CONFIG_JIT_MD_H) jit-md.h $@

md.h: stamp-h02
	@if test ! -f $@; then rm -f stamp-h02; $(MAKE) stamp-h02; fi
stamp-h02: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(CONFIG_MD_H) md.h $@
