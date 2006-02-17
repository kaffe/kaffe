/**
 * class TextComponent - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 * Copyright (c) 2006
 *      Kaffe.org developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.awt;

import java.awt.event.KeyEvent;

import org.kaffe.awt.KeyHook;

class DefKeyFilter
  implements KeyHook
{
public boolean intercept ( KeyEvent e ) {
	if ( e.isControlDown() && (e.getKeyCode() == KeyEvent.VK_F12) ){
		if ( e.id == KeyEvent.KEY_RELEASED ) {
			Component c = (Component)e.getSource();
			System.out.println( "keyTgt: " + c);
			System.out.println( "active: " + AWTEvent.activeWindow);
			c.getToplevel().dump("");
		}
		
		return false;
	}
	
	return true;
}
}
