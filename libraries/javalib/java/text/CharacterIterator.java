/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.text;

public interface CharacterIterator extends Cloneable {

char DONE = (char)0xFFFF;

char first();
char last();
char current();
char next();
char previous();
char setIndex(int pos);
int getBeginIndex();
int getEndIndex();
int getIndex();
Object clone();

}
