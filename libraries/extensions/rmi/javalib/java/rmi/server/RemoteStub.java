/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

public abstract class RemoteStub
	extends RemoteObject {

public static final long serialVersionUID = -1585587260594494182l;

protected RemoteStub() {
	super();
}

protected RemoteStub(RemoteRef ref) {
	super(ref);
}

protected static void setRef(RemoteStub stub, RemoteRef ref) {
	stub.ref = ref;
}

}
