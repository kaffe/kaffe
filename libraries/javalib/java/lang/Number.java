package java.lang;
import java.io.Serializable;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
abstract public class Number implements Serializable 
{
	/* This is what Sun's JDK1.1 "serialver java.lang.Number" spits out */
	private static final long serialVersionUID = -8742448824652078965L;

abstract public byte byteValue();

abstract public double doubleValue();

abstract public float floatValue();

abstract public int intValue();

abstract public long longValue();

abstract public short shortValue();
}
