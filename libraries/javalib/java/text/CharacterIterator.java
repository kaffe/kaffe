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

public static final char DONE = (char)0xFFFF;

public abstract char first();
public abstract char last();
public abstract char current();
public abstract char next();
public abstract char previous();
public abstract void setIndex(int pos);
public abstract int getBeginIndex();
public abstract int getEndIndex();
public abstract int getIndex();
public abstract Object clone();

};
