/**
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */

package java.awt;

import java.awt.Font;
import java.awt.Event;

public interface MenuContainer {

Font getFont();

void remove( MenuComponent c);

/**
 * @deprecated
 */
public boolean postEvent(Event evt);

}
