/*
 * kaffe.applet.AudioPlayer.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "jni.h"
#include "files.h"
#include "defs.h"
#include "jsyscall.h"
#include "kaffe_applet_AudioPlayer.h"
#include "support.h"
#include "stringSupport.h"

void
kaffe_applet_AudioPlayer_playFile(struct Hjava_lang_String* jstr)
{
	char    fName[MAXPATHLEN];
	size_t  bLen = 1024;
	int     fin, dev, rc;
	ssize_t	bRead;
	void	*buf;

	buf = KMALLOC( bLen);
	if (!buf) {
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
	}

	stringJava2CBuf( jstr, fName, sizeof(fName));

	rc = KOPEN( "/dev/audio", O_WRONLY|O_BINARY, 0, &dev);
	if (rc) {
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
	rc = KOPEN( fName, O_RDONLY|O_BINARY, 0, &fin);
	if (rc) {
		KCLOSE(dev);
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}

	while ( (KREAD( fin, buf, bLen, &bRead ) == 0) && (bRead > 0)) {
		ssize_t bWritten;
		KWRITE( dev, buf, (size_t)bRead, &bWritten );	/* XXX check error */
	}

	KCLOSE( dev);
	KCLOSE( fin);
	KFREE( buf);
}

