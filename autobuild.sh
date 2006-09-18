#!/bin/sh

set -e

# Pull in config scripts
PATH=$AUTOBUILD_INSTALL_ROOT/bin:$PATH
export PATH

# Clean up build area
#[ -f Makefile ] && make -k maintainer-clean ||:
[ -f Makefile ] && make -k clean ||:

# Re-generate autotools scripts
#autoconf
#automake -a

# Configure the build
./configure --prefix=$AUTOBUILD_INSTALL_ROOT $CONFIG_LINE

# Make
make

# run tests
if [ -n "$MAKE_CHECK" ]
then
	test -n "$1" && TEST_RESULTS_FILE=$1 || TEST_RESULTS_FILE=results.log
	rm -f $TEST_RESULTS_FILE

	make check | tee $TEST_RESULTS_FILE
fi

# Check source code dist
if [ -n "$MAKE_DISTCHECK" ]
then
	make distcheck
fi

