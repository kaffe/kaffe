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

import java.util.EventObject;

public class HttpSessionBindingEvent
  extends EventObject {

private String name;

public HttpSessionBindingEvent(HttpSession session, String name) {
	super(session);
	this.name = name;
}

public String getName() {
	return (name);
}

public HttpSession getSession() {
	return ((HttpSession)getSource());
}

}
