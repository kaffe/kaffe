package java.awt.event;

import java.util.EventListener;

/**
 * interface FocusListener - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

public interface FocusListener
 extends EventListener
{
void focusGained ( FocusEvent evt );

void focusLost ( FocusEvent evt );
}
