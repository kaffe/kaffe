/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.servlet.http;

import javax.servlet.GenericServlet;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import java.io.IOException;

public abstract class HttpServlet
  extends GenericServlet {

public HttpServlet() {
}

protected void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
	res.sendError(HttpServletResponse.SC_BAD_REQUEST, "Method GET not supported");
}

protected long getLastModified(HttpServletRequest req) {
	return (-1);
}

protected void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
	res.sendError(HttpServletResponse.SC_BAD_REQUEST, "Method POST not supported");
}

protected void doPut(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
	res.sendError(HttpServletResponse.SC_BAD_REQUEST, "Method PUT not supported");
}

protected void doDelete(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
	res.sendError(HttpServletResponse.SC_BAD_REQUEST, "Method DELETE not supported");
}

protected void doOptions(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
	res.sendError(HttpServletResponse.SC_BAD_REQUEST, "Method OPTIONS not supported");
}

protected void doTrace(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
	res.sendError(HttpServletResponse.SC_BAD_REQUEST, "Method TRACE not supported");
}

public void service(ServletRequest req, ServletResponse res) throws ServletException, IOException {
	try {
		service((HttpServletRequest)req, (HttpServletResponse)res);
	}
	catch (ClassCastException _) {
		throw new ServletException("not an http request");
	}
}

protected void service(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
	if (req.getMethod().equals("GET")) {
		doGet(req, res);
	}
	else if (req.getMethod().equals("HEAD")) {
		doGet(req, res);
	}
	else if (req.getMethod().equals("POST")) {
		doPost(req, res);
	}
	else if (req.getMethod().equals("DELETE")) {
		doDelete(req, res);
	}
	else if (req.getMethod().equals("OPTIONS")) {
		doOptions(req, res);
	}
	else if (req.getMethod().equals("PUT")) {
		doPut(req, res);
	}
	else if (req.getMethod().equals("TRACE")) {
		doTrace(req, res);
	}
	else {
		res.sendError(HttpServletResponse.SC_BAD_REQUEST, "Method " + req.getMethod() + " not supported");
	}
}

}
