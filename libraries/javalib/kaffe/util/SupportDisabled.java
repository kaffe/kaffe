/*
 * Java core library component.
 *
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
package kaffe.util;


public class SupportDisabled
    extends Error
{
    public SupportDisabled () {
        super();
    }

    public SupportDisabled (String s) {
        super(s);
    }
}
