package java.lang;

import java.io.FileDescriptor;
import java.net.InetAddress;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
abstract public class SecurityManager
{
	protected boolean inCheck;

protected SecurityManager() {
}

public void checkAccept(String host, int port) {
	throw new SecurityException();
}

public void checkAccess(Thread g) {
	throw new SecurityException();
}

public void checkAccess(ThreadGroup g) {
	throw new SecurityException();
}

public void checkAwtEventQueueAccess()
	{
	throw new SecurityException();
}

public void checkConnect(String host, int port) {
	throw new SecurityException();
}

public void checkConnect(String host, int port, Object context) {
	throw new SecurityException();
}

public void checkCreateClassLoader() {
	throw new SecurityException();
}

public void checkDelete(String file) {
	throw new SecurityException();
}

public void checkExec(String cmd) {
	throw new SecurityException();
}

public void checkExit(int status) {
	throw new SecurityException();
}

public void checkLink(String lib) {
	throw new SecurityException();
}

public void checkListen(int port) {
	throw new SecurityException();
}

public void checkMemberAccess ( Class clazz, int which )
{
	throw new SecurityException();
}

public void checkMulticast(InetAddress maddr)
	{
	throw new SecurityException();
}

public void checkMulticast(InetAddress maddr, byte ttl)
	{
	throw new SecurityException();
}

public void checkPackageAccess(String pkg) {
	throw new SecurityException();
}

public void checkPackageDefinition(String pkg) {
	throw new SecurityException();
}

public void checkPrintJobAccess()
	{
	throw new SecurityException();
}

public void checkPropertiesAccess() {
	throw new SecurityException();
}

public void checkPropertyAccess(String key) {
	throw new SecurityException();
}

public void checkPropertyAccess(String key, String def) {
	throw new SecurityException();
}

public void checkRead(FileDescriptor fd) {
	throw new SecurityException();
}

public void checkRead(String file) {
	throw new SecurityException();
}

public void checkRead(String file, Object context) {
	throw new SecurityException();
}

public void checkSecurityAccess(String action)
	{
	throw new SecurityException();
}

public void checkSetFactory() {
	throw new SecurityException();
}

public void checkSystemClipboardAccess()
	{
	throw new SecurityException();
}

public boolean checkTopLevelWindow(Object window) {
	throw new SecurityException();
}

public void checkWrite(FileDescriptor fd) {
	throw new SecurityException();
}

public void checkWrite(String file) {
	throw new SecurityException();
}

native protected int classDepth(String name);

native protected int classLoaderDepth();

native protected ClassLoader currentClassLoader();

protected Class currentLoadedClass()
	{
	return (null);
}

native protected Class[] getClassContext();

public boolean getInCheck() {
	return inCheck;
}

public Object getSecurityContext() {
	return null;
}

public ThreadGroup getThreadGroup()
	{
	return (null);
}

protected boolean inClass(String name) {
	return (classDepth(name)>0);
}

protected boolean inClassLoader() {
	return (classLoaderDepth()>0);
}
}
