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
#include "../../../kaffe/kaffevm/support.h"

void
kaffe_applet_AudioPlayer_playFile( jstring jstr ) {
	char    fName[MAXPATHLEN];
	int     bLen = 1024;
	int     bRead, fin, dev;
	void	*buf;

	buf = KMALLOC( bLen);

	javaString2CString( jstr, fName, sizeof(fName));

	dev = KOPEN( "/dev/audio", O_WRONLY|O_BINARY, 0);
	fin = KOPEN( fName, O_RDONLY|O_BINARY, 0);

	while ( (bRead = KREAD( fin, buf, bLen )) > 0 ) {
		KWRITE( dev, buf, bRead );
	}

	KCLOSE( dev);
	KCLOSE( fin);
	KFREE( buf);
}

