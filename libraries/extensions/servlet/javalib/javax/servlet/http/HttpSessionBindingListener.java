/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.servlet.http;

import java.util.EventListener;

public interface HttpSessionBindingListener
  extends EventListener {

public abstract void valueBound(HttpSessionBindingEvent event);
public abstract void valueUnbound(HttpSessionBindingEvent event);

}
