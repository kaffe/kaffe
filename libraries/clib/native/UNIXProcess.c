/*
 * kaffe.lang.UNIXProcess.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

/* Note: this file uses JNI throughout. */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include <jni.h>
#include <jsyscall.h>
#include <native.h>
#include "UNIXProcess.h"

typedef struct _child {
	jobject 		proc;
	jint			pid;
	struct _child*		next;
} child;
static child* children;

jint
Java_kaffe_lang_UNIXProcess_forkAndExec(JNIEnv* env, jobject proc, jarray args, jarray envs)
{
	jint pid;
	char** argv;
	char** arge;
	child* newchild;
	jint ioes[4];
	int arglen;
	int envlen;
	int i;
	jclass ioexc_class = (*env)->FindClass(env, "java.io.IOException");
	jclass proc_class;
	/* the names given to the stream in Java */
	const char *fd_names[] = { "stdin_fd", 
				  "stdout_fd", 
				  "stderr_fd", 
				  "sync_fd"};

	arglen = (args ? (*env)->GetArrayLength(env, args) : 0);
	envlen = (envs ? (*env)->GetArrayLength(env, envs) : 0);

	if (arglen < 1) {
		(*env)->ThrowNew(env, ioexc_class, "No such file");
		return -1;
	}

	/* Build arguments and environment */
	argv = calloc(arglen + 1, sizeof(jbyte*));
	for (i = 0; i < arglen; i++) {
		jstring argi;
		const jbyte *argichars;

		argi = (jstring)(*env)->GetObjectArrayElement(env, args, i);
		argichars = (*env)->GetStringUTFChars(env, argi, NULL);
		argv[i] = malloc(strlen(argichars));
		strcpy(argv[i], argichars);
		(*env)->ReleaseStringUTFChars(env, argi, argichars);
	}

	if (envlen > 0)
		arge = calloc(envlen + 1, sizeof(jbyte*));
	else
		arge = NULL;

	for (i = 0; i < envlen; i++) {
		jstring envi;
		const jbyte *envichars;

		envi = (jstring)(*env)->GetObjectArrayElement(env, envs, i);
		envichars = (*env)->GetStringUTFChars(env, envi, NULL);
		arge[i] = malloc(strlen(envichars));
		strcpy(arge[i], envichars);
		(*env)->ReleaseStringUTFChars(env, envi, envichars);
	}

	pid = forkexec(argv, arge, ioes);

	/* free before returning on error */
	for (i = 0; i < arglen; i++) {
		free(argv[i]);
	}
	free(argv);

	for (i = 0; i < envlen; i++) {
		free(arge[i]);
	}
	free(arge);

	if (pid == -1) {
		(*env)->ThrowNew(env, ioexc_class, "Fork&Exec failed");
		return (-1);
	}

	/* get kaffe.lang.UNIXProcess class */
	proc_class = (*env)->GetObjectClass(env, proc);

	/*
	 * Note: even though it is likely that `pfd_field' and `fd_field'
	 * will be identical throughout the loop, this code does not assume
	 * that.  Hence, it will work even when used with reloaded classes.
	 */
	for (i = 0; i < 4; i++) {
		jfieldID pfd_field = (*env)->GetFieldID(env, proc_class, 
			    fd_names[i], "java.io.FileDescriptor");
		jobject fdi = (*env)->GetObjectField(env, proc, pfd_field);
		jfieldID fd_field = (*env)->GetFieldID(env, 
					(*env)->GetObjectClass(env, fdi), 
					"fd", "I");
		(*env)->SetIntField(env, fdi, fd_field, ioes[i]);
	}

	/* Allocate somewhere to keep the child data */
	newchild = malloc(sizeof(child));

	/* Note child data and add to children list */
	newchild->proc = (*env)->NewGlobalRef(env, proc);
	newchild->pid = pid;

	/* XXX protect this list! */
	newchild->next = children;
	children = newchild;

	return pid;
}

void 
Java_kaffe_lang_UNIXProcess_destroy(JNIEnv* env, jobject proc)
{
#if defined(HAVE_KILL)
	jfieldID pid = (*env)->GetFieldID(env, 
				(*env)->GetObjectClass(env, proc), 
				"pid", "I");
	kill((*env)->GetIntField(env, proc, pid), SIGTERM);
#else
	unimp("kill() not provided");
#endif
}

/*
 * Demon thread.  This runs waiting for children to die and wakes anyone
 * interested in them.
 */
void 
Java_kaffe_lang_UNIXProcess_run(JNIEnv* env, jobject _proc_dummy)
{
	int npid;
	int status;
	child* p;
	child** pp;
	jmethodID notify_method = (*env)->GetMethodID(env, 
			(*env)->FindClass(env, "java.lang.Object"),
			"notifyAll", "()V");

	for (;;) {
		npid = waitpid(-1, &status, 0);
		for (pp = &children; *pp != 0; pp = &p->next) {
			p = *pp;
			if (p->pid == npid) {
				jclass proc_class = (*env)->GetObjectClass(
						env, p->proc);
				jfieldID isalive_field = 
					(*env)->GetFieldID(env, proc_class,
					"isalive", "B");
				jfieldID exit_code_field = 
					(*env)->GetFieldID(env, proc_class,
					"exit_code", "I");

				(*env)->MonitorEnter(env, p->proc);
				(*env)->SetBooleanField(env, p->proc, 
						isalive_field, 0 /* false */);
				(*env)->SetIntField(env, p->proc, 
						exit_code_field, status);

				(*env)->CallVoidMethod(env, 
					p->proc, notify_method);
				(*env)->MonitorExit(env, p->proc);
				(*env)->DeleteGlobalRef(env, p->proc);
				*pp = p->next;
				free(p);
				break;
			}
		}
	}
}

