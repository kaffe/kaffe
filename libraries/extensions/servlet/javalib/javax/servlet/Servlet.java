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

import java.io.IOException;

public interface Servlet {

public abstract void init(ServletConfig config) throws ServletException;
public abstract ServletConfig getServletConfig();
public abstract void service(ServletRequest req, ServletResponse res) throws ServletException, IOException;
public abstract String getServletInfo();
public abstract void destroy();

}
