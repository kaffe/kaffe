/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.net;

public interface SocketOptions {

/*
 * Various options.  Note that these constants match those used by Sun.
 */
int TCP_NODELAY 		= 0x0001;
int SO_BINDADDR  		= 0x000F;
int SO_REUSEADDR 		= 0x0004;
int IP_MULTICAST_IF 	= 0x0010;
int SO_LINGER 		= 0x0080;
int SO_TIMEOUT 		= 0x1006;
int SO_SNDBUF 		= 0x1001;
int SO_RCVBUF 		= 0x1002;

void setOption(int option, Object data) throws SocketException;
Object getOption(int option) throws SocketException;

}

