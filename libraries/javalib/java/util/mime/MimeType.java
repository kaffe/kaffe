/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util.mime;

public class MimeType {
public MimeType() {
}

public MimeType( String rawData) {
}

public MimeType( String primary, String sub) throws MimeTypeParseException {
}

public MimeType( String primary, String sub, MimeTypeParameterList mtpl)
		 throws MimeTypeParseException {
}

public String getPrimaryType() {
	return null;
}

public void setPrimaryType( String primary) throws MimeTypeParseException {
}

public String getSubType() {
	return null;
}

public void setSubType( String sub) throws MimeTypeParseException {
}

public MimeTypeParameterList getParameters() {
	return null;
}

public String getParameter( String name) {
	return null;
}

public void setParameter( String name, String value) {
}

public void removeParameter( String name) {
}

public String toString() {
	return null;
}

public String getBaseType() {
	return null;
}

public boolean match( MimeType type) {
	return false;
}

public static void main( String[] args) {
}
}
