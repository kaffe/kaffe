/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

public class Operation {

private String operation;

public Operation(String op) {
	operation = op;
}

public String getOperation() {
	return (operation);
}

public String toString() {
	return (operation);
}

}
