/*
 * AlgorithmParametersSpi.java
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

package java.security;

import java.io.IOException;

import java.security.spec.AlgorithmParameterSpec;
import java.security.spec.InvalidParameterSpecException;

public abstract class AlgorithmParametersSpi
{
    public AlgorithmParametersSpi()
    {
    }

    protected abstract void engineInit(AlgorithmParameterSpec paramSpec)
	throws InvalidParameterSpecException;

    protected abstract void engineInit(byte[] params)
	throws IOException;

    protected abstract void engineInit(byte[] params,
				       String format)
	throws IOException;

    protected abstract AlgorithmParameterSpec engineGetParameterSpec(
	Class paramSpec)
	throws InvalidParameterSpecException;

    protected abstract byte[] engineGetEncoded()
	throws IOException;

    protected abstract byte[] engineGetEncoded(String format)
	throws IOException;

    protected abstract String engineToString();
}
