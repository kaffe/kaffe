/**
 * AWTError - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */

package java.awt;


public class AWTError
  extends Error
{
private static final long serialVersionUID = -1819846354050686206L;

public AWTError ( String message ) {
	super( message);
}
}
