/*
 * java.net.InetAddress.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-net.h"
#include "config-io.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/object.h"
#include <native.h>
#include "../../../kaffe/kaffevm/itypes.h"
#include "../../../kaffe/kaffevm/stringSupport.h"
#include "../../../kaffe/kaffevm/support.h"
#include "java_net_InetAddress.h"
#include "java_net_InetAddressImpl.h"
#include "nets.h"
#include "jsyscall.h"

#define	HOSTNMSZ	80

/*
 * Get localhost name.
 */
struct Hjava_lang_String*
java_net_InetAddressImpl_getLocalHostName(struct Hjava_net_InetAddressImpl* none)
{
	char hostname[HOSTNMSZ];

	if (gethostname(hostname, HOSTNMSZ-1) < 0) {
		strcpy(hostname, "localhost");
	}
	return (checkPtr(stringC2Java(hostname)));
}

/*
 * Provide one of my local address (I guess).
 */
void
java_net_InetAddressImpl_makeAnyLocalAddress(struct Hjava_net_InetAddressImpl* none, struct Hjava_net_InetAddress* this)
{
	unhand(this)->hostName = 0;
	unhand(this)->address = INADDR_ANY;
	unhand(this)->family = AF_INET;
}

/*
 * Convert a hostname to the primary host address.
 */
jint
java_net_InetAddressImpl_lookupHostAddr(struct Hjava_net_InetAddressImpl* none, struct Hjava_lang_String* str)
{
	char name[MAXHOSTNAME];
	struct hostent* ent;
	int rc;

	stringJava2CBuf(str, name, sizeof(name));

	rc = KGETHOSTBYNAME(name, &ent);
	if (rc) {
		if (ent == (void *)-1) {
			SignalError("java.io.IOException", SYS_ERROR(rc));
		}
		else {
			SignalErrorf("java.net.UnknownHostException", "%s: %s",
				     SYS_HERROR(rc), name);
		}
	}
	return (ntohl(*(jint*)ent->h_addr_list[0]));
}

/*
 * Convert a hostname to an array of host addresses.
 */
HArrayOfInt*
java_net_InetAddressImpl_lookupAllHostAddr(struct Hjava_net_InetAddressImpl* none, struct Hjava_lang_String* str)
{
	char name[MAXHOSTNAME];
	struct hostent* ent;
	HArrayOfInt* array;
	int i;
	int alength;
	int rc;

	stringJava2CBuf(str, name, sizeof(name));

	rc = KGETHOSTBYNAME(name, &ent);
	if (rc) {
		if (ent == (void *)-1) {
			SignalError("java.io.IOException", SYS_ERROR(rc));
		}
		else {
			SignalErrorf("java.net.UnknownHostException", "%s: %s",
				     SYS_HERROR(rc), name);
		}
	}

	for (alength = 0; ent->h_addr_list[alength]; alength++)
		;

	array = (HArrayOfInt*)AllocArray(alength, TYPE_Int);
	assert(array != 0);

	for (i = 0; i < alength; i++) {
		/* Copy in the network address */
		unhand_array(array)->body[i] = ntohl(*(jint*)ent->h_addr_list[i]);
	}

	return (array);
}

/*
 * Convert a network order address into the hostname.
 */
struct Hjava_lang_String*
java_net_InetAddressImpl_getHostByAddr(struct Hjava_net_InetAddressImpl* none, jint addr)
{
	struct hostent* ent;
	int rc;

	addr = htonl(addr);
	rc = KGETHOSTBYADDR((char*)&addr, sizeof(jint), AF_INET, &ent);
	if (rc) {
		char ipaddr[16];
		sprintf(ipaddr, "%3d.%3d.%3d.%3d",
			/* XXX Make sure it is the right endiannes */
			(addr >> 24) & 0xff,
			(addr >> 16) & 0xff,
			(addr >>  8) & 0xff,
			(addr)       & 0xff);
		SignalErrorf("java.net.UnknownHostException", "%s: %s",
			SYS_HERROR(rc), ipaddr);
	}

	return (checkPtr(stringC2Java((char*)ent->h_name)));
}

/*
 * Return the inet address family.
 */
jint
java_net_InetAddressImpl_getInetFamily(struct Hjava_net_InetAddressImpl* none)
{
	return (AF_INET);
}
