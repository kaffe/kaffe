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
import java.io.IOException;
import java.io.Serializable;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.net.URL;
import java.security.ProtectionDomain;
import java.util.Vector;
import kaffe.lang.SystemClassLoader;

public final class Class implements Serializable {

/* For GCJ compatibility, we cannot define any fields in
 * java.lang.Class at this point.  Valid as of 10/28/99
 * We special case it instead in clib/native/ObjectStreamClassImpl.c
 * --gback
 */
// private static final long serialVersionUID = 3206093459760846163L;

// Only instantiable by the VM
private Class() {
}

public static Class forName(String className) throws ClassNotFoundException {
	return forName(className, true, null);
}

public static Class forName(String className, boolean initialize, ClassLoader loader) throws ClassNotFoundException {
        /*
         * NB: internally, we store class names as path names (with slashes
         *     instead of dots.  However, we must also prevent calls to
         *     "java/lang/Object" or "[[Ljava/lang/Object;" from succeeding.
         *      Since class names cannot have slashes, we reject all attempts
         *      to look up names that do.  Awkward.  Inefficient.
         */
	if (className.indexOf('/') != -1) {
		throw new ClassNotFoundException("Cannot have slashes - use dots instead.");
	}

	/* find the appropriate class loader */
	if (loader == null) {

		loader = CallStack.getCallersClassLoader();
		/* if loader is null, then the calling class has been loaded by the
		 * bootstrap class loader. We can use the system class loader to
		 * load the requested class.
		 */
		if (loader == null) {
			loader = SystemClassLoader.getClassLoader();
		}
	}

	/* The only checked exception that Class.forName() throws
	 * is ClassNotFoundException.  This is an exception, not an
	 * Error, which users often catch.
	 *
	 * However, Class.forName() can also throw errors, such as
	 * NoClassDefFoundError, if for instance a superclass for
	 * a class couldn't be found.
	 *
	 * When it throws which, we don't really know.  We try to be
	 * compatible, so we upgrade the error to an exception if it's
	 * (NoClassDefFoundError, this_class_name), or if it's a
	 * VerifyError.
	 * NB: 1.2 seems to be more consistent and throws
	 * ClassNotFoundException in most cases.
	 */

	Class cls = loader.findLoadedClass(className);

	if (cls == null) {
		try {
			cls = loader.loadClass(className, initialize);
		}
		catch (VerifyError error) {
			throw new ClassNotFoundException(error.getMessage());
		}
	}

	return cls;
}

private String fullResourceName(String name) {
	if (name.charAt(0) == '/') {
		return name;
	}
	String cname = getName();
	StringBuffer buf = new StringBuffer();
	int tail = cname.lastIndexOf('.');
	if (tail != -1) {
		buf.append(cname.substring(0, tail+1).replace('.', '/'));
	}
	buf.append(name);
	return (buf.toString());
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

/**
 * If this is an inner class, return the class that
 * declared it.  If not, return null.
 *
 * @return the declaring class of this class.
 * @since JDK1.1
 */
public Class getDeclaringClass() {
	Class[] classes = getClasses0(false);

	switch (classes.length) {
	case 0:
		return null;
	case 1:
		return classes[0];
	default:
		throw new ClassFormatError ("Too many outer classes :" + classes.length);
	}
}

/**
 * Get all the public inner classes, declared in this
 * class or inherited from superclasses, that are
 * members of this class.
 *
 * @return all public inner classes in this class.
 * @since JDK1.1
 */
public Class[] getClasses() {
	SecurityManager sm = System.getSecurityManager();
	Vector v = new Vector();
	Class clazz = this;
	while (clazz != null) {
		if (sm != null)
			sm.checkMemberAccess(clazz, Member.PUBLIC);
		Class[] classes = clazz.getClasses0(true);

		for (int i = 0; i < classes.length; i++) {
			if (Modifier.isPublic(classes[i].getModifiers())) {
				v.add (classes[i]);
			}
		}
		clazz = clazz.getSuperclass();
	}

	return (Class[])v.toArray (new Class[v.size()]);
}

native private Class[] getClasses0(boolean inner);

native public Class getComponentType();

public Constructor getConstructor(Class parameterTypes[]) throws NoSuchMethodException, SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.PUBLIC);
	return (getConstructor0(parameterTypes, false));
}

native private Constructor getConstructor0(Class[] args, boolean declared);

public Constructor[] getConstructors() throws SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.PUBLIC);
	return (getConstructors0(false));
}

native private Constructor[] getConstructors0(boolean declared);

/**
 * Get all the inner classes declared in this class.
 *
 * @return all inner classes declared in this class.
 * @exception SecurityException if you do not have access
 *            to non-public inner classes of this class.
 * @since JDK1.1
 */
public Class[] getDeclaredClasses() throws SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.DECLARED);
	return (getClasses0(true));
}

public Constructor getDeclaredConstructor(Class parameterTypes[]) throws NoSuchMethodException, SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.DECLARED);
	return (getConstructor0(parameterTypes, true));
}

public Constructor[] getDeclaredConstructors() throws SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.DECLARED);
	return (getConstructors0(true));
}

public Field getDeclaredField(String name) throws NoSuchFieldException, SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.DECLARED);
	return (getField0(name, true));
}

public Field[] getDeclaredFields() throws SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.DECLARED);
	return (getFields0(true));
}

public Method getDeclaredMethod(String name, Class parameterTypes[]) throws NoSuchMethodException, SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.DECLARED);
	return (getMethod0(name, parameterTypes, true));
}

public Method[] getDeclaredMethods() throws SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.DECLARED);
	return (getMethods0(true));
}

public Field getField(String name) throws NoSuchFieldException, SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.PUBLIC);
	return (getField0(name, false));
}

native private Field getField0(String name, boolean declared);

public Field[] getFields() throws SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.PUBLIC);
	return (getFields0(false));
}

native private Field[] getFields0(boolean declared);

native public Class[] getInterfaces();

public Method getMethod(String name, Class parameterTypes[]) throws NoSuchMethodException, SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.PUBLIC );
	if( name.equals("<init>") || name.equals("<clinit>") )
		throw new NoSuchMethodException();
	return (getMethod0(name, parameterTypes, false));
}

native private Method getMethod0(String name, Class[] args, boolean declared);

public Method[] getMethods() throws SecurityException
{
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkMemberAccess(this, Member.PUBLIC);
	return (getMethods0(false));
}

native private Method[] getMethods0(boolean declared);

native public int getModifiers();

native public String getName();

native static Class getPrimitiveClass(String name);

public ProtectionDomain getProtectionDomain() {
	return getClassLoader().getProtectionDomain(this);
}

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
 * resource with the specified name is found.  */
public URL getResource(String name) {
	ClassLoader loader = getClassLoader();
	name = fullResourceName(name);

	if (loader == null) {
		return ClassLoader.getSystemResource(name);
	}
	return (loader.getResource(name));
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
  	try {
  		return (getResource(name).openStream());
  	}
  	catch (IOException e) {
  		return null;
  	}
}

native public Object[] getSigners();
native void setSigners(Object[] signers);

native public Class getSuperclass();

native public boolean isArray();

native public boolean isAssignableFrom(Class cls);

native public boolean isInstance(Object obj);

native public boolean isInterface();

native public boolean isPrimitive();

public Object newInstance() throws InstantiationException, IllegalAccessException {
    if (Modifier.isAbstract(getModifiers()) || isInterface() || isPrimitive()) {
	throw new InstantiationException(getName());
    }
    else {
	try {
	    return getDeclaredConstructor(null).newInstance(null);
	}
	catch (InvocationTargetException e) {
	    // we rethrow runtime exceptions thrown by constructors, in order to
	    // pass on exceptions like NullPointerException to the caller.
	    if (e.getTargetException() instanceof RuntimeException) {
		throw (RuntimeException) e.getTargetException();
	    }
	    else {
		throw new InstantiationException(e.getTargetException().getMessage());
	    }
	}
	catch (NoSuchMethodException e) {
	    throw new InstantiationException(e.getMessage());
	}
    }
}

/*
 * toString()
 */
public String toString() {
	return (isInterface() ? "interface " : isPrimitive() ? "" : "class ")
	    + getName();
}

/* CallStack is used to encapsulate call stack based
 * class loader inspection.
 *
 * It is used by Class.forName and other methods in 
 * java.lang that need to get hold of the ClassLoader
 * associated with their calling class.
 */
static class CallStack {
	private Class [] classStack;

	CallStack() {
		classStack = SecurityManager.getClassContext0();
	}

	/* This method walks the call stack to find the
	 * class loader of the calling class. It takes care
	 * of recursive calls within library implementation,
	 * and Method.invoke.
	 */
	static ClassLoader getCallersClassLoader() {
		CallStack callStack = new CallStack();
		/* This method is caling CallStack(),
		 * and we want the method calling this method.
		 * So the start frame is 2.
		 */
		int frame = 2;
		Class callingClass = callStack.getStackClass(frame);

		/* We could have been called recursively
		 * within a class implementation. Then we
		 * need to walk up the stack to find the
		 * real calling class.
		 */
		if (null != callingClass
		    && callStack.getStackClass(frame - 1) == callingClass) {
			while (callStack.getStackClass(++frame) == callingClass) {
			}

			callingClass = callStack.getStackClass(frame);
		}

		/*
		 * We must use the ClassLoader associated with the calling method/class.
		 * We must handle the special case of code like this:
		 *
		 *    Class c = Class.class;
		 *    Method m = c.getMethod("forName", new Class[] { String.class });
		 *    c = (Class)m.invoke(c, new Object[] { "Class2" });
		 *
		 * If we didn't, then we would detect java.lang.reflect.Method as the
		 * calling class, and end up always using the bootstrap ClassLoader.
		 * To deal with this, we skip over java.lang.reflect.Method.
		 *
		 * Since Method.invoke implementation has a java and a native part,
		 * we need to skip both.
		 */

		if (callingClass != null
		    && callingClass.getName().equals("java.lang.reflect.Method")) {
			frame += 2;
			callingClass = callStack.getStackClass(frame);
		}

		if (callingClass != null) {
			return callingClass.getClassLoader();
		}

		return null;
	}

/*
 * Determine the Class associated with the method N frames up the stack:
 *
 * Frame #      Method
 * -------      ------
 *   -2		SecurityManager.getClassContext0()
 *   -1         Class.CallStack()
 *    0		The method calling Class.CallStack()
 *    1		The method calling the method calling Class.CallStack()
 *    2		...etc...
 *
 * Returns null if not found.
 */
	private Class getStackClass(int frame) {
		frame += 2;
		if (frame >= 0 && frame < classStack.length) {
			return classStack[frame];
		}
		return null;
	}
}
}
