/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.activation;

import java.rmi.RemoteException;

public class ActivateFailedException
	extends RemoteException {

public ActivateFailedException(String s) {
	super(s);
}

public ActivateFailedException(String s, Exception ex) {
	super(s, ex);
}

}
