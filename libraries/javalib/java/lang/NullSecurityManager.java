package java.lang;

import java.io.File;
import java.io.FileDescriptor;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class NullSecurityManager
  extends SecurityManager
{
	protected boolean inCheck = false;

NullSecurityManager() {
}

public void checkAccept(String host, int port) {
}

public void checkAccess(Thread other)
	{
	ThreadGroup ctg = Thread.currentThread().getThreadGroup();
	ThreadGroup otg = other.getThreadGroup();
	if (ctg != otg) {
		throw new SecurityException("must be in same thread group");
	}
}

public void checkAccess(ThreadGroup g) {
}

public void checkConnect(String host, int port) {
}

public void checkConnect(String host, int port, Object context) {
}

public void checkCreateClassLoader() {
}

public void checkDelete(String file) {
}

public void checkExec(String cmd) {
}

public void checkExit(int status) {
}

public void checkLink(String lib) {
}

public void checkListen(int port) {
}

public void checkMemberAccess ( Class clazz, int which ) {
}

public void checkPackageAccess(String pkg) {
}

public void checkPackageDefinition(String pkg) {
}

public void checkPropertiesAccess() {
}

public void checkPropertyAccess(String key) {
}

public void checkPropertyAccess(String key, String def) {
}

public void checkRead(FileDescriptor fd) {
}

public void checkRead(String file) {
}

public void checkRead(String file, Object context) {
}

public void checkSecurityAccess(String action)
	{
}

public void checkSetFactory() {
}

public boolean checkTopLevelWindow(Object window) {
	return (true);
}

public void checkWrite(FileDescriptor fd) {
}

public void checkWrite(String file) {
}

native protected int classDepth(String name);

native protected int classLoaderDepth();

native protected ClassLoader currentClassLoader();

native protected Class[] getClassContext();

public boolean getInCheck() {
	return inCheck;
}

public Object getSecurityContext() {
	return null;
}

protected boolean inClass(String name) {
	return (classDepth(name)>0);
}

protected boolean inClassLoader() {
	return (classLoaderDepth()>0);
}
}
