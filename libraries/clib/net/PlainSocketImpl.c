/*
 * java.net.PlainSocketImpl.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "config-net.h"
#include "support.h"
#include "java_lang_Integer.h"
#include "java_net_SocketImpl.h"
#include "java_net_InetAddress.h"
#include "gnu_java_net_PlainSocketImpl.h"
#include "java_net_SocketOptions.h"
#include "java_io_InterruptedIOException.h"
#include "nets.h"
#include <jsyscall.h>
#include <jthread.h>
#include "debug.h"
#include "object.h"
#include "itypes.h"
#include "exception.h"

#include "dummyin6.h"

#define IPV4_ADDRESS_SIZE 4
#define IPV6_ADDRESS_SIZE 16

#if !defined(HAVE_GETADDRINFO) || !defined(HAVE_GETNAMEINFO)
#include "getaddrinfo.h"
#endif /* !defined(HAVE_GETADDRINFO) || !defined(HAVE_GETNAMEINFO) */

/*
 * Supported socket options
 */
  static const struct {
	  int jopt;
	  int level;
	  int copt;
  } socketOptions[] = {

#if defined(SO_SNDBUF)
    { java_net_SocketOptions_SO_SNDBUF,		SOL_SOCKET,	SO_SNDBUF },
#endif /* defined(SO_SNDBUF) */

#if defined(SO_RCVBUF)
    { java_net_SocketOptions_SO_RCVBUF,		SOL_SOCKET,	SO_RCVBUF },
#endif /* defined(SO_RCVBUF) */

#if defined(SO_LINGER)
    { java_net_SocketOptions_SO_LINGER,		SOL_SOCKET,	SO_LINGER },
#endif /* defined(SO_LINGER) */

#if defined(SO_REUSEADDR)
    { java_net_SocketOptions_SO_REUSEADDR,	SOL_SOCKET,	SO_REUSEADDR },
#endif /* defined(SO_REUSEADDR) */

#if defined(TCP_NODELAY)
    { java_net_SocketOptions_TCP_NODELAY,	IPPROTO_TCP,	TCP_NODELAY },
#endif /* defined(TCP_NODELAY) */
  };

#if defined(KAFFE_VMDEBUG) && !defined(NDEBUG)
/*
 * Option names (for debugging only)
 */
  static const struct {
	  int opt;
	  const char *name;
  } optionNames[] = {

#if defined(SO_SNDBUF)
    { java_net_SocketOptions_SO_SNDBUF, "SO_SNDBUF" },
#endif /* defined(SO_SNDBUF) */

#if defined(SO_RCVBUF)
    { java_net_SocketOptions_SO_RCVBUF, "SO_RCVBUF" },
#endif /* defined(SO_RCVBUF) */

#if defined(SO_LINGER)
    { java_net_SocketOptions_SO_LINGER, "SO_LINGER" },
#endif /* defined(SO_LINGER) */

#if defined(SO_REUSEADDR)
    { java_net_SocketOptions_SO_REUSEADDR, "SO_REUSEADDR" },
#endif /* defined(SO_REUSEADDR) */

#if defined(TCP_NODELAY)
    { java_net_SocketOptions_TCP_NODELAY, "TCP_NODELAY" },
#endif /* defined(TCP_NODELAY) */

    { java_net_SocketOptions_SO_BINDADDR, "SO_BINDADDR" },
    { java_net_SocketOptions_SO_TIMEOUT, "SO_TIMEOUT" },
    { java_net_SocketOptions_IP_MULTICAST_IF, "IP_MULTICAST_IF" }
  };

static char *
ip2str(uint32 addr) 
{
	static char addrbuf[16];

	addr = ntohl(addr);
	sprintf(addrbuf, "%u.%u.%u.%u",
	  (addr >> 24) & 0xff,
	  (addr >> 16) & 0xff,
	  (addr >>  8) & 0xff,
	  (addr      ) & 0xff);
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
 * Create a stream or datagram socket.
 */
void
gnu_java_net_PlainSocketImpl_socketCreate(struct Hgnu_java_net_PlainSocketImpl* this, jboolean stream)
{
	int fd;
	int type;
	int rc;

	if (stream == 0) {
		type = SOCK_DGRAM;
	}
	else {
		type = SOCK_STREAM;
	}

	DBG(NATIVENET,
	    dprintf("socketCreate(%p, %s)\n", this, stream ? "stream" : "datagram");
	    );

	rc = KSOCKET(AF_INET, type, 0, &fd);
	if (rc) {
		unhand(this)->native_fd = -1;
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}

	DBG(NATIVENET,
	    dprintf("socketCreate(%p, %s) -> fd=%d\n", 
		    this, stream ? "stream" : "datagram", fd);
	    );

	unhand(this)->native_fd = fd;
	unhand(this)->native_fd = fd;
}

/*
 * Connect the socket to someone.
 */
void
gnu_java_net_PlainSocketImpl_socketConnect(struct Hgnu_java_net_PlainSocketImpl* this,
					   struct Hjava_net_InetAddress* daddr, 
					   jint dport, jint timeout)
{
	int fd;
	int r;
	KaffeSocketAddr addr;
	socklen_t alen;

	memset(&addr, 0, sizeof(addr));
	if (obj_length(unhand(daddr)->addr) == IPV4_ADDRESS_SIZE) {
	        alen = sizeof(addr.addr4); 

#if defined(BSD44)
		addr.addr4.sin_len = sizeof(addr.addr4);
#endif /* defined(BSD44) */

		addr.addr4.sin_family = AF_INET;
		addr.addr4.sin_port = htons(dport);
		memcpy(&addr.addr4.sin_addr, 
		       unhand_byte_array(unhand(daddr)->addr), sizeof(addr.addr4.sin_addr));

#if defined(HAVE_STRUCT_SOCKADDR_IN6)
	} else if (obj_length(unhand(daddr)->addr) == IPV6_ADDRESS_SIZE) {
	        alen = sizeof(addr.addr6);

#if defined(BSD44)
		addr.addr6.sin6_len = sizeof(addr.addr6);
#endif /*  defined(BSD44) */

		addr.addr6.sin6_family = AF_INET6;
		addr.addr6.sin6_port = htons(dport);
		memcpy(&addr.addr6.sin6_addr, 
		       unhand_byte_array(unhand(daddr)->addr), sizeof(addr.addr6.sin6_addr));
#endif /* defined(HAVE_STRUCT_SOCKADDR_IN6) */

	} else {
		SignalError("java.net.SocketException", "Unsupported address family");
	}

	DBG(NATIVENET,
	    dprintf("socketConnect(%p, %s, %d, %d)\n", 
		    this, ip2str(addr.addr4.sin_addr.s_addr), dport, timeout);
	    );

	fd = (int)unhand(this)->native_fd;
	r = KCONNECT(fd, (struct sockaddr*)&addr, alen, timeout);
	if (r == EINTR) {
		SignalError("java.io.InterruptedIOException", 
			    "Connect was interrupted");
	}
	if (r == ETIMEDOUT) {
	        SignalError("java.net.SocketTimeoutException",
			    "Connect timed out");
	}
	if (r == EWOULDBLOCK && unhand(this)->blocking) {
		unhand(this)->connecting = true;
		return;
	}
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}

	/* Enter information into socket object */
	alen = sizeof(addr);
	r = KGETSOCKNAME(fd, (struct sockaddr*)&addr, &alen);
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}

	DBG(NATIVENET,
	    dprintf("socketConnect(%p, %s, %d) -> (lport: %d)\n",
		    this, ip2str(addr.addr4.sin_addr.s_addr), dport,
		    ntohs(addr.addr4.sin_port)
		    );
	    );

	unhand(this)->address = daddr;
	unhand(this)->port = dport;
	unhand(this)->localport = ntohs(addr.addr4.sin_port);
}

/*
 * Bind this socket to an address.
 */
void
gnu_java_net_PlainSocketImpl_socketBind(struct Hgnu_java_net_PlainSocketImpl* this,
					struct Hjava_net_InetAddress* laddr, 
					jint lport)
{
	int r;
	KaffeSocketAddr addr;
	int fd;
	int on = 1;
	socklen_t alen;

	DBG(NATIVENET,
	    dprintf("socketBind(%p, %s, %d)\n", 
		    this, ip2str(unhand(laddr)->address), lport);
	    );

	memset(&addr, 0, sizeof(addr));
	if (obj_length(unhand(laddr)->addr) == IPV4_ADDRESS_SIZE) {
	        alen = sizeof(addr.addr4);

#if defined(BSD44)
		addr.addr4.sin_len = sizeof(addr.addr4);
#endif /*  defined(BSD44) */

		addr.addr4.sin_family = AF_INET;
		addr.addr4.sin_port = htons(lport);
		memcpy(&addr.addr4.sin_addr, 
		       unhand_byte_array(unhand(laddr)->addr), sizeof(addr.addr4.sin_addr));

		DBG(NATIVENET,
		    dprintf("socketBind(%p, %s, -) -> (lport: %d)\n", this,
			    ip2str(addr.addr4.sin_addr.s_addr), lport);
		    );

#if defined(HAVE_STRUCT_SOCKADDR_IN6)
	} else if (obj_length(unhand(laddr)->addr) == IPV6_ADDRESS_SIZE) {
	        alen = sizeof(addr.addr6);

#if defined(BSD44)
		addr.addr6.sin6_len = sizeof(addr.addr6);
#endif /* defined(BSD44) */

		addr.addr6.sin6_family = AF_INET6;
		addr.addr6.sin6_port = htons(lport);
		memcpy(&addr.addr6.sin6_addr, 
		       unhand_byte_array(unhand(laddr)->addr), sizeof(addr.addr6.sin6_addr));

		DBG(NATIVENET,
		    dprintf("socketBind(%p, %s, -) -> (lport: %d)\n", this,
			    ip62str(&addr.addr6.sin6_addr), lport);
		    );
#endif /* defined(HAVE_STRUCT_SOCKADDR_IN6) */

	} else {
		SignalError("java.net.SocketException", "Unsupported address family");
	}
	fd = (int)unhand(this)->native_fd;

	/* Allow rebinding to socket - ignore errors */
	(void)KSETSOCKOPT(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
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

	/* Enter information into socket object */
	unhand(this)->address = laddr;
	if (lport == 0) {
		alen = sizeof(addr);
		r = KGETSOCKNAME(fd, (struct sockaddr*)&addr, &alen);
		if (r) {
			SignalError("java.io.IOException", SYS_ERROR(r));
		}
#if defined(HAVE_STRUCT_SOCKADDR_IN6)
		if (obj_length(unhand(laddr)->addr) == IPV6_ADDRESS_SIZE) {
		  lport = ntohs(addr.addr6.sin6_port);
		} else
#endif
		{
		  lport = ntohs(addr.addr4.sin_port);
		}
	}
	unhand(this)->localport = lport;
}

/*
 * Turn this socket into a listener.
 */
void
gnu_java_net_PlainSocketImpl_socketListen(struct Hgnu_java_net_PlainSocketImpl* this, jint count)
{
	int r;

	DBG(NATIVENET,
	    dprintf("socketListen(%p, count=%d)\n", this, count);
	    );

	r = KLISTEN((int)unhand(this)->native_fd, count);
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
}

/*
 * Accept a connection.
 */
void
gnu_java_net_PlainSocketImpl_socketAccept(struct Hgnu_java_net_PlainSocketImpl* this, struct Hjava_net_SocketImpl* sock)
{
	int r;
	int rc, rc1;
	socklen_t alen;
	struct sockaddr_in addr;
	HArrayOfByte *remote_addr;
	struct Hgnu_java_net_PlainSocketImpl* accepted_socket =
	  (struct Hgnu_java_net_PlainSocketImpl *)sock;
	jvalue jv;

	remote_addr = NULL;
	memset(&addr, 0, sizeof(addr));

#if defined(BSD44)
	addr.sin_len = sizeof(addr);
#endif /* defined(BSD44) */

	addr.sin_family = AF_INET;
	addr.sin_port = htons(unhand(sock)->localport);
	/* I guess the next line is too much as unhand(sock)->address is not supposed
	 * to be initialized.
	 * addr.sin_addr.s_addr = htonl(unhand(unhand(sock)->address)->address);
	 */
	addr.sin_addr.s_addr = INADDR_ANY;

	DBG(NATIVENET,
	    dprintf("socketAccept(%p, localport=%d, addr=%s, timeout=%d)\n", 
		    this, ntohs(addr.sin_port), ip2str(addr.sin_addr.s_addr), unhand(this)->timeout);
	    );

	alen = sizeof(addr);
	do {
	        rc = KACCEPT(unhand(this)->native_fd,
			     (struct sockaddr*)&addr, &alen, unhand(this)->timeout, &r);
	} while (rc == EINTR);
	if (rc == ETIMEDOUT) {
	  	DBG(NATIVENET,
	    		dprintf("socketAcceptTimedOut(%p, localport=%d, addr=%s, timeout=%d)\n", 
			this, ntohs(addr.sin_port), ip2str(addr.sin_addr.s_addr), unhand(this)->timeout);
	    	);
	        SignalError("java.net.SocketTimeoutException",
			    "Accept timed out");
	}
	if (rc) {
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
	if (unhand(accepted_socket)->native_fd != -1) {
		rc1 = KSOCKCLOSE(unhand((struct Hgnu_java_net_PlainSocketImpl *)sock)->native_fd);
		if (rc1) {
			SignalError("java.io.IOException", SYS_ERROR(rc1));
		}
	}

	unhand(accepted_socket)->native_fd = r;

	/* Enter information into socket object */
	alen = sizeof(addr);
	r = KGETPEERNAME(r, (struct sockaddr*)&addr, &alen);
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}

	/* create a byte array containing the raw address of the connected socket */
	switch (addr.sin_family) {	
		case AF_INET:
			remote_addr = (HArrayOfByte *)newArray(TYPE_CLASS(TYPE_Byte),
							       sizeof(addr.sin_addr));
			memcpy(unhand_byte_array(remote_addr),
			       &addr.sin_addr,
			       sizeof(addr.sin_addr));
			break;

#if defined(notnow) && defined(AF_INET6)
		case AF_INET6:
			remote_addr = (HArrayOfByte *)newArray(TYPE_CLASS(TYPE_Byte),
							       sizeof(in6->sin6_addr));
			memcpy(unhand_byte_array(remote_addr),
			       &in6->sin6_addr,
			       sizeof(in6->sin6_addr));
			break;
#endif /* defined(AF_INET6) */

		default:
			/* Ignore */
			break;
	}

	/* and use that byte array to create an appropriate Inet*Address instance */
	do_execute_java_class_method (&jv, "java.net.InetAddress",
							      NULL, 
							      "getByAddress",
							      "([B)Ljava/net/InetAddress;",
							      remote_addr);
	unhand(sock)->address = jv.l;
	unhand(sock)->port = ntohs(addr.sin_port);

	alen = sizeof(addr);
	r = KGETSOCKNAME(unhand(accepted_socket)->native_fd, (struct sockaddr*)&addr, &alen);
	if (r) {
	  SignalError("java.io.IOException", SYS_ERROR(r));
	}
	unhand(accepted_socket)->localport = ntohs(addr.sin_port);
	
	DBG(NATIVENET,
	    dprintf("socketAccept(%p, localport=-, addr=-) -> (sock: %p; addr: %s; port:%d)\n", 
		    this, sock, ip2str(addr.sin_addr.s_addr), ntohs(addr.sin_port));
	    );
}

/*
 * Return how many bytes can be read without blocking.
 */
jint
gnu_java_net_PlainSocketImpl_socketAvailable(struct Hgnu_java_net_PlainSocketImpl* this)
{
	int r;
	jint len;
	int fd;

#if (!(defined(HAVE_IOCTL) && defined(FIONREAD)) && !defined(__WIN32__))
	static struct timeval tm = { 0, 0 };
	fd_set rd;
#endif /* (!(defined(HAVE_IOCTL) && defined(FIONREAD)) && !defined(__WIN32__)) */

	DBG(NATIVENET,
	    dprintf("socketAvailable(%p)\n", this);
	    );
	fd = (int)unhand(this)->native_fd;

#if defined(HAVE_IOCTL) && defined(FIONREAD)
	/* XXX make part of system call interface to protect errno */
	r = ioctl(fd, FIONREAD, &len);
	if (r < 0) {
		SignalError("java.io.IOException", SYS_ERROR(errno));
	}
#else /* !(defined(HAVE_IOCTL) && defined(FIONREAD)) */
#if defined(__WIN32__) /* Windows hack - XXX */
	len = 0;
#else /* !defined(__WIN32__) */
	/* This uses KSELECT() to work out if we can read - but what
	 * happens at the end of file?
	 */

	FD_ZERO(&rd);
	FD_SET(fd, &rd);
	KSELECT(fd+1, &rd, NULL, NULL, &tm, &r);	/* XXX ignore ret code*/
	if (r == 1) {
		len = 1;
	}
	else {
		len = 0;
	}
#endif	/* defined(__WIN32__) */
#endif  /* defined(HAVE_IOCTL) && defined(FIONREAD) */

	DBG(NATIVENET,
	    dprintf("socketAvailable(%p) -> %d\n", this, len);
	    );

	return (len);
}

/*
 * Close this socket.
 */
void
gnu_java_net_PlainSocketImpl_socketClose(struct Hgnu_java_net_PlainSocketImpl* this)
{
	int r;

	DBG(NATIVENET,
	    dprintf("socketClose(%p)\n", this);
	    );

	if (unhand(this)->native_fd != -1) {
		r = KSOCKCLOSE((int)unhand(this)->native_fd);
		unhand(this)->native_fd = -1;
		if (r) {
			SignalError("java.io.IOException", SYS_ERROR(r));
		}
	}
}

void
gnu_java_net_PlainSocketImpl_socketSetOption(struct Hgnu_java_net_PlainSocketImpl* this,
					     jint opt, 
					     struct Hjava_lang_Object* arg)
{
	int r, v;
	unsigned int k;

	DBG(NATIVENET,
	    const char *optstr = "UNKNOWN";
	    for (k = 0; k < sizeof(optionNames) / sizeof(optionNames[0]); k++) 
		    if (optionNames[k].opt == opt)
			    optstr = optionNames[k].name;
	    dprintf("socketSetOption(%p, %s, arg=%p)\n", this, optstr, arg);
	    );

	/* Do easy cases */
	for (k = 0; k < sizeof(socketOptions) / sizeof(*socketOptions); k++) {
		if (opt == socketOptions[k].jopt) {
			struct linger ling;
			char *optdata;
			int optlen;
			
			v = unhand((struct Hjava_lang_Integer*)arg)->value;
			if( socketOptions[k].copt == SO_LINGER )
			{
				ling.l_onoff = v;
				ling.l_linger = v;
				optdata = (char *)&ling;
				optlen = sizeof(ling);
			}
			else
			{
				optdata = (char *)&v;
				optlen = sizeof(v);
			}
			r = KSETSOCKOPT((int)unhand(this)->native_fd,
				socketOptions[k].level, socketOptions[k].copt,
				optdata, optlen);
			if (r) {
				SignalError("java.net.SocketException", SYS_ERROR(r));
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
gnu_java_net_PlainSocketImpl_socketGetOption(struct Hgnu_java_net_PlainSocketImpl* this, jint opt)
{
	struct sockaddr_in addr;
	socklen_t alen = sizeof(addr);
	int r = 0, v;
	socklen_t vsize = sizeof(v);
	unsigned int k;

	DBG(NATIVENET,
	    const char *optstr = "UNKNOWN";
	    for (k = 0; k < sizeof(optionNames) / sizeof(optionNames[0]); k++) 
		    if (optionNames[k].opt == opt)
			    optstr = optionNames[k].name;
	    dprintf("socketGetOption(%p, %s)\n", this, optstr);
	    );

	/* Do easy cases */
	for (k = 0; k < sizeof(socketOptions) / sizeof(*socketOptions); k++) {
		if (opt == socketOptions[k].jopt) {
			r = KGETSOCKOPT((int)unhand(this)->native_fd,
				socketOptions[k].level, socketOptions[k].copt,
				&v, &vsize);
			if (r) {
				SignalError("java.net.SocketException", SYS_ERROR(r));
			}
			DBG(NATIVENET,
			    dprintf("socketGetOption(%p, -) -> %d\n", this, v);
			    );
			return v;
		}
	}

	/* Do harder cases */
	switch(opt) {
	case java_net_SocketOptions_SO_BINDADDR:
		r = KGETSOCKNAME((int)unhand(this)->native_fd,
			(struct sockaddr*)&addr, &alen);
		if (r) {
			SignalError("java.net.SocketException", SYS_ERROR(r));
		}
		r = htonl(addr.sin_addr.s_addr);
		break;
	case java_net_SocketOptions_IP_MULTICAST_IF:
	case java_net_SocketOptions_SO_TIMEOUT: /* java takes care */
	default:
		SignalError("java.net.SocketException", "Unimplemented socket option");    
	} 
	DBG(NATIVENET,
	    dprintf("socketGetOption(%p, -) -> %d\n", this, r);
	    );
	return (r);
}

jint
gnu_java_net_PlainSocketImpl_socketRead(struct Hgnu_java_net_PlainSocketImpl* this, HArrayOfByte* buf, jint offset, jint len)
{
        ssize_t r;
	int rc;
	int fd;
	int total_read;

	DBG(NATIVENET,
	    dprintf("socket_read(%p, %p, %d, %d)\n", 
		    this, buf, offset, len);
	    );

	fd = (int)unhand(this)->native_fd;
	if (fd < 0) {
		SignalError("java.io.IOException", "fd invalid"); 
	}

	total_read = 0;
	r = 0;
	do {
		rc = KSOCKREAD(fd, &unhand_array(buf)->body[offset], (unsigned)len, unhand(this)->timeout, &r);

		 if (rc == ETIMEDOUT) {
		         struct Hjava_io_InterruptedIOException* except;

			 except = (struct Hjava_io_InterruptedIOException *)
			   execute_java_constructor(
						    "java.net.SocketTimeoutException", NULL, NULL,
						    "(Ljava/lang/String;)V",
						    checkPtr(stringC2Java("Read was interrupted")));
			 except->bytesTransferred = r;
	      
			 throwException((struct Hjava_lang_Throwable*)except);
		 } else if (rc != EINTR && rc != 0) {
		   SignalError("java.io.IOException", SYS_ERROR(rc));
		 } else if (rc == 0 && r == 0 && len > 0) {
		   return (-1);
		 }
		 offset += r;
		 len -= r;
		 total_read += r;
	} while (rc == EINTR);
	return (total_read);
}

void
gnu_java_net_PlainSocketImpl_socketWrite(struct Hgnu_java_net_PlainSocketImpl* this, HArrayOfByte* buf, jint offset, jint len)
{
	int r;
	int fd;
	ssize_t nw;

	DBG(NATIVENET,
	    dprintf("socket_write(%p, %p, %d, %d)\n", 
		    this, buf, offset, len);
	    );

	fd = (int)unhand(this)->native_fd;
	if (fd >= 0) {
		while (len > 0) {
			r = KSOCKWRITE(fd,
			    &unhand_array(buf)->body[offset], (unsigned)len, &nw);
			if (r) {
				SignalError("java.io.IOException", SYS_ERROR(r));
			}
			offset += nw;
			len -= nw;
		}
	} else {
		SignalError("java.io.IOException", "fd invalid"); 
	}
}

void
gnu_java_net_PlainSocketImpl_setBlocking(struct Hgnu_java_net_PlainSocketImpl* this, jboolean blocking)
{
	if (blocking == unhand(this)->blocking)
		return;
	
	unhand(this)->blocking = true;
	KTHREAD(set_blocking)((int)unhand(this)->native_fd, blocking);
}

void
gnu_java_net_PlainSocketImpl_waitForConnection(struct Hgnu_java_net_PlainSocketImpl* this)
{
	fd_set w;
	int fd = (int)unhand(this)->native_fd;
	int o, r;
	struct timeval tv;
	struct timeval *ptv = NULL;
	
	if (!unhand(this)->blocking) {
		if (!unhand(this)->connecting)
			return;
		
		FD_ZERO(&w);
		FD_SET(fd, &w);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		ptv = &tv;
	}

	r = KSELECT(fd+1, NULL, &w, NULL, ptv, &o);
	if (r == EINTR) {
		SignalError("java.io.InterruptedIOException", SYS_ERROR(r));
	}
	if (r != 0) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
	if (o != 0 && FD_ISSET(fd, &w))
		unhand(this)->connecting = false;
}
