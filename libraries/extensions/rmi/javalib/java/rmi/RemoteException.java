/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi;

import java.lang.Throwable;
import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;


public class RemoteException 
	extends IOException {

public static final long serialVersionUID = -5148567311918794206l;

public Throwable detail;

public RemoteException() {
	super();
	detail = null;
}

public RemoteException(String s) {
	super(s);
	detail = null;
}

public RemoteException(String s, Throwable e) {
	super(s);
	detail = e;
}

public String getMessage() {
	if (detail == null) {
		return (super.getMessage());
	}
	else {
		return (super.getMessage() + "; nested exception is: " + detail.getMessage());
	}
}

public void printStackTrace(PrintStream s) {
	if (detail != null) {
		detail.printStackTrace(s);
	}
	super.printStackTrace(s);
}

public void printStackTrace(PrintWriter s) {
	if (detail != null) {
		detail.printStackTrace(s);
	}
	super.printStackTrace(s);
}

public void printStackTrace() {
	printStackTrace(System.err);
}

}
