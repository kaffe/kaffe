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

import java.net.SocketImplFactory;
import java.net.PlainSocketImpl;
import java.net.SocketImpl;

public class DefaultSocketImplFactory
  implements SocketImplFactory {

public SocketImpl createSocketImpl() {
	return (new PlainSocketImpl());
}

}
