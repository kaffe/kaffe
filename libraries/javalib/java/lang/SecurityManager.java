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

import java.util.PropertyPermission;
import java.io.FileDescriptor;
import java.net.InetAddress;
import java.security.Permission;

public class SecurityManager {

public SecurityManager() {
	SecurityManager current = System.getSecurityManager();
	if (current != null) {
		current.checkPermission(
			new RuntimePermission("createSecurityManager"));
	}
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

public void checkPermission(Permission perm) {
	throw new SecurityException();
}

public void checkPermission(Permission perm, Object context) {
	throw new SecurityException();
}

public void checkPrintJobAccess() {
	throw new SecurityException();
}

public void checkPropertiesAccess() {
	this.checkPermission(new PropertyPermission("*", "read,write"));
}

public void checkPropertyAccess(String key) {
	this.checkPermission(new PropertyPermission(key, "read"));
}

void checkPropertyAccess(String key, String def) {
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

/**
 * @deprecated, Deprecated in 1.2
 */
protected int classDepth(String name) {
	Class[] classes = getClassContext();
	for (int i = 0; i < classes.length; i++) {
		if (name.equals(classes[i].getName())) {
			return (i);
		}
	}
	return (-1);
}

/**
 * @deprecated, Deprecated in 1.2
 */
protected int classLoaderDepth() {
	Class[] classes = getClassContext();
	for (int i = 0; i < classes.length; i++) {
		if (ClassLoader.class.isAssignableFrom(classes[i])) {
			return (i);
		}
	}
	return (-1);
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

/**
 * @deprecated, Deprecated in 1.2
 */
protected boolean inCheck;

/**
 * @deprecated, Deprecated in 1.2
 */
public boolean getInCheck() {
	return (inCheck);
}

public Object getSecurityContext() {
	return (null);
}

public ThreadGroup getThreadGroup() {
	return (Thread.currentThread().getThreadGroup());
}

/**
 * @deprecated, Deprecated in 1.2
 */
protected boolean inClass(String name) {
	return (classDepth(name) != -1);
}

/**
 * @deprecated, Deprecated in 1.2
 */
protected boolean inClassLoader() {
	return (classLoaderDepth() != -1);
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

// used by java.lang.Class.getStackClass()
native static /* private */ Class[] getClassContext0();

}
