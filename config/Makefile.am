# Configure makefile.
#
# Copyright (c) 1996, 1997, 1998, 1999
#	Transvirtual Technologies, Inc.  All rights reserved.
#
# Copyright (c) 2003
#       Kaffe.org contributors. See ChangeLog for details.
#
# See the file "license.terms" for information on usage and redistribution 
# of this file. 

if USE_JIT
ENGINE_GEN_HDRS = jit-md.h
else
ENGINE_GEN_HDRS =
endif

if ENABLE_XPROF
XPROF_HDRS = xprofile-md.h
else
XPROF_HDRS =
endif

GEN_HDRS = \
	md.h \
	callKaffeException.h \
	$(XPROF_HDRS) \
	$(ENGINE_GEN_HDRS)

DISTCLEANFILES = \
	$(GEN_HDRS) \
	stamp-h01 \
	stamp-h02 \
	stamp-h03 \
	stamp-h04

nodist_noinst_HEADERS = \
	$(GEN_HDRS)

noinst_HEADERS = \
	config-hacks.h \
	config-io.h \
	config-math.h \
	config-mem.h \
	config-net.h \
	config-setjmp.h \
	config-signal.h \
	config-std.h \
	katomic.h \
	kaffe-unix-stack.h \
	sysdepCallMethod-ffi.h

EXTRA_DIST = \
	config.alias \
	regen-forward \
	stamp-h1.in \
	generic/comparexch.h \
	generic/genatomic.h \
	generic/gentypes.h \
	alpha/alpha.c \
	alpha/atomic.h \
	alpha/callKaffeException.h \
	alpha/common.h \
	alpha/jit-alpha.def \
	alpha/jit-icode.h \
	alpha/jit.h \
	alpha/linux/config.frag \
	alpha/linux/jit-md.h \
	alpha/linux/md.c \
	alpha/linux/md.h \
	alpha/linux/sigcontextinfo.h \
	alpha/netbsd1/config.frag \
	alpha/netbsd1/jit-md.h \
	alpha/netbsd1/md.c \
	alpha/netbsd1/md.h \
	alpha/osf/config.frag \
	alpha/osf/jit-md.h \
	alpha/osf/md.c \
	alpha/osf/md.h \
	alpha/sysdepCallMethod.h \
	alpha/threads.h \
	alpha/trampolines.S \
	arm/atomicity.h \
	arm/callKaffeException.h \
	arm/common.h \
	arm/jit-arm.def \
	arm/jit-icode.h \
	arm/jit.h \
	arm/jit3-arm.def \
	arm/jit3-icode.h \
	arm/linux/config.frag \
	arm/linux/jit-md.h \
	arm/linux/md.c \
	arm/linux/md.h \
	arm/linux/sigcontextinfo.h \
	arm/netbsd1/config.frag \
	arm/netbsd1/jit-md.h \
	arm/netbsd1/jit3-md.h \
	arm/netbsd1/md.c \
	arm/netbsd1/md.h \
        arm/riscos/config.frag \
        arm/riscos/jit-md.h \
        arm/riscos/md.c \
        arm/riscos/md.h \
	arm/threads.h \
	arm/sysdepCallMethod.h \
	arm/trampolines.S \
	ia64/atomic.h \
	ia64/common.h \
	ia64/hpux/config.frag \
	ia64/hpux/md.h \
	ia64/linux/config.frag \
	ia64/linux/md.c \
	ia64/linux/md.h \
	ia64/threads.h \
	ia64/sysdepCallMethod.h \
	x86_64/common.h \
	x86_64/atomic.h \
	x86_64/freebsd2/config.frag \
	x86_64/freebsd2/md.c \
	x86_64/freebsd2/md.h \
	x86_64/linux/config.frag \
	x86_64/linux/md.c \
	x86_64/linux/md.h \
	x86_64/linux/sigcontextinfo.h \
	x86_64/threads.h \
	x86_64/sysdepCallMethod.h \
	i386/atomic.h \
	i386/fpu-set.h \
	i386/beos/config.frag \
	i386/beos/jit-md.h \
	i386/beos/jit3-md.h \
	i386/beos/md.c \
	i386/beos/md.h \
	i386/bsdi/config.frag \
	i386/bsdi/jit-md.h \
	i386/bsdi/md.c \
	i386/bsdi/md.h \
	i386/callKaffeException.h \
	i386/common.h \
	i386/cygwin32/config.frag \
	i386/cygwin32/jit-md.h \
	i386/cygwin32/md.c \
	i386/cygwin32/md.h \
	i386/darwin/config.frag \
	i386/darwin/md.c \
	i386/darwin/md.h \
	i386/dgux/config.frag \
	i386/dgux/jit-md.h \
	i386/dgux/md.c \
	i386/dgux/md.h \
	i386/freebsd2/config.frag \
	i386/freebsd2/jit-md.h \
	i386/freebsd2/jit3-md.h \
	i386/freebsd2/xprofile-md.h \
	i386/freebsd2/md.c \
	i386/freebsd2/md.h \
	i386/gnu/config.frag \
	i386/gnu/jit-md.h \
	i386/gnu/jit3-md.h \
	i386/gnu/md.c \
	i386/gnu/md.h \
	i386/jit-i386.def \
	i386/jit-icode.h \
	i386/jit.h \
	i386/jit3-i386.def \
	i386/jit3-icode.h \
	i386/linux/config.frag \
	i386/linux/jit-md.h \
	i386/linux/jit3-md.h \
	i386/linux/md.c \
	i386/linux/md.h \
	i386/linux/sigcontextinfo.h \
	i386/linux/xprofile-md.h \
	i386/netbsd1/config.frag \
	i386/netbsd1/jit-md.h \
	i386/netbsd1/jit3-md.h \
	i386/netbsd1/md.c \
	i386/netbsd1/md.h \
	i386/nextstep3/config.frag \
	i386/nextstep3/jit-md.h \
	i386/nextstep3/md.c \
	i386/nextstep3/md.h \
	i386/openbsd2/config.frag \
	i386/openbsd2/jit-md.h \
	i386/openbsd2/jit3-md.h \
	i386/openbsd2/md.c \
	i386/openbsd2/md.h \
	i386/oskit/README \
	i386/oskit/config.frag \
	i386/oskit/jit-md.h \
	i386/oskit/jit3-md.h \
	i386/oskit/ld-oskit.sh \
	i386/oskit/md.c \
	i386/oskit/md.h \
	i386/oskit/mkimage.sh \
	i386/oskit/oskit-configure \
	i386/qnx/config.frag \
	i386/qnx/jit-md.h \
	i386/qnx/md.c \
	i386/qnx/md.h \
	i386/sco3.2v5/config.frag \
	i386/sco3.2v5/jit-md.h \
	i386/sco3.2v5/md.c \
	i386/sco3.2v5/md.h \
	i386/solaris2/config.frag \
	i386/solaris2/jit-md.h \
	i386/solaris2/jit3-md.h \
	i386/solaris2/md.c \
	i386/solaris2/md.h \
	i386/sysdepCallMethod.h \
	i386/threads.h \
	i386/trampolines.S \
	i386/univel_svr4/config.frag \
	i386/univel_svr4/jit-md.h \
	i386/univel_svr4/jit3-md.h \
	i386/univel_svr4/md.c \
	i386/univel_svr4/md.h \
	i386/win32/jit-md.h \
	i386/win32/md.h \
	i386/win32/win32-md.asm \
	m68k/atomic.h \
	m68k/a-ux/config.frag \
	m68k/a-ux/jit-md.h \
	m68k/a-ux/md.c \
	m68k/a-ux/md.h \
	m68k/amigaos/README \
	m68k/amigaos/REPORT.amiga \
	m68k/amigaos/config.frag \
	m68k/amigaos/md.c \
	m68k/amigaos/md.h \
	m68k/callKaffeException.h \
	m68k/common.h \
	m68k/jit-icode.h \
	m68k/jit-m68k.def \
	m68k/jit.h \
	m68k/jit3-icode.h \
	m68k/jit3-m68k.def \
	m68k/linux/README \
	m68k/linux/clear-cache.h \
	m68k/linux/config.frag \
	m68k/linux/jit-md.h \
	m68k/linux/jit3-md.h \
	m68k/linux/md.c \
	m68k/linux/md.h \
	m68k/linux/sigcontextinfo.h \
	m68k/netbsd1/config.frag \
	m68k/netbsd1/jit-md.h \
	m68k/netbsd1/jit3-md.h \
	m68k/netbsd1/md.c \
	m68k/netbsd1/md.h \
	m68k/nextstep3/config.frag \
	m68k/nextstep3/md.c \
	m68k/nextstep3/md.h \
	m68k/openbsd2/config.frag \
	m68k/openbsd2/jit-md.h \
	m68k/openbsd2/jit.h \
	m68k/openbsd2/md.c \
	m68k/openbsd2/md.h \
	m68k/sunos4/config.frag \
	m68k/sunos4/md.c \
	m68k/sunos4/md.h \
	m68k/sysdepCallMethod.h \
	m68k/threads.h \
	m68k/trampolines.S \
	mips/atomic.h \
	mips/callKaffeException.h \
	mips/common.h \
	mips/irix5/config.frag \
	mips/irix5/jit-md.h \
	mips/irix5/jit3-md.h \
	mips/irix5/md.c \
	mips/irix5/md.h \
	mips/jit.h \
	mips/jit3-icode.h \
	mips/jit3-mips.def \
	mips/linux/config.frag \
	mips/linux/jit-md.h \
	mips/linux/jit3-md.h \
	mips/linux/md.c \
	mips/linux/md.h \
	mips/linux/sigcontextinfo.h \
	mips/mips.c \
	mips/n32-sysdepCallMethod.h \
	mips/netbsd1/config.frag \
	mips/netbsd1/jit-md.h \
	mips/netbsd1/jit3-md.h \
	mips/netbsd1/md.c \
	mips/netbsd1/md.h \
	mips/o32-sysdepCallMethod.h \
	mips/sysdepCallMethod.h \
	mips/threads.h \
	mips/trampolines.S \
	mips/ultrix4/Make.frag \
	mips/ultrix4/config.frag \
	mips/ultrix4/md.c \
	mips/ultrix4/md.h \
	parisc/common.h \
	parisc/hpux/config.frag \
	parisc/hpux/md.h \
	parisc/linux/config.frag \
	parisc/linux/md.h \
	parisc/sysdepCallMethod.h \
	parisc/threads.h \
	powerpc/aix/config.frag \
	powerpc/aix/md.c \
	powerpc/aix/md.h \
	powerpc/bits/atomic.h \
	powerpc/common.h \
	powerpc/darwin/config.frag \
	powerpc/darwin/md.c \
	powerpc/darwin/md.h \
	powerpc/darwin/jit3-md.h \
	powerpc/linux/config.frag \
	powerpc/linux/md.c \
	powerpc/linux/md.h \
	powerpc/linux/sigcontextinfo.h \
	powerpc/linux/jit3-md.h \
	powerpc/machten/config.frag \
	powerpc/machten/md.c \
	powerpc/machten/md.h \
	powerpc/netbsd1/config.frag \
	powerpc/netbsd1/md.c \
	powerpc/netbsd1/md.h \
	powerpc/netbsd1/jit3-md.h \
	powerpc/powerpc32/atomic.h \
	powerpc/powerpc64/atomic.h \
	powerpc/callmethod_ppc.h \
	powerpc/jit.h \
	powerpc/jit3-icode.h \
	powerpc/jit3-powerpc.def \
	powerpc/jit-labels.h \
	powerpc/ppc_isa.defs \
	powerpc/ppc_isa.h \
	powerpc/ppc_macros.h \
	powerpc/ppc_stack_frame.h \
	powerpc/sysdepCallMethod.h \
	powerpc/threads.h \
	powerpc/trampolines.S \
	powerpc/atomic.h \
	s390/atomic.h \
	s390/common.h \
	s390/linux/config.frag \
	s390/linux/md.c \
	s390/linux/md.h \
	s390/linux/sigcontextinfo.h \
	s390/sysdepCallMethod.h \
	s390/threads.h \
	sparc/atomic-sparc32.h \
	sparc/atomic-sparc32v9.h \
	sparc/atomic-sparc64.h \
	sparc/bsdi3/config.frag \
	sparc/bsdi3/jit-md.h \
	sparc/bsdi3/md.c \
	sparc/bsdi3/md.h \
	sparc/callKaffeException.h \
	sparc/common.h \
	sparc/jit-icode.h \
	sparc/jit-sparc.def \
	sparc/jit.h \
	sparc/linux/config.frag \
	sparc/linux/jit-md.h \
	sparc/linux/md.c \
	sparc/linux/md.h \
	sparc/linux/sigcontextinfo32.h \
	sparc/linux/sigcontextinfo64.h \
	sparc/netbsd1/config.frag \
	sparc/netbsd1/jit-md.h \
	sparc/netbsd1/md.c \
	sparc/netbsd1/md.h \
	sparc/nextstep3/config.frag \
	sparc/nextstep3/jit-md.h \
	sparc/nextstep3/md.c \
	sparc/nextstep3/md.h \
	sparc/openbsd2/config.frag \
	sparc/openbsd2/jit-md.h \
	sparc/openbsd2/md.c \
	sparc/openbsd2/md.h \
	sparc/solaris2/config.frag \
	sparc/solaris2/jit-md.h \
	sparc/solaris2/md.c \
	sparc/solaris2/md.h \
	sparc/sunos4/config.frag \
	sparc/sunos4/jit-md.h \
	sparc/sunos4/md.c \
	sparc/sunos4/md.h \
	sparc/sysdepCallMethod.h \
	sparc/threads.h \
	sparc/trampolines.S \
	sparc/uxpds/config.frag \
	sparc/uxpds/jit-md.h \
	sparc/uxpds/md.c \
	sparc/uxpds/md.h \
        sh/common.h \
        sh/linux/config.frag \
        sh/linux/md.c \
        sh/linux/md.h \
	sh/sysdepCallMethod.h \
        sh/threads.h \
        sh/trampolines.S

jit-md.h: stamp-h01
	@if test ! -f $@; then rm -f stamp-h01; $(MAKE) stamp-h01; \
	else :; fi
stamp-h01: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(CONFIG_JIT_MD_H) jit-md.h $@

md.h: stamp-h02
	@if test ! -f $@; then rm -f stamp-h02; $(MAKE) stamp-h02; \
	else :; fi
stamp-h02: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(CONFIG_MD_H) md.h $@

xprofile-md.h: stamp-h03
	@if test ! -f $@; then rm -f stamp-h03; $(MAKE) stamp-h03; \
	else :; fi
stamp-h03: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(CONFIG_XPROFILE_MD_H) xprofile-md.h $@

callKaffeException.h: stamp-h04
	@if test ! -f $@; then rm -f stamp-h04; $(MAKE) stamp-h04; \
	else :; fi
stamp-h04: $(top_builddir)/config.status
	$(SHELL) $(REGEN_FORWARD) $(CONFIG_CALLKAFFEEXCEPTION_H) callKaffeException.h $@

dist-hook:
	@for file in $(DISTCLEANFILES); do \
	  rm -f $(distdir)/$$file; \
	done
