/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.lang.CloneNotSupportedException;
import java.io.PrintStream;
import java.io.PrintWriter;

public class ServerCloneException
	extends CloneNotSupportedException {

public Exception detail;

public ServerCloneException(String s) {
	super(s);
	detail = null;
}

public ServerCloneException(String s, Exception e) {
	super(s);
	detail = e;
}

public String getMessage() {
	if (detail != null) {
		return (super.getMessage() + ":" + detail.getMessage());
	}
	else {
		return (super.getMessage());
	}
}

public void printStackTrace(PrintStream s) {
	if (detail != null) {
		detail.printStackTrace(s);
	}
	super.printStackTrace(s);
}

public void printStackTrace() {
	printStackTrace(System.err);
}

public void printStackTrace(PrintWriter s) {
	if (detail != null) {
		detail.printStackTrace(s);
	}
	super.printStackTrace(s);
}

}
