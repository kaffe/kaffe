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
#include "InetAddress.h"
#include "InetAddressImpl.h"
#include "nets.h"

#define	HOSTNMSZ	80

/*
 * Get localhost name.
 */
struct Hjava_lang_String*
java_net_InetAddressImpl_getLocalHostName(struct Hjava_net_InetAddressImpl* none)
{
	char hostname[HOSTNMSZ];

	if (gethostname(hostname, HOSTNMSZ-1) < 0) {
		strcpy("localhost", hostname);
	}
	return (makeJavaString(hostname, strlen(hostname)));
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

	javaString2CString(str, name, sizeof(name));

	ent = gethostbyname(name);
	if (ent == 0) {
		SignalError("java.net.UnknownHostException", SYS_HERROR);
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

	javaString2CString(str, name, sizeof(name));

	ent = gethostbyname(name);
	if (ent == 0) {
		SignalError("java.net.UnknownHostException", SYS_HERROR);
	}

	for (alength = 0; ent->h_addr_list[alength]; alength++)
		;

	array = (HArrayOfInt*)AllocArray(alength, TYPE_Int);
	assert(array != 0);

	for (i = 0; i < alength; i++) {
		/* Copy in the network address */
		unhand(array)->body[i] = ntohl(*(jint*)ent->h_addr_list[i]);
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

	addr = htonl(addr);
	ent = gethostbyaddr((char*)&addr, sizeof(jint), AF_INET);
	if (ent == 0) {
		SignalError("java.net.UnknownHostException", SYS_HERROR);
	}

	return (makeJavaString((char*)ent->h_name, strlen(ent->h_name)));
}

/*
 * Return the inet address family.
 */
jint
java_net_InetAddressImpl_getInetFamily(struct Hjava_net_InetAddressImpl* none)
{
	return (AF_INET);
}
