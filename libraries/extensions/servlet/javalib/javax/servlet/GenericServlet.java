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

import java.io.Serializable;
import java.io.IOException;
import java.util.Enumeration;

public abstract class GenericServlet
  implements Servlet, ServletConfig, Serializable {

private ServletConfig config;

public GenericServlet() {
}

public ServletContext getServletContext() {
	return (config.getServletContext());
}

public String getInitParameter(String name) {
	return (config.getInitParameter(name));
}

public Enumeration getInitParameterNames() {
	return (config.getInitParameterNames());
}

public void log(String msg) {
	config.getServletContext().log(getClass().toString() + ": " + msg);
}

public String getServletInfo() {
	return (null);
}

public void init(ServletConfig config) throws ServletException {
	this.config = config;
	log("Initializing");
}

public ServletConfig getServletConfig() {
	return (config);
}

public void destroy() {
	log("Destroying");
}

public abstract void service(ServletRequest req, ServletResponse res) throws ServletException, IOException;

}
