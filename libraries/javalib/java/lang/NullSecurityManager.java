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
import java.lang.SecurityManager;
import java.net.InetAddress;
import java.security.Permission;

class NullSecurityManager extends SecurityManager {

NullSecurityManager() {
}

public void checkAccept(String host, int port) {
}

public void checkAccess(Thread g) {
}

public void checkAccess(ThreadGroup g) {
}

public void checkAwtEventQueueAccess() {
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

public void checkMulticast(InetAddress maddr) {
}

public void checkMulticast(InetAddress maddr, byte ttl) {
}

public void checkPackageAccess(String pkg) {
}

public void checkPackageDefinition(String pkg) {
}

public void checkPermission(Permission perm) {
}

public void checkPermission(Permission perm, Object context) {
}

public void checkPrintJobAccess() {
}

public void checkPropertiesAccess() {
}

public void checkPropertyAccess(String key) {
}

/* public void checkPropertyAccess(String key, String def) {
}*/

public void checkRead(FileDescriptor fd) {
}

public void checkRead(String file) {
}

public void checkRead(String file, Object context) {
}

public void checkSecurityAccess(String action) {
}

public void checkSetFactory() {
}

public void checkSystemClipboardAccess() {
}

public boolean checkTopLevelWindow(Object window) {
	return (true);
}

public void checkWrite(FileDescriptor fd) {
}

public void checkWrite(String file) {
}

}
