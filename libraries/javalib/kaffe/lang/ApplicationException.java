/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.lang;

import java.lang.String;

public class ApplicationException extends Exception {

public ApplicationException () {
        super();
}

public ApplicationException (String s) {
        super(s);
}

}
