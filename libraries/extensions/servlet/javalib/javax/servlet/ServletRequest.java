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
import java.io.BufferedReader;
import java.io.IOException;

public interface ServletRequest {

public abstract int getContentLength();
public abstract String getContentType();
public abstract String getProtocol();
public abstract String getScheme();
public abstract String getServerName();
public abstract int getServerPort();
public abstract String getRemoteAddr();
public abstract String getRemoteHost();
public abstract String getRealPath(String path);
public abstract ServletInputStream getInputStream() throws IOException;
public abstract String getParameter(String name);
public abstract String[] getParameterValues(String name);
public abstract Enumeration getParameterNames();
public abstract Object getAttribute(String name);
public abstract BufferedReader getReader() throws IOException;
public abstract String getCharacterEncoding();

}
