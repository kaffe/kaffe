/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.servlet;

public class UnavailableException
  extends ServletException {

private int delay;
private Servlet serv;

public UnavailableException(Servlet servlet, String msg) {
	this(-1, servlet, msg);
}

public UnavailableException(int seconds, Servlet servlet, String msg) {
	super(msg);
	delay = seconds;
	serv = servlet;
}

public boolean isPermanent() {
	if (delay < 0) {
		return (true);
	}
	else {
		return (false);
	}
}

public Servlet getServlet() {
	return (serv);
}

public int getUnavailableSeconds() {
	return (delay);
}

}
