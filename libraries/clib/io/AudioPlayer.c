/*
 * kaffe.applet.AudioPlayer.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

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

	buf = malloc( bLen);

	javaString2CString( jstr, fName, sizeof(fName));

	dev = open( "/dev/audio", O_WRONLY|O_BINARY, 0);
	fin = open( fName, O_RDONLY|O_BINARY, 0);

	while ( (bRead = read( fin, buf, bLen )) > 0 ) {
		write( dev, buf, bRead );
	}

	close( dev);
	close( fin);
	free( buf);
}

