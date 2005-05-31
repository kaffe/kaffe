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
#include <arpa/inet.h>
#include "gtypes.h"
#include "object.h"
#include <native.h>
#include "itypes.h"
#include "stringSupport.h"
#include "support.h"
#include "java_net_VMInetAddress.h"
#include "gnu_java_net_SysInetAddressImpl.h"
#include "nets.h"
#include "jsyscall.h"
#include "classMethod.h"
#include "utf8const.h"

#include "baseClasses.h"
#include "locks.h"

#include "dummyin6.h"

#if !defined(HAVE_GETADDRINFO) || !defined(HAVE_GETNAMEINFO)
#include "getaddrinfo.h"
#endif

#define	HOSTNMSZ	1024

HArrayOfByte*
java_net_VMInetAddress_lookupInaddrAny(void)
{
  HArrayOfByte* addr = NULL;
  errorInfo einfo;

  addr = (HArrayOfByte *)newArrayChecked(TYPE_CLASS(TYPE_Byte),
					 4,
					 &einfo);
  if (addr)
    {
      unhand_byte_array(addr)[0] = (INADDR_ANY >> 24) & 0xFF; 
      unhand_byte_array(addr)[1] = (INADDR_ANY >> 16) & 0xFF; 
      unhand_byte_array(addr)[2] = (INADDR_ANY >> 8) & 0xFF; 
      unhand_byte_array(addr)[3] = (INADDR_ANY) & 0xFF;
    }
  else
    throwError(&einfo);

  return addr;
}

static Hjava_lang_Class* inetClass;
static Hjava_lang_Class* SysInetClass;
static int inetLockInit;
static int nsLockInit;
static iStaticLock inetLock;
static iStaticLock nsLock;

/* TODO: This is functional but ugly and should be updated later. */
static void initInetLock(void)
{
  errorInfo einfo;

  if (inetLockInit != 0)
    return;

  if (inetClass == NULL)
    {
      Utf8Const *name = utf8ConstNew("java/net/InetAddress", -1);
      inetClass = loadClass(name, NULL, &einfo);
      utf8ConstRelease(name);
      assert(inetClass != NULL);
    }
  lockClass(inetClass);
  if (inetLockInit == 0)
    {
      initStaticLock(&inetLock);
      inetLockInit = 1;
    }
  unlockClass(inetClass);
}

/*
 * Get localhost name.
 */
struct Hjava_lang_String*
java_net_VMInetAddress_getLocalHostname(void)
{
  static char hostname[HOSTNMSZ] = "localhost";
	
  struct Hjava_lang_String *retval = NULL;

  initInetLock();
  lockStaticMutex(&inetLock);
  if( gethostname(hostname, HOSTNMSZ - 1) < 0 )
    {
      perror("gethostname");
      KAFFEVM_ABORT();
    }
  retval = stringC2Java(hostname);
  unlockStaticMutex(&inetLock);

  return( checkPtr(retval) );
}

static void initNsLock(void)
{
  errorInfo einfo;

  if (nsLockInit != 0)
    return;

  if (SysInetClass == NULL)
    {
      Utf8Const *name = utf8ConstNew("gnu/java/net/SysInetAddressImpl", -1);
      SysInetClass = loadClass(name, NULL, &einfo);
      utf8ConstRelease(name);
      assert(SysInetClass != NULL);
    }

  lockClass(SysInetClass);
  if (nsLockInit == 0)
    {
      initStaticLock(&nsLock);
      nsLockInit = 1;
    }
  unlockClass(SysInetClass);
}

/*
 * Convert a hostname to an array of host addresses.
 */
HArrayOfArray*
gnu_java_net_SysInetAddressImpl_getHostByName(
		struct Hgnu_java_net_SysInetAddressImpl *this UNUSED,
		struct Hjava_lang_String* jStr)
{
#if defined(HAVE_GETADDRINFO)
  int i = 0, retryCount = 5, rc;
  jsize count = 0;
  struct addrinfo hints, *ai = NULL, *curr;
  HArrayOfArray *retval = NULL;
  errorInfo einfo;
  char *name;

  initNsLock();

  name = checkPtr(stringJava2C(jStr));
	
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  lockStaticMutex(&nsLock);
  while( ((rc = getaddrinfo(name, NULL, &hints, &ai)) ==
	  EAI_AGAIN) &&
	 (retryCount > 0) )
    {
      unlockStaticMutex(&nsLock);
      KTHREAD(sleep)((jlong)(1 * 1000));
      lockStaticMutex(&nsLock);
      retryCount -= 1;
    }
  unlockStaticMutex(&nsLock);
	
  switch( rc )
    {
    case 0:
      /* Count the number of addresses. */
      curr = ai;
      while( curr )
	{
	  switch( curr->ai_family )
	    {
	    case PF_INET:
#if defined(PF_INET6)
	    case PF_INET6:
#endif
	      count += 1;
	      break;
	    default:
	      break;
	    }
	  curr = curr->ai_next;
	}
		
      retval = (HArrayOfArray *)
	newArrayChecked(ObjectClass, count, &einfo);
      curr = ai;
      while( curr && retval )
	{
#if defined(PF_INET6)
	  struct sockaddr_in6 *in6;
#endif
	  struct sockaddr_in *in4;
	  HArrayOfByte *addr = NULL;
			
	  switch( curr->ai_family )
	    {
	    case PF_INET:
	      in4 = (struct sockaddr_in *)
		curr->ai_addr;
	      addr = (HArrayOfByte *)newArrayChecked(
						     TYPE_CLASS(TYPE_Byte),
						     sizeof(in4->sin_addr),
						     &einfo);
	      if( addr )
		{
		  memcpy(unhand_byte_array(addr),
			 &in4->sin_addr,
			 sizeof(in4->sin_addr));
		}
	      else
		{
		  retval = NULL;
		}
	      break;
#if defined(PF_INET6)
	    case PF_INET6:
	      in6 = (struct sockaddr_in6 *)
		curr->ai_addr;
	      addr = (HArrayOfByte *)newArrayChecked(
						     TYPE_CLASS(TYPE_Byte),
						     sizeof(in6->sin6_addr),
						     &einfo);
	      if( addr )
		{
		  memcpy(unhand_byte_array(addr),
			 &in6->sin6_addr,
			 sizeof(in6->sin6_addr));
		}
	      else
		{
		  retval = NULL;
		}
	      break;
#endif
	    default:
	      /* Ignore */
	      break;
	    }
	  if( addr && retval )
	    {
	      unhand_array_array(retval)[i] =
		&addr->base;
	      i += 1;
	    }
	  curr = curr->ai_next;
	}
      break;
    case EAI_FAIL:
    case EAI_AGAIN:
      postExceptionMessage(&einfo,
			   JAVA_NET(UnknownHostException),
			   "Unable to contact name server");
      break;
    case EAI_NONAME:
#if defined(EAI_NODATA) && EAI_NODATA != EAI_NONAME
    case EAI_NODATA:
#endif
      postExceptionMessage(&einfo,
			   JAVA_NET(UnknownHostException),
			   "Unknown host: %s",
			   name);
      break;
    case EAI_MEMORY:
      postOutOfMemory(&einfo);
      break;
    case EAI_SYSTEM:
      postExceptionMessage(&einfo,
			   JAVA_NET(UnknownHostException),
			   "%s: %s",
			   SYS_ERROR(errno),
			   name);
      break;
		
    default:
      postExceptionMessage(&einfo,
			   JAVA_LANG(InternalError),
			   "Unhandled getaddrinfo error: %s: %s",
			   gai_strerror(rc),
			   name);
      break;
    }
  if( ai )
    {
      freeaddrinfo(ai);
    }
  gc_free(name);
  if( !retval )
    {
      throwError(&einfo);
    }
  return( retval );
#else
  HArrayOfArray* retval = 0;
  char name[MAXHOSTNAME];
  struct hostent* ent;
  int i, alength, rc;
  errorInfo einfo;
	
  stringJava2CBuf(jStr, name, sizeof(name));
  rc = KGETHOSTBYNAME(name, &ent);
  if( rc == 0 )
    {
      for (alength = 0; ent->h_addr_list[alength]; alength++)
	;
		
      if( (retval = (HArrayOfArray*)
	   newArrayChecked(ObjectClass,
			   alength,
			   &einfo)) == 0 )
	{
	  postOutOfMemory(&einfo);
	  goto done;
	}
		
      for( i = 0; i < alength; i++ )
	{
	  HArrayOfByte *addr = 0;
			
	  /* Copy in the network address */
	  if( (addr = (HArrayOfByte *)
	       newArrayChecked(TYPE_CLASS(TYPE_Byte),
			       ent->h_length,
			       &einfo)) == 0 )
	    {
	      postOutOfMemory(&einfo);
	      goto done;
	    }
	  memcpy(unhand_byte_array(addr),
		 ent->h_addr_list[i],
		 ent->h_length);
			
	  unhand_array_array(retval)[i] = &addr->base;
	}
    }
  else
    {
      const char *msg;
		
      if( ent == (void *)-1 )
	{
	  msg = SYS_ERROR(rc);
	}
      else
	{
	  msg = SYS_HERROR(rc);
	}
      postExceptionMessage(&einfo,
			   JAVA_NET(UnknownHostException),
			   "%s: %s",
			   msg,
			   name);
    }
	
 done:
  if( !retval )
    {
      throwError(&einfo);
    }

  return( retval );
#endif
}

/*
 * Convert a network order address into the hostname.
 */
struct Hjava_lang_String*
gnu_java_net_SysInetAddressImpl_getHostByAddr(
		struct Hgnu_java_net_SysInetAddressImpl *this UNUSED,
		HArrayOfByte *addr)
{
#if defined(HAVE_GETADDRINFO)
  struct Hjava_lang_String *retval = NULL;
#if defined(AF_INET6)
  struct sockaddr_in6 sa_buf;
  struct sockaddr_in6 *sain6 = &sa_buf;
#else
  struct sockaddr_in sa_buf;
#endif
  struct sockaddr_in *sain = (struct sockaddr_in *)&sa_buf;
  int rc, retryCount = 5;
  errorInfo einfo;
  char *hostname;
  unsigned int sin_len;

  hostname = gc_malloc(NI_MAXHOST, KGC_ALLOC_FIXED);
  switch( addr->length )
    {
    case 4:
#if defined(BSD44)
      sain->sin_len = sizeof(struct sockaddr_in);
#else
      sin_len = sizeof(struct sockaddr_in);
#endif
      sain->sin_family = AF_INET;
      sain->sin_port = 0;
      memcpy(&sain->sin_addr, unhand_byte_array(addr), (size_t) addr->length);
      break;
#if defined(AF_INET6)
    case 16:
#if defined(BSD44)
      sain6->sin6_len = sizeof(struct sockaddr_in6);
#else
      sin_len = sizeof(struct sockaddr_in);
#endif
      sain6->sin6_family = AF_INET6;
      sain6->sin6_port = 0;
#if defined(HAVE_STRUCT_SOCKADDR_IN6_SIN6_FLOWINFO)
      sain6->sin6_flowinfo = 0;
#endif /* HAVE_STRUCT_SOCKADDR_IN6_SIN6_FLOWINFO */
      memcpy(&sain6->sin6_addr,
	     unhand_byte_array(addr),
	     (size_t) addr->length);
#if defined(HAVE_STRUCT_SOCKADDR_IN6_SIN6_SCOPE_ID)
      sain6->sin6_scope_id = 0;
#endif /* HAVE_STRUCT_SOCKADDR_IN6_SIN6_SCOPE_ID */
      break;
#endif
    default:
      postExceptionMessage(&einfo,
			   JAVA_LANG(InternalError),
			   "Illegal address length: %d",
			   addr->length);
      goto done;
    }
  lockStaticMutex(&nsLock);
  while( ((rc = getnameinfo((const struct sockaddr *)&sa_buf,
#if defined(BSD44)
			    sain->sin_len,
#else
			    sin_len,
#endif
			    hostname,
			    NI_MAXHOST,
			    NULL,
			    0,
			    NI_NAMEREQD)) == EAI_AGAIN) &&
	 (retryCount > 0) )
    {
      unlockStaticMutex(&nsLock);
      KTHREAD(sleep)((jlong)(1 * 1000));
      lockStaticMutex(&nsLock);
      retryCount -= 1;
    }

  unlockStaticMutex(&nsLock);
  switch( rc )
    {
    case 0:
      if( (retval = stringC2Java(hostname)) == 0 )
	{
	  postOutOfMemory(&einfo);
	}
      break;
    case EAI_FAIL:
    case EAI_AGAIN:
      postExceptionMessage(&einfo,
			   JAVA_NET(UnknownHostException),
			   "Unable to contact name server");
      break;
    case EAI_MEMORY:
      postOutOfMemory(&einfo);
      break;
    case EAI_NONAME:
      inet_ntop(sain->sin_family,
		unhand_byte_array(addr),
		hostname,
		NI_MAXHOST);
      postExceptionMessage(&einfo,
			   JAVA_NET(UnknownHostException),
			   "Unknown host: %s",
			   hostname);
      break;
    case EAI_SYSTEM:
      inet_ntop(sain->sin_family,
		unhand_byte_array(addr),
		hostname,
		NI_MAXHOST);
      postExceptionMessage(&einfo,
			   JAVA_NET(UnknownHostException),
			   "%s: %s",
			   SYS_ERROR(errno),
			   hostname);
      break;
    default:
      inet_ntop(sain->sin_family,
		unhand_byte_array(addr),
		hostname,
		NI_MAXHOST);
      postExceptionMessage(&einfo,
			   JAVA_LANG(InternalError),
			   "Unhandled getnameinfo error: %s: %s",
			   gai_strerror(rc),
			   hostname);
      break;
    }
  gc_free(hostname);
 done:
  if( !retval )
    {
      throwError(&einfo);
    }
	
  return( retval );
#else
  struct Hjava_lang_String *retval = 0;
#define MAX_IPV6_STRING_SIZE 128
  char ipaddr[MAX_IPV6_STRING_SIZE];
  struct hostent* ent;
  int family, rc = 0;
  const char *msg;
  errorInfo einfo;
	
  switch( obj_length(addr) )
    {
    case 4:
      family = AF_INET;
      break;
#if defined(AF_INET6)
    case 16:
      family = AF_INET6;
      break;
#endif
    default:
      postExceptionMessage(&einfo,
			   JAVA_LANG(InternalError),
			   "Illegal address length: %d",
			   obj_length(addr));
      goto done;
    }
  lockStaticMutex(&nsLock);
  rc = KGETHOSTBYADDR(unhand_byte_array(addr),
		      obj_length(addr),
		      family,
		      &ent);
  switch( rc )
    {
    case 0:
      retval = stringC2Java((char *)ent->h_name);
      break;
    default:
      if( ent == (void *)-1 )
	{
	  msg = SYS_ERROR(rc);
	}
      else
	{
	  msg = SYS_HERROR(rc);
	}
      postExceptionMessage(&einfo,
			   JAVA_NET(UnknownHostException),
			   "%s: %s",
			   msg,
			   inet_ntop(family,
				     unhand_byte_array(addr),
				     ipaddr,
				     sizeof(ipaddr)));
      break;
    }
 done:
  unlockStaticMutex(&nsLock);
  if( !retval )
    {
      throwError(&einfo);
    }
	
  return( checkPtr(retval) );
#endif
}
