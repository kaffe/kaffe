package java.awt.datatransfer;

import java.io.IOException;
import java.io.Externalizable;
import java.io.ObjectInput;
import java.io.ObjectOutput;

/**
 * DataFlavor - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */
public class DataFlavor implements Externalizable
{
	String type;
	String name;
	Class repClass;
	public static DataFlavor plainTextFlavor = null;
	public static DataFlavor stringFlavor = null;
	static Class defaultRepClass;
	static String defaultMimeType;
	private static final long serialVersionUID = 8367026044764648243L;

/**
 * Serialize this DataFlavor
 */
public void writeExternal(ObjectOutput os) throws IOException {
	/* XXX */
}

/**
 * Sun says: restore this DataFlavor from an Serialized state
 */
public void readExternal(ObjectInput is) 
	throws IOException, ClassNotFoundException {
	/* XXX */
}

static {
	try {
		defaultRepClass = Class.forName( "java.io.InputStream");
		defaultMimeType = "application/x-java-serialized-object";
	
		plainTextFlavor = new DataFlavor( "text/plain; charset=unicode", "plain text");
		stringFlavor = new DataFlavor( Class.forName( "java.lang.String"),"unicode string");
	}
	catch ( Exception x ) {
		x.printStackTrace();
	}
}

public DataFlavor ( Class representationClass, String humanPresentableName ) {
	type = defaultMimeType;
	
	repClass = representationClass;
	name = humanPresentableName;
}

public DataFlavor ( String mimeType, String humanPresentableName ) {
	repClass = defaultRepClass;

	type     = mimeType;
	name     = humanPresentableName;
}

public boolean equals ( DataFlavor flavor ) {
	// repClasses have to be identical, not subtypes
	return isMimeTypeEqual( flavor) && (repClass == flavor.repClass);
}

public String getHumanPresentableName() {
	return name;
}

public String getMimeType() {
	return type;
}

public Class getRepresentationClass() {
	return repClass;
}

public boolean isMimeTypeEqual ( DataFlavor flavor ) {
	return isMimeTypeEqual( flavor.type); // might be re-implemented by subs
}

public boolean isMimeTypeEqual( String mimeType) {
	// this has to be case sensitive
	return type.equals( mimeType); // assuming that we already are normalized
}

protected String normalizeMimeType ( String mimeType ) {
	// this is just a hook for derived classes
	return mimeType;
}

protected String normalizeMimeTypeParameter ( String paramName, String paramValue ) {
	// just a hook for derived classes to normalize own/specialized MIME
	// parameter types
	return paramName + '=' + paramValue;
}

public void setHumanPresentableName( String humanPresentableName) {
	name = humanPresentableName;
}

public String toString() {
	return "DataFlavor [" + name + ',' + type + ',' + repClass.getName() + ']';
}
}
