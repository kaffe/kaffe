/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Checked Spec: JDK 1.3
 */

package java.lang.reflect;


public interface Member {

int PUBLIC = 0;
int DECLARED = 1;

Class getDeclaringClass();

int getModifiers();

String getName();
}
