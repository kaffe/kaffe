/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */


package kaffe.awt;


public interface FlavorConverter
{
public byte[] exportObject ( Object data );

public Object importBytes ( byte[] data );
}
