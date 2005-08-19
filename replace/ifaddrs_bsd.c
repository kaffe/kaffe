/*
 * ifaddrs_bsd.c
 *
 * Replacement of getifaddrs() function using BSD syscalls (SIOC*)
 *
 * Copyright(c) 2005 The kaffe.org's developpers.
 *   All Rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include "config.h"

#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined(HAVE_STROPTS_H)
#include <stropts.h>
#endif

#include "config-std.h"
#include "config-net.h"
#include "config-io.h"
#include "ifaddrs.h"

#if defined(HAVE_SYS_SOCKIO_H)
#include <sys/sockio.h>
#endif

#if defined(HAVE_NET_IF_H)
#include <net/if.h>
#endif

static int
getSockAddrLen(struct sockaddr* addr)
{
  switch (addr->sa_family)
    {
    case AF_INET:
      return sizeof(struct sockaddr_in);
#if defined(HAVE_STRUCT_SOCKADDR_IN6)
    case AF_INET6:
      return sizeof(struct sockaddr_in6);
#endif
    default:
      return sizeof(struct sockaddr);
    }
}

int getifaddrs(struct ifaddrs **ifap)
{
  int sockfd;
  int bufSize = 1024;
  char *buf = NULL;
  char *ptr;
  int isFull;
  int i;
  int numberOfIfaces;
  size_t len;
  struct ifaddrs *allIfaces;
  struct ifconf ifc;
  struct ifreq *ifr;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&ifc, 0, sizeof(struct ifconf));
  do
    {
      if (buf != NULL)
	free(buf);
      else
	bufSize *= 2;

      buf = (char *)malloc(bufSize);
      memset(buf, 0, bufSize);
      
      ifc.ifc_len = bufSize;
      ifc.ifc_req = (struct ifreq *)buf;

      if (ioctl (sockfd, SIOCGIFCONF, &ifc) <  0)
	{
	  free(buf);
	  return -1;
	}

      isFull = (ifc.ifc_len == bufSize);
    }
  while (isFull);

  numberOfIfaces = ifc.ifc_len / (sizeof(struct sockaddr) + sizeof(ifr->ifr_name));
  
  allIfaces = (struct ifaddrs *)malloc(numberOfIfaces * sizeof(struct ifaddrs));

  for (ptr = buf, i = 0; ptr < buf + ifc.ifc_len; i++)
  {
    ifr = (struct ifreq *) ptr;
    
    allIfaces[i].ifa_name = strdup(ifr->ifr_name);
#if defined(HAVE_SOCKADDR_SA_LEN) || defined(__FreeBSD__)
    if (ifr->ifr_addr.sa_len > len)
      len = ifr->ifr_addr.sa_len;	/* length > 16 */
    else
#endif
      len = getSockAddrLen(&ifr->ifr_addr);

    allIfaces[i].ifa_addr = (struct sockaddr *)malloc(len);
    allIfaces[i].ifa_next = &allIfaces[i+1];
    
    memcpy(allIfaces[i].ifa_addr, &ifr->ifr_addr, len);
    
    ptr += sizeof(ifr->ifr_name) + len;
  }
  allIfaces[i-1].ifa_next = NULL;
  
  free(ptr);
  close(sockfd);

  *ifap = allIfaces;

  return 0;
}

void freeifaddrs(struct ifaddrs *ifp)
{
  struct ifaddrs *allIfaces = ifp;

  while (allIfaces != NULL)
    {
      free(allIfaces->ifa_name);
      free(allIfaces->ifa_addr);
      allIfaces = allIfaces->ifa_next;
    }

  if (ifp != NULL)
    free(ifp);
}

