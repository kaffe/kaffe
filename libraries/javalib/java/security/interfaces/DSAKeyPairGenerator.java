/*
 * DSAKeyPairGenerator.java
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

package java.security.interfaces;

import java.security.SecureRandom;
import java.security.InvalidParameterException;

public interface DSAKeyPairGenerator
{
    void initialize(DSAParams params,
		    SecureRandom random)
	throws InvalidParameterException;
    
    void initialize(int modlen,
		    boolean genParams,
		    SecureRandom random)
	throws InvalidParameterException;
}
