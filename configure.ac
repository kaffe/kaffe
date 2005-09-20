AC_INIT([Kaffe Virtual Machine],[1.1.6],[kaffe@kaffe.org],[kaffe])

AC_PREREQ(2.59)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_SRCDIR([kaffe])

# Copyright information. The nice sounding paragraphs about
# GPLed portions has been taken from the GPL-d configure.ac
# file of gimp-print project

AC_COPYRIGHT([Copyright 1996 - 2005 Kaffe.org project contributors. All rights reserved.
Portions Copyright 1996 - 2002 Transvirtual Technologies, Inc.
Portions Copyright by various third parties, see file THIRDPARTY for details.

This file is part of Kaffe Virtual Machine, a free software runtime
for java applications. See http://www.kaffe.org for details.

Parts of this configure script come from the `configure.in' file, and
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

AM_INIT_AUTOMAKE([1.9.5 tar-ustar])

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
AX_CFLAGS_GCC_OPTION([-mregnames])

GCC_ATTRIBUTE_ALWAYS_INLINE
GCC_ATTRIBUTE_CONST
GCC_ATTRIBUTE_FORMAT
GCC_ATTRIBUTE_NORETURN
GCC_ATTRIBUTE_PACKED
GCC_ATTRIBUTE_UNUSED
	 
if test "$GCC" != "yes"; then
	AC_MSG_ERROR([Kaffe can only build with GCC])
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

AC_ARG_WITH([staticbin],
            AS_HELP_STRING([--with-staticbin],
                           [Force the creation of a static virtual machine binary]))

if test x"$with_staticbin" = x"yes" ; then
	vm_dynamic_binary=no
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
Khost_os=$host_os
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
AC_LIBLTDL_CONVENIENCE
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)
AC_SUBST(LIBLTDL)
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
dnl This is a hack to ease the generation of forward-sources, i.e.,
dnl sources that just #include others, because automake can't use
dnl sources from other directories
dnl -------------------------------------------------------------------------

AC_SUBST(REGEN_FORWARD)
REGEN_FORWARD="\$(top_srcdir)/config/regen-forward"

dnl =========================================================================
dnl Set the base set of platform-dependent files 
dnl One-liners with #include directives will be created in the Makefiles
dnl -------------------------------------------------------------------------

CONFIG_MD_H="\$(srcdir)/$Khost_cpu/$Khost_os/md.h"
CONFIG_CALLKAFFEEXCEPTION_H="\$(top_srcdir)/config/$Khost_cpu/callKaffeException.h"
KAFFEVM_MD_C="\$(top_srcdir)/config/$Khost_cpu/$Khost_os/md.c"
AC_SUBST(CONFIG_MD_H)
AC_SUBST(CONFIG_CALLKAFFEEXCEPTION_H)
AC_SUBST(KAFFEVM_MD_C)

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
    HAVE_CONFIG_JIT_MD_H=1
    CONFIG_JIT_MD_H="\$(top_srcdir)/config/$Khost_cpu/$Khost_os/$with_engine-md.h"
    AC_SUBST(CONFIG_JIT_MD_H)
    KAFFEVM_JIT_DEF="\$(top_srcdir)/config/$Khost_cpu/$with_engine-$Khost_cpu.def"
    AC_SUBST(KAFFEVM_JIT_DEF)
    KAFFEVM_ICODE_H="\$(top_srcdir)/config/$Khost_cpu/$with_engine-icode.h"
    AC_SUBST(KAFFEVM_ICODE_H)
else
    HAVE_CONFIG_JIT_MD_H=0
fi

AC_SUBST(HAVE_CONFIG_JIT_MD_H)

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

AC_ARG_ENABLE([debug],
	      AS_HELP_STRING([--disable-debug],
			     [Disable run-time debugging machinery. It is enabled by default.]),
			     [],
	[enable_debug="yes"])
case "$enable_debug" in
no) CPPFLAGS="$CPPFLAGS -DNDEBUG";;
*) CPPFLAGS="$CPPFLAGS -DKAFFE_VMDEBUG";;
esac

AC_MSG_CHECKING([whether to enable run-time debugging machinery])
AC_MSG_RESULT($enable_debug)

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
    CONFIG_XPROFILE_MD_H="\$(top_srcdir)/config/$Khost_cpu/$Khost_os/xprofile-md.h"
    AC_SUBST(CONFIG_XPROFILE_MD_H)
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
	AC_CHECK_LIB(semaphore,sem_init,SEMAPHORE_LIB="-lsemaphore")
	AC_SUBST(SEMAPHORE_LIB)
	KAFFE_LIB_SOLARIS_PTHREAD
	KAFFE_CHECK_SEMAPHORE

	if test -z "$THREADLIBS"; then
		AC_MSG_ERROR([Missing library libpthread, read FAQ/FAQ.pthreads])
	fi

	VMLIBS="$VM_LIBS $PTHREAD_LIBS"
	CPPFLAGS="$CPPFLAGS -D_REENTRANT"
 else
        CPPFLAGS="$CPPFLAGS -DBR_PTHREADS=0"
fi

AC_SUBST(VM_LIBS)

dnl =========================================================================
dnl Select the right garbage collector.
dnl -------------------------------------------------------------------------


AC_ARG_WITH([gc],
	    AS_HELP_STRING([--with-gc=GC],
			   [Force use given execution engine (kaffe-gc or boehm-gc)]),
	    [ GC_NAME="$withval" ]
)

dnl -----------------------------------------------------------------------
dnl                    Support for using a precompiled class library
dnl -----------------------------------------------------------------------
AC_ARG_WITH([rt_jar],
	    AS_HELP_STRING([--with-rt-jar=ABS.PATH],
			   [use precompiled kaffe class library]))

case "$with_rt_jar" in
"")
	use_rt_jar=false
	;;
"no" )
	use_rt_jar=false
	;;
"yes")
	AC_MSG_ERROR([Please suply an absolute path to a precompiled rt.jar from this version of kaffe])
	;;
*)
	use_rt_jar=true
	PATH_TO_RT_JAR=$with_rt_jar
	;;
esac;

AM_CONDITIONAL(USE_PRECOMPILED_RT_JAR, test x$use_rt_jar = xtrue)
AC_SUBST(PATH_TO_RT_JAR)

dnl =========================================================================
dnl Use jikes
dnl =========================================================================

AC_CHECK_PROG(JIKESPROG, jikes, jikes)
AC_ARG_WITH([jikes],
            AS_HELP_STRING([--with-jikes<=JIKES>],
                           [Define to use jikes compiler @<:@default=yes@:>@.  If yes, use jikes +Pno-switchcheck +Pno-shadow +E]),
	[JIKES="$withval"],
	[with_jikes="yes"])

AC_MSG_CHECKING([whether to use jikes])

case $with_jikes in
yes)
	if test x"$JIKESPROG" = x"" ; then
	  if  test x$use_rt_jar = xtrue ; then
	    AC_MSG_WARN([No jikes compiler found. It is required to build the regression tests. Recommended is jikes version 1.21 or higher.])
          else
	    AC_MSG_ERROR([No jikes compiler found. It is required to build Kaffe's class library. Recommended is jikes version 1.21 or higher.])
          fi
	fi
	JIKES="$JIKESPROG +Pno-switchcheck +Pno-shadow +E"
	use_jikes="yes ($JIKES)"
	;;
no)
	use_jikes="no"
	  if  test x$use_rt_jar = xtrue ; then
	    AC_MSG_WARN([You have to use jikes 1.21 or higher to build kaffe's regression tests, or to pass a different external java compiler to configure script.])
          else
	    AC_MSG_ERROR([You have to use jikes 1.21 or higher to build kaffe's class library, or to pass a different external java compiler to configure script.])
          fi
	;;
*)
	JIKES="$with_jikes"
	use_jikes="yes ($JIKES)"
	;;
esac

AC_MSG_RESULT($use_jikes)
AC_SUBST(JIKES)

dnl =========================================================================
dnl Look for trampoline code if in JIT mode
dnl -------------------------------------------------------------------------
KAFFEVM_TRAMPOLINES_S=
if test x"$with_engine" != x"intrp" ; then
	AC_MSG_CHECKING(for trampolines)
	if test -f $srcdir/config/$Khost_cpu/trampolines.S ; then
		KAFFEVM_TRAMPOLINES_S="\$(top_srcdir)/config/$Khost_cpu/trampolines.S"
		AC_SUBST(KAFFEVM_TRAMPOLINES_S)
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
AC_CHECK_PROG(ZIP, zip, zip)
if test  x"$ZIP" = x"" ; then
   AC_MSG_ERROR([zip not found. You need the zip utility for the class library.])
fi
AC_CHECK_PROG(MKTEMP, mktemp, mktemp)
if test  x"$MKTEMP" = x"" ; then
   AC_MSG_WARN([mktemp not found. You need mktemp to be able to debug kaffe.])
fi
AC_CHECK_PROG(JASMIN, jasmin, jasmin)

AM_CONDITIONAL(HAVE_ZIP, test x"$ZIP" != x"")
AM_CONDITIONAL(HAVE_JIKES, test x"$JIKES" != x"")
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
pushdef([AC_DEFINE_SAVED],defn([AC_DEFINE]))
pushdef([AC_DEFINE],[AC_DEFINE_SAVED([$1],[$2],[Define to $2 if $1 is not defined])])
AC_CHECK_TYPE(ssize_t, int)
popdef([AC_DEFINE])
popdef([AC_DEFINE_SAVED])

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

dnl Check for libraries for native library.
dnl None required.

dnl Check for libraries for net library.
AC_CHECK_FUNC(gethostname)
if test $ac_cv_func_gethostname = no; then
      AC_CHECK_LIBRARY(nsl, gethostname, NET_LIBS)
fi
AC_CHECK_FUNC(gethostbyname)
if test $ac_cv_func_gethostbyname = no; then
      AC_CHECK_LIBRARY(nsl, gethostbyname, NET_LIBS)
fi
AC_CHECK_FUNC(connect)
if test $ac_cv_func_connect = no; then
  AC_CHECK_LIBRARY(socket, connect, NET_LIBS)
fi
AC_SUBST(NET_LIBS)

dnl Check for libraries for zip library.
AC_CHECK_LIBRARY(z,deflate,ZIP_LIBS)
ZIP_LIBS="$CTLIBS $ZIP_LIBS"
AC_SUBST(ZIP_LIBS)

m4_LIB_GMP

dnl Check for libraries for security library.
AC_CHECK_FUNCS(MD2Init MD4Init MD5Init)
if test $ac_cv_func_MD5Init = no; then
  AC_CHECK_LIBRARY(md, MD5Init, SECURITY_LIBS)
fi
SECURITY_LIBS="$CTLIBS $SECURITY_LIBS"
AC_SUBST(SECURITY_LIBS)

dnl -------------------------------------------------------------------------

dnl =========================================================================
dnl Checks for header files.
dnl -------------------------------------------------------------------------

AC_HEADER_STDC

AC_CHECK_HEADERS([stdarg.h \
alloca.h \
bsd/libc.h \
crt_externs.h \
dlfcn.h \
fcntl.h \
features.h \
gmp.h \
iconv.h \
jpeglib.h \
kernel/OS.h \
limits.h \
mach/mach.h \
mach-o/rld.h \
malloc.h \
memory.h \
mips/cachectl.h \
netdb.h \
netinet/in.h \
netinet/in_systm.h \
netinet/tcp.h \
png.h \
poll.h \
pwd.h \
sgidefs.h \
sigcontext.h \
signal.h \
stdlib.h \
string.h \
stropts.h \
sys/cachectl.h \
sys/filio.h \
sys/ioctl.h \
sys/mman.h \
sys/param.h \
sys/poll.h \
sys/select.h \
sys/signal.h \
sys/socket.h \
sys/sockio.h \
sys/stat.h \
sys/sysctl.h \
sys/sysinfo.h \
sys/time.h \
sys/types.h \
sys/utsname.h \
sys/ucontext.h \
syslog.h \
time.h \
unistd.h \
utime.h \
wintypes.h \
wtypes.h \
zlib.h])

# check net/if.h seperately, as it requrires other headers on OS X
# and HP-UX 10.20
AC_CHECK_HEADERS([net/if.h], [], [],
[#include <stdio.h>
#if STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#if HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
])

dnl GNU/Linux sigcontext headers
AC_CHECK_HEADERS([asm/sigcontext.h])
AC_CHECK_HEADERS([asm/signal.h], [], [], [
#include <linux/types.h>
])

dnl needed for common ppc asm syntax on GNU/Linux
AC_CHECK_HEADERS([asm/ppc_asm.h])

dnl needed for MIPS cache flushing on GNU/Linux
AC_CHECK_HEADERS([asm/cachectl.h])

dnl needed for ia64
AC_CHECK_HEADERS([ia64intrin.h])

dnl needed to check for Linux 2.0
AC_CHECK_HEADERS([linux/socket.h])
AC_CHECK_HEADERS([linux/netlink.h], [], [],[
#if defined(HAVE_LINUX_SOCKET_H)
#include <linux/socket.h>
#endif /* HAVE_LINUX_SOCKET_H */
])
AC_CHECK_HEADERS([linux/rtnetlink.h], [], [], [
#if defined(HAVE_LINUX_SOCKET_H)
#include <linux/socket.h>
#endif /* HAVE_LINUX_SOCKET_H */
])
AC_CHECK_HEADERS([netpacket/packet.h])

AC_HEADER_SYS_WAIT
AC_HEADER_DIRENT

dnl We need both getrusage and the header containing its decls.
have_rusage=no
have_resource_h=no
AC_CHECK_HEADERS(sys/resource.h, have_resource_h=yes)
if test x"$have_resource_h" = x"yes"; then
   AC_CHECK_FUNCS(getrusage, have_rusage=yes)
fi

dnl Check whether there exists mprotect. If it exists and we are
dnl on HPUX, then prevent kaffe from using it when the pointer does not
dnl come from mmap.
AC_CHECK_FUNCS(mprotect)
if test x"$ac_cv_func_mprotect" = x"yes"; then
   case "$Khost_os" in
	hpux*)
		;;
	*) AC_DEFINE(HAVE_COMPATIBLE_MPROTECT, 1, [Does mprotect support protecting region not covered by mmap ?])
		;;
   esac
fi

dnl needed for valgrind
AC_CHECK_HEADERS([valgrind/memcheck.h])

dnl =========================================================================
dnl Select GC to use
dnl -------------------------------------------------------------------------

KAFFE_CHECK_GLIBC_VER

AC_MSG_CHECKING(which garbage collector to use)

dnl Save the requested gc and set the default to 'kaffe-gc'
if test x"${GC_NAME+set}" != x"set" ; then
	GC_NAME="kaffe-gc"
fi

gc_frag=$srcdir/kaffe/kaffevm/$GC_NAME/Makefile.frag
AC_SUBST(GC_NAME)
AC_SUBST_FILE(gc_frag)

dnl If the user has chosen the boehm-gc, build the specific flags.
if test x"${GC_NAME}" = x"boehm-gc"; then
	if test x"$with_threads" != x"unix-pthreads"; then
		AC_MSG_ERROR(BoehmGC only supports unix-pthreads threads. Please switch to unix-pthreads using --with-threads=unix-pthreads or use the standard kaffe's gc.)
	fi
	
	BOEHMGC_SPECIFIC_FLAGS="-DSILENT -DATOMIC_UNCOLLECTABLE -DGC_GCJ_SUPPORT -DALL_INTERIOR_POINTERS"

	if test x"${Khost_cpu+set}" = "xi686"; then
		BOEHMGC_SPECIFIC_FLAGS="$BOEHMGC_SPECIFIC_FLAGS -DUSE_I686_PREFETCH"
	fi

	case "$Khost_os" in
		solaris*)
			BOEHMGC_SPECIFIC_FLAGS="$BOEHMGC_SPECIFIC_FLAGS -DGC_SOLARIS_THREADS -DGC_SOLARIS_PTHREADS"
			;;
		irix*)
			BOEHMGC_SPECIFIC_FLAGS="$BOEHMGC_SPECIFIC_FLAGS -DGC_IRIX_THREADS"
			;;
		hpux*)
			BOEHMGC_SPECIFIC_FLAGS="$BOEHMGC_SPECIFIC_FLAGS -DGC_HPUX_THREADS"
			;;
		linux*)
			if test "$ac_cv_c_compile_value_glibc" -le "2"; then
			   BOEHMGC_SPECIFIC_FLAGS="$BOEHMGC_SPECIFIC_FLAGS -DGC_LINUX_THREADS"
			   if test "$Khost_cpu" = "i386"; then
			     BOEHMGC_SPECIFIC_FLAGS="$BOEHMGC_SPECIFIC_FLAGS -DTHREAD_LOCAL_ALLOC"
			   fi
			fi
			;;
		*)
			AC_MSG_ERROR(Threads support for $Khost_os unavailable in the boehm-gc. Please use the standard kaffe's gc.)
	esac

	ac_configure_args="$ac_configure_args --enable-libgc-convenience"
fi

AC_ARG_ENABLE([boehm-gc-configuration],
               AS_HELP_STRING([--disable-boehm-gc-configuration],
			      [Don't run boehm-gc configure script. Boehm gc does not work on all platforms. Default is to run it.]),
               [],
               [enable_boehm_gc_configuration=yes])

if test x"$enable_boehm_gc_configuration" = x"yes" ; then
  AC_CONFIG_SUBDIRS([kaffe/kaffevm/boehm-gc/boehm])
fi

AC_SUBST(BOEHMGC_SPECIFIC_FLAGS)

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

AC_HEADER_TIME
AC_STRUCT_TM
AC_STRUCT_TIMEZONE
AC_C_BIGENDIAN_CROSS
AC_C_BIGENDIAN
AC_C_CHAR_UNSIGNED
AC_C_CONST
AC_C_INLINE

dnl =========================================================================
dnl Check for [u]int{8,16,32,64} and bool.
dnl -------------------------------------------------------------------------

AX_CREATE_STDINT_H([include/config-int.h])

if test x"$ac_cv_typedef_bool" = x"yes"; then
  AC_DEFINE(HAVE_BOOL, 1, [Do we have bool])
fi

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

AC_FUNC_STRFTIME
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_UTIME_NULL

AC_CHECK_FUNCS([select socket getsockname getpagesize fstat])
AC_CHECK_FUNCS([memcpy memmove])
AC_CHECK_FUNCS([mkdir rmdir])
AC_CHECK_FUNCS([getcwd chdir getwd gettimeofday ftime time uname getuid])
AC_CHECK_FUNCS([localtime get_nprocs_conf])
AC_CHECK_FUNCS([munmap mincore msync madvise getpagesize sysconf])

AM_ICONV

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
AC_CHECK_FUNCS([pthread_attr_setschedpolicy pthread_yield sched_yield])
AC_CHECK_HEADERS(sched.h)

dnl getifaddrs replacement for linux
AC_REPLACE_FUNCS([getifaddrs])
AC_CHECK_HEADERS(ifaddrs.h,use_compat_ifaddrs="no",use_compat_ifaddrs="yes")
AM_CONDITIONAL(USE_COMPAT_IFADDRS, test "$use_compat_ifaddrs" = "yes")

dnl inet_ntop and inet_pton replacement for cygwin
AC_REPLACE_FUNCS([inet_ntop])
AC_REPLACE_FUNCS([inet_pton])
AC_CHECK_HEADERS([arpa/nameser.h])

dnl getaddrinfo replacement for Linux 2.0.x
AC_REPLACE_FUNCS([memset])
AC_TYPE_SOCKLEN_T
AC_TYPE_IN_PORT_T
AC_DECL_H_ERRNO

saved_LIBS="$LIBS"
LIBS="$LIBS $NET_LIBS"
AC_CHECK_FUNCS([getaddrinfo getnameinfo])
LIBS="$saved_LIBS"
if test "$ac_cv_func_getaddrinfo$ac_cv_func_getnameinfo" != yesyes ; then
   AC_LIBOBJ(getaddrinfo)
fi

dnl IPv6 macros on systems lacking them
AC_STRUCT_IN6_ADDR
AC_STRUCT_SOCKADDR_IN6
AC_STRUCT_SOCKADDR_STORAGE
AC_DECL_IN6ADDR_ANY
AC_DECL_IN6ADDR_LOOPBACK
AC_TYPE_SA_FAMILY_T

dnl some Linux IPv6 stacks don't have sin6_flowinfo and sin6_scope_id
dnl in their struct sockaddr_in6
AC_CHECK_MEMBERS([struct sockaddr_in6.sin6_flowinfo, struct sockaddr_in6.sin6_scope_id],,,[
#if defined(HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif
])

KSAVE_LIBS="$LIBS"
LIBS="$M_LIBS $LIBS"
AC_CHECK_FUNCS([remainder remainderf fmod fmodf drem])
AC_CHECK_FUNCS([rint floor ceil strtod mergesort])
AC_CHECK_FUNCS([strdup strstr strtod strtol strtoul strtouq strtoull])

dnl isinf, isnan, finite checking code taken from Sablotron 1.0,
dnl dually licensed licensed under MPL 1.1 and GPL
dnl available from http://www.gingerall.cz/charlie/ga/xml/p_sab.xml

dnl check for isnan etc...in the std. way and try link if not found
dnl this is due HPUX, where isnan & Co. are defined as macros

AC_CHECK_FUNC( isnan, [has_isnan=1; AC_DEFINE(HAVE_ISNAN, 1, isnan)], has_isnan=0)
AC_CHECK_FUNC( isinf, [has_isinf=1; AC_DEFINE(HAVE_ISINF, 1, isinf)], has_isinf=0)
AC_CHECK_FUNC( finite,[has_finite=1;AC_DEFINE(HAVE_FINITE, 1, finite)], has_finite=0)

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

dnl check for macro finite in math
if test $has_finite = 0; then
  AC_MSG_CHECKING(for macro finite)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <math.h>]], [[{ int foo; foo = finite(0); }]])],[has_finite=1],[has_finite=0 ])
  if test $has_finite = 1; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_FINITE, 1, finite)
  else
    AC_MSG_RESULT(no)
  fi
fi

LIBS="$KSAVE_LIBS"

dnl Test whether strtod("-0.0") is broken
AC_CACHE_CHECK([whether strtod(\"-0.0\") is broken],
  ac_cv_strtod_m0_broken, [dnl
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdlib.h>
/* definition DOUBLE_ORDER_OPPOSITE from config/config-hacks.h */
#if defined(__arm__)
#define DOUBLE_ORDER_OPPOSITE
#endif
/* definition of uint32 from kaffe/kaffevm/gtypes.h */
#if SIZEOF_INT == 4
#define uint32	unsigned int
#elif SIZEOF_LONG == 4
#define uint32	unsigned long
#else
#define uint32	int
#endif
/* definition of uint64 from kaffe/kaffevm/gtypes.h */
#if SIZEOF_LONG == 8
#define uint64	unsigned long
#elif SIZEOF_LONG_LONG == 8
#define uint64	unsigned long long
#elif SIZEOF___INT64 == 8
#define uint64	unsigned __int64
#else
#define uint64	int
#endif
int main()
{
    union { double d; uint32 i; uint64 j; } d;
    if (sizeof(uint64) != 8) return 2;
#if defined(HAVE_STRTOD)
    d.d = strtod("-0.0", 0);
#else
    d.d = atof("-0.0");
#endif
    /* doubleToLong() from kaffe/kaffevm/fp.c */
#if defined(DOUBLE_ORDER_OPPOSITE)
    {
	/* swap low and high word */
	uint32 r = *(uint32*)&d.j;
	uint32 *s = (uint32*)&d.j + 1;
	d.i = *s;
	*s = r;
    }
#endif
    if ((d.j >> 63) & 1) return 0; else return 1;
}
]])],[ac_cv_strtod_m0_broken=no],[ac_cv_strtod_m0_broken=yes],[ac_cv_strtod_m0_broken=cross])])
if test x"$ac_cv_strtod_m0_broken" = x"yes"; then
  AC_DEFINE(STRTOD_m0_BROKEN, 1, [Define if strtod(\"-0.0\") is broken])
fi

AC_CHECK_FUNCS([strerror hstrerror strerror_r])
AC_CHECK_FUNCS([fcntl ioctl])
AC_CHECK_FUNCS([alarm setitimer])
AC_CHECK_FUNCS([sigprocmask sigsetmask sigemptyset sigaddset signal sigaction])
AC_CHECK_FUNCS([sbrk valloc memalign mallopt getrlimit setrlimit sigaltstack])
AC_CHECK_FUNCS([madvise])
AC_CHECK_FUNCS([waitpid kill fork execve execvp])
AC_CHECK_FUNCS([sync fsync ftruncate])
AC_CHECK_FUNCS([atexit on_exit vsnprintf snprintf])

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
dnl Work out if select is declared
dnl -------------------------------------------------------------------------

AC_CACHE_CHECK([for declaration of select],
  ac_cv_declared_select,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
]], [[ void *foo = select; ]])],[ac_cv_declared_select=yes],[ac_cv_declared_select=no])
])
if test x"$ac_cv_declared_select" = x"yes"; then
  AC_DEFINE(HAVE_DECLARED_SELECT, 1, [Is function select declared])
fi

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

dnl ------------------------------------------------------------
dnl determine whether to enable the cairo GTK Graphics2D backend
dnl ------------------------------------------------------------
AC_ARG_ENABLE([gtk-cairo],
              [AS_HELP_STRING(--enable-gtk-cairo,build the cairo Graphics2D implementation on GTK [default=no])],
              [case "${enableval}" in
                yes) GTK_CAIRO_ENABLED=true ;;
                no) GTK_CAIRO_ENABLED=false ;;
                *) GTK_CAIRO_ENABLED=true ;;
              esac],
              [GTK_CAIRO_ENABLED=false])
AC_SUBST(GTK_CAIRO_ENABLED)
if test "x${GTK_CAIRO_ENABLED}" = xtrue; then
  AC_DEFINE(GTK_CAIRO, 1, [defined if cairo support was built in])
fi
AM_CONDITIONAL(GTK_CAIRO, test "x${GTK_CAIRO_ENABLED}" = xtrue)

dnl -------------------------------------------------------------------------
dnl Configure AWT engine
dnl -------------------------------------------------------------------------

AC_ARG_ENABLE([native-awt],
	AS_HELP_STRING([--disable-native-awt],
		       [disable build of Kaffe's all native AWT backends (enabled by default)]),
	[],
	[enable_native_awt=yes]
)

AM_CONDITIONAL(COND_NATIVE_AWT, test x"$enable_native_awt" != x"no")

AC_ARG_WITH([kaffe-x-awt],
	AS_HELP_STRING([--with-kaffe-x-awt],
		       [enable build of Kaffe's Xlib-based AWT engine (not built by default)]),
	[],
	[with_kaffe_x_awt=no]
)

AM_CONDITIONAL(COND_KAFFE_X_AWT, test x"$with_kaffe_x_awt" != x"no")

if test x"$enable_native_awt" = x"yes"; then
   if test x"$with_kaffe_x_awt" = x"yes"; then

      dnl Checks for libraries for X related libraries.
      AC_PATH_XTRA
      if test x"$no_x" = x"yes"; then
	    AWT_LIBS=
      else
	dnl Csup is needed for IRIX libraries such as libjpeg and libpng
	dnl Instead of adding it to both JPEG_LIBS and PNG_LIBS, use X_LIBS
	AC_CHECK_LIBRARY(Csup,__T_9__nothrow,X_LIBS)
	OLIBS=$LIBS
	LIBS="$ZIP_LIBS $M_LIBS $X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS $OLIBS"
	AC_CHECK_LIBRARY(Xext,XShmQueryExtension,X_PRE_LIBS)
	OCPPFLAGS=$CPPFLAGS
	CPPFLAGS="$CPPFLAGS $X_CFLAGS"
	AC_CHECK_HEADERS([sys/ipc.h])
	AC_CHECK_HEADERS([sys/shm.h])
	AC_CHECK_HEADERS([X11/extensions/XShm.h], [], [],
	[[#include <X11/Xlib.h>
	]])
	CPPFLAGS=$OCPPFLAGS	
	LIBS="$ZIP_LIBS $M_LIBS $X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS $OLIBS"
	AC_CHECK_LIBRARY(jpeg,jpeg_read_header,JPEG_LIBS)
	AC_CHECK_LIBRARY(png,png_create_info_struct,PNG_LIBS)
	AC_CACHE_CHECK([for a usable version of gif_lib.h],
		kaffe_cv_gif_lib_version, [dnl
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <gif_lib.h>
]], [[
	((GifFileType *)0)->UserData;
]])],[kaffe_cv_gif_lib_version=yes],[kaffe_cv_gif_lib_version=no])])
	if test x"$kaffe_cv_gif_lib_version" = x"yes"; then
	   AC_DEFINE(HAVE_GIF_LIB_H, 1, [Do we have the right version of libungif])
	   AC_CHECK_LIBRARY(ungif,DGifOpen,GIF_LIBS)
        fi

	LIBS=$OLIBS
	dnl If libpng is included, we also need libz - if we don't have it then
	dnl we can't include png support.
	if test -n "$PNG_LIBS" && test -n "$ZIP_LIBS"; then
		PNG_LIBS="$PNG_LIBS $ZIP_LIBS $M_LIBS"
	else
		PNG_LIBS=""
	fi

	dnl For each -L switch in X_LIBS, add a corresponding -R, unless
	dnl AC_PATH_XTRA has already taken care of them.
	case "$X_LIBS" in
	     "-R"* | *" -R"*) ;;
	     "-L"* | *" -L"*)
	     for ac_xlibdir in $X_LIBS; do
	     case "$ac_xlibdir" in
		  -L*) X_LIBS="$X_LIBS "`echo "$ac_xlibdir" | sed 's/^-L/-R/'`;;
	     esac
	     done ;;
        esac

	AWT_LIBS="$JPEG_LIBS $PNG_LIBS $GIF_LIBS $X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS"
	AWT_LIBS="$CTLIBS $AWT_LIBS"
	AC_SUBST(AWT_LIBS)
     fi

     if test "$have_x" = "no"; then
	AC_MSG_ERROR([Configuration --with-kaffe-x-awt not supported - missing X support])
     fi

     AWT_LIB="$AWT_LIB \$(top_builddir)/libraries/clib/awt/X/libxawt.la"
   fi
fi

AC_ARG_WITH([kaffe-qt-awt],
            AS_HELP_STRING([--with-kaffe-qt-awt],
                           [enable build of Kaffe's Qt2/3/embedded-based AWT engine (not built by default)]),
        [],
        [with_kaffe_qt_awt=no]
)

AM_CONDITIONAL(COND_KAFFE_QT_AWT, test x"$with_kaffe_qt_awt" != x"no")

if test x"$enable_native_awt" = x"yes"; then
   if test x"$with_kaffe_qt_awt" = x"yes"; then
        AC_PATH_X
	gw_CHECK_QT

        AWT_LIB="$AWT_LIB \$(top_builddir)/libraries/clib/awt/qt/libqtawt.la"
   fi
fi

AC_ARG_WITH([kaffe-nanox-awt],
	AS_HELP_STRING([--with-kaffe-nanox-awt],
			[enable build of Kaffe's Nano-X AWT engine (not built by default)]),
	[],
	[with_kaffe_nanox_awt=no]
)

AM_CONDITIONAL(COND_KAFFE_NANOX_AWT, test x"$with_kaffe_nanox_awt" != x"no")

if test x"$enable_native_awt" = x"yes"; then
   if test x"$with_kaffe_nanox_awt" = x"yes"; then

   	dnl -----------------------------------------------------------------------
	dnl 	Nano-X AWT support
	dnl -----------------------------------------------------------------------

	AC_ARG_WITH(nanox-include,
		AS_HELP_STRING([--with-nanox-include=dir],
		[Specify NanoX include directory]),
		[ NANOXINC=$withval ],
		[ NANOXINC="/usr/include/microwin" ])
		
	AC_ARG_WITH(nanox-library,
		AS_HELP_STRING([--with-nanox-library=dir],
		[Specify NanoX library directory]),
		[ NANOXLIB=$withval ],
		[ NANOXLIB="/usr/lib"])
		
	NANOX_CFLAGS="-I${NANOXINC}"
        AWT_LIBS="-L${NANOXLIB} -lnano-X -lmwengine -lmwdrivers -lmwfonts"
	AWT_LIBS="$CTLIBS $AWT_LIBS"
	AC_SUBST(NANOX_CFLAGS)
	AC_SUBST(AWT_LIBS)
			
	AWT_LIB="$AWT_LIB \$(top_builddir)/libraries/clib/awt/nano-X/libnanoxawt.la"
   fi
fi

AC_ARG_WITH([classpath-gtk-awt],
            AS_HELP_STRING([--without-classpath-gtk-awt],
                           [disable build of GNU Classpath's Gtk+-based AWT engine (built by default)]),
        [],
        [with_classpath_gtk_awt=yes]
)

AM_CONDITIONAL(COND_CLASSPATH_GTK_AWT, test x"$with_classpath_gtk_awt" != x"no")

AC_ARG_WITH([classpath-qt-awt],
            AS_HELP_STRING([--with-classpath-qt-awt],
                           [enable build of GNU Classpath's Qt4-based AWT engine (not built by default)]),
        [],
        [with_classpath_qt_awt=no]
)

AM_CONDITIONAL(COND_CLASSPATH_QT_AWT, test x"$with_classpath_qt_awt" != x"no")


if test x"$enable_native_awt" = x"yes"; then
   if test x"$with_classpath_gtk_awt" = x"yes"; then

	dnl -----------------------------------------------------------------------
	dnl                    Classpath Gtk peer support
	dnl -----------------------------------------------------------------------

	if test x"$with_threads" != x"unix-pthreads"; then
                AC_MSG_ERROR([Configuration --with-classpath-gtk-awt not supported - only available with pthreads])
	fi
	
	dnl Check for AWT related gthread/gtk.
	dnl for some obscure reason, it doesn't work without AC_PATH_X
	dnl with Kaffe's configure acript, but it works without it with
	dnl classpath's script. m4 is bizarre.
	AC_PATH_X
	AC_PATH_XTRA
	if test "$no_x" = yes; then
          AC_MSG_ERROR([GTK+ peers requested but no X library available])
        fi
        dnl We explicitly want the XTest Extension for Robot support.
        AC_CHECK_LIB([Xtst], [XTestQueryExtension], [true],
                     [AC_MSG_ERROR([libXtst NOT found, required for GdkRobot])],
                     [${X_LIBS}])
    	PKG_CHECK_MODULES(PANGOFT2, pangoft2)
  	AC_SUBST(PANGOFT2_LIBS)
  	AC_SUBST(PANGOFT2_CFLAGS)

	PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.4 gthread-2.0 >= 2.2 gdk-pixbuf-2.0)
	AC_SUBST(GTK_CFLAGS)
	AC_SUBST(GTK_LIBS)

	if test "x${enable_gtk_cairo}" = xyes
  	then
		PKG_CHECK_MODULES(CAIRO, cairo >= 0.5.0)
  	fi
  	AC_SUBST(CAIRO_LIBS)
  	AC_SUBST(CAIRO_CFLAGS)

        AWT_LIB="$AWT_LIB \$(top_builddir)/libraries/clib/awt/classpath-gtk/gtk-peer/libgtkpeer.la"
   fi

   if test x"$with_classpath_qt_awt" = x"yes"; then
	PKG_CHECK_MODULES(QT, QtGui >= 4.0)
	AC_SUBST(QT_CFLAGS)
	AC_SUBST(QT_LIBS)
	AC_CHECK_PROG(MOC, [moc], [moc])

        AWT_LIB="$AWT_LIB \$(top_builddir)/libraries/clib/awt/classpath-qt/libqtpeer.la"
   fi
fi

dnl -------------------------------------------------------------------------
dnl Configure sound backend
dnl -------------------------------------------------------------------------

dnl Check for alsa

if test x$with_threads = xunix-pthreads ; then

AM_PATH_ALSA(1.0.1,
 	     ,
 	     [AC_MSG_RESULT([Useable version of ALSA not found.])]
)

dnl alsa.m4 adds stuff to these variables we don't want, so revert
CFLAGS=$alsa_save_CFLAGS
LDFLAGS=$alsa_save_LDFLAGS
LIBS=$alsa_save_LIBS

else

   AC_MSG_WARN([Not searching for ALSA - only available with pthreads.])

fi

AM_PATH_ESD([0.2.1],
	    [esd_found=yes],
	    [AC_MSG_RESULT([Useable version of ESD not found.])
	     esd_found=no]
)

dnl Use alsa
dnl default is yes, if present.

AC_ARG_WITH([alsa],
	    AS_HELP_STRING([--without-alsa],
			   [Don't use ALSA for sound [default is to use it if present]]))

case $with_alsa in
no)
	;;
*)
	if test x"$alsa_found" = x"yes"; then
		ALSA_DIR=alsa
		ALSA_LIB="\$(top_builddir)/libraries/clib/sound/alsa/libtritonusalsa.la"
	fi
	;;
esac

dnl Use esd
dnl default is yes, if present.

AC_ARG_WITH([esd],
            AS_HELP_STRING([--without-esd],
                           [Don't use esd for sound [default is to use it if present]]))

case $with_esd in
no)
	;;
*)
	if test x"$esd_found" = x"yes"; then
		ESD_DIR=esd
		ESD_LIB="\$(top_builddir)/libraries/clib/sound/esd/libtritonusesd.la"
	fi
	;;
esac

dnl Global sound setting
dnl No overrides --with-* settings

AC_ARG_ENABLE([sound],
	      AS_HELP_STRING([--disable-sound],
			     [Disable javax.sound support]))

case "$enable_sound" in
no)
	ALSA_DIR=""
	ALSA_LIB=""
	ESD_DIR=""
	ESD_LIB=""
	;;
*)
	;;
esac

AM_CONDITIONAL([COND_SOUND_ALSA], [test "$ALSA_DIR" = alsa])
AM_CONDITIONAL([COND_SOUND_ESD], [test "$ESD_DIR" = esd])
AM_CONDITIONAL([COND_NATIVE_SOUND], [test x"$enable_sound" != x"no"])

dnl -----------------------------------------------------------------------
dnl                 Pure java imlementation of java.math.*
dnl -----------------------------------------------------------------------

AC_ARG_ENABLE([pure_java_math],
              AS_HELP_STRING([--enable-pure-java-math],
                             [Enable pure java java.math.* package @<:@default=native@:>@]),
[case "$enableval" in
yes)
	pure_java_math=true
	;;
no)
dnl  Use native implementation
	pure_java_math=false
	;;
*)
	AC_MSG_ERROR(bad value ${enableval} for --enable-pure-java-math)
	;;
esac],
[
dnl  If nothing is set, use native implementation if GNU MP can be found.
        if test "$ac_cv_lib_gmp_mpz_get_d" = no; then
                if test "$ac_cv_lib_gmp___gmpz_get_d" = no; then
			AC_MSG_ERROR([Could not find GNU MP library.  Install the gmp development package, or pass --enable-pure-java-math to configure.])
		fi
	fi
	AC_CHECK_HEADERS([gmp.h],
			 [pure_java_math=false],
			 [AC_MSG_ERROR([Found GNU MP library, but can't find gmp.h.  Install the gmp development package, or pass --enable-pure-java-math to  configure.])])
]
)

AM_CONDITIONAL([COND_PURE_JAVA_MATH], [test "$enable_pure_java_math" = yes])

if test x"$pure_java_math" = x"true" ; then
	AC_MSG_RESULT(Information: Using pure Java math library)
	MATH_LIB=""
else
	AC_MSG_RESULT(Information: Using native math library)
	MATH_LIB="\$(top_builddir)/libraries/clib/math/libmath.la"
fi

AC_SUBST(MATH_LIB)

dnl -----------------------------------------------------------------------
dnl                       Support selection of extensions
dnl -----------------------------------------------------------------------

AC_ARG_WITH([extensions], 
            AS_HELP_STRING([--with-extensions],
                           [the extensions to include (all|comm|microsoft|pjava|servlet) separated by ',' @<:@default=none@:>@]))

AC_MSG_CHECKING([which extensions to include])

case $with_extensions in
        *)
                for i in `echo $with_extensions | sed -e 's/,/ /g'` ; do
                        if test -d $srcdir/libraries/extensions/$i ; then
                        	INCLUDED_EXTENSIONS="$INCLUDED_EXTENSIONS $i"
                        fi
                done
                ;;

	no | "")
		;;

	"yes"| "all")
		INCLUDED_EXTENSIONS="comm  microsoft  pjava  servlet"
		;;
esac

AC_MSG_RESULT($INCLUDED_EXTENSIONS)
AC_SUBST(INCLUDED_EXTENSIONS)

dnl -----------------------------------------------------------------------
dnl                    Memory debugging support
dnl -----------------------------------------------------------------------

AM_WITH_DMALLOC

dnl -----------------------------------------------------------------------
dnl                    Support for different class library profiles
dnl -----------------------------------------------------------------------
AC_ARG_WITH([class_library_profile],
	    AS_HELP_STRING([--with-class-library-profile=ABS.PATH],
			   [specify class library profile]))

if test x"$with_class_library_profile" = x"" ; then
	PROFILE=default
else
	PROFILE=$with_class_library_profile
fi

AC_SUBST(PROFILE)

dnl -----------------------------------------------------------------------
dnl                    Support for libxmlj
dnl -----------------------------------------------------------------------
AC_ARG_ENABLE([libxmlj],
            AS_HELP_STRING([--enable-libxmlj],
                           [Enable building of libxmlj]))

if test x"$enable_libxmlj" = x"yes" ; then
  if test x"$with_threads" = x"unix-pthreads" ; then
    AM_PATH_XML2([2.6.8],[CFLAGS="${CFLAGS}"] [CPPFLAGS="${CPPFLAGS}"] [LIBS="${XML_LIBS} ${LIBS}"] ,
      AC_MSG_ERROR([unable to locate libxml.]))
    AM_PATH_XSLT([1.1.11],[CFLAGS="${XSLT_CFLAGS} ${CFLAGS}"] [CPPFLAGS="${XSLT_CFLAGS} ${CPPFLAGS}"] 
[LIBS="${XSLT_LIBS} ${LIBS}"] ,
      AC_MSG_ERROR([unable to locate libxslt.]))
    LIBXMLJ_LIB="\$(top_builddir)/libraries/clib/libxmlj/libxmlj.la"
  else
    AC_MSG_ERROR([Not building libxmlj - only available with pthreads.])
    enable_libxmlj="no"
    LIBXMLJ_LIB=""
  fi
fi

AM_CONDITIONAL([COND_LIBXMLJ], [test x"$enable_libxmlj" = x"yes"])

dnl =========================================================================
dnl Building of gjdoc
dnl -------------------------------------------------------------------------

AC_ARG_ENABLE([gjdoc],
		AS_HELP_STRING([--enable-gjdoc],
				[Enable building of gjdoc]),
		[],
		[enable_gjdoc=no])

AM_CONDITIONAL([COND_GJDOC], [test x"$enable_gjdoc" = x"yes"])

JAVA_LIBS=" \
\$(top_builddir)/libraries/clib/native/libnative.la \
\$(top_builddir)/libraries/clib/net/libnet.la \
\$(top_builddir)/libraries/clib/io/libio.la \
\$(top_builddir)/libraries/clib/nio/libnio.la \
\$(top_builddir)/libraries/clib/zip/libzip.la \
\$(top_builddir)/libraries/clib/management/libmanagement.la \
\$(top_builddir)/libraries/clib/security/libsecurity.la \
$JAVA_LIBS \
"

DLOPEN_JAVA_LIBS=
for lib in $JAVA_LIBS; do
  DLOPEN_JAVA_LIBS="$DLOPEN_JAVA_LIBS -dlopen $lib"
done
AC_SUBST(DLOPEN_JAVA_LIBS)


OPT_JAVA_LIBS=" \
$ALSA_LIB \
$ESD_LIB \
$LIBXMLJ_LIB \
$AWT_LIB \
"

OPT_DLOPEN_JAVA_LIBS=
for lib in $OPT_JAVA_LIBS; do
  OPT_DLOPEN_JAVA_LIBS="$OPT_DLOPEN_JAVA_LIBS -dlopen $lib"
done
AC_SUBST(OPT_DLOPEN_JAVA_LIBS)


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
dnl Enable execution of all static initializer loadLibrary() 
dnl calls at compile time.  By default most people will not
dnl want to disable this, but some VMs (gcj) don't need or want 
dnl them.
dnl -----------------------------------------------------------
AC_ARG_ENABLE([classpath-load-library],
              [AS_HELP_STRING(--enable-classpath-load-library,enable to use JNI native methods [default=yes])],
              [case "${enableval}" in 
                yes) INIT_LOAD_LIBRARY="true" ;;
                no) INIT_LOAD_LIBRARY="false" ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --enable-classpath-load-library) ;;
              esac],
              [INIT_LOAD_LIBRARY="true"])
AC_SUBST(INIT_LOAD_LIBRARY)


dnl =========================================================================
dnl Configure Binreloc support
dnl -------------------------------------------------------------------------

AC_CHECK_HEADERS([pthread.h])
if test x"$ac_cv_header_pthread_h" = x"yes"; then
  dnl We can now check whether we want binreloc
  AM_BINRELOC
  CFLAGS="$CFLAGS \$(BINRELOC_CFLAGS)"
fi
AM_CONDITIONAL(ENABLE_BINRELOC, test x"$enable_binreloc" = x"yes")

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

securitydir=$jredir/lib/security
AC_SUBST(securitydir)

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
CPPFLAGS="-I$abssrcdir/kaffe/kaffevm -I$abssrcdir/kaffe/kaffevm/$with_engine -I$abssrcdir/kaffe/kaffevm/systems/$with_threads -I\$(top_srcdir)/replace -I\$(top_srcdir)/binreloc -I\$(top_srcdir)/config -I\$(top_srcdir)/include $CPPFLAGS"
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

dnl -----------------------------------------------------------
dnl Add the include files for the native abstraction layer.
dnl Used by AM_CPPFLAGS in the different modules.
dnl -----------------------------------------------------------
CLASSPATH_INCLUDES="-I\$(top_srcdir)/libraries/clib/target/Linux -I\$(top_srcdir)/libraries/clib/target/generic"
AC_SUBST(CLASSPATH_INCLUDES)

AC_CONFIG_SUBDIRS([libltdl])
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
AC_CONFIG_FILES([kaffe/kaffevm/systems/beos-native/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/systems/unix-pthreads/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/verifier/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/kaffe-gc/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/boehm-gc/Makefile])
AC_CONFIG_FILES([kaffe/kaffevm/jni/Makefile])
AC_CONFIG_FILES([kaffe/kaffe/Makefile])
AC_CONFIG_FILES([kaffe/kaffeh/Makefile])
AC_CONFIG_FILES([kaffe/xprof/Makefile])
AC_CONFIG_FILES([kaffe/man/Makefile])
AC_CONFIG_FILES([kaffe/scripts/Makefile])
AC_CONFIG_FILES([kaffe/scripts/bat/Makefile])
AC_CONFIG_FILES([kaffe/scripts/compat/Makefile])
AC_CONFIG_FILES([libraries/javalib/gnu/classpath/Configuration.java])
AC_CONFIG_FILES([libraries/javalib/Makefile])
AC_CONFIG_FILES([libraries/javalib/awt-implementations/Makefile])
AC_CONFIG_FILES([libraries/javalib/awt-implementations/kaffe/Makefile])
AC_CONFIG_FILES([libraries/javalib/gmp-math/Makefile])
AC_CONFIG_FILES([libraries/clib/Makefile])
AC_CONFIG_FILES([libraries/clib/target/Makefile])
AC_CONFIG_FILES([libraries/clib/target/generic/Makefile])
AC_CONFIG_FILES([libraries/clib/target/Linux/Makefile])
AC_CONFIG_FILES([libraries/clib/classpath/Makefile])
AC_CONFIG_FILES([libraries/clib/fdlibm/Makefile])
AC_CONFIG_FILES([libraries/clib/native/Makefile])
AC_CONFIG_FILES([libraries/clib/io/Makefile])
AC_CONFIG_FILES([libraries/clib/libxmlj/Makefile])
AC_CONFIG_FILES([libraries/clib/nio/Makefile])
AC_CONFIG_FILES([libraries/clib/net/Makefile])
AC_CONFIG_FILES([libraries/clib/zip/Makefile])
AC_CONFIG_FILES([libraries/clib/math/Makefile])
AC_CONFIG_FILES([libraries/clib/management/Makefile])
AC_CONFIG_FILES([libraries/clib/security/Makefile])
AC_CONFIG_FILES([libraries/clib/awt/Makefile])
AC_CONFIG_FILES([libraries/clib/awt/classpath-gtk/Makefile])
AC_CONFIG_FILES([libraries/clib/awt/classpath-gtk/gtk-peer/Makefile])
AC_CONFIG_FILES([libraries/clib/awt/classpath-gtk/resource/Makefile])
AC_CONFIG_FILES([libraries/clib/awt/classpath-qt/Makefile])
AC_CONFIG_FILES([libraries/clib/awt/X/Makefile])
AC_CONFIG_FILES([libraries/clib/awt/qt/Makefile])
AC_CONFIG_FILES([libraries/clib/awt/nano-X/Makefile])
AC_CONFIG_FILES([libraries/clib/sound/Makefile])
AC_CONFIG_FILES([libraries/clib/sound/alsa/Makefile])
AC_CONFIG_FILES([libraries/clib/sound/common/Makefile])
AC_CONFIG_FILES([libraries/clib/sound/esd/Makefile])
AC_CONFIG_FILES([libraries/javalib/rebuildLib])
AC_CONFIG_FILES([libraries/extensions/Makefile])
AC_CONFIG_FILES([libraries/extensions/comm/Makefile])
AC_CONFIG_FILES([libraries/extensions/comm/javalib/Makefile])
AC_CONFIG_FILES([libraries/extensions/pjava/Makefile])
AC_CONFIG_FILES([libraries/extensions/pjava/javalib/Makefile])
AC_CONFIG_FILES([libraries/extensions/servlet/Makefile])
AC_CONFIG_FILES([libraries/extensions/servlet/javalib/Makefile])
AC_CONFIG_FILES([libraries/extensions/microsoft/Makefile])
AC_CONFIG_FILES([libraries/extensions/microsoft/javalib/Makefile])
AC_CONFIG_FILES([libraries/extensions/microsoft/clib/Makefile])
AC_CONFIG_FILES([kaffe/man/kaffe.1])
AC_CONFIG_FILES([kaffe/scripts/install-jar])
AC_CONFIG_FILES([kaffe/scripts/kaffe])
AC_CONFIG_FILES([kaffe/scripts/kaffexprof])
AC_CONFIG_FILES([kaffe/scripts/jar])
AC_CONFIG_FILES([kaffe/scripts/javah])
AC_CONFIG_FILES([kaffe/scripts/javap])
AC_CONFIG_FILES([kaffe/scripts/jdb])
AC_CONFIG_FILES([kaffe/scripts/javadoc])
AC_CONFIG_FILES([kaffe/scripts/appletviewer])
AC_CONFIG_FILES([kaffe/scripts/rmic])
AC_CONFIG_FILES([kaffe/scripts/rmiregistry])
AC_CONFIG_FILES([kaffe/scripts/serialver])
AC_CONFIG_FILES([kaffe/scripts/native2ascii])
AC_CONFIG_FILES([kaffe/scripts/compat/java])
AC_CONFIG_FILES([kaffe/scripts/compat/javac])
AC_CONFIG_FILES([kaffe/scripts/compat/appletviewer])
AC_CONFIG_FILES([kaffe/scripts/compat/javadoc])
AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([tools/Makefile])
AC_CONFIG_FILES([tools/gjdoc/javalib/version.properties])
AC_CONFIG_FILES([tools/gjdoc/Makefile])
AC_CONFIG_FILES([tools/gjdoc/javalib/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([test/internal/Makefile])
AC_CONFIG_FILES([test/regression/Makefile])
AC_CONFIG_FILES([test/regression/TestScript])
AC_CONFIG_FILES([test/regression/compile_time/Makefile])
AC_CONFIG_FILES([test/regression/compile_time/CompileScript])
AC_CONFIG_FILES([test/regression/compiler/Makefile])
AC_CONFIG_FILES([test/regression/compiler/CompileScript])
AC_CONFIG_FILES([test/regression/run_time/Makefile])
AC_CONFIG_FILES([test/regression/run_time/CompileScript])
AC_CONFIG_FILES([test/jni/Makefile])

AC_OUTPUT
