/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

import java.io.FileDescriptor;
import java.lang.Thread;
import java.lang.Class;
import java.net.InetAddress;
import kaffe.util.Deprecated;

abstract public class SecurityManager {

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

public void checkAwtEventQueueAccess() {
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

public void checkMemberAccess ( Class clazz, int which ) {
	throw new SecurityException();
}

public void checkMulticast(InetAddress maddr) {
	throw new SecurityException();
}

public void checkMulticast(InetAddress maddr, byte ttl) {
	throw new SecurityException();
}

public void checkPackageAccess(String pkg) {
	throw new SecurityException();
}

public void checkPackageDefinition(String pkg) {
	throw new SecurityException();
}

public void checkPrintJobAccess() {
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

public void checkSecurityAccess(String action) {
	throw new SecurityException();
}

public void checkSetFactory() {
	throw new SecurityException();
}

public void checkSystemClipboardAccess() {
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

protected int classDepth(String name) {
	throw new Deprecated();
}

protected int classLoaderDepth() {
	throw new Deprecated();
}

protected Class currentLoadedClass() {
	Class[] classes = getClassContext();
	for (int i = 0; i < classes.length; i++) {
		if (classes[i].getClassLoader() != null) {
			return (classes[i]);
		}
	}
	return (null);
}

public boolean getInCheck() {
	throw new Deprecated();
}

public Object getSecurityContext() {
	return (null);
}

public ThreadGroup getThreadGroup() {
	return (Thread.currentThread().getThreadGroup());
}

protected boolean inClass(String name) {
	throw new Deprecated();
}

protected boolean inClassLoader() {
	throw new Deprecated();
}

protected Class[] getClassContext() {
	return (getClassContext0());
}

protected ClassLoader currentClassLoader() {
	Class[] classes = getClassContext0();
	for (int i = 0; i < classes.length; i++) {
		ClassLoader loader = classes[i].getClassLoader();
		if (loader != null) {
			return (loader);
		}
	}
	return (null);
}

native static private Class[] getClassContext0();

}
