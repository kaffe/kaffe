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

public class Cookie {

private String name;
private String value;
private String comment;
private String domain;
private int age;
private String path;
private boolean secure;
private int version;

public Cookie(String name, String value) {
	this.name = name;
	this.value = value;
}

public void setComment(String purpose) {
	comment = purpose;
}

public String getComment() {
	return (comment);
}

public void setDomain(String pattern) {
	domain = pattern;
}

public String getDomain() {
	return (domain);
}

public void setMaxAge(int expiry) {
	age = expiry;
}

public int getMaxAge() {
	return (age);
}

public void setPath(String url) {
	path = url;
}

public String getPath() {
	return (path);
}

public void setSecure(boolean flag) {
	secure = flag;
}

public boolean getSecure() {
	return (secure);
}

public String getName() {
	return (name);
}

public void setValue(String newValue) {
	value = newValue;
}

public String getValue() {
	return (value);
}

public int getVersion() {
	return (version);
}

public void setVersion(int v) {
	version = v;
}

public Object clone() {
	try {
		return (super.clone());
	}
	catch (CloneNotSupportedException _) {
		return (null);
	}
}

}
