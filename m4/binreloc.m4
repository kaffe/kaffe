# Check for binary relocation support
# Hongli Lai
# http://autopackage.org/

AC_DEFUN([AM_BINRELOC],
[
AC_ARG_ENABLE(binreloc,
	[  --enable-binreloc       compile with binary relocation support
                          (default=enable when available)],
	enable_binreloc=$enableval,enable_binreloc=auto)
BINRELOC_CFLAGS=

AC_MSG_CHECKING(whether binary relocation support should be enabled)
if test "$enable_binreloc" = "yes"; then
	AC_MSG_RESULT(yes)
	AC_MSG_CHECKING(for linker mappings at /proc/self/maps)
	if test -e /proc/self/maps; then
		AC_MSG_RESULT(yes)
		BINRELOC_CFLAGS=-DENABLE_BINRELOC
	else
		AC_MSG_RESULT(no)
		AC_MSG_ERROR(/proc/self/maps is not available. Binary relocation cannot be enabled.)
	fi

elif test "$enable_binreloc" = "auto"; then
	AC_MSG_RESULT(yes when available)
	AC_MSG_CHECKING(for linker mappings at /proc/self/maps)
	if test -e /proc/self/maps; then
		AC_MSG_RESULT(yes)
		enable_binreloc=yes
		
		AC_MSG_CHECKING(whether everything is installed to the same prefix)
		if test "$bindir" = '${exec_prefix}/bin' -a "$sbindir" = '${exec_prefix}/sbin' -a \
			"$datadir" = '${prefix}/share' -a "$libdir" = '${exec_prefix}/lib' -a \
			"$libexecdir" = '${exec_prefix}/libexec' -a "$sysconfdir" = '${prefix}/etc'
		then
			BINRELOC_CFLAGS=-DENABLE_BINRELOC
			AC_MSG_RESULT(yes)
		else
			AC_MSG_RESULT(no)
			AC_MSG_NOTICE(Binary relocation support will be disabled.)
			enable_binreloc=no
		fi

	else
		AC_MSG_RESULT(no)
		enable_binreloc=no
	fi

elif test "$enable_binreloc" = "no"; then
	AC_MSG_RESULT(no)
else
	AC_MSG_RESULT(no (unknown value "$enable_binreloc"))
	enable_binreloc=no
fi

AC_SUBST(BINRELOC_CFLAGS)
])
