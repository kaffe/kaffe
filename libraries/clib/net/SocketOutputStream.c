/*
 * java.net.SocketOutputStream.c
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
#include "config-io.h"
#include <native.h>
#include "../native/FileDescriptor.h"
#include "../native/FileOutputStream.h"
#include "SocketImpl.h"
#include "SocketOutputStream.h"
#include "nets.h"
#include <jsyscall.h>

void
java_net_SocketOutputStream_socketWrite(struct Hjava_net_SocketOutputStream* this, HArrayOfByte* buf, jint offset, jint len)
{
	int r;

	if (unhand(unhand(unhand(this)->impl)->fd)->fd < 0) {
		return;
	}
	r = sockwrite(unhand(unhand(unhand(this)->impl)->fd)->fd, &unhand(buf)->body[offset], len);
	if (r < 0) {
		SignalError("java.io.IOException", SYS_ERROR);
	}
}
