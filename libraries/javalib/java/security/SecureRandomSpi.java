/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.security;

import java.io.Serializable;

public abstract class SecureRandomSpi extends Object implements Serializable {

protected abstract void engineSetSeed(byte[] seed);

protected abstract void engineNextBytes(byte[] bytes);

protected abstract byte[] engineGenerateSeed(int numBytes);

}
