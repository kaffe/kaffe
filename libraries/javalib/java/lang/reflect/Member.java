/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang.reflect;

import java.lang.Class;
import java.lang.String;

public interface Member {

final public static int PUBLIC = 0;
final public static int DECLARED = 1;

abstract public Class getDeclaringClass();

abstract public int getModifiers();

abstract public String getName();
}
