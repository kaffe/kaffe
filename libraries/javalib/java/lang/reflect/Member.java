package java.lang.reflect;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public interface Member
{
	final public static int PUBLIC = 1;
	final public static int DECLARED = 2;

abstract public Class getDeclaringClass();

abstract public int getModifiers();

abstract public String getName();
}
