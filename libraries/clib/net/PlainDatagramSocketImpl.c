/*
 * java.net.PlainDatagramSocketImpl.c
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
#include "config-hacks.h"
#include <native.h>
#include "java_lang_Integer.h"
#include "java_io_FileDescriptor.h"
#include "java_io_InterruptedIOException.h"
#include "java_net_DatagramPacket.h"
#include "java_net_NetworkInterface.h"
#include "java_net_SocketAddress.h"
#include "gnu_java_net_PlainDatagramSocketImpl.h"
#include "java_net_InetSocketAddress.h"
#include "java_net_InetAddress.h"
#include "java_net_SocketOptions.h"
#include "java_util_Vector.h"
#include "nets.h"
#include <jsyscall.h>
#include "../../../kaffe/kaffevm/debug.h"
#include "../../../kaffe/kaffevm/itypes.h"
#include "../../../kaffe/kaffevm/exception.h"

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
#ifdef SO_REUSEADDR
    { java_net_SocketOptions_SO_REUSEADDR,	SOL_SOCKET,	SO_REUSEADDR },
#endif
  };

#ifdef KAFFE_VMDEBUG
static const struct {
    int opt;
    const char *name;
} optionNames[] = {
#ifdef SO_SNDBUF
    { java_net_SocketOptions_SO_SNDBUF,		"SO_SNDBUF" },
#endif
#ifdef SO_RCVBUF
    { java_net_SocketOptions_SO_RCVBUF,		"SO_RCVBUF" },
#endif
#ifdef SO_REUSEADDR
    { java_net_SocketOptions_SO_REUSEADDR,	"SO_REUSEADDR" },
#endif
};
#endif /*KAFFE_VMDEBUG*/

#ifdef KAFFE_VMDEBUG
/* Generate a string for an inet addr (in host form). */
static char *
ip2str(jint addr) 
{
	static char addrbuf[16];
	unsigned int top = (addr >> 24) & 0xFF;
	unsigned int tmid = (addr >> 16) & 0xFF;
	unsigned int bmid = (addr >> 8) & 0xFF;
	unsigned int bottom = addr & 0xFF;

	sprintf(addrbuf, "%u.%u.%u.%u", top, tmid, bmid, bottom);
	return addrbuf;
}
#endif /* KAFFE_VMDEBUG */



/*
 * Create a datagram socket.
 */
void
gnu_java_net_PlainDatagramSocketImpl_datagramSocketCreate(struct Hgnu_java_net_PlainDatagramSocketImpl* this)
{
	int fd;
	int rc;

DBG(NATIVENET,
	dprintf("datagram_create(%p)\n", this);
)

	rc = KSOCKET(AF_INET, SOCK_DGRAM, 0, &fd);
	if (rc) {
		unhand(unhand(this)->fd)->nativeFd = -1;
		SignalError("java.net.SocketException", SYS_ERROR(rc));
	}
	unhand(unhand(this)->fd)->nativeFd = fd;
	unhand(this)->native_fd = fd;

DBG(NATIVENET,
	dprintf("datagram_create(%p) -> fd=%d\n", this, fd);
)

#if defined(SOL_SOCKET) && defined(SO_BROADCAST)
	/* On some systems broadcasting is off by default - enable it here */
	{
		int brd = 1;
		KSETSOCKOPT(fd, SOL_SOCKET, SO_BROADCAST, &brd, sizeof(brd));
		/* ignore return code */
	}
#endif
}

/*
 * Bind a port to the socket.
 */
void
gnu_java_net_PlainDatagramSocketImpl_bind(struct Hgnu_java_net_PlainDatagramSocketImpl* this, jint port, struct Hjava_net_InetAddress* laddr)
{
	int r;
	struct sockaddr_in addr;
	int alen;
	const int fd = unhand(unhand(this)->fd)->nativeFd;

DBG(NATIVENET,
	dprintf("datagram_bind(%p, %s, %d)\n", 
		this, ip2str(unhand(laddr)->address), port);
)

	memset(&addr, 0, sizeof(addr));
#if defined(BSD44)
	addr.sin_len = sizeof(addr);
#endif
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(unhand(laddr)->address);

	r = KBIND(fd, (struct sockaddr*)&addr, sizeof(addr));
	switch( r )
	{
	case 0:
		break;
	case EADDRNOTAVAIL:
	case EADDRINUSE:
	case EACCES:
		SignalError("java.net.BindException", SYS_ERROR(r));
		break;
	default:
		SignalError("java.net.SocketException", SYS_ERROR(r));
		break;
	}

	if (port == 0) {
		alen = sizeof(addr);
		r = KGETSOCKNAME(fd, (struct sockaddr*)&addr, &alen);
		if (r) {
			SignalError("java.net.SocketException", SYS_ERROR(r));
		}
		port = ntohs(addr.sin_port);
	}
	unhand(this)->localPort = port;

DBG(NATIVENET,
	dprintf("  datagram_bind(%p, %s, -) -> (localPort: %d)\n",
		this, ip2str(unhand(laddr)->address), port);
)
}

void
gnu_java_net_PlainDatagramSocketImpl_send(struct Hgnu_java_net_PlainDatagramSocketImpl* this, struct Hjava_net_DatagramPacket* pkt)
{
	int rc;
	ssize_t bsent;
	struct sockaddr_in addr;

DBG(NATIVENET,
	dprintf("datagram_send(%p, %p [%d bytes])\n",
		this, pkt, unhand(pkt)->length);
)

	memset(&addr, 0, sizeof(addr));
#if defined(BSD44)
	addr.sin_len = sizeof(addr);
#endif
	addr.sin_family = AF_INET;
	addr.sin_port = htons(unhand(pkt)->port);
	addr.sin_addr.s_addr = htonl(unhand(unhand(pkt)->address)->address);

DBG(NATIVENET,
	dprintf("  datagram_send() to %s:%d\n",
		ip2str(unhand(unhand(pkt)->address)->address),
		unhand(pkt)->port);
)

	rc = KSENDTO(unhand(unhand(this)->fd)->nativeFd,
		unhand_array(unhand(pkt)->buffer)->body, unhand(pkt)->length,
		0, (struct sockaddr*)&addr, sizeof(addr), &bsent);

DBG(NATIVENET,
	dprintf("  datagram_send() -> rc=%d bsent=%ld\n", rc, (long) bsent);
)

	if (rc) {
		SignalError("java.net.SocketException", SYS_ERROR(rc));
	}
}

jint
gnu_java_net_PlainDatagramSocketImpl_peek(struct Hgnu_java_net_PlainDatagramSocketImpl* this, struct Hjava_net_InetAddress* addr)
{
	ssize_t r;
	int rc;
	struct sockaddr_in saddr;
	int alen = sizeof(saddr);

	rc = KRECVFROM(unhand(unhand(this)->fd)->nativeFd,
		0, 0, MSG_PEEK, (struct sockaddr*)&saddr,
		&alen, NOTIMEOUT /* timeout */, &r);
	if (rc) {
		SignalError("java.net.SocketException", SYS_ERROR(rc));
	}

	unhand(addr)->address = ntohl(saddr.sin_addr.s_addr);

	return ((jint)r);
}

void
gnu_java_net_PlainDatagramSocketImpl_receive(struct Hgnu_java_net_PlainDatagramSocketImpl* this, struct Hjava_net_DatagramPacket* pkt)
{
	ssize_t r;
	int rc;
	struct sockaddr_in addr;
	int alen = sizeof(addr);
	HArrayOfByte *array_address;
	int i;
	int to_read, offset;

	assert(this != NULL);
	if (pkt == NULL || unhand(pkt)->buffer == NULL)
		SignalError("java.lang.NullPointerException", "null datagram packet");
	assert(unhand(pkt)->length <= unhand(unhand(pkt)->buffer)->length);

DBG(NATIVENET,
	dprintf("datagram_receive(%p, %p [%d bytes])\n",
		this, pkt, unhand(pkt)->length);
)

	/* Which port am I receiving from */
	addr.sin_port = htons(unhand(this)->localPort);

	/* XXX should assert (unhand(pkt)->length <= unhand_array(unhand(pkt)->buf)->length), no? */
        offset = unhand(pkt)->offset; 
	to_read = unhand(pkt)->length;
        do { 
	        rc = KRECVFROM(unhand(unhand(this)->fd)->nativeFd,
			       &(unhand_array(unhand(pkt)->buffer)->body)[offset],
			       to_read, 0, (struct sockaddr*)&addr,
			       &alen, unhand(this)->timeout, &r);
		switch( rc )
		{
		case 0:
		        break;
		case ETIMEDOUT: {
		        struct Hjava_io_InterruptedIOException* except;
		  
			except = (struct Hjava_io_InterruptedIOException *)
			  execute_java_constructor(
						   "java.net.SocketTimeoutException", 0, 0,
						   "(Ljava/lang/String;)V",
						   checkPtr(stringC2Java("Read timed out")));
			except->bytesTransferred = offset-unhand(pkt)->offset;
			
			throwException((struct Hjava_lang_Throwable*)except);
			break;
		}
		case EINTR:
		        break;
		default:
		        SignalError("java.net.SocketException", SYS_ERROR(rc));
			break;
		}
		to_read -= r;
		offset += r;
	} while (rc == EINTR);

	unhand(pkt)->length = r;
	unhand(pkt)->port = ntohs(addr.sin_port);
	
	array_address = (HArrayOfByte*)AllocArray(4, TYPE_Byte);
	for (i=0;i<4;i++)
	  unhand_array(array_address)->body[i] = (ntohl(addr.sin_addr.s_addr) >> (8*i)) & 0xFF;
	unhand(pkt)->address = (struct Hjava_net_InetAddress*)
	  execute_java_constructor("java/net/InetAddress", 0, 0, "([B)V",
				   array_address);
	unhand(unhand(pkt)->address)->address = ntohl(addr.sin_addr.s_addr);
	/* zero out hostname to overwrite old name which does not match
	 * the new address from which the packet came.
	 */
	unhand(unhand(pkt)->address)->hostName = 0;

DBG(NATIVENET,
	dprintf("  datagram_receive(%p, %p) -> from %s:%d; brecv=%ld\n",
		this, pkt, ip2str(ntohl(addr.sin_addr.s_addr)),
		ntohs(addr.sin_port), (long) r);
)
}

/*
 * Close the socket.
 */
void
gnu_java_net_PlainDatagramSocketImpl_datagramSocketClose(struct Hgnu_java_net_PlainDatagramSocketImpl* this)
{
	int r;

DBG(NATIVENET,
	dprintf("datagram_close(%p)\n", this);
)

	if (unhand(unhand(this)->fd)->nativeFd != -1) {
		r = KSOCKCLOSE(unhand(unhand(this)->fd)->nativeFd);
		unhand(unhand(this)->fd)->nativeFd = -1;
		if (r) {
			SignalError("java.net.SocketException", SYS_ERROR(r));
		}
	}
}


void
gnu_java_net_PlainDatagramSocketImpl_socketSetOption(struct Hgnu_java_net_PlainDatagramSocketImpl* this, jint opt, struct Hjava_lang_Object* arg)
{
	struct Hjava_net_InetAddress* addrp;
	int k, v, r;

	/* Do easy cases */
	for (k = 0; k < sizeof(socketOptions) / sizeof(*socketOptions); k++) {
		if (opt == socketOptions[k].jopt) {
			v = unhand((struct Hjava_lang_Integer*)arg)->value;
			r = KSETSOCKOPT(unhand(unhand(this)->fd)->nativeFd,
				socketOptions[k].level, socketOptions[k].copt,
				&v, sizeof(v));
			if (r) {
				SignalError("java.net.SocketException",
					SYS_ERROR(r));
			}
			return;
		}
	}

	switch(opt) {
	case java_net_SocketOptions_IP_MULTICAST_IF:
#if defined(IP_MULTICAST_IF)
		addrp = (struct Hjava_net_InetAddress*)arg;
		{
			struct in_addr ia;
			
			ia.s_addr = htonl(unhand(addrp)->address);
			r = KSETSOCKOPT(unhand(unhand(this)->fd)->nativeFd,
					IPPROTO_IP,
					IP_MULTICAST_IF,
					&ia,
					sizeof(ia));
			if (r) {
				SignalError("java.net.SocketException",
					    SYS_ERROR(r));
			}
#else
			SignalError("java.net.SocketException",
				    "IP_MULTICAST_IF is not supported");
#endif
		}
		break;

	case java_net_SocketOptions_SO_BINDADDR:
		SignalError("java.net.SocketException",
			"Read-only socket option");
		break;
	case java_net_SocketOptions_SO_TIMEOUT: /* JAVA takes care */
	default:
		SignalError("java.net.SocketException",
			"Unimplemented socket option");
	} 
}

jint
gnu_java_net_PlainDatagramSocketImpl_socketGetOption(struct Hgnu_java_net_PlainDatagramSocketImpl* this, jint opt)
{
	int k, r, v;
	int vsize = sizeof(v);
	struct sockaddr_in addr;
	int alen = sizeof(addr);

	/* Do easy cases */
	for (k = 0; k < sizeof(socketOptions) / sizeof(*socketOptions); k++) {
		if (opt == socketOptions[k].jopt) {
			r = KGETSOCKOPT(unhand(unhand(this)->fd)->nativeFd,
				socketOptions[k].level, socketOptions[k].copt,
				&v, &vsize);
			if (r) {
				SignalError("java.net.SocketException",
					SYS_ERROR(r));
			}
			return v;
		}
	}

	switch(opt) {
	case java_net_SocketOptions_SO_BINDADDR:
		r = KGETSOCKNAME(unhand(unhand(this)->fd)->nativeFd,
			(struct sockaddr*)&addr, &alen);
		if (r) {
			SignalError("java.net.SocketException", SYS_ERROR(r));
		}
		r = htonl(addr.sin_addr.s_addr);
		break;
#if defined(IP_MULTICAST_IF)
	case java_net_SocketOptions_IP_MULTICAST_IF:
		r = KGETSOCKOPT(unhand(unhand(this)->fd)->nativeFd,
			IPPROTO_IP, IP_MULTICAST_IF, &addr, &alen);
		if (r) {
			SignalError("java.net.SocketException", SYS_ERROR(r));
			return (0);	/* NOT REACHED, avoid warning */
		}
		r = ntohl(addr.sin_addr.s_addr);
		break;
#endif
	case java_net_SocketOptions_SO_TIMEOUT: /* JAVA takes care */
	default:
		SignalError("java.net.SocketException",
			"Unimplemented socket option");
	} 
	return r;
}

/*
 * Join multicast group
 */
void
gnu_java_net_PlainDatagramSocketImpl_join(struct Hgnu_java_net_PlainDatagramSocketImpl* this, struct Hjava_net_InetAddress* laddr)
{
#if defined(IP_ADD_MEMBERSHIP)
	int r;
	struct ip_mreq ipm;

	ipm.imr_multiaddr.s_addr = htonl(unhand(laddr)->address);
	ipm.imr_interface.s_addr = htonl(INADDR_ANY);

	r = KSETSOCKOPT(unhand(unhand(this)->fd)->nativeFd,
		IPPROTO_IP, IP_ADD_MEMBERSHIP, &ipm, sizeof(ipm));
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
#else
	SignalError("java.net.SocketException",
		"IP_ADD_MEMBERSHIP not supported");
#endif
}

/*
 * leave multicast group
 */
void
gnu_java_net_PlainDatagramSocketImpl_leave(struct Hgnu_java_net_PlainDatagramSocketImpl* this, struct Hjava_net_InetAddress* laddr)
{
#if defined(IP_DROP_MEMBERSHIP)
	int r;
	struct ip_mreq ipm;

	ipm.imr_multiaddr.s_addr = htonl(unhand(laddr)->address);
	ipm.imr_interface.s_addr = htonl(INADDR_ANY);

	r = KSETSOCKOPT(unhand(unhand(this)->fd)->nativeFd,
		IPPROTO_IP, IP_DROP_MEMBERSHIP, &ipm, sizeof(ipm));
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
#else
	SignalError("java.net.SocketException",
		"IP_DROP_MEMBERSHIP not supported");
#endif
}

/*
 * Join multicast group
 */
void
gnu_java_net_PlainDatagramSocketImpl_joinGroup(struct Hgnu_java_net_PlainDatagramSocketImpl* this, struct Hjava_net_SocketAddress *jsa, struct Hjava_net_NetworkInterface *jni)
{
#if defined(IP_ADD_MEMBERSHIP)
	struct Hjava_net_InetSocketAddress *jisa;
	struct ip_mreq ipm;
	int r;

	jisa = (struct Hjava_net_InetSocketAddress *)jsa;
	
	ipm.imr_multiaddr.s_addr = htonl(unhand(unhand(jisa)->addr)->address);
	if( jni && obj_length(unhand(unhand(jni)->inetAddresses)->elementData) != 0)
	{
	  struct Hjava_net_InetAddress* if_addr = (struct Hjava_net_InetAddress *)
	    unhand_object_array(unhand(unhand(jni)->inetAddresses)->elementData);
	  
	  ipm.imr_interface.s_addr =
	    htonl(unhand(if_addr)->address);
	}
	else
	{
		ipm.imr_interface.s_addr = htonl(INADDR_ANY);
	}

	r = KSETSOCKOPT(unhand(unhand(this)->fd)->nativeFd,
			IPPROTO_IP,
			IP_ADD_MEMBERSHIP,
			&ipm,
			sizeof(ipm));
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
#else
	SignalError("java.net.SocketException",
		"IP_ADD_MEMBERSHIP not supported");
#endif
}

/*
 * Leave multicast group
 */
void
gnu_java_net_PlainDatagramSocketImpl_leaveGroup(struct Hgnu_java_net_PlainDatagramSocketImpl* this, struct Hjava_net_SocketAddress *jsa, struct Hjava_net_NetworkInterface *jni)
{
#if defined(IP_ADD_MEMBERSHIP)
	struct Hjava_net_InetSocketAddress *jisa;
	struct ip_mreq ipm;
	int r;

	jisa = (struct Hjava_net_InetSocketAddress *)jsa;
	
	ipm.imr_multiaddr.s_addr = htonl(unhand(unhand(jisa)->addr)->address);
	if( jni && obj_length(unhand(unhand(jni)->inetAddresses)->elementData) != 0)
	{
	        struct Hjava_net_InetAddress* if_addr = (struct Hjava_net_InetAddress *)
		  unhand_object_array(unhand(unhand(jni)->inetAddresses)->elementData);
	  
		ipm.imr_interface.s_addr =
		  htonl(unhand(if_addr)->address);
	}
	else
	{
		ipm.imr_interface.s_addr = htonl(INADDR_ANY);
	}

	r = KSETSOCKOPT(unhand(unhand(this)->fd)->nativeFd,
			IPPROTO_IP,
			IP_DROP_MEMBERSHIP,
			&ipm,
			sizeof(ipm));
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
#else
	SignalError("java.net.SocketException",
		"IP_ADD_MEMBERSHIP not supported");
#endif
}

/*
 * set multicast-TTL
 */
void
gnu_java_net_PlainDatagramSocketImpl_setTTL(struct Hgnu_java_net_PlainDatagramSocketImpl* this, jbool ttl)
{
#if defined(IP_MULTICAST_TTL)
	int r;
	unsigned char v = (unsigned char)ttl;

	r = KSETSOCKOPT(unhand(unhand(this)->fd)->nativeFd,
		IPPROTO_IP, IP_MULTICAST_TTL, &v, sizeof(v));
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
#else
	SignalError("java.net.SocketException",
		"IP_MULTICAST_TTL not supported");
#endif
}

/*
 * get multicast-TTL
 */
jbyte
gnu_java_net_PlainDatagramSocketImpl_getTTL(struct Hgnu_java_net_PlainDatagramSocketImpl* this)
{
#if defined(IP_MULTICAST_TTL)
	unsigned char v;
	int s;
	int r;

	r = KGETSOCKOPT(unhand(unhand(this)->fd)->nativeFd,
		IPPROTO_IP, IP_MULTICAST_TTL, &v, &s);
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
	return (jbyte)v;
#else
	SignalError("java.net.SocketException",
		"IP_MULTICAST_TTL not supported");
#endif
}
