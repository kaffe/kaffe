/*
 * java.net.PlainSocketImpl.c
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
#include <netinet/in.h>
#if defined(HAVE_NETINET_TCP_H)
#include <netinet/tcp.h>
#endif
#include <native.h>
#include "../native/FileDescriptor.h"
#include "../native/Integer.h"
#include "SocketImpl.h"
#include "InetAddress.h"
#include "PlainSocketImpl.h"
#include "SocketOptions.h"
#include "nets.h"
#include <jsyscall.h>

/*
 * Supported socket options
 */
  static const struct {
	  int jopt;
	  int level;
	  int copt;
  } socketOptions[] = {
#ifdef SO_SNDBUF
    { java_net_SocketOptions_SO_SNDBUF,		SOL_SOCKET,	SO_SNDBUF },
#endif
#ifdef SO_RCVBUF
    { java_net_SocketOptions_SO_RCVBUF,		SOL_SOCKET,	SO_RCVBUF },
#endif
#ifdef SO_LINGER
    { java_net_SocketOptions_SO_LINGER,		SOL_SOCKET,	SO_LINGER },
#endif
#ifdef SO_REUSEADDR
    { java_net_SocketOptions_SO_REUSEADDR,	SOL_SOCKET,	SO_REUSEADDR },
#endif
#ifdef TCP_NODELAY
    { java_net_SocketOptions_TCP_NODELAY,	IPPROTO_TCP,	TCP_NODELAY },
#endif
  };

/*
 * Create a stream or datagram socket.
 */
void
java_net_PlainSocketImpl_socketCreate(struct Hjava_net_PlainSocketImpl* this, jbool stream)
{
	int fd;
	int type;

	if (stream == 0) {
		type = SOCK_DGRAM;
	}
	else {
		type = SOCK_STREAM;
	}

	fd = socket(AF_INET, type, 0);
	if (fd < 0) {
		SignalError("java.io.IOException", SYS_ERROR);
	}
	unhand(unhand(this)->fd)->fd = fd;
}

/*
 * Connect the socket to someone.
 */
void
java_net_PlainSocketImpl_socketConnect(struct Hjava_net_PlainSocketImpl* this, struct Hjava_net_InetAddress* daddr, jint dport)
{
	int fd;
	int r;
	struct sockaddr_in addr;
	size_t alen;

#if defined(BSD44)
	addr.sin_len = sizeof(addr);
#endif
	addr.sin_family = AF_INET;
	addr.sin_port = htons(dport);
	addr.sin_addr.s_addr = htonl(unhand(daddr)->address);

	fd = unhand(unhand(this)->fd)->fd;
	r = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (r < 0) {
		SignalError("java.io.IOException", SYS_ERROR);
	}

	/* Enter information into socket object */
	alen = sizeof(addr);
	r = getsockname(fd, (struct sockaddr*)&addr, &alen);
	if (r < 0) {
		SignalError("java.io.IOException", SYS_ERROR);
	}

	unhand(this)->address = daddr;
	unhand(this)->port = dport;
	unhand(this)->localport = ntohs(addr.sin_port);
}

/*
 * Bind this socket to an address.
 */
void
java_net_PlainSocketImpl_socketBind(struct Hjava_net_PlainSocketImpl* this, struct Hjava_net_InetAddress* laddr, jint lport)
{
	int r;
	struct sockaddr_in addr;
	int fd;
	int on = 1;
	size_t alen;

#if defined(BSD44)
	addr.sin_len = sizeof(addr);
#endif
	addr.sin_family = AF_INET;
	addr.sin_port = htons(lport);
	addr.sin_addr.s_addr = htonl(unhand(laddr)->address);

	fd = unhand(unhand(this)->fd)->fd;

	/* Allow rebinding to socket - ignore errors */
	(void)setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
	r = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (r < 0) {
		SignalError("java.io.IOException", SYS_ERROR);
	}

	/* Enter information into socket object */
	unhand(this)->address = laddr;
	if (lport == 0) {
		alen = sizeof(addr);
		r = getsockname(fd, (struct sockaddr*)&addr, &alen);
		if (r < 0) {
			SignalError("java.io.IOException", SYS_ERROR);
		}
		lport = ntohs(addr.sin_port);
	}
	unhand(this)->localport = lport;
}

/*
 * Turn this socket into a listener.
 */
void
java_net_PlainSocketImpl_socketListen(struct Hjava_net_PlainSocketImpl* this, jint count)
{
	int r;

	r = listen(unhand(unhand(this)->fd)->fd, count);
	if (r < 0) {
		SignalError("java.io.IOException", SYS_ERROR);
	}
}

/*
 * Accept a connection.
 */
void
java_net_PlainSocketImpl_socketAccept(struct Hjava_net_PlainSocketImpl* this, struct Hjava_net_SocketImpl* sock)
{
	int r;
	size_t alen;
	struct sockaddr_in addr;

	alen = sizeof(addr);
#if defined(BSD44)
	addr.sin_len = sizeof(addr);
#endif
	addr.sin_family = AF_INET;
	addr.sin_port = htons(unhand(sock)->localport);
	addr.sin_addr.s_addr = unhand(unhand(sock)->address)->address;

	r = accept(unhand(unhand(this)->fd)->fd, (struct sockaddr*)&addr, &alen);
	if (r < 0) {
		SignalError("java.io.IOException", SYS_ERROR);
	}
	unhand(unhand(sock)->fd)->fd = r;

	/* Enter information into socket object */
	alen = sizeof(addr);
	r = getpeername(r, (struct sockaddr*)&addr, &alen);
	if (r < 0) {
		SignalError("java.io.IOException", SYS_ERROR);
	}

	unhand(unhand(sock)->address)->address = ntohl(addr.sin_addr.s_addr);
	unhand(sock)->port = ntohs(addr.sin_port);
}

/*
 * Return how many bytes can be read without blocking.
 */
jint
java_net_PlainSocketImpl_socketAvailable(struct Hjava_net_PlainSocketImpl* this)
{
	int r;
	jint len;

#if defined(HAVE_IOCTL) && defined(FIONREAD)
	r = ioctl(unhand(unhand(this)->fd)->fd, FIONREAD, &len);
	if (r < 0) {
		SignalError("java.io.IOException", SYS_ERROR);
	}
#else
	/* This uses select() to work out if we can read - but what
	 * happens at the end of file?
	 */
	static struct timeval tm = { 0, 0 };
	int fd;
	fd_set rd;

	fd = unhand(unhand(this)->fd)->fd;
	FD_ZERO(&rd);
	FD_SET(fd, &rd);
	r = select(fd+1, &rd, NULL, NULL, &tm);
	if (r == 1) {
		len = 1;
	}
	else {
		len = 0;
	}
#endif
	return (len);
}

/*
 * Close this socket.
 */
void
java_net_PlainSocketImpl_socketClose(struct Hjava_net_PlainSocketImpl* this)
{
	int r;

	if (unhand(unhand(this)->fd)->fd != -1) {
		r = close(unhand(unhand(this)->fd)->fd);
		unhand(unhand(this)->fd)->fd = -1;
		if (r < 0) {
			SignalError("java.io.IOException", SYS_ERROR);
		}
	}
}

void
java_net_PlainSocketImpl_initProto(struct Hjava_net_PlainSocketImpl* this)
{
	/* ??? */
}

void
java_net_PlainSocketImpl_socketSetOption(struct Hjava_net_PlainSocketImpl* this, jint opt, struct Hjava_lang_Object* arg)
{
	int k, r, v;

	/* Do easy cases */
	for (k = 0; k < sizeof(socketOptions) / sizeof(*socketOptions); k++) {
		if (opt == socketOptions[k].jopt) {
			v = unhand((struct Hjava_lang_Integer*)arg)->value;
			r = setsockopt(unhand(unhand(this)->fd)->fd,
				socketOptions[k].level, socketOptions[k].copt,
				&v, sizeof(v));
			if (r < 0) {
				SignalError("java.net.SocketException", SYS_ERROR);    
			}
			return;
		}
	}

	/* Do harder cases */
	switch(opt) {
	case java_net_SocketOptions_SO_BINDADDR:
		SignalError("java.net.SocketException", "Read-only socket option");    
		break;
	case java_net_SocketOptions_SO_TIMEOUT: /* java takes care */
	case java_net_SocketOptions_IP_MULTICAST_IF:
	default:
		SignalError("java.net.SocketException", "Unimplemented socket option");    
		break;
	} 
}

jint
java_net_PlainSocketImpl_socketGetOption(struct Hjava_net_PlainSocketImpl* this, jint opt)
{
	struct sockaddr_in addr;
	int alen = sizeof(addr);
	int k, r, v;
	int vsize = sizeof(v);

	/* Do easy cases */
	for (k = 0; k < sizeof(socketOptions) / sizeof(*socketOptions); k++) {
		if (opt == socketOptions[k].jopt) {
			r = getsockopt(unhand(unhand(this)->fd)->fd,
				socketOptions[k].level, socketOptions[k].copt,
				&v, &vsize);
			if (r < 0) {
				SignalError("java.net.SocketException", SYS_ERROR);    
			}
			return v;
		}
	}

	/* Do harder cases */
	switch(opt) {
	case java_net_SocketOptions_SO_BINDADDR:
		r = getsockname(unhand(unhand(this)->fd)->fd,
			(struct sockaddr*)&addr, &alen);
		if (r < 0) {
			SignalError("java.net.SocketException", SYS_ERROR);    
		}
		r = htonl(addr.sin_addr.s_addr);
		break;
	case java_net_SocketOptions_IP_MULTICAST_IF:
	case java_net_SocketOptions_SO_TIMEOUT: /* java takes care */
	default:
		SignalError("java.net.SocketException", "Unimplemented socket option");    
	} 
	return (r);
}
