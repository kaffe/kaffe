package java.net;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class InetAddressImpl
{
native public String getHostByAddr(int addr) throws UnknownHostException;

native public int getInetFamily();

native public String getLocalHostName();

native public byte[][] lookupAllHostAddr(String host) throws UnknownHostException;

native public byte[] lookupHostAddr(String host) throws UnknownHostException;

native public void makeAnyLocalAddress(InetAddress addr);
}
