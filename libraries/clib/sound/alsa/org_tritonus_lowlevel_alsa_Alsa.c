/*
 *	org_tritonus_lowlevel_alsa_Alsa.cc
 */

/*
 *  Copyright (c) 1999 - 2001 by Matthias Pfisterer <Matthias.Pfisterer@gmx.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published
 *   by the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "common.h"
#include "org_tritonus_lowlevel_alsa_Alsa.h"



/*
 * Class:     org_tritonus_lowlevel_alsa_Alsa
 * Method:    getStringError
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_tritonus_lowlevel_alsa_Alsa_getStringError
(JNIEnv *env, jclass cls UNUSED, jint nErrnum)
{
	jstring	strError;
	const char*	err;

	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_Alsa_getStringError(): begin\n"); }
	err = snd_strerror(nErrnum);
	if (err == NULL)
	{
		throwRuntimeException(env, "snd_strerror() failed");
	}
	// strError = env->NewStringUTF(err);
	strError = (*env)->NewStringUTF(env, err);
	if (strError == NULL)
	{
		throwRuntimeException(env, "NewStringUTF() failed");
	}
	if (debug_flag) { (void) fprintf(debug_file, "Java_org_tritonus_lowlevel_alsa_Alsa_getStringError(): end\n"); }
	return strError;
}



/*** org_tritonus_lowlevel_alsa_Alsa.cc ***/
