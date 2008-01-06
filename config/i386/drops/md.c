/*
 * Getenv hack
 * Mockup implementations of unimplemented/unsupported functions in DROPS
 * 
 * Copyright (c) 2004 - 2006
 *	TU Dresden, Operating System Group.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * written by Alexander Boettcher <boettcher@os.inf.tu-dresden.de>
 */

#include <stdio.h>      /* printf */
#include <string.h>     /* memcpy, str* */
#include <stdlib.h>     /* malloc, free */
#include <fcntl.h>      /* O_RDONLY */
#include <signal.h>	/* kill */
#ifdef USE_UCLIBC
#include <unistd.h>     /* environ */
#endif
#include <sys/socket.h> /* getnameinfo, getaddrinfo, freeaddrinfo */
#include <sys/types.h>  /* utime */
#include <utime.h>      /* utime */
#include <sys/wait.h>	/* waitpid */
#include <sys/stat.h>	/* chmod */

#include <l4/l4vfs/basic_name_server.h>
#include <l4/log/l4log.h> /* LOG */
#include <l4/util/util.h> /* for l4_sleep*/
#include <l4/sys/types.h>
#include <l4/l4vfs/name_server.h>
#include <l4/l4vfs/extendable.h>

#ifdef REALTIME_EXTENSION
extern void area_immortal_init(void);
#endif

#include "config.h"

char LOG_tag[9] = "kaffe";

const int l4thread_max_threads = 128;

/**
 * functions and variables not implemented by dietlibc or by uclibc
 */
#ifdef USE_UCLIBC
char **environ = NULL;
#endif
#ifdef USE_DIETLIBC
int h_errno = 0;
#endif

struct servent *getservbyname (UNUSED const char *__name,
                               UNUSED const char *__proto)
{
  LOG("to do");
  return 0;
}

struct servent *getservbyport (UNUSED int __port,
                               UNUSED const char *__proto)
{
  LOG("to do");
  return 0;
}

#ifdef USE_UCLIBC
int utime(const char* filename UNUSED, const struct utimbuf* buf UNUSED) {
#else
int utime(const char* filename UNUSED, struct utimbuf* buf UNUSED) {
#endif
  LOG("to do");
  return 0;
}

int pipe(int filedes[2] UNUSED) {
  LOG("to do");
  return -1;
}

int dup2 (int oldfd UNUSED, int newfd UNUSED) {
  LOG("to do");
  return 0;
}


long sysconf(int name UNUSED) {
  LOG("to do");
  return 0;
}

struct hostent *gethostbyaddr (UNUSED const void *__addr,
                               UNUSED socklen_t __len,
                               UNUSED int __type)
{
  LOG("to do");
  return 0;
}

int fsync(int fd UNUSED) {
  LOG("to do");
  return 0;
}

#ifdef USE_UCLIBC
int getnameinfo (const struct sockaddr *sa,
		 socklen_t salen,
		 char *host,
		 socklen_t hostlen,
		 UNUSED char *serv,
	 	 UNUSED socklen_t servlen,
		 UNUSED unsigned int flags)
#else
int getnameinfo(const struct sockaddr *sa,
		socklen_t salen,
	       	char *host,
		size_t hostlen,
	       	UNUSED char *serv,
	       	UNUSED size_t servlen,
	       	UNUSED int flags)
#endif
{
  unsigned long iaddr = *(const unsigned long *)&sa->sa_data[2];

  LOG("to do %lu %d",iaddr,hostlen);

  if (sa != NULL && host != NULL)
  {
    if (salen == 16)
      switch(iaddr)
      {
        case 0:
          return EAI_NONAME;
        case 0x0100007F: // 127.0.0.1
          memcpy(host,"localhost",9);
          return 0;
        default :
          return EAI_SYSTEM;
      }
  }
  return EAI_SYSTEM;
}

int getaddrinfo(UNUSED const char *node,
                UNUSED const char *service,
                UNUSED const struct addrinfo *hints,
                UNUSED struct addrinfo **res)
{
  LOG("to do");
  return -1;
}

void freeaddrinfo(UNUSED struct addrinfo *res)
{
  LOG("to do");
}

int chmod (UNUSED const char *__file,
           UNUSED mode_t __mode)
{
  LOG("to do");
  return 0;
}

/**
 * DROPS specific
 */

#define l4_environment_len 4 
static struct { const char * name;
                const char * env;
              } l4_environment[l4_environment_len]
  = {{"BOOTCLASSPATH"   , "/kaffe/rt.jar"},
     {"KAFFELIBRARYPATH", "."},
     {"KAFFECLASSPATH"  , "/kaffe"},
     {"TZ"              , "CET-1CEST"}};

void drops_kaffe_main(int argc, char *argv[]);

void drops_kaffe_main(UNUSED int argc,
                      UNUSED char *argv[])
{
  unsigned int i;
  unsigned long len;
  int fd = -1;
  FILE * fEnv;
  char buffer[255];
  char * tmp;

  l4_sleep(3000);

  fd = open("/linux/vc0", O_RDONLY);
  if (fd != 0) LOG("stdin could not be opened, fd was %d!=0 !",fd);
  fd = open("/linux/vc0", O_WRONLY);
  if (fd != 1) LOG("stdout could not be opened, fd was %d!=1 !",fd);
  fd = open("/linux/vc0", O_WRONLY);
  if (fd != 2) LOG("stdout could not be opened, fd was %d!=2 !",fd);

  fEnv = fopen("/kaffe/kaffepath.env","r");

  if (fEnv != NULL){
    while(fgets((char *)&buffer, 254, fEnv) != NULL){
      for (i=0; i < l4_environment_len; i++){
        if ( strncmp(l4_environment[i].name, buffer,
                     strlen(l4_environment[i].name)) == 0 ){
          //for example "PATH=/home"
          len = strlen(buffer) - strlen(l4_environment[i].name) - 1;
          if (len > 0){
            tmp = malloc(len);
            strncpy(tmp,
                    &buffer[strlen(l4_environment[i].name)+1],
                    len-1 );
            *((char * )tmp + len - 1) = 0;
            l4_environment[i].env = tmp;
          }
        }
      }
    }
    fclose(fEnv);
  }else LOG("File kaffepath.env not found. Use standard environment values !");

 #ifdef REALTIME_EXTENSION
   area_immortal_init();
 #endif

}

char * getenv(const char*env){
  char *cReturn = 0;	
  unsigned int i;

  for (i=0;i<l4_environment_len;i++){
    if (strncmp(l4_environment[i].name, env,
                strlen(l4_environment[i].name)) == 0){
      cReturn = l4_environment[i].env;
      i = l4_environment_len;
    }
  }
  return cReturn;
}
