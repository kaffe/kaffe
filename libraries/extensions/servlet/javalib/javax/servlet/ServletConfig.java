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

public interface ServletConfig {

public abstract ServletContext getServletContext();
public abstract String getInitParameter(String name);
public abstract Enumeration getInitParameterNames();

}
