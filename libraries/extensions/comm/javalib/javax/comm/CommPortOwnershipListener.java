/*
 * Java comm library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.comm;

import java.util.EventListener;

public interface CommPortOwnershipListener
  extends EventListener {

    int PORT_OWNED = 1;
    int PORT_UNOWNED = 2;
    int PORT_OWNERSHIP_REQUESTED = 3;

    void ownershipChange(int type);

}
