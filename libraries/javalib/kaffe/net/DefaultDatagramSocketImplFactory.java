/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.net;

import java.net.DatagramSocketImplFactory;
import java.net.PlainDatagramSocketImpl;
import java.net.DatagramSocketImpl;

public class DefaultDatagramSocketImplFactory
  implements DatagramSocketImplFactory {

public DatagramSocketImpl createDatagramSocketImpl() {
	return (new PlainDatagramSocketImpl());
}

}
