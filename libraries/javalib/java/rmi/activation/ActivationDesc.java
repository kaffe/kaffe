/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.activation;

import java.io.Serializable;
import java.rmi.MarshalledObject;

public final class ActivationDesc
	implements Serializable {

private ActivationGroupID groupid;
private String classname;
private String location;
private MarshalledObject data;
private boolean restart;

public ActivationDesc(String className, String location, MarshalledObject data) throws ActivationException {
	this(ActivationGroup.currentGroupID(), className, location, data, false);
}

public ActivationDesc(String className, String location, MarshalledObject data, boolean restart) throws ActivationException {
	this(ActivationGroup.currentGroupID(), className, location, data, restart);
}

public ActivationDesc(ActivationGroupID groupID, String className, String location, MarshalledObject data) {
	this(groupID, className, location, data, false);
}

public ActivationDesc(ActivationGroupID groupID, String className, String location, MarshalledObject data, boolean restart) {
	this.groupid = groupID;
	this.classname = className;
	this.location = location;
	this.data = data;
	this.restart = restart;
}

public ActivationGroupID getGroupID() {
	return (groupid);
}

public String getClassName() {
	return (classname);
}

public String getLocation() {
	return (location);
}

public MarshalledObject getData() {
	return (data);
}

public boolean getRestartMode() {
	return (restart);
}

public boolean equals(Object obj) {
	if (!(obj instanceof ActivationDesc)) {
		return (false);
	}
	ActivationDesc that = (ActivationDesc)obj;

	if (this.groupid.equals(that.groupid) &&
	    this.classname.equals(that.classname) &&
	    this.location.equals(that.location) &&
	    this.data.equals(that.data) &&
	    this.restart == that.restart) {
		return (true);
	}
	return (false);
}

public int hashCode() {
	return (groupid.hashCode() ^ classname.hashCode() ^ location.hashCode() ^ data.hashCode());
}

}
