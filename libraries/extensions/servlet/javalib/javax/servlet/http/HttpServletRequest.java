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

import javax.servlet.ServletRequest;
import java.util.Enumeration;

public interface HttpServletRequest
  extends ServletRequest {

    Cookie[] getCookies();
    String getMethod();
    String getRequestURI();
    String getServletPath();
    String getPathInfo();
    String getPathTranslated();
    String getQueryString();
    String getRemoteUser();
    String getAuthType();
    String getHeader(String name);
    int getIntHeader(String name);
    long getDateHeader(String name);
    Enumeration getHeaderNames();
    HttpSession getSession(boolean create);
    String getRequestedSessionId();
    boolean isRequestedSessionIdValid();
    boolean isRequestedSessionIdFromCookie();
    boolean isRequestedSessionIdFromUrl();

}
