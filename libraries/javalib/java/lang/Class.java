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

import java.io.InputStream;
import java.io.Serializable;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.net.URL;
import kaffe.lang.SystemClassLoader;

public final class Class implements Serializable {

private Class() { /* this class is not instantiable by the general public */ }

native public static Class forName(String className) throws ClassNotFoundException;

private String fullResourceName(String name) {
	if (name.charAt(0) != '/') {
		String cname = getName();
		StringBuffer buf = new StringBuffer();
		int tail = cname.lastIndexOf('.');
		if (tail != -1) {
			buf.append(cname.substring(0, tail+1).replace('.', '/'));
		}
		else {
			buf.append('/');
		}
		buf.append(name);
		name = buf.toString();
	}
	return (name);
}

/**
 * Determines the class loader for the class.
 *
 * @returns   the class loader that created the class or interface
 *            represented by this object, or null if the class was not
 *            created by a class loader.
 * @see       java.lang.ClassLoader
 */
public ClassLoader getClassLoader() {
	return (getClassLoader0());
}

native private ClassLoader getClassLoader0();

public Class[] getClasses() {
	return (getClasses0(false));
}

native private Class[] getClasses0(boolean declared);

native public Class getComponentType();

public Constructor getConstructor(Class parameterTypes[]) throws NoSuchMethodException, SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.PUBLIC);
	return (getConstructor0(parameterTypes, false));
}

native private Constructor getConstructor0(Class[] args, boolean declared);

public Constructor[] getConstructors() throws SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.PUBLIC);
	return (getConstructors0(false));
}

native private Constructor[] getConstructors0(boolean declared);

public Class[] getDeclaredClasses() throws SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.DECLARED);
	return (getClasses0(true));
}

public Constructor getDeclaredConstructor(Class parameterTypes[]) throws NoSuchMethodException, SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.DECLARED);
	return (getConstructor0(parameterTypes, true));
}

public Constructor[] getDeclaredConstructors() throws SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.DECLARED);
	return (getConstructors0(true));
}

public Field getDeclaredField(String name) throws NoSuchFieldException, SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.DECLARED);
	return (getField0(name, true));
}

public Field[] getDeclaredFields() throws SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.DECLARED);
	return (getFields0(true));
}

public Method getDeclaredMethod(String name, Class parameterTypes[]) throws NoSuchMethodException, SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.DECLARED);
	return (getMethod0(name, parameterTypes, true));
}

public Method[] getDeclaredMethods() throws SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.DECLARED);
	return (getMethods0(true));
}

public Class getDeclaringClass()
	{
	// Not sure what this is about.
	return (null);
}

public Field getField(String name) throws NoSuchFieldException, SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.PUBLIC);
	return (getField0(name, false));
}

native private Field getField0(String name, boolean declared);

public Field[] getFields() throws SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.PUBLIC);
	return (getFields0(false));
}

native private Field[] getFields0(boolean declared);

native public Class[] getInterfaces();

public Method getMethod(String name, Class parameterTypes[]) throws NoSuchMethodException, SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.PUBLIC );
	return (getMethod0(name, parameterTypes, false));
}

native private Method getMethod0(String name, Class[] args, boolean declared);

public Method[] getMethods() throws SecurityException
{
	System.getSecurityManager().checkMemberAccess( this, Member.PUBLIC);
	return (getMethods0(false));
}

native private Method[] getMethods0(boolean declared);

native public int getModifiers();

native public String getName();

native public static Class getPrimitiveClass(String name);

/**
 * Finds a resource with the specified name.  
 * The rules for searching for resources associated with a given class 
 * are implemented by the class loader of the class. 
 * <p>
 * The Class methods delegate to ClassLoader methods, after applying a 
 * naming convention: if the resource name starts with "/", it is used as 
 * is. Otherwise, the name of the package is prepended, after converting 
 * "." to "/". 
 *
 * @param 	name the string representing the resource to be found. 
 * @return 	the URL object having the specified name, or null if no 
 *		resource with the specified name is found. 
 */
public URL getResource(String name) {
	ClassLoader loader = getClassLoader();
	if (loader == null) {
		loader = SystemClassLoader.getClassLoader();
	}
	return (loader.getResource(fullResourceName(name)));
}

/**
 * Finds a resource with a given name.  
 * Will return null if no resource with this name is found. The rules
 * for searching a resources associated with a given class are implemented 
 * by the ClassLoader of the class.
 *
 * The Class methods delegate to ClassLoader methods, after applying a 
 * naming convention: if the resource name starts with "/", it is used 
 * as is. Otherwise, the name of the package is prepended, after 
 * converting "." to "/". 
 *
 * @param 	name the string representing the resource to be found 
 * @return 	the InputStream object having the specified name, or null 
 *		if no resource with the specified name is found. 
 */
public InputStream getResourceAsStream(String name) {
	ClassLoader loader = getClassLoader();
	if (loader == null)  {
		loader = SystemClassLoader.getClassLoader();
	}
	return (loader.getResourceAsStream(fullResourceName(name)));
}

native public Object[] getSigners();

native public Class getSuperclass();

native public boolean isArray();

native public boolean isAssignableFrom(Class cls);

native public boolean isInstance(Object obj);

native public boolean isInterface();

native public boolean isPrimitive();

native public Object newInstance() throws InstantiationException, IllegalAccessException;

/*
 * toString() 
 */
public String toString() {
	return (isInterface() ? "interface " : "class ") + getName();
}
}
