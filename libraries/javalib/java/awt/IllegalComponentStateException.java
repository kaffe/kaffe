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

public class IllegalComponentStateException
  extends IllegalStateException
{
private static final long serialVersionUID = -1889339587208144238L;

public IllegalComponentStateException () {
	super();
}

public IllegalComponentStateException ( String message ) {
	super( message);
}
}
