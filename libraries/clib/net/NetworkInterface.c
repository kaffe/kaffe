/*
 * NetworkInterface.c
 * Native implementation of java.net.NetworkInterface functions.
 *
 * Copyright (c) 2002 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-net.h"
#include "config-io.h"
#include "config-hacks.h"
#include <native.h>
#include "java_net_NetworkInterface.h"
#include "nets.h"
#include <arpa/inet.h>
#include <jsyscall.h>
#include "../../../kaffe/kaffevm/debug.h"

#include <ifaddrs.h>

struct Hkaffe_util_Ptr *
java_net_NetworkInterface_detectInterfaces(void)
{
	struct Hkaffe_util_Ptr *retval = NULL;
	struct ifaddrs *ifa;
	errorInfo einfo;

	if( getifaddrs(&ifa) == 0 )
	{
		retval = (struct Hkaffe_util_Ptr *)ifa;
	}
	else
	{
		switch( errno )
		{
		case ENOMEM:
			postOutOfMemory(&einfo);
			break;
		case ENOSYS:
			postExceptionMessage(
				&einfo,
				"kaffe.util.NotImplemented",
				"OS doesn't support getifaddrs()");
			break;
		default:
			postExceptionMessage(
				&einfo,
				"java.net.SocketException",
				"%s",
				SYS_ERROR(errno));
			break;
		}
		throwError(&einfo);
	}
	return( retval );
}

void
java_net_NetworkInterface_freeInterfaces(struct Hkaffe_util_Ptr *jifa)
{
	if( jifa )
	{
		freeifaddrs((struct ifaddrs *)jifa);
	}
}

struct Hkaffe_util_Ptr *
java_net_NetworkInterface_getNext(struct Hkaffe_util_Ptr *jifa)
{
	struct Hkaffe_util_Ptr *retval = NULL;
	
	if( jifa )
	{
		struct ifaddrs *ifa = (struct ifaddrs *)jifa;
		
		retval = (struct Hkaffe_util_Ptr *)ifa->ifa_next;
	}
	return( retval );
}

struct Hjava_lang_String *
java_net_NetworkInterface_getName(struct Hkaffe_util_Ptr *jifa)
{
	struct Hjava_lang_String *retval = NULL;

	if( jifa )
	{
		struct ifaddrs *ifa = (struct ifaddrs *)jifa;
		
		retval = stringC2Java(ifa->ifa_name);
	}
	return( retval );
}

struct Hjava_lang_String *
java_net_NetworkInterface_getIPv4Address(struct Hkaffe_util_Ptr *jifa)
{
	struct Hjava_lang_String *retval = NULL;

	if( jifa )
	{
		struct ifaddrs *ifa = (struct ifaddrs *)jifa;
		struct sockaddr *sa;

		if( (sa = ifa->ifa_addr) )
		{
			char addr[20];
			
			switch( sa->sa_family )
			{
			case AF_INET:
				inet_ntop(sa->sa_family,
					  &((struct sockaddr_in *)sa)->
					  sin_addr,
					  addr,
					  20);
				retval = stringC2Java(addr);
				break;
			default:
				/* XXX What to do? */
				break;
			}
		}
	}
	return( retval );
}
