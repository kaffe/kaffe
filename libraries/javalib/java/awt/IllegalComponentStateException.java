package java.awt;

/**
 * IllegalComponentStateException - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */

import java.lang.String;

public class IllegalComponentStateException
  extends IllegalStateException
{
public IllegalComponentStateException () {
	super();
}

public IllegalComponentStateException ( String message ) {
	super( message);
}
}
