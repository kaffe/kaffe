/*
 * java.net.InetAddress.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include <netinet/in.h>
#include "config-io.h"
#include <netdb.h>
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/object.h"
#include <native.h>
#include "../../../kaffe/kaffevm/itypes.h"
#include "InetAddress.h"
#include "InetAddressImpl.h"
#include "nets.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

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
	unhand(this)->address = htonl(INADDR_ANY);
	unhand(this)->family = AF_INET;
}

/*
 * Convert a hostname to the primary host address.
 */
HArrayOfByte*
java_net_InetAddressImpl_lookupHostAddr(struct Hjava_net_InetAddressImpl* none, struct Hjava_lang_String* str)
{
	char name[MAXHOSTNAME];
	struct hostent* ent;
	Hjava_lang_Object* obj;

	javaString2CString(str, name, sizeof(name));

	ent = gethostbyname(name);
	if (ent == 0) {
		SignalError("java.net.UnknownHostException", SYS_HERROR);
	}

	/* Copy in the network address */
	obj = AllocArray(sizeof(jint), TYPE_Byte);
	assert(obj != 0);
	*(jint*)ARRAY_DATA(obj) = *(jint*)ent->h_addr_list[0];

	return ((HArrayOfByte*)obj);
}

/*
 * Convert a hostname to an array of host addresses.
 */
HArrayOfArray* /* HArrayOfArrayOfBytes */
java_net_InetAddressImpl_lookupAllHostAddr(struct Hjava_net_InetAddressImpl* none, struct Hjava_lang_String* str)
{
	char name[MAXHOSTNAME];
	struct hostent* ent;
	Hjava_lang_Object* obj;
	Hjava_lang_Object* array;
	int i;
	int alength;

	javaString2CString(str, name, sizeof(name));

	ent = gethostbyname(name);
	if (ent == 0) {
		SignalError("java.net.UnknownHostException", SYS_HERROR);
	}

	for (alength = 0; ent->h_addr_list[alength]; alength++)
	  ;

	array = AllocObjectArray(alength, "[[B");
	assert(array != 0);

	for (i = 0; i < alength; i++) {
		/* Copy in the network address */
		obj = AllocArray(sizeof(jint), TYPE_Byte);
		assert(obj != 0);
		*(jint*)ARRAY_DATA(obj) = *(jint*)ent->h_addr_list[i];
		OBJARRAY_DATA(array)[i] = obj;
	}

	return ((HArrayOfArray*)array);
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
