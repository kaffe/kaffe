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
private static final long serialVersionUID = 1224463164541339165L;

public Stack() {
}

public boolean empty() {
	return isEmpty();
}

public synchronized Object peek() {
	try {
		return elementAt(size() - 1);
	} catch (ArrayIndexOutOfBoundsException _) {
		throw new EmptyStackException();
	}
}

public synchronized Object pop() {
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

