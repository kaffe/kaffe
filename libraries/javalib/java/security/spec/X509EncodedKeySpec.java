/*
 * X509EncodedKeySpec.java
 *
 * Copyright (c) 2001 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

package java.security.spec;

public class X509EncodedKeySpec
    extends EncodedKeySpec
{
    public X509EncodedKeySpec(byte encodedKey[])
    {
	super(encodedKey);
    }

    public final String getFormat()
    {
	return "X.509";
    }
}
