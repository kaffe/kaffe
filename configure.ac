AC_INIT([Kaffe Virtual Machine],[1.1.10-pre],[kaffe@kaffe.org],[kaffe])

AC_PREREQ(2.61)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_SRCDIR([kaffe])

# Copyright information. The nice sounding paragraphs about
# GPLed portions has been taken from the GPL-d configure.ac
# file of gimp-print project

AC_COPYRIGHT([Copyright 1996 - 2008 Kaffe.org project contributors. All rights reserved.
Portions Copyright 1996 - 2002 Transvirtual Technologies, Inc.
Portions Copyright by various third parties, see file THIRDPARTY for details.

This file is part of Kaffe Virtual Machine, a free software runtime
for java applications. See http://www.kaffe.org for details.

Parts of this configure script come from the `configure.ac' file, and
are not based on freely-redistributable m4 macros; you can redistribute
them and/or modify them under the terms of the GNU General Public
as published by the Free Software Foundation; either version 2, or (at
your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
])

AM_INIT_AUTOMAKE([1.9 dist-bzip2 dist-zip tar-ustar])

AC_PREFIX_DEFAULT(/usr/local/kaffe)

AM_MAINTAINER_MODE

dnl needed for autoheader to find acconfig.h
AUTOHEADER="$AUTOHEADER --include=config"

dnl abssrcdir is the absolute path to the source base (regardless of where
dnl you are building it)
case x$srcdir in
x/*)
	abssrcdir=$srcdir
	;;
*)
	abssrcdir=`pwd`/$srcdir
	;;
esac
AC_SUBST(abssrcdir)


dnl =========================================================================
dnl Allow specification of header-file and library directories
dnl -------------------------------------------------------------------------

AC_ARG_WITH([includes],
  	    AS_HELP_STRING([--with-includes=dirs],
			   [Search specified directories for header files]),
  [ CPPFLAGS="$CPPFLAGS"`echo " $withval" |
	sed -e 's/ / -I/g' -e 's/-I //g' -e 's/-I$//'` ],
  [ CPPFLAGS="$CPPFLAGS" ])

AC_ARG_WITH([libraries],
	    AS_HELP_STRING([--with-libraries=dirs],
			   [Search for libraries in specified directories]),
  [ CTLIBS="$CTLIBS"`echo " $withval" |
	sed -e 's/ / -L/g' -e 's/-L //g' -e 's/-L$//'`
    LTLIBS="$LTLIBS"`echo " $withval" |
	sed -e 's/ / -R/g' -e 's/-R //g' -e 's/-R$//'`],
  [ CTLIBS="$CTLIBS"
    LTLIBS="$LTLIBS"])

LIBS="$CTLIBS $LIBS"

dnl Find the compiler early on in case we need to override.
AC_PROG_CC
AC_PROG_CC_C99

dnl If gcc is the compiler, compile with -Wall for lots of warnings

AX_CFLAGS_WARN_ALL

# the following are covered by -Wall, but we list them separately, so
# that it's easier to control.

# AX_CFLAGS_GCC_OPTION([-Wno-import])
# AX_CFLAGS_GCC_OPTION([-Wchar-subscripts])
# AX_CFLAGS_GCC_OPTION([-Wcomment])

# AX_CFLAGS_GCC_OPTION([-Wformat])
# AX_CFLAGS_GCC_OPTION([-Wformat-y2k])
# AX_CFLAGS_GCC_OPTION([-Wno-format-extra-args])
# AX_CFLAGS_GCC_OPTION([-Wno-format-zero-length])
# AX_CFLAGS_GCC_OPTION([-Wformat-nonliteral])
# AX_CFLAGS_GCC_OPTION([-Wformat-security])
# -Wformat=2 includes -Wformat, -Wformat-nonliteral -Wformat-security, -Wformat-y2k
# AX_CFLAGS_GCC_OPTION([-Wformat=2])

# -Wnonnull is included in -Wformat
# AX_CFLAGS_GCC_OPTION([-Wnonnull])

# -Winit-self requires(-Wunitialized)
# AX_CFLAGS_GCC_OPTION([-Winit-self])

# AX_CFLAGS_GCC_OPTION([-Wimplicit-function-declaration])
# AX_CFLAGS_GCC_OPTION([-Wimplicit-int])
# -Wimplicit includes -Wimplicit-int, -Wimplicit-function-declaration
# AX_CFLAGS_GCC_OPTION([-Wimplicit])
# AX_CFLAGS_GCC_OPTION([-Wmain])
# AX_CFLAGS_GCC_OPTION([-Wmissing-braces])
# AX_CFLAGS_GCC_OPTION([-Wparentheses])
# AX_CFLAGS_GCC_OPTION([-Wsequence-point])
# AX_CFLAGS_GCC_OPTION([-Wreturn-type])
# AX_CFLAGS_GCC_OPTION([-Wswitch])
# AX_CFLAGS_GCC_OPTION([-Wswitch-default])
# AX_CFLAGS_GCC_OPTION([-Wswitch-enum])
# AX_CFLAGS_GCC_OPTION([-Wtrigraphs])
# AX_CFLAGS_GCC_OPTION([-Wunused-function])
# AX_CFLAGS_GCC_OPTION([-Wunused-label])
# AX_CFLAGS_GCC_OPTION([-Wunused-parameter])
# AX_CFLAGS_GCC_OPTION([-Wunused-variable])
# AX_CFLAGS_GCC_OPTION([-Wunused-value])
# -Wunused includes -Wunused-function, -Wunused-label, -Wunused-parameter, -Wunused-variable, -Wunused-value
# AX_CFLAGS_GCC_OPTION([-Wunused])
## disable -Wuninitialized as it clashes with -O0
## leading to trouble for libtool, which
## misinterprets the compiler warnings for
## something more serious.
# AX_CFLAGS_GCC_OPTION([-Wuninitialized])
# AX_CFLAGS_GCC_OPTION([-Wunknown-pragmas])
# requires -fstrict-aliasing
# AX_CFLAGS_GCC_OPTION([-Wstrict-aliasing])
# includes all of the above
# AX_CFLAGS_GCC_OPTION([-Wall])

# turn on all other gcc warnings for more fixing fun

# AX_CFLAGS_GCC_OPTION([-pedantic])
# AX_CFLAGS_GCC_OPTION([-ansi])

AX_CFLAGS_GCC_OPTION([-W])	dnl new name is -Wextra
AX_CFLAGS_GCC_OPTION([-Wextra])

# AX_CFLAGS_GCC_OPTION([-Wfloat-equal])
# AX_CFLAGS_GCC_OPTION([-Wtraditional])
# AX_CFLAGS_GCC_OPTION([-Wdeclaration-after-statement])
# AX_CFLAGS_GCC_OPTION([-Wundef])
# AX_CFLAGS_GCC_OPTION([-Wendif-labels])
# AX_CFLAGS_GCC_OPTION([-Wshadow])
# AX_CFLAGS_GCC_OPTION([-Wpointer-arith])
# AX_CFLAGS_GCC_OPTION([-Wbad-function-cast])
# AX_CFLAGS_GCC_OPTION([-Wcast-qual])
# AX_CFLAGS_GCC_OPTION([-Wcast-align])
# AX_CFLAGS_GCC_OPTION([-Wwrite-strings]) 
# AX_CFLAGS_GCC_OPTION([-Wconversion])
# AX_CFLAGS_GCC_OPTION([-Wsign-compare]) 
# AX_CFLAGS_GCC_OPTION([-Waggregate-return]) 
# AX_CFLAGS_GCC_OPTION([-Wstrict-prototypes])
# AX_CFLAGS_GCC_OPTION([-Wold-style-definition])
# AX_CFLAGS_GCC_OPTION([-Wmissing-prototypes])
# AX_CFLAGS_GCC_OPTION([-Wmissing-declarations])
# AX_CFLAGS_GCC_OPTION([-Wmissing-noreturn])
# AX_CFLAGS_GCC_OPTION([-Wmissing-format-attribute]) 
# AX_CFLAGS_GCC_OPTION([-Wno-multichar])
# AX_CFLAGS_GCC_OPTION([-Wno-deprecated-declarations])
# AX_CFLAGS_GCC_OPTION([-Wpacked])
# AX_CFLAGS_GCC_OPTION([-Wpadded])
# AX_CFLAGS_GCC_OPTION([-Wredundant-decls])
# AX_CFLAGS_GCC_OPTION([-Wnested-externs])
# AX_CFLAGS_GCC_OPTION([-Wunreachable-code])
# AX_CFLAGS_GCC_OPTION([-Winvalid-pch])
# AX_CFLAGS_GCC_OPTION([-Winline])
# AX_CFLAGS_GCC_OPTION([-Wlong-long])
# AX_CFLAGS_GCC_OPTION([-Wdisabled-optimization])

## required for old darwin versions because Apples'
## gcc falls appart all over assembler statements 
## and produces a ton of false warnings otherwise.
## AX_CFLAGS_GCC_OPTION([-no-cpp-precomp])

## required for powerpc assember register names
## in order to use a common notation on darwin
## and GNU/Linux.
AX_CFLAGS_GCC_OPTION([-mregnames],KAFFE_CCASFLAGS)

## Required for gcc 4.1.x as long as kaffe's
## not cleaned up to use unions in classMethod.h
## where necessary.
AX_CFLAGS_GCC_OPTION([-fno-strict-aliasing])

## Required for gcc 4.1.x and above, as it 
## omits the frame pointer on -O1 and above on
## some architectures, notably i386, making
## it impossible to unwind exceptions.
AX_CFLAGS_GCC_OPTION([-fno-omit-frame-pointer])

GCC_ATTRIBUTE_ALWAYS_INLINE
GCC_ATTRIBUTE_CONST
GCC_ATTRIBUTE_FORMAT
GCC_ATTRIBUTE_NORETURN
GCC_ATTRIBUTE_PACKED
GCC_ATTRIBUTE_UNUSED
	 
if test "$GCC" != "yes"; then
	AC_MSG_WARN([Kaffe can only build with GCC])
fi

dnl Find the c++ compiler early on
AC_PROG_CXX

dnl find the assembler
AM_PROG_AS

dnl Find the machine type and setup the relevant links
AC_CANONICAL_HOST
AC_EXEEXT
AC_SYS_LARGEFILE

dnl =========================================================================
dnl By default we use dynamic libraries for everything
dnl -------------------------------------------------------------------------

dynamic_libraries=yes
vm_dynamic_library=yes
vm_dynamic_binary=yes

dnl -------------------------------------------------------------------------

dnl =========================================================================
dnl Force use of static libraries
dnl -------------------------------------------------------------------------

AC_ARG_WITH([staticlib],
            AS_HELP_STRING([--with-staticlib],
                           [Force the use of static libraries rather than shared]))

if test x"$with_staticlib" = x"yes" ; then
	dynamic_libraries=no
	if test x"${with_staticvm+set}" != x"set"; then
		with_staticvm=yes
	fi
fi

AC_ARG_WITH([staticvm],
            AS_HELP_STRING([--with-staticvm],
                           [Force the creation of a static virtual machine]))

if test x"$with_staticvm" = x"yes" ; then
	vm_dynamic_library=no
fi

dnl If the vm is static or all libraries are static then make sure we set
dnl the no-share-vmlibrary flag.
if test x"$vm_dynamic_library" = x"no" || test x"$dynamic_libraries" = x"no"; then
	AC_DEFINE(NO_SHARED_VMLIBRARY, 1, [If the vm is static or all libraries are static])
fi

dnl =========================================================================
dnl Look for the configuration information
dnl -------------------------------------------------------------------------

known=no
Khost_cpu=$host_cpu
AC_SUBST([Khost_cpu])
Khost_os=$host_os
AC_SUBST([Khost_os])
if test -f $srcdir/config/config.alias ; then
	. $srcdir/config/config.alias
fi
if test -f $srcdir/config/$Khost_cpu/$Khost_os/config.frag ; then
	known=yes
	. $srcdir/config/$Khost_cpu/$Khost_os/config.frag
fi
if test x"$known" = x"no" ; then
	AC_MSG_ERROR(Configuration $Khost_cpu-$Khost_os not supported)
fi

if test x"$needs_altstack_end_pointer" = x"yes"; then
	AC_DEFINE(SIGALTSTACK_NEEDS_END, 1, [Defined if sigaltstack needs the stackbase+size pointer instead of stackbase])
fi

dnl =========================================================================

dnl =========================================================================
dnl Configure libtool
dnl -------------------------------------------------------------------------

case "$dynamic_libraries,$vm_dynamic_library" in
no,no) : ${enable_shared=no};;
yes,yes) : ${enable_static=no};;
# otherwise, we have to compile everything twice
esac

AC_LIBTOOL_DLOPEN
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_CHECK_HEADER([ltdl.h],
  [AC_SEARCH_LIBS([lt_dlinterface_register], [ltdl], [], [AC_MSG_ERROR([Can't find the libltdl library.])])],
  [AC_MSG_ERROR([Can't find the ltdl.h header file from the libltdl library])])

## Let users override the linker used by libtool, but default to $CC.
CCLD=${CCLD:-'$(CC)'}
AC_SUBST(CCLD)

AC_LTDL_SHLIBEXT

# ===========================================================================
# Configure gettext (needed for libiconv scripts)
# ===========================================================================

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.14.1)

KAFFE_LOCALEDIR="${datadir}/locale"
eval KAFFE_LOCALEPATH=$KAFFE_LOCALEDIR
AC_DEFINE_UNQUOTED(KAFFE_LOCALEDIR, "$KAFFE_LOCALEPATH", [Full path to the directory containing kaffe's locales])

AM_LC_MESSAGES

AM_CONDITIONAL(MAY_RUN_PO, test x"$MSGFMT" != x":" || test -f $ac_top_srcdir/po/fr.gmo)

dnl =========================================================================
dnl Do we need underscore?
dnl -------------------------------------------------------------------------

AC_LIBTOOL_SYS_GLOBAL_SYMBOL_PIPE
AC_LTDL_SYMBOL_USCORE

if test x"$ac_cv_sys_symbol_underscore" = x"yes"; then
    AC_DEFINE(HAVE_UNDERSCORED_C_NAMES, 1, [Define if C symbol names start with underscore])
fi

dnl =========================================================================
dnl Start of default configurations
dnl -------------------------------------------------------------------------

dnl What kind of system are we using?
NOSIGCONTEXT=false

case $Khost_os in
win32*)
	SYSTEM=win32 ;;
cygwin*)
	SYSTEM=unix;
	NOSIGCONTEXT=true;;
*)
	SYSTEM=unix ;;
esac

dnl -------------------------------------------------------------------------
dnl End of default configurations
dnl =========================================================================

dnl =========================================================================
dnl Set the base set of platform-dependent files 
dnl -------------------------------------------------------------------------

CONFIG_MD_H="config/$Khost_cpu/$Khost_os/md.h"
CONFIG_CALLKAFFEEXCEPTION_H="config/$Khost_cpu/callKaffeException.h"
KAFFEVM_MD_C="config/$Khost_cpu/$Khost_os/md.c"

dnl =========================================================================

dnl =========================================================================
dnl Look for configuration specific Makefile fragement and include it
dnl -------------------------------------------------------------------------

if test -f $srcdir/config/$Khost_cpu/$Khost_os/Make.frag ; then
	cpu_os_frag=$srcdir/config/$Khost_cpu/$Khost_os/Make.frag
else
	cpu_os_frag=/dev/null
fi
if test -f $srcdir/config/$Khost_cpu/Make.frag ; then
	cpu_frag=$srcdir/config/$Khost_cpu/Make.frag
else
	cpu_frag=/dev/null
fi
AC_SUBST_FILE(cpu_os_frag)dnl
AC_SUBST_FILE(cpu_frag)dnl

dnl =========================================================================

dnl =========================================================================
dnl Allow selection of a specific execution engine
dnl -------------------------------------------------------------------------

AC_ARG_WITH([engine],
	    AS_HELP_STRING([--with-engine=ENGINE],
			   [Force use given execution engine (intrp, jit, or jit3)]))

dnl Translate 'interpreter' to 'intrp'
if test x"$with_engine" = x"interpreter" ; then
	with_engine=intrp
fi

dnl Save the requested engine and set the default to 'jit'
want_engine="$with_engine"
if test x"${with_engine+set}" != x"set" ; then
	with_engine="jit"
fi

dnl =========================================================================

dnl -------------------------------------------------------------------------
dnl Check for JIT support and add in the necessary configuration files
dnl -------------------------------------------------------------------------

dnl AC_KAFFE_IF_SUPPORTED_ENGINE(ENGINE, SUP_CMDS, UPDATE_CMDS, UNSUP_CMDS)
dnl If the engine is fully supported, run SUP_CMDS.
dnl If it needs updating, run UPDATE_CMDS.
dnl If it is not supported at all, run UNSUP_CMDS.
AC_DEFUN([AC_KAFFE_IF_SUPPORTED_ENGINE], [
  if test x"$1" = x"intrp"; then
    ifelse([$2],[],[:],[$2])
  else
    if test -f "$srcdir/config/$Khost_cpu/$1-$Khost_cpu.def" && test -f "$srcdir/config/$Khost_cpu/$Khost_os/$1-md.h"; then
      if sed 1q < "$srcdir/config/$Khost_cpu/$Khost_os/$1-md.h" |
	 fgrep "Needs update, do not use" > /dev/null; then
	ifelse([$3],[],[:],[$3])
      else
	ifelse([$2],[],[:],[$2])
      fi
    else
      ifelse([$4],[],[:],[$4])
    fi
  fi
])

dnl If they request a specific engine, we must support it or else fail
dnl Otherwise, pick the most preferable supported engine. Note that
dnl the "intrp" engine is always supported, so we can't fail there.

if test x"${want_engine}" != x""; then
	AC_KAFFE_IF_SUPPORTED_ENGINE([${want_engine}],,
		[AC_MSG_WARN([The $want_engine engine was not updated for $Khost_cpu-$Khost_os])], 
		[AC_MSG_ERROR([Configuration $Khost_cpu-$Khost_os does not support the $want_engine engine])])
	with_engine="${want_engine}"
else
	for engine in jit3 jit intrp; do
		AC_KAFFE_IF_SUPPORTED_ENGINE([${engine}], [
			with_engine="${engine}"
			break;
		])
	done
fi

if test x"$with_engine" != x"intrp" ; then
    CONFIG_JIT_MD_H="config/$Khost_cpu/$Khost_os/$with_engine-md.h"
    KAFFEVM_JIT_DEF="config/$Khost_cpu/$with_engine-$Khost_cpu.def"
    KAFFEVM_ICODE_H="config/$Khost_cpu/$with_engine-icode.h"
fi

AC_MSG_CHECKING(execution engine)
AC_MSG_RESULT($with_engine)
AM_CONDITIONAL(USE_JIT, test x"$with_engine" != x"intrp")

ENGINE_NAME="$with_engine"
AC_SUBST(ENGINE_NAME)

AC_SUBST_FILE(engine_frag)
engine_frag=$srcdir/kaffe/kaffevm/$with_engine/Makefile.frag

dnl =========================================================================
dnl Allow selection of debugging mechanisms
dnl -------------------------------------------------------------------------

AC_ARG_ENABLE([vmdebug],
	      AS_HELP_STRING([--disable-vmdebug],
			     [Disable run-time debugging machinery. It is enabled by default.]),
			     [],
	[enable_vmdebug="yes"])
case "$enable_vmdebug" in
no) CPPFLAGS="$CPPFLAGS -DNDEBUG";;
*) CPPFLAGS="$CPPFLAGS -DKAFFE_VMDEBUG";;
esac

AC_MSG_CHECKING([whether to enable run-time debugging machinery])
AC_MSG_RESULT($enable_vmdebug)

dnl =========================================================================
dnl Allow user to specify a bcel.jar file for class file testing.
dnl -------------------------------------------------------------------------

AC_ARG_WITH([bcel],
              AS_HELP_STRING([--with-bcel=<jar>],
			     [Use the specified Bytecode Engineering Jar file. It is not used by default.]),
  [ BCEL_JAR="$withval" ],
  [ 
BCEL_JAR="" 
with_bcel="no"
])

AC_MSG_CHECKING([whether to use BCEL for additional regression tests])

AC_SUBST(BCEL_JAR)
AM_CONDITIONAL(HAVE_BCEL, test x"$BCEL_JAR" != x"")

AC_MSG_RESULT($with_bcel)

dnl =========================================================================
dnl Allow activation of jvmpi
dnl -------------------------------------------------------------------------

AC_ARG_ENABLE([jvmpi],
              AS_HELP_STRING([--enable-jvmpi],
                             [Enable JVMPI. It is disabled by default.]),
[
case "$enable_jvmpi" in
no) ;;
*) if test "x$NOSIGCONTEXT" = "xtrue"; then
	echo "JVMPI does not support Cygwin"
   else
	AC_DEFINE(ENABLE_JVMPI, 1, [JVM Profiling Interface])
   fi;;
esac
],
	[enable_jvmpi="no"])

AC_MSG_CHECKING([whether to enable JVMPI support])

AM_CONDITIONAL(ENABLE_JVMPI, test x"$enable_jvmpi" != x"no")

AC_MSG_RESULT($enable_jvmpi)

dnl =========================================================================
dnl Allow support for profiling of C/jitted code
dnl -------------------------------------------------------------------------

need_libxprof=no

AC_ARG_ENABLE([xprofiling],
              AS_HELP_STRING([--enable-xprofiling],
                             [Enable profiling for C and jitted code. 
			     It is disabled by default.]),
[
if test "x$NOSIGCONTEXT" = "xtrue"; then
  echo "XProfiling does not support Cygwin"
  enable_xprofiling=no
fi

case "$enable_xprofiling" in
no) xprofiling=no ;;
*)  xprofiling=yes; need_libxprof=yes; CPPFLAGS="$CPPFLAGS -DKAFFE_XPROFILER";;
esac
],
[
enable_xprofiling=no
xprofiling=no
])

AC_MSG_CHECKING([whether to enable profiling for C and jitted code])

dnl ENABLE_XPROF controls installation of xprof scripts in kaffe/scripts/Makefile.am
AM_CONDITIONAL(ENABLE_XPROF, test x"$xprofiling" = x"yes")

if test x"$xprofiling" = x"yes"; then
  if test -f "$srcdir/config/$Khost_cpu/$Khost_os/xprofile-md.h"; then
    CONFIG_XPROFILE_MD_H="config/$Khost_cpu/$Khost_os/xprofile-md.h"
  else
    AC_MSG_ERROR(Profiling not supported in $Khost_cpu-$Khost_os)
  fi
fi

AC_MSG_RESULT($enable_xprofiling)

dnl =========================================================================
dnl Allow support for debugging of C/jitted code
dnl -------------------------------------------------------------------------

AC_ARG_ENABLE([xdebugging],
              AS_HELP_STRING([--enable-xdebugging],
                             [Enable debugging symbol generation for jitted code.
			     It is disabled by default.]),
[
case "$enable_xdebugging" in
no) ;;
*) need_libxprof=yes; 
JAVAC_FLAGS="-g $JAVAC_FLAGS";
AC_DEFINE(KAFFE_XDEBUGGING, 1, [Enable debugging symbol generation for jitted code]);;
esac
],
[
enable_xdebugging="no"
])

AC_MSG_CHECKING([whether to enable debugging symbol generation for jitted code])
AC_SUBST(JAVAC_FLAGS)
AC_MSG_RESULT($enable_xdebugging)

dnl =========================================================================
dnl Allow generation feedback data
dnl -------------------------------------------------------------------------

AC_ARG_ENABLE([feedback],
              AS_HELP_STRING([--enable-feedback],
                             [Enable generation of feedback data]),
[
case "$enable_feedback" in
no) ;;
*) need_libxprof=yes; CPPFLAGS="$CPPFLAGS -DKAFFE_FEEDBACK";;
esac
],
[enable_feedback=no
])

AM_CONDITIONAL(NEED_LIBXPROF, test x"$need_libxprof" = x"yes")

dnl =========================================================================
dnl Allow enabling of profiling
dnl -------------------------------------------------------------------------

AC_ARG_WITH([profiling],
              AS_HELP_STRING([--with-profiling],
                             [Compile with gprof(1) profiling support]))

AC_MSG_CHECKING(gprof(4) profiling support)
if test x"${with_profiling}" = x"yes" ; then
	CFLAGS_PG="-pg"
	CPPFLAGS="$CPPFLAGS -DKAFFE_CPROFILER"
else
	with_profiling=no
fi
AC_MSG_RESULT($with_profiling)

AC_SUBST(CFLAGS_PG)

dnl =========================================================================
dnl Do we include our Sun compatibility scripts?
dnl -------------------------------------------------------------------------

AC_ARG_WITH([suncompat],
              AS_HELP_STRING([--without-suncompat],
                             [Don't include Sun compatibility scripts]))

AC_MSG_CHECKING(sun compatibility mode)
if test x"${with_suncompat+set}" != x"set" ; then
	with_suncompat=yes
fi
AM_CONDITIONAL(SUN_COMPAT, test x"$with_suncompat" = x"yes")
AC_MSG_RESULT($with_suncompat)

dnl =========================================================================
dnl Use the new internal threading system "jthreads"
dnl -------------------------------------------------------------------------

AC_ARG_WITH([threads],
              AS_HELP_STRING([--with-threads=SYSTEM],
                             [Define which threading system to use @<:@default=unix-pthreads@:>@]))

AC_MSG_CHECKING(thread system)
if test x"$with_threads" = x"" ; then
	with_threads=unix-pthreads
fi
if test ! -d $srcdir/kaffe/kaffevm/systems/${with_threads} ; then
	AC_MSG_ERROR(Configuration $Khost_cpu-$Khost_os does not support $with_threads threads)
fi
THREAD_SYSTEM=$with_threads
AC_MSG_RESULT($THREAD_SYSTEM)
AC_SUBST_FILE(threads_frag)dnl
threads_frag=$srcdir/kaffe/kaffevm/systems/$with_threads/Makefile.frag
AC_SUBST(THREAD_DIR)
THREAD_DIR=systems/$with_threads

dnl Check for thread libraries
if test x$with_threads = xunix-pthreads ; then
	ACX_PTHREAD([THREADLIBS="$PTHREAD_LIBS "
		     CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
                     CC="$PTHREAD_CC"])
	OLD_LIBS="$LIBS"
	LIBS="$LIBS $THREADLIBS"
	OLD_CFLAGS="$CFLAGS"
	AC_CHECK_FUNCS([pthread_getattr_np])
	LIBS="$OLD_LIBS"
	CFLAGS="$OLD_CFLAGS"
	AC_CHECK_LIB(semaphore,sem_init,SEMAPHORE_LIB="-lsemaphore")
	AC_SUBST(SEMAPHORE_LIB)
	KAFFE_LIB_SOLARIS_PTHREAD
	KAFFE_CHECK_SEMAPHORE

	if test -z "$THREADLIBS"; then
		AC_MSG_ERROR([Missing library libpthread, read FAQ/FAQ.pthreads])
	fi

	VMLIBS="$VM_LIBS $PTHREAD_LIBS"
	CPPFLAGS="$CPPFLAGS -D_REENTRANT"

	if test "x$Khost_os" = "xnetbsd1"; then
		AC_DEFINE(KAFFE_BUGGY_NETBSD_SIGWAIT, 1, [Defined on NetBSD because sigwait needs the handle not to be registered])
	fi
 else
        CPPFLAGS="$CPPFLAGS -DBR_PTHREADS=0"
fi

if test x$with_threads = xunix-pthreads ; then
	AC_DEFINE([KAFFE_SYSTEM_UNIX_PTHREADS], 1, [Defined if we are using the unix-pthreads threading system])
fi

AC_SUBST(VM_LIBS)

AC_CHECK_WITH_CLASSPATH_PREFIX
AC_CHECK_WITH_CLASSPATH_CLASSES
AC_CHECK_WITH_TARGET_CLASSPATH_CLASSES
AC_CHECK_WITH_CLASSPATH_LIBDIR
AC_CHECK_WITH_CLASSPATH_INCLUDEDIR

AC_MSG_CHECKING([if class library exists and is readable at ${CLASSPATH_CLASSES}])
if test -r ${CLASSPATH_CLASSES} ; then
   AC_MSG_RESULT([yes])
else 
   AC_MSG_RESULT([no])
   AC_MSG_ERROR([Couldn't find readable class library at ${CLASSPATH_CLASSES}])
fi
   
GLIBJ_ZIP=${CLASSPATH_CLASSES}
AC_SUBST(GLIBJ_ZIP)

JAVACFLAGS="-classpath ${CLASSPATH_CLASSES}"
AC_PROG_JAVAC
AC_PROG_JAVADOC 
AC_PATH_PROGS([JAVAP],[javap-cp javap])

dnl =========================================================================
dnl Look for trampoline code if in JIT mode
dnl -------------------------------------------------------------------------
KAFFEVM_TRAMPOLINES_S=
if test x"$with_engine" != x"intrp" ; then
	AC_MSG_CHECKING(for trampolines)
	if test -f $srcdir/config/$Khost_cpu/trampolines.S ; then
		KAFFEVM_TRAMPOLINES_S="config/$Khost_cpu/trampolines.S"
		AC_MSG_RESULT(yes)
	else
		AC_MSG_ERROR(does not support trampolines - now mandatory)
	fi
fi

dnl =========================================================================
dnl Set install root directory which prefixes all install target dirs
dnl -------------------------------------------------------------------------

if test x"$with_installroot" != x""; then
    AC_MSG_ERROR([--with-installroot is deprecated, use `make install DESTDIR=...'])
fi

dnl =========================================================================

dnl =========================================================================
dnl Make the necessary substitutions
dnl -------------------------------------------------------------------------

AC_SUBST(JAVA_LIBS)
AC_SUBST(with_engine)
AC_SUBST(THREAD_SYSTEM)
if test x"$program_prefix" = x"NONE" ; then
	KPREFIX=""
else
	KPREFIX=$program_prefix
fi
AC_SUBST(KPREFIX)
KAFFE_ARCHOS="$Khost_cpu-$Khost_os"
AC_DEFINE_UNQUOTED(ARCHOS, "$KAFFE_ARCHOS", [Define the version we're compiling for])
AC_SUBST(KAFFE_ARCHOS)

dnl =========================================================================

dnl =========================================================================
dnl Checks for programs.
dnl -------------------------------------------------------------------------

AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_SED
AC_CHECK_PROG(MKTEMP, mktemp, mktemp)
if test  x"$MKTEMP" = x"" ; then
   AC_MSG_WARN([mktemp not found. You need mktemp to be able to debug kaffe.])
fi
AC_CHECK_PROG(JASMIN, jasmin, jasmin)

AM_CONDITIONAL(HAVE_JAVAC, test x"$JAVAC" != x"")
AM_CONDITIONAL(HAVE_MKTEMP, test x"$MKTEMP" != x"")
AM_CONDITIONAL(HAVE_JASMIN, test x"$JASMIN" != x"")

dnl If symlink is overridden then don't bother with the test.
if test x"$LN_S" = x"" ; then
	AC_PROG_LN_S
fi
dnl This shell macro caches uses of ln -s in the generated script and
dnl converts them to LN_S
ln() {
	if test x"$1" = x"-s" ; then
		$LN_S $2 $3
	else
		ln $1 $2
	fi
}

dnl Check for AWK. Used by kaffexprof
AC_PROG_AWK

dnl =========================================================================
dnl Checks for types
dnl -------------------------------------------------------------------------

AC_CHECK_SIZEOF(short,0)
AC_CHECK_SIZEOF(int,0)
AC_CHECK_SIZEOF(long,0)
AC_CHECK_SIZEOF(long long,0)
AC_CHECK_SIZEOF(__int64,0)
AC_CHECK_SIZEOF(void *,0)

AC_TYPE_OFF_T
AC_TYPE_PID_T

dnl -------------------------------------------------------------------------

AC_TYPE_SIGNAL

AC_CACHE_CHECK([whether signal handlers are one-shot],
	kaffe_cv_signal_one_shot, [ dnl
	AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <signal.h>
int counter = 0;
RETSIGTYPE hup_handler () { ++counter; }
int get_counter () { return counter; }
int main() {
  signal(1, hup_handler);
  kill(getpid(), 1);
  if (get_counter() == 0)
    exit(1); /* signal was not delivered, can't test reliably */
  kill(getpid(), 1);
  if (get_counter() != 2)
    exit(1); /* signal was not delivered, seems to be one-shot */
  exit(0);
}
]])],[kaffe_cv_signal_one_shot=no],[kaffe_cv_signal_one_shot=yes],[kaffe_cv_signal_one_shot=cross])
])
if test x"$kaffe_cv_signal_one_shot" != x"no"; then
  AC_DEFINE(KAFFE_SIGNAL_ONE_SHOT, 1, [Define if signal handlers must be reset upon delivery])
fi

AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

dnl -------------------------------------------------------------------------

AC_DEFUN([AC_CHECK_LIBRARY], [dnl
KSAVE_LIBS="$LIBS"
LIBS="$$3 $KSAVE_LIBS"
AC_CHECK_LIB($1,$2)
$3=`echo "$LIBS" | sed "s% $KSAVE_LIBS$%%"`
LIBS="$KSAVE_LIBS"
])

dnl =========================================================================
dnl Checks for libraries
dnl -------------------------------------------------------------------------

AC_PROG_GCC_TRADITIONAL

dnl Libraries for programs such as Kaffe and kaffeh

KAFFE_LIBS="$KAFFE_LIBS"
AC_SUBST(KAFFE_LIBS)

dnl Checks for general math libraries

AC_CHECK_LIBRARY(mw,_mwvalidcheckl,M_LIBS)
AC_CHECK_LIBRARY(m,sin,M_LIBS)

AC_SUBST(M_LIBS)

dnl If there is no native sysdepCallMethod, let's use libffi

AC_ARG_WITH([libffi],
	    AS_HELP_STRING([--with-libffi],
			   [Use libffi for sysdepCallMethod]))

if test x"$with_libffi" = x"yes" ; then
	AC_CHECK_LIBRARY(ffi,ffi_call,VM_LIBS)
	if test $ac_cv_lib_ffi_ffi_call = no; then
		AC_MSG_ERROR([Missing library libffi, read FAQ/FAQ.libffi])
	fi
fi

dnl Checks for libraries for vm library.
dnl Include work around FreeBSD modulo bug.

case "$Khost_os" in
freebsd2|oskit)
	AC_CHECK_LIBRARY(gcc,__moddi3,VM_LIBS);;
esac

dnl -------------------------------------------------------------------------

dnl =========================================================================
dnl Checks for header files.
dnl -------------------------------------------------------------------------

AC_HEADER_STDC

AC_CHECK_HEADERS_ONCE([stdarg.h \
alloca.h \
bsd/libc.h \
dlfcn.h \
fcntl.h \
features.h \
gmp.h \
gc/gc.h \
jpeglib.h \
kernel/OS.h \
limits.h \
mach/mach.h \
mach-o/rld.h \
malloc.h \
memory.h \
mips/cachectl.h \
png.h \
poll.h \
pthread.h \
pwd.h \
sgidefs.h \
sigcontext.h \
signal.h \
stdlib.h \
string.h \
sys/cachectl.h \
sys/filio.h \
sys/ioctl.h \
sys/mman.h \
sys/param.h \
sys/poll.h \
sys/select.h \
sys/signal.h \
sys/socket.h \
sys/stat.h \
sys/sysinfo.h \
sys/time.h \
sys/types.h \
sys/utsname.h \
sys/ucontext.h \
unistd.h \
wintypes.h])

# check sys/sysctl.h seperately, as it requires other headers on at least OpenBSD
AC_CHECK_HEADERS([sys/sysctl.h], [], [],
[[#if HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
]])

dnl GNU/Linux sigcontext headers
AC_CHECK_HEADERS_ONCE([asm/sigcontext.h])
AC_CHECK_HEADERS([asm/signal.h], [], [], [
#include <linux/types.h>
])

dnl needed for common ppc asm syntax on GNU/Linux
AC_CHECK_HEADERS_ONCE([asm/ppc_asm.h])

dnl needed for MIPS cache flushing on GNU/Linux
AC_CHECK_HEADERS_ONCE([asm/cachectl.h])

dnl needed for ia64
AC_CHECK_HEADERS_ONCE([ia64intrin.h])

AC_HEADER_SYS_WAIT
AC_HEADER_DIRENT

dnl We need both getrusage and the header containing its decls.
AC_CHECK_HEADERS_ONCE([sys/resource.h])

dnl Check whether there exists mprotect. If it exists and we are
dnl on HPUX, then prevent kaffe from using it when the pointer does not
dnl come from mmap.
AC_CHECK_FUNCS_ONCE(mprotect)
if test x"$ac_cv_func_mprotect" = x"yes"; then
   case "$Khost_os" in
	hpux*)
		;;
	*) AC_DEFINE(HAVE_COMPATIBLE_MPROTECT, 1, [Does mprotect support protecting region not covered by mmap ?])
		;;
   esac
fi

dnl needed for valgrind
AC_CHECK_HEADERS_ONCE([valgrind/memcheck.h])

gl_SIZE_MAX

dnl =========================================================================
dnl Select GC to use
dnl -------------------------------------------------------------------------

AC_ARG_WITH([gc],
	    AS_HELP_STRING([--with-gc=GC],
			   [Force use given execution engine (kaffe-gc or boehm-gc)]),
	    [ GC_NAME="$withval" ]
)


if test x$GC_NAME = xboehm-gc ; then
   if test x"$with_threads" != x"unix-pthreads"; then
      AC_MSG_ERROR([boehm-gc only supports unix-pthreads threads. Please switch to unix-pthreads using --with-threads=unix-pthreads or use kaffe-gc.])
   fi

   if test x$ac_cv_header_gc_gc_h = xno ; then
      AC_MSG_ERROR([boehm-gc headers not found.])
   fi

   AC_CHECK_LIBRARY([gc],[GC_free],[VM_LIBS])

   if test x$ac_cv_lib_gc_GC_free = xno; then
      AC_MSG_ERROR([boehm-gc library not found.])
   fi

   AC_DEFINE([KAFFE_BOEHM_GC], 1, [Defined if we are using the Boehm-Weiser garbage collector])
fi

KAFFE_CHECK_GLIBC_VER

AC_MSG_CHECKING(which garbage collector to use)

dnl Save the requested gc and set the default to 'kaffe-gc'
if test x"${GC_NAME+set}" != x"set" ; then
	GC_NAME="kaffe-gc"
fi

gc_frag=$srcdir/kaffe/kaffevm/$GC_NAME/Makefile.frag
AC_SUBST(GC_NAME)
AC_SUBST_FILE(gc_frag)
AC_MSG_RESULT($GC_NAME)

dnl =========================================================================
dnl Do we build a vm that collects statistics?
dnl -------------------------------------------------------------------------

AC_ARG_WITH([stats],
	    AS_HELP_STRING([--with-stats],
			   [Gather statistics on vm execution]))

if test x"$with_stats" = x"yes" ; then
	if test x"$have_rusage" = x"no" ; then
		echo "Need rusage to build stats version"
	else
		AC_DEFINE(KAFFE_STATS, 1, [Do we enable stats in the vm?])
	fi
fi

dnl -------------------------------------------------------------------------

dnl =========================================================================
dnl Do we enable GCJ support
dnl -------------------------------------------------------------------------

if test "x$enable_gcj" = "xyes"; then
  enable_gcj=no # pessimist

  have_recent_gcc=yes # optimist
  AC_CHECK_FUNCS([__throw __terminate_func __get_eh_info], [],
		 [have_recent_gcc=no])
  pushdef([AC_MSG_ERROR], [have_recent_gcc=no])
  AC_PROG_CXX
  popdef([AC_MSG_ERROR])

  if test "x$have_recent_gcc" = xyes; then
    AC_LANG_PUSH([C++])
    AC_CHECK_HEADER([java/lang/Class.h],
      [have_libgcj_headers=yes], [have_libgcj_headers=no])
    AC_LANG_POP([])

    if test "x$have_libgcj_headers" = xyes; then
      enable_gcj=yes
      AC_DEFINE([HAVE_GCJ_SUPPORT], 1, [Enable GCJ support. ])
      CFLAGS="$CFLAGS -fexceptions"
    fi
  fi
fi

AC_ARG_ENABLE([gcj],
	      AS_HELP_STRING([--enable-gcj],
			     [Enable GCJ support]))

AC_MSG_CHECKING([whether to enable GCJ support])
AC_MSG_RESULT($enable_gcj)
AM_CONDITIONAL(HAVE_GCJ_SUPPORT, test x"$enable_gcj" = x"yes")

dnl -------------------------------------------------------------------------

dnl =========================================================================
dnl Do we disable MIPS LL/SC (MIPS II instructions)
dnl -------------------------------------------------------------------------

AC_ARG_ENABLE([mips2],
              AS_HELP_STRING([--disable-mips2],
                             [Disable MIPS II instructions]))

AC_MSG_CHECKING([whether to enable MIPS II instructions])
if test x"$enable_mips2" != x"no" -a x"$Khost_cpu" = x"mips"; then
    AC_DEFINE([HAVE_MIPSII_INSTRUCTIONS], 1, [Enable MIPS II instructions])
    enable_mips2=yes
    case "$Khost_os" in
	linux*)
		CFLAGS="$CFLAGS -mips2"
		;;
    esac
fi
AC_MSG_RESULT($enable_mips2)

dnl -------------------------------------------------------------------------

dnl =========================================================================
dnl Do we enable XScale
dnl -------------------------------------------------------------------------

AC_ARG_ENABLE([xscale],
              AS_HELP_STRING([--enable-xscale],
                             [Enable XScale support]))

AC_MSG_CHECKING([whether to enable XScale support])
if test x"$enable_xscale" != x"no"; then
    AC_DEFINE([HAVE_XSCALE], 1, [Enable XScale support])
fi
AC_MSG_RESULT($enable_xscale)

dnl -------------------------------------------------------------------------

dnl =========================================================================
dnl Checks for typedefs, structures, and compiler characteristics.
dnl -------------------------------------------------------------------------

AC_C_BIGENDIAN_CROSS
AC_C_BIGENDIAN
AC_C_CHAR_UNSIGNED
AC_C_CONST
AC_C_INLINE

dnl =========================================================================
dnl Check for struct sigcontext.
dnl -------------------------------------------------------------------------

AC_CACHE_CHECK([for struct sigcontext],
  ac_cv_struct_sigcontext,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <asm/signal.h>
#include <asm/sigcontext.h>]], [[struct sigcontext t;]])],[ac_cv_struct_sigcontext=yes],[ac_cv_struct_sigcontext=no])])
if test $ac_cv_struct_sigcontext = yes; then
  AC_DEFINE(HAVE_STRUCT_SIGCONTEXT, 1, [Do we have sigcontext])
fi

dnl Check for struct sigcontext_struct.
dnl Just to complicate things there are two different sets of assembly files
dnl we must check which are mutually incompatible.

AC_CACHE_CHECK([for struct sigcontext_struct (without asm/sigcontext.h)],
  ac_cv_struct_sigcontext_struct,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <asm/signal.h>]], [[struct sigcontext_struct t;]])],[ac_cv_struct_sigcontext_struct=yes],[ac_cv_struct_sigcontext_struct=no])])

AC_CACHE_CHECK([for struct sigcontext_struct (with asm/sigcontext.h)],
  ac_cv_struct_sigcontext_struct_with,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <asm/signal.h>
#include <asm/sigcontext.h>]], [[struct sigcontext_struct t;]])],[ac_cv_struct_sigcontext_struct_with=yes],[ac_cv_struct_sigcontext_struct_with=no])])

if test $ac_cv_struct_sigcontext_struct = yes -o $ac_cv_struct_sigcontext_struct_with = yes; then
  AC_DEFINE(HAVE_STRUCT_SIGCONTEXT_STRUCT, 1, [Do we have sigcontext_struct])
fi

dnl -------------------------------------------------------------------------

dnl =========================================================================
dnl Checks for library functions.
dnl -------------------------------------------------------------------------

AC_FUNC_MEMCMP
AC_FUNC_MMAP

AC_CHECK_FUNCS_ONCE([getpagesize memcpy memmove 
getcwd getwd gettimeofday ftime time uname getuid get_nprocs_conf])

# sched_get_priority_* functions are not implemented on OpenBSD <= 3.4
# at least. setting pthread priorities on OpenBSD has unexpected results,
# so don't enable setting them for OpenBSD.
if test x"$Khost_os" != x"openbsd2" ; then
  AC_CHECK_FUNCS([sched_get_priority_max])
  AC_CHECK_FUNCS([sched_get_priority_min])
  AC_EGREP_HEADER(Found_SCHED_OTHER,
    [
#include <sched.h>
#ifdef SCHED_OTHER
Found_SCHED_OTHER
#endif
    ],
    [ AC_DEFINE(HAVE_SCHED_OTHER_IN_SCHED, 1, [Defined if SCHED_OTHER is in sched.h])
  ])
fi

dnl pthread_attr_setschedpolicy in not implemented on NetBSD <= 1.6
dnl pthread_yield is not implemented on any systems.
AC_CHECK_FUNCS_ONCE([pthread_attr_setschedpolicy pthread_yield sched_yield])
AC_CHECK_HEADERS_ONCE(sched.h)

KSAVE_LIBS="$LIBS"
LIBS="$M_LIBS $LIBS"
AC_CHECK_FUNCS([fmod fmodf \
floor ceil strtouq strtoull])

dnl isinf, isnan checking code taken from Sablotron 1.0,
dnl dually licensed licensed under MPL 1.1 and GPL
dnl available from http://www.gingerall.cz/charlie/ga/xml/p_sab.xml

dnl check for isnan etc...in the std. way and try link if not found
dnl this is due HPUX, where isnan & Co. are defined as macros

AC_CHECK_FUNC( isnan, [has_isnan=1; AC_DEFINE(HAVE_ISNAN, 1, isnan)], has_isnan=0)
AC_CHECK_FUNC( isinf, [has_isinf=1; AC_DEFINE(HAVE_ISINF, 1, isinf)], has_isinf=0)

dnl check for macro isnan in math
if test $has_isnan = 0; then
  AC_MSG_CHECKING(for macro isnan)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <math.h>]], [[{ int foo; foo = isnan(0); }]])],[has_isnan=1],[has_isnan=0 ])
  if test $has_isnan = 1; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_ISNAN, 1, isnan)
  else
    AC_MSG_RESULT(no)
  fi
fi

dnl check for macro isinf in math
if test $has_isinf = 0; then
  AC_MSG_CHECKING(for macro isinf)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <math.h>]], [[{ int foo; foo = isinf(0); }]])],[has_isinf=1],[has_isinf=0 ])
  if test $has_isinf = 1; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_ISINF, 1, isinf)
  else
    AC_MSG_RESULT(no)
  fi
fi

LIBS="$KSAVE_LIBS"

AC_CHECK_FUNCS_ONCE([strerror alarm setitimer
sigprocmask sigsetmask sigemptyset sigaddset signal sigaction
sbrk valloc memalign mallopt getrlimit setrlimit sigaltstack
atexit on_exit vsnprintf])

if test x"$Khost_cpu" = x"alpha" ; then
  AC_CACHE_CHECK([for alpha support of amask instruction],
    ac_cv_alpha_asm_amask,
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[ long r; __asm__("amask 1,%0" : "=r"(r)); return !r; ]])],[ac_cv_alpha_asm_amask=yes],[ac_cv_alpha_asm_amask=no])
  ])
  if test x"$ac_cv_alpha_asm_amask" = x"yes" ; then
    AC_DEFINE(HAVE_ALPHA_ASM_AMASK, 1, [Do we have amask instruction on alpha])
  fi
fi

dnl check if va_list is an array (powerpc-linux, s390-linux)

AC_TYPE_VA_LIST

dnl check for different methods of copying va_lists
dnl some platforms (s390-linux) define va_lists as arrays,
dnl so we can't just copy them by assignment.

AC_FUNC_VA_COPY
AC_FUNC__VA_COPY

dnl -------------------------------------------------------------------------

dnl =========================================================================
dnl Work out if swab is declared
dnl -------------------------------------------------------------------------

AC_CACHE_CHECK([for declaration of swab],
  ac_cv_declared_swab,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STRING_H /* FreeBSD puts proto in string.h */
#include <string.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
]], [[ void *foo = swab; ]])],[ac_cv_declared_swab=yes],[ac_cv_declared_swab=no])
])
if test x"$ac_cv_declared_swab" = x"yes"; then
  AC_DEFINE(HAVE_DECLARED_SWAB, 1, [Is function swab declared])
fi

dnl ------------------------------------------------------------
dnl Check for old linux threads (glibc <= 2.2)
dnl ------------------------------------------------------------

if test "$ac_cv_c_compile_value_glibc" -le "2"; then
	if test "$ac_cv_c_compile_value_glibc_minor" -le "2"; then
		AC_DEFINE([OLD_LINUXTHREADS],1, [Is set if the current GLIBC is using the old Linuxthreads library])
	fi
fi

dnl ------------------------------------------------------------
dnl Check for decalaration of scalb. Second parameter can be 
dnl either int or double on some systemns, and that breaks
dnl compilation of some code on them.
dnl ------------------------------------------------------------

AC_CACHE_CHECK([for type of second parameter of scalb being int],
               [ac_cv_scalb_second_parameter_type_is_int],
               [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <math.h>]],
                                                   [[double scalb (double, int);]])],
               [ac_cv_scalb_second_parameter_type_is_int=yes],
               [ac_cv_scalb_second_parameter_type_is_int=no])])

if test $ac_cv_scalb_second_parameter_type_is_int = yes; then
  AC_DEFINE(_SCALB_INT, 1, [Is second parameter of scalb an int])
fi

# Make sure that we get a zziplib instance with zzip_fstat.
PKG_CHECK_MODULES([ZZIP],[zziplib >= 0.13.24])

PKG_CHECK_MODULES([GLIB],[glib-2.0 >= 2.8.6])

AC_ARG_ENABLE([zlib_zip],
              AS_HELP_STRING([--enable-zlib-zip],
                             [Enable java.util.zip implementation using zlib]),
                             [], [enable_zlib_zip="no"])

AM_CONDITIONAL([COND_ZLIB_ZIP], [test x"$enable_zlib_zip" = x"yes"])

dnl -----------------------------------------------------------------------
dnl                    Memory debugging support
dnl -----------------------------------------------------------------------

AM_WITH_DMALLOC

if test x"$enable_zlib_zip" != x"no"; then
  JAVA_LIBS="\$(top_builddir)/libraries/clib/zip/libzip.la $JAVA_LIBS";
fi

DLOPEN_JAVA_LIBS=
for lib in $JAVA_LIBS; do
  DLOPEN_JAVA_LIBS="$DLOPEN_JAVA_LIBS -dlopen $lib"
done
AC_SUBST(DLOPEN_JAVA_LIBS)


if test x"$dynamic_libraries" = x"no"; then
	KLIBFLAGS=-static
else
	KLIBFLAGS=
fi
AC_SUBST(KLIBFLAGS)

if test x"$vm_dynamic_binary" = x"no"; then
	KVMBINFLAGS=-all-static
else
	KVMBINFLAGS=
fi
AC_SUBST(KVMBINFLAGS)

if test x"$vm_dynamic_library" = x"no"; then
	KVMLIBFLAGS=-static
else
	KVMLIBFLAGS=
fi
AC_SUBST(KVMLIBFLAGS)

dnl -----------------------------------------------------------
dnl Enable direct threading 
dnl -----------------------------------------------------------
AC_ARG_ENABLE([direct-threading],
		AS_HELP_STRING([--disable-direct-threading],
				[Use switched interpreter instead of direct-threading]),
		[],
		[enable_direct_threading=yes])

AM_CONDITIONAL([COND_DIRECT_THREADING], [test x"$enable_direct_threading" = x"yes"])


dnl =========================================================================
dnl Set up for local build of kaffeh when cross-compiling
dnl -------------------------------------------------------------------------

if test x"$cross_compiling" = x"yes"; then
  dnl check for variables that, if not defined in config.frag or config.cache,
  dnl may take wrong values
  if test x"${ac_cv_c_bigendian+set}" != x"set"; then
    AC_MSG_WARN([when cross compiling, you may want to set ac_cv_c_bigendian to yes or no])
  fi
  if test x"${ac_cv_c_char_unsigned}" != x"set"; then
    AC_MSG_WARN([when cross compiling, you may want to set ac_cv_c_char_unsigned to yes or no])
  fi
  AC_PATH_PROG(KAFFEH, kaffeh)
  if test -z "${KAFFEH}"; then
    AC_MSG_ERROR([please set KAFFEH to the full pathname of a locally executable kaffeh])
  fi
  MAKE_KAFFEH=":"
else
  KAFFEH="\$(top_builddir)/kaffe/kaffeh/kaffeh\$(EXEEXT)"
  MAKE_KAFFEH="cd \$(top_builddir)/kaffe/kaffeh && \$(MAKE)"
fi

dnl KAFFEH is AC_SUBSTed by AC_PATH_PROG
AC_SUBST(MAKE_KAFFEH)

# set absolute builddir path

kaffe_builddir=`pwd`
AC_SUBST([kaffe_builddir])

# detect fastjar and set FASTJAR to its path

AC_PATH_PROGS([FASTJAR], [fastjar gjar-4.2 gjar-4.1 gjar jar])

if test -z "${FASTJAR}"; then
  AC_MSG_ERROR([No JAR tool found. Please install fastjar or jar.])
fi

AC_SUBST([FASTJAR])

dnl =========================================================================
dnl Define directory to put native library support
dnl -------------------------------------------------------------------------

# We do this hear to because we need prefixes set.
test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'

toolslibdir=$prefix/lib
AC_SUBST(toolslibdir)

jredir=$prefix/jre
AC_SUBST(jredir)

jrebindir=$jredir/bin
AC_SUBST(jrebindir)

jrelibdir=$jredir/lib
AC_SUBST(jrelibdir)

nativedir=$jredir/lib/$Khost_cpu
AC_SUBST(nativedir)

dnl =========================================================================
dnl Additional directories for jni library path
dnl -------------------------------------------------------------------------

AC_ARG_WITH([jni_library_path],
		AS_HELP_STRING([--with-jni-library-path=dir],
				[additional directories to search for native jni libraries]))

if test x"$with_jni_library_path" != "x"; then
	AC_DEFINE_UNQUOTED(JNI_LIBRARY_PATH, "$with_jni_library_path",
				[additional directories to search for native jni libraries])
fi

dnl =========================================================================
dnl Define kaffehome.
dnl -------------------------------------------------------------------------

AC_DEFINE_UNQUOTED(DEFAULT_KAFFEHOME, "$jrelibdir", [Define default KAFFEHOME])

dnl -------------------------------------------------------------------------

dnl =========================================================================
dnl Extract path and dir separator information from paths.h
dnl -------------------------------------------------------------------------

AC_PROG_CPP

AC_CACHE_CHECK([for path separator],ac_cv_kaffe_pathsep,
[	rm -f conftest.c
	cat ${srcdir}/include/system.h > conftest.c
	echo path_separator >> conftest.c
	eval ac_cv_kaffe_pathsep=`$CPP $CPPFLAGS conftest.c 2>/dev/null| sed '$p; d'`
	rm -f conftest.c
])
PATHSEP=$ac_cv_kaffe_pathsep
AC_SUBST(PATHSEP)

AC_CACHE_CHECK([for directory separator],ac_cv_kaffe_dirsep, 
[	rm -f conftest.c
	cat ${srcdir}/include/system.h > conftest.c
	echo file_separator >> conftest.c
	eval ac_cv_kaffe_dirsep=`$CPP $CPPFLAGS conftest.c 2>/dev/null| sed '$p; d'`
	rm -f conftest.c
])
DIRSEP=`echo "$ac_cv_kaffe_dirsep" | sed 's,\\\\,\\\\\\\\,g'`
AC_SUBST(DIRSEP)

dnl Set up program transform names
kaffe_TRANSF=`echo kaffe | sed ${program_transform_name}`
kaffeh_TRANSF=`echo kaffeh | sed "s/${EXEEXT}$//" | sed ${program_transform_name}`
kaffebin_TRANSF=`echo kaffe-bin | sed "s/${EXEEXT}$//" | sed ${program_transform_name}`
AC_SUBST(kaffe_TRANSF)
AC_SUBST(kaffeh_TRANSF)
AC_SUBST(kaffebin_TRANSF)

dnl These are not to be used during configuration, only during build.
CPPFLAGS="-I$abssrcdir/kaffe/kaffevm -I$abssrcdir/kaffe/kaffevm/$with_engine -I$abssrcdir/kaffe/kaffevm/systems/$with_threads -I\$(top_srcdir)/replace -I\$(top_srcdir)/config -I\$(top_srcdir)/include $CPPFLAGS"
LIBS="$LIBS $LTLIBS"

dnl Avoid GNU Make $(filter-out -pg,@CFLAGS@) in kaffe/xprof/Makefile.am
CFLAGS_WITHOUT_PG=""
for ac_option in $CFLAGS; do
    if test x"$ac_option" = x"-pg"; then :;
    else
        CFLAGS_WITHOUT_PG="$CFLAGS_WITHOUT_PG $ac_option"
    fi
done
AC_SUBST(CFLAGS_WITHOUT_PG)

dnl Replace KAFFE_CCASFLAGS by the generated one
AC_SUBST(KAFFE_CCASFLAGS)

AC_CONFIG_HEADERS([config/config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([project.xml project.properties])
AC_CONFIG_FILES([config/Makefile])
AC_CONFIG_FILES([include/Makefile])
AC_CONFIG_FILES([replace/Makefile])
AC_CONFIG_FILES([kaffe/Makefile])
AC_CONFIG_FILES([libraries/Makefile])
AC_CONFIG_FILES([kaffe/jvmpi/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/intrp/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/jit/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/jit3/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/gcj/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/systems/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/systems/unix-jthreads/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/systems/oskit-pthreads/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/systems/drops-l4threads/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/systems/beos-native/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/systems/unix-pthreads/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/kaffe-gc/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/boehm-gc/Makefile])
AC_CONFIG_FILES([kaffe/kaffe/Makefile])
AC_CONFIG_FILES([kaffe/kaffeh/Makefile])
AC_CONFIG_FILES([kaffe/xprof/Makefile])
AC_CONFIG_FILES([kaffe/man/Makefile])
AC_CONFIG_FILES([kaffe/scripts/Makefile])
AC_CONFIG_FILES([kaffe/scripts/bat/Makefile])
AC_CONFIG_FILES([kaffe/scripts/compat/Makefile])
AC_CONFIG_FILES([libraries/javalib/Makefile])
AC_CONFIG_FILES([libraries/javalib/zlib-zip/Makefile])
AC_CONFIG_FILES([libraries/javalib/vmspecific/Makefile])
AC_CONFIG_FILES([libraries/clib/Makefile])
AC_CONFIG_FILES([libraries/clib/zip/Makefile])
AC_CONFIG_FILES([kaffe/man/kaffe.1])
AC_CONFIG_FILES([kaffe/scripts/install-jar])
AC_CONFIG_FILES([kaffe/scripts/kaffe])
AC_CONFIG_FILES([kaffe/scripts/kaffexprof])
AC_CONFIG_FILES([kaffe/scripts/javah])
AC_CONFIG_FILES([kaffe/scripts/jdb])
AC_CONFIG_FILES([kaffe/scripts/compat/java])
AC_CONFIG_FILES([kaffe/scripts/compat/javac])
AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([test/internal/Makefile])
AC_CONFIG_FILES([test/regression/Makefile])
AC_CONFIG_FILES([test/regression/TestScript])
AC_CONFIG_FILES([test/regression/compile_time/Makefile])
AC_CONFIG_FILES([test/regression/compiler/Makefile])
AC_CONFIG_FILES([test/regression/run_time/Makefile])
AC_CONFIG_FILES([test/jni/Makefile])

if test x"$with_engine" != x"intrp" ; then
  AC_CONFIG_LINKS([config/jit-md.h:$CONFIG_JIT_MD_H])
  AC_CONFIG_LINKS([config/callKaffeException.h:$CONFIG_CALLKAFFEEXCEPTION_H])
  AC_CONFIG_LINKS([kaffe/kaffevm/$with_engine/icode.h:$KAFFEVM_ICODE_H])
  AC_CONFIG_LINKS([kaffe/kaffevm/$with_engine/jit.def:$KAFFEVM_JIT_DEF])
  AC_CONFIG_LINKS([kaffe/kaffevm/$with_engine/trampolines.S:$KAFFEVM_TRAMPOLINES_S])
fi

if test x"$xprofiling" = x"yes"; then
  AC_CONFIG_LINKS([config/xprofile-md.h:$CONFIG_XPROFILE_MD_H])
fi

AC_CONFIG_LINKS([config/md.h:$CONFIG_MD_H])
AC_CONFIG_LINKS([kaffe/kaffevm/md.c:$KAFFEVM_MD_C])

AC_OUTPUT
