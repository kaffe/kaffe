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
	int topOfStack = 0;

/* Next empty space */
public Stack() {
}

public boolean empty() {
	return isEmpty();
}

public Object peek() {
	if (empty()) throw new EmptyStackException();

	return elementAt(topOfStack-1);
}

public Object pop() {
	if (empty()) throw new EmptyStackException();
	else {
		Object peeked=peek();

		topOfStack--;
		removeElementAt(topOfStack);

		return peeked;
	}
}

public Object push(Object item) {
	addElement(item);
	topOfStack++;

	return item;
}

public int search(Object o) {
	return lastIndexOf(o)+1;
}
}
