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

public abstract Cookie[] getCookies();
public abstract String getMethod();
public abstract String getRequestURI();
public abstract String getServletPath();
public abstract String getPathInfo();
public abstract String getPathTranslated();
public abstract String getQueryString();
public abstract String getRemoteUser();
public abstract String getAuthType();
public abstract String getHeader(String name);
public abstract int getIntHeader(String name);
public abstract long getDateHeader(String name);
public abstract Enumeration getHeaderNames();
public abstract HttpSession getSession(boolean create);
public abstract String getRequestedSessionId();
public abstract boolean isRequestedSessionIdValid();
public abstract boolean isRequestedSessionIdFromCookie();
public abstract boolean isRequestedSessionIdFromUrl();

}
