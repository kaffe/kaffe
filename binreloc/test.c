/* Simple test program. Undefine ENABLE_BINRELOC to disable BinReloc. */

#define SELFPATH "unknown"
#define PREFIX "/usr/local"
#define DATADIR PREFIX "/share"
#define SYSCONFDIR PREFIX "/etc"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "prefix.h"


extern void libtest_start ();


static void test ()
{
	printf ("Full path: %s\n", SELFPATH);
	printf ("PREFIX is: %s\n", PREFIX);
	printf ("DATADIR is: %s\n", DATADIR);
	printf ("Data file : %s\n", BR_DATADIR ("/test/logo.png"));
	printf ("Conf file : %s\n\n", BR_SYSCONFDIR ("/test/configuration.txt"));

	libtest_start ();
}


int main ()
{
	test ();
	return 0;
}
