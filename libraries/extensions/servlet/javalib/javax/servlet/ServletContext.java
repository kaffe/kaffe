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

import java.util.Enumeration;

public interface ServletContext {

public abstract Servlet getServlet(String name) throws ServletException;
public abstract Enumeration getServlets();
public abstract Enumeration getServletNames();
public abstract void log(String msg);
public abstract void log(Exception exception, String msg);
public abstract String getRealPath(String path);
public abstract String getMimeType(String file);
public abstract String getServerInfo();
public abstract Object getAttribute(String name);

}
