#! /bin/sh
# generate mauve result page
#
# the result page is $MAUVE_SRC/mauve-results.html
#
# needs a few environment variables to be set:
#
#  MAUVE_SRC location of mauve source directory from CVS
#  KAFFE_SRC location of kaffe source directory from CVS
#  JTIDY_CPATH location of Tidy.jar from jTidy
#
# FIXME: The header and footer messages used n the perl script should be better/configurable.
#
cd $MAUVE_SRC
autoreconf
./configure JAVA=kaffe JAVAC=kjc
cp $KAFFE_SRC/developers/mauve-kaffe .
make check KEYS=kaffe TESTFLAGS=-verbose > kaffe-log
perl $KAFFE_SRC/developers/mauve-html-gen.pl "for kaffe from CVS HEAD" "kaffe on i686-linux using jit3 engine and unix-jthreads implementation. Tests are run with developers/mauve-kaffe file from CVS HEAD" < kaffe-log > mauve-results.html
kaffe -cp $JTIDY_CPATH  org/w3c/tidy/Tidy -imu mauve-results.html
