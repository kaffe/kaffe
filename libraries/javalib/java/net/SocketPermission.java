/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *	Dalibor Topic.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Dalibor Topic <robilad@yahoo.com>
 */

package java.net;

import java.security.Permission;

public class SocketPermission extends Permission {

    private static final String LOCALHOST = "localhost";

    private String host;
    private String action;

    public SocketPermission(String host, String action) {
	super(host);

	if (host == null) {
	    host = LOCALHOST;
	}

	// FIXME : Add argument checking
	this.host = host;
	this.action = action;
    }

    public boolean equals(Object obj) {
	if (this == obj) {
	    return true;
	}
	else if (obj == null || getClass() != obj.getClass()) {
	    return false;
	}

	SocketPermission that = (SocketPermission) obj;

	return getActions().equals(that.getActions());
    }

    public String getActions() {
	return action;
    }

    public int hashCode() {
	return getActions().hashCode();
    }

    public boolean implies(Permission p) {
	// FIXME : Implement me!
	return false;
    }
}


