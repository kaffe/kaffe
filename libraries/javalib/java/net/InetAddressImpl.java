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


class InetAddressImpl {

static {
	System.loadLibrary("net");
}

native public int getInetFamily();
native public int[] lookupAllHostAddr(String host) throws UnknownHostException;
native public int lookupHostAddr(String host) throws UnknownHostException;
native public String getHostByAddr(int addr) throws UnknownHostException;
native public String getLocalHostName();
native public void makeAnyLocalAddress(InetAddress addr);

}
