/*
 * java.net.PlainDatagramSocketImpl.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 * Copyright (c) 2003
 *       Kaffe's team.
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
#include "support.h"
#include "java_lang_Integer.h"
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
#include "Arrays.h"
#include "debug.h"
#include "itypes.h"
#include "exception.h"

#include "dummyin6.h"

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

#if defined(KAFFE_VMDEBUG) && !defined(NDEBUG)
/* Generate a string for an inet addr (in host form). */
static char *
ip2str(uint32 addr) 
{
	static char addrbuf[16];
	unsigned int top = (addr >> 24) & 0xFF;
	unsigned int tmid = (addr >> 16) & 0xFF;
	unsigned int bmid = (addr >> 8) & 0xFF;
	unsigned int bottom = addr & 0xFF;

	sprintf(addrbuf, "%u.%u.%u.%u", top, tmid, bmid, bottom);
	return addrbuf;
}

#if defined(HAVE_STRUCT_SOCKADDR_IN6)
/* Generate a string for an inet6 addr (in host form). */
static char *
ip62str(struct in6_addr *addr) 
{
	static char addrbuf[255];
	int i, count;

	for (count=0,i=0;i<16;i++) {
	    count += sprintf(&addrbuf[count], i == 0 && addr->s6_addr[i] != 0 ? "%x" : addr->s6_addr[i] != 0 ? "%x:" : i != 0 ? ":" : "", addr->s6_addr[i]);
	}
	return addrbuf;
}
#endif /* defined(HAVE_STRUCT_SOCKADDR_IN6) */
#endif /* defined(KAFFE_VMDEBUG) && !defined(NDEBUG) */

/*
 * Create a datagram socket.
 */
void
gnu_java_net_PlainDatagramSocketImpl_datagramSocketCreate(struct Hgnu_java_net_PlainDatagramSocketImpl* obj)
{
	int fd;
	int rc;

DBG(NATIVENET,
	dprintf("datagram_create(%p)\n", obj);
);

	rc = KSOCKET(AF_INET, SOCK_DGRAM, 0, &fd);
	if (rc) {
		SignalError("java.net.SocketException", SYS_ERROR(rc));
	}
	unhand(obj)->native_fd = fd;

DBG(NATIVENET,
	dprintf("datagram_create(%p) -> fd=%d\n", obj, fd);
);

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
 * Bind a port to the socket (IPV4 version).
 */
void
gnu_java_net_PlainDatagramSocketImpl_bind(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, jint port,
					  struct Hjava_net_InetAddress* laddr)
{
	int r;
	KaffeSocketAddr addr;
	socklen_t alen;
	const int fd = unhand(obj)->native_fd;

	memset(&addr, 0, sizeof(addr));
	if (obj_length(unhand(laddr)->addr) == 4) {
		alen = sizeof(addr.addr4);

#if defined(BSD44)
		addr.addr4.sin_len = sizeof(addr.addr4);
#endif /* defined(BSD44) */

		addr.addr4.sin_family = AF_INET;
		addr.addr4.sin_port = htons(port);
		memcpy(&addr.addr4.sin_addr, unhand_byte_array(unhand(laddr)->addr),
		       sizeof(addr.addr4.sin_addr));

DBG(NATIVENET,
	dprintf("datagram_bind4(%p, %s, %d)\n", 
		obj, ip2str(addr.addr4.sin_addr.s_addr), port);
);

#if defined(HAVE_STRUCT_SOCKADDR_IN6)
	} else if (obj_length(unhand(laddr)->addr) == 16) {
		alen = sizeof(addr.addr6);

#if defined(BSD44)
		addr.addr6.sin6_len = sizeof(addr.addr6);
#endif /* defined(BSD44) */

		addr.addr6.sin6_family = AF_INET6;
		addr.addr6.sin6_port = htons(port);
		memcpy(&addr.addr6.sin6_addr, unhand_byte_array(unhand(laddr)->addr),
		       sizeof(addr.addr6.sin6_addr));

DBG(NATIVENET,
	dprintf("datagram_bind6(%p, %s, %d)\n", 
		obj, ip62str(&addr.addr6.sin6_addr), port);
);		
#endif /* defined(HAVE_STRUCT_SOCKADDR_IN6) */

	} else {
		SignalError("java.net.SocketException", "Unsupported address family");
	}

	r = KBIND(fd, (struct sockaddr*)&addr, alen);
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
		port = ntohs(addr.addr4.sin_port);
	}
	unhand(obj)->localPort = port;

DBG(NATIVENET,
	dprintf("  datagram_bind(%p, %s, -) -> (localPort: %d)\n",
		obj, ip2str(addr.addr4.sin_addr.s_addr), port);
);
}

void
gnu_java_net_PlainDatagramSocketImpl_send0(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, struct Hjava_net_DatagramPacket* pkt)
{
	int rc;
	ssize_t bsent;
	KaffeSocketAddr addr;
	int alen = 0;
	
DBG(NATIVENET,
	dprintf("datagram_send(%p, %p [%d bytes])\n",
		obj, pkt, unhand(pkt)->length);
);

        memset(&addr, 0, sizeof(addr));
        if (obj_length(unhand(unhand(pkt)->address)->addr) == 4) {
	    alen = sizeof(addr.addr4);

#if defined(BSD44)
	    addr.addr4.sin_len = sizeof(addr.addr4);
#endif /* defined(BSD44) */

	    addr.addr4.sin_family = AF_INET;
	    addr.addr4.sin_port = htons(unhand(pkt)->port);
	    memcpy(&addr.addr4.sin_addr.s_addr, unhand_byte_array(unhand(unhand(pkt)->address)->addr),
		   4);
DBG(NATIVENET,
	dprintf("  datagram_send() to %s:%d\n",
		ip2str(ntohl(addr.addr4.sin_addr.s_addr)),
		unhand(pkt)->port);
);

#if defined(HAVE_STRUCT_SOCKADDR_IN6)
	} else if (obj_length(unhand(unhand(pkt)->address)->addr) == 16) {
	    alen = sizeof(addr.addr6);

#if defined(BSD44)
	    addr.addr6.sin6_len = sizeof(addr.addr6);
#endif /* defined(BSD44) */

	    addr.addr6.sin6_family = AF_INET6;
	    addr.addr6.sin6_port = htons(unhand(pkt)->port);
	    memcpy(&addr.addr6.sin6_addr, unhand_byte_array(unhand(unhand(pkt)->address)->addr),
		   sizeof(addr.addr6.sin6_addr));
	    
DBG(NATIVENET,
	dprintf("  datagram_send() to %s / %d\n",
		ip62str(&addr.addr6.sin6_addr),
		unhand(pkt)->port);
);
#endif /* defined(HAVE_STRUCT_SOCKADDR_IN6) */

        } else {
	    SignalError("java.net.SocketException", "Unsupported packet internet address");
	}

	rc = KSENDTO(unhand(obj)->native_fd,
		unhand_array(unhand(pkt)->buffer)->body, (size_t) unhand(pkt)->length,
		0, (struct sockaddr *)&addr, alen, &bsent);

DBG(NATIVENET,
	dprintf("  datagram_send() -> rc=%d bsent=%ld\n", rc, (long) bsent);
);

	if (rc) {
		SignalError("java.net.SocketException", SYS_ERROR(rc));
	}
}

jint
gnu_java_net_PlainDatagramSocketImpl_peek(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, struct Hjava_net_InetAddress* addr)
{
	ssize_t r;
	int rc;
	KaffeSocketAddr saddr;
	socklen_t alen = sizeof(saddr);

	rc = KRECVFROM(unhand(obj)->native_fd,
		NULL, 0, MSG_PEEK, (struct sockaddr*)&saddr,
		&alen, NOTIMEOUT /* timeout */, &r);
	if (rc) {
		SignalError("java.net.SocketException", SYS_ERROR(rc));
	}

	if (saddr.addr4.sin_family == AF_INET) {
	    memcpy(unhand_byte_array(unhand(addr)->addr), &saddr.addr4.sin_addr, sizeof(saddr.addr4.sin_addr));

#if defined(HAVE_STRUCT_SOCKADDR_IN6)
	} else if (saddr.addr6.sin6_family == AF_INET6) {
	    memcpy(unhand_byte_array(unhand(addr)->addr), &saddr.addr6.sin6_addr, sizeof(saddr.addr6.sin6_addr));
#endif /*  defined(HAVE_STRUCT_SOCKADDR_IN6) */

	} else {
	    SignalError("java.net.SocketException", "Unsupported address family");
	}
	return ((jint)r);
}

void
gnu_java_net_PlainDatagramSocketImpl_receive0(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, struct Hjava_net_DatagramPacket* pkt)
{
	ssize_t r;
	int rc;
	KaffeSocketAddr addr;
	socklen_t alen = sizeof(addr);
	HArrayOfByte *array_address;
	int to_read, offset;

	assert(obj != NULL);
	if (pkt == NULL || unhand(pkt)->buffer == NULL)
		SignalError("java.lang.NullPointerException", "null datagram packet");
	assert(unhand(pkt)->length <= unhand(unhand(pkt)->buffer)->length);

DBG(NATIVENET,
	dprintf("datagram_receive(%p, %p [%d bytes])\n",
		obj, pkt, unhand(pkt)->length);
);

	/* Which port am I receiving from */
	addr.addr4.sin_port = htons(unhand(obj)->localPort);

	/* XXX should assert (unhand(pkt)->length <= unhand_array(unhand(pkt)->buf)->length), no? */
        offset = unhand(pkt)->offset; 
	to_read = unhand(pkt)->length;
        do { 
	        rc = KRECVFROM(unhand(obj)->native_fd,
			       &(unhand_array(unhand(pkt)->buffer)->body)[offset],
			       (unsigned)to_read, 0, (struct sockaddr*)&addr,
			       &alen, unhand(obj)->timeout, &r);
		switch( rc )
		{
		case 0:
		        break;
		case ETIMEDOUT: {
		        struct Hjava_io_InterruptedIOException* except;
		  
			except = (struct Hjava_io_InterruptedIOException *)
			  execute_java_constructor(
						   "java.net.SocketTimeoutException", NULL, NULL,
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
	unhand(pkt)->port = ntohs(addr.addr4.sin_port);

	if (addr.addr4.sin_family == AF_INET) {
		array_address = (HArrayOfByte*)AllocArray(sizeof(addr.addr4.sin_addr), TYPE_Byte);
		memcpy(unhand_byte_array(array_address), &addr.addr4.sin_addr, sizeof(addr.addr4.sin_addr));
		
		unhand(pkt)->address = (struct Hjava_net_InetAddress*)
			execute_java_constructor("java/net/Inet4Address", NULL, NULL, "([BLjava/lang/String;)V",
						 array_address, NULL);

#if defined(HAVE_STRUCT_SOCKADDR_IN6)
	} else if (addr.addr6.sin6_family == AF_INET6) {
		array_address = (HArrayOfByte*)AllocArray(sizeof(addr.addr6.sin6_addr), TYPE_Byte);
		memcpy(unhand_byte_array(array_address), &addr.addr6.sin6_addr, sizeof(addr.addr6.sin6_addr));
		
		unhand(pkt)->address = (struct Hjava_net_InetAddress*)
			execute_java_constructor("java/net/Inet6Address", NULL, NULL, "([BLjava/lang/String;)V",
						 array_address, NULL);
#endif /* defined(HAVE_STRUCT_SOCKADDR_IN6) */
		
	} else {
		SignalError("java.net.SocketException", "Unsupported address family");
	}

	/* zero out hostname to overwrite old name which does not match
	 * the new address from which the packet came.
	 */
	unhand(unhand(pkt)->address)->hostName = NULL;

DBG(NATIVENET,
	dprintf("  datagram_receive(%p, %p) -> from %s:%d; brecv=%ld\n",
		obj, pkt, ip2str(ntohl(addr.addr4.sin_addr.s_addr)),
		ntohs(addr.addr4.sin_port), (long) r);
);
}

/*
 * Close the socket.
 */
void
gnu_java_net_PlainDatagramSocketImpl_datagramSocketClose(struct Hgnu_java_net_PlainDatagramSocketImpl* obj)
{
	int r;

DBG(NATIVENET,
	dprintf("datagram_close(%p)\n", obj);
);

	if (unhand(obj)->native_fd != -1) {
		r = KSOCKCLOSE(unhand(obj)->native_fd);
		unhand(obj)->native_fd = -1;
		if (r) {
			SignalError("java.net.SocketException", SYS_ERROR(r));
		}
	}
}


void
gnu_java_net_PlainDatagramSocketImpl_socketSetOption(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, jint opt, struct Hjava_lang_Object* arg)
{
	struct Hjava_net_InetAddress* addrp;
	int v, r;
	unsigned int k;

	/* Do easy cases */
	for (k = 0; k < sizeof(socketOptions) / sizeof(*socketOptions); k++) {
		if (opt == socketOptions[k].jopt) {
			v = unhand((struct Hjava_lang_Integer*)arg)->value;
			r = KSETSOCKOPT(unhand(obj)->native_fd,
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
			
			memcpy(&ia, unhand_byte_array(unhand(addrp)->addr),
			       sizeof(ia));

			r = KSETSOCKOPT(unhand(obj)->native_fd,
					IPPROTO_IP,
					IP_MULTICAST_IF,
					&ia,
					sizeof(ia));
			if (r) {
				SignalError("java.net.SocketException",
					    SYS_ERROR(r));
			}
		}
#else /* !defined(IP_MULTICAST_IF) */
			SignalError("java.net.SocketException",
				    "IP_MULTICAST_IF is not supported");
#endif /* defined(IP_MULTICAST_IF) */

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
gnu_java_net_PlainDatagramSocketImpl_socketGetOption(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, jint opt)
{
	int r = 0, v;
	unsigned int k;
	socklen_t vsize = sizeof(v);
	struct sockaddr_in addr;
	socklen_t alen = sizeof(addr);
	struct in_addr ia;
	socklen_t ia_len = sizeof(ia);

	/* Do easy cases */
	for (k = 0; k < sizeof(socketOptions) / sizeof(*socketOptions); k++) {
		if (opt == socketOptions[k].jopt) {
			r = KGETSOCKOPT(unhand(obj)->native_fd,
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
		r = KGETSOCKNAME(unhand(obj)->native_fd,
			(struct sockaddr*)&addr, &alen);
		if (r) {
			SignalError("java.net.SocketException", SYS_ERROR(r));
		}
		r = htonl(addr.sin_addr.s_addr);
		break;

#if defined(IP_MULTICAST_IF)
	case java_net_SocketOptions_IP_MULTICAST_IF:
		r = KGETSOCKOPT(unhand(obj)->native_fd,
			IPPROTO_IP, IP_MULTICAST_IF, &ia, &ia_len);
		if (r) {
			SignalError("java.net.SocketException", SYS_ERROR(r));
			return (0);	/* NOT REACHED, avoid warning */
		}
		r = ntohl(ia.s_addr);
		break;
#endif /* defined(IP_MULTICAST_IF) */

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
gnu_java_net_PlainDatagramSocketImpl_join(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, struct Hjava_net_InetAddress* laddr)
{

#if defined(IP_ADD_MEMBERSHIP)
	int r;
	struct ip_mreq ipm;
	int iface_addr;
	
	memset(&ipm, 0, sizeof(ipm));
	memcpy(&ipm.imr_multiaddr, unhand_byte_array(unhand(laddr)->addr),
	       sizeof(ipm.imr_multiaddr));
	
        iface_addr =
	  gnu_java_net_PlainDatagramSocketImpl_socketGetOption(obj, 
	    java_net_SocketOptions_IP_MULTICAST_IF);
	ipm.imr_interface.s_addr = htonl(iface_addr);
	
DBG(NATIVENET,
	dprintf("datagram_join (%p, %p) => %s\n",
		obj, laddr, ip2str(ipm.imr_interface.s_addr));
   );

	r = KSETSOCKOPT(unhand(obj)->native_fd,
		IPPROTO_IP, IP_ADD_MEMBERSHIP, &ipm, sizeof(ipm));
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
#else /* !defined(IP_ADD_MEMBERSHIP) */
	SignalError("java.net.SocketException",
		"IP_ADD_MEMBERSHIP not supported");
#endif /* defined(IP_ADD_MEMBERSHIP) */

}

/*
 * leave multicast group
 */
void
gnu_java_net_PlainDatagramSocketImpl_leave(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, struct Hjava_net_InetAddress* laddr)
{

#if defined(IP_DROP_MEMBERSHIP)
	int r;
	struct ip_mreq ipm;
	int iface_addr;

	memset(&ipm, 0, sizeof(ipm));
	memcpy(&ipm.imr_multiaddr, unhand_byte_array(unhand(laddr)->addr),
	       sizeof(ipm.imr_multiaddr));

        iface_addr = 
	  gnu_java_net_PlainDatagramSocketImpl_socketGetOption(obj, 
	    java_net_SocketOptions_IP_MULTICAST_IF);
	ipm.imr_interface.s_addr = htonl(iface_addr);

DBG(NATIVENET,
	dprintf("datagram_leave (%p, %p) => %s\n",
		obj, laddr, ip2str(ipm.imr_interface.s_addr));
   );

	r = KSETSOCKOPT(unhand(obj)->native_fd,
		IPPROTO_IP, IP_DROP_MEMBERSHIP, &ipm, sizeof(ipm));
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
#else /* !defined(IP_DROP_MEMBERSHIP) */
	SignalError("java.net.SocketException",
		"IP_DROP_MEMBERSHIP not supported");
#endif /* defined(IP_DROP_MEMBERSHIP) */

}

/*
 * Join multicast group
 */
void
gnu_java_net_PlainDatagramSocketImpl_joinGroup(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, struct Hjava_net_SocketAddress *jsa, struct Hjava_net_NetworkInterface *jni)
{

#if defined(IP_ADD_MEMBERSHIP)
	struct Hjava_net_InetSocketAddress *jisa;
	struct ip_mreq ipm;
	int r = 0;

	jisa = (struct Hjava_net_InetSocketAddress *)jsa;

	memset(&ipm, 0, sizeof(ipm));
	memcpy(&ipm.imr_multiaddr,
	       unhand_byte_array(unhand(unhand(jisa)->addr)->addr),
	       sizeof(ipm.imr_multiaddr));

DBG(NATIVENET,
	dprintf("  datagram_joinGroup(%p, %p, %p) -> join %s\n",
		obj, jsa, jni, ip2str(ipm.imr_multiaddr.s_addr));
   );

	if( jni && obj_length(unhand(unhand(jni)->inetAddresses)->elementData) != 0)
	{
	  struct Hjava_net_InetAddress* if_addr = (struct Hjava_net_InetAddress *)
	    unhand_object_array(unhand(unhand(jni)->inetAddresses)->elementData);
	  
	  memcpy(&ipm.imr_interface, unhand_byte_array(unhand(if_addr)->addr),
	  	 sizeof(ipm.imr_interface));;
	}
	else
	{
	  ipm.imr_interface.s_addr =
	    gnu_java_net_PlainDatagramSocketImpl_socketGetOption(obj, 
	      java_net_SocketOptions_IP_MULTICAST_IF);
	}

	r = KSETSOCKOPT(unhand(obj)->native_fd,
			IPPROTO_IP,
			IP_ADD_MEMBERSHIP,
			&ipm,
			sizeof(ipm));
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
#else /* !defined(IP_ADD_MEMBERSHIP) */
	SignalError("java.net.SocketException",
		"IP_ADD_MEMBERSHIP not supported");
#endif /* defined(IP_ADD_MEMBERSHIP) */

}

/*
 * Leave multicast group
 */
void
gnu_java_net_PlainDatagramSocketImpl_leaveGroup(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, struct Hjava_net_SocketAddress *jsa, struct Hjava_net_NetworkInterface *jni)
{

#if defined(IP_ADD_MEMBERSHIP)
	struct Hjava_net_InetSocketAddress *jisa;
	struct ip_mreq ipm;
	int r;

	jisa = (struct Hjava_net_InetSocketAddress *)jsa;

	memset(&ipm, 0, sizeof(ipm));
	memcpy(&ipm.imr_multiaddr,
	       unhand_byte_array(unhand(unhand(jisa)->addr)->addr),
	       sizeof(ipm.imr_multiaddr));
	if( jni && obj_length(unhand(unhand(jni)->inetAddresses)->elementData) != 0)
	{
	        struct Hjava_net_InetAddress* if_addr = (struct Hjava_net_InetAddress *)
		  unhand_object_array(unhand(unhand(jni)->inetAddresses)->elementData);
	  
	  	memcpy(&ipm.imr_interface,
		       unhand_byte_array(unhand(if_addr)->addr),
		       sizeof(ipm.imr_interface));;
	}
	else
	{
	  ipm.imr_interface.s_addr =
	    gnu_java_net_PlainDatagramSocketImpl_socketGetOption(obj, 
	      java_net_SocketOptions_IP_MULTICAST_IF);
	}

	r = KSETSOCKOPT(unhand(obj)->native_fd,
			IPPROTO_IP,
			IP_DROP_MEMBERSHIP,
			&ipm,
			sizeof(ipm));
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
#else /* !defined(IP_ADD_MEMBERSHIP) */
	SignalError("java.net.SocketException",
		"IP_ADD_MEMBERSHIP not supported");
#endif /* defined(IP_ADD_MEMBERSHIP) */

}

/*
 * set multicast-TTL
 */
void
gnu_java_net_PlainDatagramSocketImpl_setTTL(struct Hgnu_java_net_PlainDatagramSocketImpl* obj, jbyte ttl)
{

#if defined(IP_MULTICAST_TTL)
	int r;
	unsigned char v = (unsigned char)ttl;

	r = KSETSOCKOPT(unhand(obj)->native_fd,
		IPPROTO_IP, IP_MULTICAST_TTL, &v, sizeof(v));
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
#else /* !defined(IP_MULTICAST_TTL) */
	SignalError("java.net.SocketException",
		"IP_MULTICAST_TTL not supported");
#endif /* defined(IP_MULTICAST_TTL) */

}

/*
 * get multicast-TTL
 */
jbyte
gnu_java_net_PlainDatagramSocketImpl_getTTL(struct Hgnu_java_net_PlainDatagramSocketImpl* obj)
{

#if defined(IP_MULTICAST_TTL)
	unsigned char v;
	socklen_t s = 1;
	int r;

	r = KGETSOCKOPT(unhand(obj)->native_fd,
		IPPROTO_IP, IP_MULTICAST_TTL, &v, &s);
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
	return (jbyte)v;
#else /* !defined(IP_MULTICAST_TTL) */
	SignalError("java.net.SocketException",
		"IP_MULTICAST_TTL not supported");
#endif /* defined(IP_MULTICAST_TTL) */

}

