/*
 * java.net.SocketInputStream.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include <native.h>
#include "../native/FileDescriptor.h"
#include "../native/FileInputStream.h"
#include "SocketImpl.h"
#include "SocketInputStream.h"
#include "nets.h"
#include <jsyscall.h>
#include "../../../kaffe/kaffevm/support.h"

jint
java_net_SocketInputStream_socketRead(struct Hjava_net_SocketInputStream* this, HArrayOfByte* buf, jint offset, jint len)
{
	int r;

	r = sockread(unhand(unhand(unhand(this)->impl)->fd)->fd, &unhand(buf)->body[offset], len);
	if (r < 0) {
		SignalError("java.io.IOException", SYS_ERROR);
	}
	else if (r == 0) {
		return (-1);	/* EOF */
	}
	else {
		return (r);
	}
}
