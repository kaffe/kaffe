#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SELFPATH "/usr/local"
#include "prefix.h"


void libtest_start ()
{
	printf ("libfoo is located at: %s\n", SELFPATH);
}
