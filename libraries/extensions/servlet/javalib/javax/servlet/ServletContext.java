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

    Servlet getServlet(String name) throws ServletException;
    Enumeration getServlets();
    Enumeration getServletNames();
    void log(String msg);
    void log(Exception exception, String msg);
    String getRealPath(String path);
    String getMimeType(String file);
    String getServerInfo();
    Object getAttribute(String name);

}
