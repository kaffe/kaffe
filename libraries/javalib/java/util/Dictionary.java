package java.util;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
abstract public class Dictionary
{
abstract public Enumeration elements();

abstract public Object get(Object key);

abstract public boolean isEmpty();

abstract public Enumeration keys();

abstract public Object put(Object key, Object value);

abstract public Object remove(Object key);

abstract public int size();
}
