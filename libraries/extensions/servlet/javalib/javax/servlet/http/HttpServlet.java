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
import java.io.Serializable;
import java.io.IOException;

public abstract class HttpServlet
  extends GenericServlet
  implements Serializable {

public HttpServlet() {
}

protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
	throw new kaffe.util.NotImplemented(HttpServlet.class.getName() + ".doGet()");
}

protected long getLastModified(HttpServletRequest req) {
	throw new kaffe.util.NotImplemented(HttpServlet.class.getName() + ".getLastModified()");
}

protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
	throw new kaffe.util.NotImplemented(HttpServlet.class.getName() + ".doPost()");
}

protected void doPut(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
	throw new kaffe.util.NotImplemented(HttpServlet.class.getName() + ".doPut()");
}

protected void doDelete(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
	throw new kaffe.util.NotImplemented(HttpServlet.class.getName() + ".doDelete()");
}

protected void doOptions(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
	throw new kaffe.util.NotImplemented(HttpServlet.class.getName() + ".doOptions()");
}

protected void doTrace(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
	throw new kaffe.util.NotImplemented(HttpServlet.class.getName() + ".doTrace()");
}

protected void service(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
	throw new kaffe.util.NotImplemented(HttpServlet.class.getName() + ".service(HttpServletRequest,HttpServletResponse)");
}

public void service(ServletRequest req, ServletResponse res) throws ServletException, IOException {
	throw new kaffe.util.NotImplemented(HttpServlet.class.getName() + ".service(ServletRequest,ServletResponse)");
}

}
