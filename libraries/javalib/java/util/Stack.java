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
public class Stack
  extends Vector
{

public Stack() {
}

public boolean empty() {
	return isEmpty();
}

public synchronized Object peek() throws EmptyStackException {
	try {
		return elementAt(size() - 1);
	} catch (ArrayIndexOutOfBoundsException _) {
		throw new EmptyStackException();
	}
}

public synchronized Object pop() throws EmptyStackException {
	Object peeked = peek();

	removeElementAt(size() - 1);
	return peeked;
}

public Object push(Object item) {
	addElement(item);
	return item;
}

public synchronized int search(Object o) {
	int index = lastIndexOf(o);

	if (index == -1) {
		return -1;
	}
	return size() - index;
}
}

