AC_DEFUN([AC_PROG_ANTLR],[
  AC_REQUIRE([AC_PROG_JAVA])dnl
  AC_MSG_CHECKING([for antlr $1.$2.$3 or better])
  for antlr_lib_home in `ls -d /usr/local/share/antlr* 2> /dev/null` /usr/share/java; do
	CLASSPATH=""
	BOOTCLASSPATH=""
        antlr_version_str=`$JAVA -classpath "$antlr_lib_home/antlr.jar" antlr.Tool 2>&1 | head -n 1 | sed '/.*Version */!d; s///;q'`
	if test "$antlr_version_str"; then
              antlr_version_regex='s/\([[[:digit:]]]\+\)\.\([[[:digit:]]]\+\)\.\([[[:digit:]]]\+\).*$/'
              antlr_version_major=`echo $antlr_version_str | sed "$antlr_version_regex\\1/"`
              antlr_version_minor=`echo $antlr_version_str | sed "$antlr_version_regex\\2/"`
              antlr_version_micro=`echo $antlr_version_str | sed "$antlr_version_regex\\3/"`
              (test $antlr_version_major -gt $1 ||
                      (test $antlr_version_major -eq $1 && \
                       test $antlr_version_minor -gt $2) || \
                      (test $antlr_version_major -eq $1 && \
                       test $antlr_version_minor -eq $2 && \
                       test $antlr_version_micro -ge $3)) && \

              (test -z $ANTLR_JAR || \
                (test $antlr_version_major -gt $antlr_use_major ||
                      (test $antlr_version_major -eq $antlr_use_major && \
                       test $antlr_version_minor -gt $antlr_use_minor) || \
                      (test $antlr_version_major -eq $antlr_use_major && \
                       test $antlr_version_minor -eq $antlr_use_minor && \
                       test $antlr_version_micro -ge $antlr_use_micro))) && \
                      ANTLR_JAR=$antlr_lib_home/antlr.jar && \
                      antlr_use_major=$antlr_version_major && \
                      antlr_use_minor=$antlr_version_minor && \
                      antlr_use_micro=$antlr_version_micro
	fi
  done
  ANTLR="$JAVA -classpath $ANTLR_JAR antlr.Tool"
  test -z $ANTLR_JAR && \
        AC_MSG_ERROR(no suitable antlr.jar found for version $1.$2.$3)
  AC_MSG_RESULT($antlr_use_major.$antlr_use_minor.$antlr_use_micro)
  AC_SUBST(ANTLR)
  AC_SUBST(ANTLR_JAR)
  AC_PROVIDE([$0])dnl
])
