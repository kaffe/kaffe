/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *      Dalibor Topic.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Dalibor Topic <robilad@yahoo.com>
 */

package java.awt.event;

import java.awt.AWTEvent;
import java.util.EventListener;

/** @since 1.2 */
public interface AWTEventListener extends EventListener {
    void eventDispatched(AWTEvent event);
}
