/*
 * config-io.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __config_io_h
#define __config_io_h

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_POLL_H)
#include <poll.h>
#endif
#if defined(HAVE_SYS_POLL_H)
#include <sys/poll.h>
#endif
#if defined(HAVE_SYS_SELECT_H)
#include <sys/select.h>
#endif
#if defined(HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif
#if defined(HAVE_SYS_IOCTL_H)
#include <sys/ioctl.h>
#endif
#if defined(HAVE_IO_H)
#include <io.h>
#endif
#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined(HAVE_SYS_FILIO_H)
#include <sys/filio.h>
#endif
#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_SYS_PARAM_H)
#include <sys/param.h>
#endif

#if defined(HAVE_DIRENT_H)
# include <dirent.h>
# define NAMLEN(dirent) (strlen((dirent)->d_name))
#elif defined(HAVE_SYS_NDIR_H) || defined(HAVE_NDIR_H) || defined(HAVE_SYS_DIR_H)
# define dirent direct
# define NAMLEN(dirent) ((dirent)->d_namlen)
# if defined(HAVE_SYS_NDIR_H)
#  include <sys/ndir.h>
# endif
# if defined(HAVE_NDIR_H)
#  include <ndir.h>
# endif
# if defined(HAVE_SYS_DIR_H)
#  include <sys/dir.h>
# endif
#endif

#if !defined(S_ISDIR)
#define	S_ISDIR(m)	((m) & S_IFDIR)
#endif
#if !defined(S_ISREG)
#define	S_ISREG(m)	((m) & S_IFREG)
#endif

#if !defined(SEEK_SET)
#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2
#endif

#if !defined(O_BINARY)
#define	O_BINARY	0
#endif

#if !defined(FD_COPY)
#define	FD_COPY(a, b)	memcpy(b, a, sizeof(*(b)))
#endif

#endif
