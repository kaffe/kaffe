
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */
 
package java.security;

import java.io.Serializable;

public interface Key extends Serializable {

	long serialVersionUID = 6603384152749567654L;

	String getAlgorithm();
	String getFormat();
	byte[] getEncoded();

}

