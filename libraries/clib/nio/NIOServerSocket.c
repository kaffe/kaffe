/*
 * NIOServerSocket.c
 *
 * Copyright (c) 2003 Kaffe's team.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "support.h"

/* Classpath uses the same name for different variables in the inheritancy
 * tree. Kaffeh treats them straightly and it confuses the compiler.
 * As it is not important here, I just declare the class manually.
 */
struct Hgnu_java_net_PlainSocketImpl;
struct Hgnu_java_nio_NIOServerSocket;

struct Hgnu_java_net_PlainSocketImpl*
gnu_java_nio_NIOServerSocket_getPlainSocketImpl(struct Hgnu_java_nio_NIOServerSocket* this)
{
  return (struct Hgnu_java_net_PlainSocketImpl *)
    do_execute_java_method(this, "getImpl", "()Ljava/net/SocketImpl;", 0, 0).l;
}
