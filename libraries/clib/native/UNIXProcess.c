/*
 * java.lang.UNIXProcess.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "files.h"
#include "FileDescriptor.h"
#include "UNIXProcess.h"
#include "../../../kaffe/kaffevm/support.h"
#include <jsyscall.h>
#include <native.h>

typedef struct _child {
	Hjava_lang_UNIXProcess*	proc;
	int			pid;
	struct _child*		next;
} child;
static child* children;

jint
java_lang_UNIXProcess_forkAndExec(Hjava_lang_UNIXProcess* this, HArrayOfObject* args, HArrayOfObject* envs)
{
	int pid;
	int in[2];
	int out[2];
	int err[2];
	int sync[2];
	char** argv;
	char** arge;
	int i;
	char* path;
	int arglen;
	int envlen;
	char b[1];
	child* newchild;

	arglen = (args ? obj_length(args) : 0);
	envlen = (envs ? obj_length(envs) : 0);

DBG(	printf("args %d envs %d\n", arglen, envlen); fflush(stdout);	)

	if (arglen < 1) {
		SignalError("java.io.IOException", "No such file");
	}

	path = makeCString((struct Hjava_lang_String*)unhand(args)->body[0]);
	/* Check program exists and we can execute it */
        i = access(path, X_OK);
	if (i < 0) {
		free(path);
		SignalError("java.io.IOException", SYS_ERROR);
	}

	/* Allocate somewhere to keep the child data */
	newchild = malloc(sizeof(child));

	/* Create the pipes to communicate with the child */
	pipe(in);
	pipe(out);
	pipe(err);
	pipe(sync);

#if defined(HAVE_FORK)
	pid = fork();
#else
	unimp("fork() not provided");
#endif
	switch (pid) {
	case 0:
		/* Child */
		dup2(in[0], 0);
		dup2(out[1], 1);
		dup2(err[1], 2);

		/* What is sync about anyhow?  Well my current guess is that
		 * the parent writes a single byte to it when it's ready to
		 * proceed.  So here I wait until I get it before doing
		 * anything.
		 */
		read(sync[0], b, sizeof(b));

		close(in[0]);
		close(in[1]);
		close(out[0]);
		close(out[1]);
		close(err[0]);
		close(err[1]);
		close(sync[0]);
		close(sync[1]);
		break;

	case -1:
		/* Error */
		close(in[0]);
		close(in[1]);
		close(out[0]);
		close(out[1]);
		close(err[0]);
		close(err[1]);
		close(sync[0]);
		close(sync[1]);
		free(newchild);
		SignalError("java.io.IOException", "Fork failed");
		return (-1);

	default:
		/* Parent */
		unhand(unhand(this)->stdin_fd)->fd = in[1];
		unhand(unhand(this)->stdout_fd)->fd = out[0];
		unhand(unhand(this)->stderr_fd)->fd = err[0];
		unhand(unhand(this)->sync_fd)->fd = sync[1];
		close(in[0]);
		close(out[1]);
		close(err[1]);
		close(sync[0]);

		/* Note child data and add to children list */
		newchild->proc = this;
		newchild->pid = pid;
		newchild->next = children;
		children = newchild;

		return (pid);
	}

	/* Child: execute new program ... */

	/* Build arguments and environment */
	argv = calloc(arglen + 1, sizeof(char*));
	arge = calloc(envlen + 1, sizeof(char*));
	for (i = 0; i < arglen; i++) {
		argv[i] = makeCString((struct Hjava_lang_String*)unhand(args)->body[i]);
	}
	for (i = 0; i < envlen; i++) {
		arge[i] = makeCString((struct Hjava_lang_String*)unhand(envs)->body[i]);
	}
	/* Execute program */
#if defined(HAVE_EXECVE)
	execve(path, argv, arge);
#else
	unimp("execve() not provided");
#endif
	exit(-1);
}

void
java_lang_UNIXProcess_destroy(Hjava_lang_UNIXProcess* this)
{
#if defined(HAVE_KILL)
	kill(unhand(this)->pid, SIGTERM);
#else
	unimp("kill() not provided");
#endif
}

/*
 * Demon thread.  This runs waiting for children to die and wakes anyone
 * interested in them.
 */
void
java_lang_UNIXProcess_run(Hjava_lang_UNIXProcess* this)
{
	int npid;
	int status;
	child* p;
	child** pp;

	for (;;) {
		npid = waitpid(-1, &status, 0);
		for (pp = &children; *pp != 0; pp = &p->next) {
			p = *pp;
			if (p->pid == npid) {
				_lockMutex(p->proc);
				unhand(p->proc)->isalive = 0;
				unhand(p->proc)->exit_code = status;
				_broadcastCond(p->proc);
				_unlockMutex(p->proc);
				*pp = p->next;
				free(p);
				break;
			}
		}
	}
}

void
java_lang_UNIXProcess_notifyReaders(Hjava_lang_UNIXProcess* this)
{
	unimp("java.lang.UNIXProcess:notifyReaders not implemented");
}
