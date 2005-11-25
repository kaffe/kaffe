/*
 * syscall.c
 * Definition of jsyscall interface for Kaffe on DROPS.
 * 
 * Copyright (c) 2004, 2005
 *	TU Dresden, Operating System Group.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * written by Alexander Boettcher <ab764283@os.inf.tu-dresden.de>
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-net.h"
#include "config-io.h"
#include "config-signal.h"
#include "jthread.h"
#include "jsyscall.h"

static int drops_open(const char *path,
                      int f, int m,
                      int *outfd) {

  *outfd = open(path, f, m);
  if (*outfd == -1) 
    return errno;

  return 0;
}

static int drops_read(int fd, void* buffer,
                      size_t len, ssize_t *out) {
  int nReturn = 0;

  *out = read(fd, buffer, len);

  if (*out == -1)
    nReturn = errno;

  return nReturn;
}

static int drops_write(int fd, const void* buffer,
                       size_t len, ssize_t *out) {
  int nReturn = 0;

  if (fd==1 || fd==2)
  { //STDOUT, STDERR
    nReturn = write(fd,buffer,len);
    if (nReturn == -1)
      nReturn = errno;
    else
    {
      nReturn = 0;
      *out    = len;
    }
  }
  else
    printf("unknown fd=%d len=%d\n",fd,len);

  return nReturn;
}

static int drops_lseek(int fd, off_t o, int type,
                       off_t *out) {
  int nReturn = 0;

  *out = lseek(fd, o, type);

  if (*out == -1)
    nReturn = errno;
 
  return nReturn;
}

static int drops_close(int fd) {
  int nReturn = 0;

  if (close(fd) < 0)
    nReturn = errno;

  return nReturn;
}

static int drops_fstat(int fd, struct stat *st) {
  int nReturn = 0;

  if (fstat(fd, st) == -1)
    nReturn = errno;
  
  return nReturn;
}

static int drops_stat(const char *p, struct stat *st){
  int nReturn = 0;

  if (stat(p, st) == -1)
    nReturn = errno;
 
  return nReturn;
}

static int drops_ftruncate(int fd UNUSED,
                           off_t new_size UNUSED) {
 unimp("ftruncate() not implemented");
 return -1;
}

static int drops_fsync(int a UNUSED) {
 unimp("fsync() not implemented");
 return -1;
}

static int drops_mkdir(const char *p UNUSED,
                       int m UNUSED) {
 unimp("mkdir() not implemented");
 return -1;
}

static int drops_rmdir(const char *p UNUSED) {
 unimp("rmdir() not implemented");
 return -1;
}

static int drops_rename(const char *p UNUSED,
                        const char *q UNUSED) {
 unimp("rename() not implemented");
 return -1;
}

static int drops_remove(const char *p UNUSED) {
 unimp("remove() not implemented");
 return -1;
}

static int drops_socket(int fd, int type,
                        int proto, int *outfd) {
  int r = socket(fd, type, proto);

  if (r < 0)
    r = errno;
  else {
    *outfd = r;
    r = 0;
  }

  return (r);
}

static int drops_connect(int fd, struct sockaddr* serv_addr,
                         socklen_t addrlen, int timeout UNUSED) {
  int nReturn = connect(fd, serv_addr, (socklen_t)addrlen);

  if (nReturn < 0)
    nReturn = errno;

  return nReturn;
}

static int drops_bind(int fd, struct sockaddr* addr,
                      socklen_t addrlen) {
  int nReturn = bind(fd, addr, (socklen_t)addrlen);

  if (nReturn < 0)
    nReturn = errno;

  return nReturn;
}

static int drops_listen(int fd, int backlog) {
  int nReturn = listen(fd, backlog);

  if (nReturn < 0)
    nReturn = errno;

  return nReturn;
}

static int drops_accept(int socketfd, struct sockaddr* addr,
                        socklen_t *addrlen, int timeout UNUSED, int* outfd) {
  int nReturn = accept(socketfd, addr, addrlen);

  if (nReturn < 0)
    nReturn = errno;
  else {
    *outfd  = nReturn;
    nReturn = 0;
  }

  return nReturn;
}

static int drops_sockread(int socketfd, void* buf, size_t len,
                          int timeout UNUSED, ssize_t *out) {
  int nReturn = recv(socketfd, buf, len, 0);
 
  if (nReturn < 0)
    nReturn = errno;
  else {
    *out = nReturn;
    nReturn = 0;
  }

  return nReturn;
}

static int drops_recvfrom(int a UNUSED, void* b UNUSED,
                          size_t c UNUSED, int d UNUSED,
                          struct sockaddr* e UNUSED,
                          socklen_t* f UNUSED, int timeout UNUSED,
                          ssize_t *out UNUSED) {
  unimp("recvfrom() not implemented");
  return -1;
}

static int drops_sockwrite(int socketfd, const void* msg,
                           size_t len, ssize_t *out) { 
  int nReturn = send(socketfd, msg, len, 0);

  if (nReturn < 0)
    nReturn = errno;
  else {
    *out = nReturn;
    nReturn = 0;
  }

  return nReturn;
}

static int drops_sendto(int a UNUSED,
                                     const void* b UNUSED,
                                     size_t c UNUSED,
                                     int d UNUSED,
                                     const struct sockaddr* e UNUSED,
                                     int f UNUSED, ssize_t *out UNUSED) {
  unimp("sendto() not implemented");
  return -1;
}

static int drops_setsockopt(int fd, int level,
                            int optname,
                            const void* optval,
                            int optlen) {
  int nReturn = setsockopt(fd, level, optname, optval, (socklen_t)optlen);

  if (nReturn < 0)
    nReturn = errno;

  return nReturn;
}

static int drops_getsockopt(int a UNUSED,
                                         int b UNUSED,
                                         int c UNUSED,
                                         void* d UNUSED,
                                         socklen_t* e UNUSED) {
  unimp("getsockopt() not implemented");
  return -1;
}

static int drops_getsockname(int socketfd, struct sockaddr* name,
                             socklen_t * namelen) {
  int nReturn = getsockname(socketfd, name , namelen);

  if (nReturn < 0)
    nReturn = errno;

  return nReturn;
}

static int drops_getpeername(int a UNUSED, struct sockaddr* b UNUSED,
                             socklen_t* c UNUSED) {
  printf("getpeername todo\n");
  return 0;
}

static int drops_sockclose(int fd) {
  int nReturn = close(fd);

  if (nReturn < 0)
    nReturn = errno;

  return nReturn;
}

static int drops_sockshutdown(int fd)
{
  int nReturn = shutdown(fd, 2);

  if (nReturn < 0)
    nReturn = errno;

  return nReturn;
}

static int drops_gethostbyname(const char *name UNUSED,
                               struct hostent ** out2 UNUSED)
{
  unimp("gethostbyname not implemented");
  return HOST_NOT_FOUND;
}

static int drops_gethostbyaddr(const char *name UNUSED, size_t len UNUSED,
                               int type UNUSED, struct hostent**out UNUSED) {

  unimp("gethostbyaddr not implemented");
  return HOST_NOT_FOUND;
}

static int drops_select(int a, fd_set* readfds, fd_set* writefds,
                        fd_set* exceptfds, struct timeval* timeout, int* out) {
  int nReturn = 0;

  *out = select(a, readfds, writefds, exceptfds, timeout);

  if (*out < 0)
    nReturn = errno;

  return nReturn; 
}

static int drops_forkexec(char *argv[] UNUSED, char *env[] UNUSED,
                          int fd[4] UNUSED, int *outpid UNUSED,
                          const char * a UNUSED) {
  unimp("forkexec() not implemented");
  return -1;
}

static int drops_waitpid(int a UNUSED, int* b UNUSED,
                         int c UNUSED, int* out UNUSED) {
  unimp("waitpid() not implemented");
  return -1;
}

static int drops_kill(int a UNUSED, int b UNUSED) {
  unimp("kill() not implemented");
  return -1;
}

static int drops_mmap(void **memory UNUSED, size_t *size UNUSED,
                      int mode UNUSED, int fd UNUSED,
                      off_t *offset UNUSED) {
  unimp("mmap() not implemented");
  return -1;
}

static int drops_munmap(void *memory UNUSED,
                        size_t size UNUSED) {
  unimp("munmap() not implemented");
  return -1;
}

static int drops_msync(void *memory UNUSED,
                       size_t length UNUSED) {
  unimp("msync() not implemented");
  return -1;
}

static int drops_pipecreate(int * a UNUSED, int *b UNUSED) {
  unimp("pipecreate() not implemented");
  return -1;
}

static int drops_piperead(int a UNUSED, void * b UNUSED,
                          size_t c UNUSED, int timeout UNUSED,
                          ssize_t *d UNUSED) {
  unimp("piperead() not implemented");
  return -1;
}

static int drops_pipewrite(int a UNUSED, const void* b UNUSED,
                           size_t c UNUSED, int timeout UNUSED,
                           ssize_t * d UNUSED) {
  unimp("pipewrite() not implemented");
  return -1;
}

SystemCallInterface Kaffe_SystemCallInterface = {
	drops_open,
	drops_read,
	drops_write, 
	drops_lseek,
	drops_close,
	drops_fstat,
	drops_stat,
	drops_ftruncate,
        drops_fsync,
	drops_mkdir,
	drops_rmdir,
	drops_rename,
	drops_remove,
	drops_socket,
	drops_connect,
	drops_bind,
	drops_listen,
	drops_accept, 
	drops_sockread,	
	drops_recvfrom,
	drops_sockwrite, 
	drops_sendto,	
	drops_setsockopt,
	drops_getsockopt,
	drops_getsockname, 
	drops_getpeername,
	drops_sockclose,
	drops_sockshutdown,
	drops_gethostbyname,
	drops_gethostbyaddr,
	drops_select,	
	drops_forkexec,
	drops_waitpid,
	drops_kill,
	drops_mmap,
        drops_munmap,
        drops_msync,
        drops_pipecreate,
        drops_piperead,
        drops_pipewrite
};
