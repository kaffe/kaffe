/*
 * a program to guess the correct value of SP_OFFSET for your architecture.
 *
 * Compile with cc sp_offset.c and run with ./a.out
 * This program may leave core dumps in your current dir.
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */
#include <setjmp.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#define STACK_SIZE	65536
#define STACK_COPY	128
#define GET_SP(E)       (((void**)(E))[SP_OFFSET])
#define SET_SP(E, V)    ((void**)(E))[SP_OFFSET] = (V)

int fork();

int  SP_OFFSET;
char new_stack[STACK_SIZE];
jmp_buf buf, buf2;

void alarm()
{
    longjmp(buf2, 1);
}

static void
check()
{
    char x;
    if (&x > new_stack && &new_stack[sizeof new_stack] > &x)
	printf("#define\tSP_OFFSET\t%d\n", SP_OFFSET);
    exit(0);
}

int
main(int ac, char *av[])
{
    int  stat;
    pid_t child;
    void *oldsp, *newsp;

    for (; SP_OFFSET < sizeof (buf) / sizeof (int); SP_OFFSET++) {

	if ((child = fork()) == 0) {	/* child */

	    if (setjmp(buf)) 
		check();

	    oldsp = GET_SP(buf);
	    newsp = &new_stack[(sizeof new_stack)/2];
	    SET_SP(buf, newsp);
	    memcpy(newsp, oldsp, STACK_COPY);
	    longjmp(buf, 1);
	    exit(-1);

	} else {	/* parent */

	    if (!setjmp(buf2)) {
		signal(SIGALRM, alarm);
		alarm(3);
		assert(child == wait(&stat));
		alarm(0);
	    }
	    /* else timed out, continue */
	}
    }
    return 0;
}
