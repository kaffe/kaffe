/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.rmi.server;

import java.rmi.server.ExportException;

public class SocketSecurityException
	extends ExportException {

public SocketSecurityException(String s) {
	super(s);
}

public SocketSecurityException(String s, Exception e) {
	super(s, e);
}

}
