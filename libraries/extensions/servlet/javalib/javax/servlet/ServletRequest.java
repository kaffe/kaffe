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

    int getContentLength();
    String getContentType();
    String getProtocol();
    String getScheme();
    String getServerName();
    int getServerPort();
    String getRemoteAddr();
    String getRemoteHost();
    String getRealPath(String path);
    ServletInputStream getInputStream() throws IOException;
    String getParameter(String name);
    String[] getParameterValues(String name);
    Enumeration getParameterNames();
    Object getAttribute(String name);
    BufferedReader getReader() throws IOException;
    String getCharacterEncoding();

}
