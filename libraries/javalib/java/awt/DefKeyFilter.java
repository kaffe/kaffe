package java.awt;

import java.awt.event.KeyEvent;

import kaffe.awt.KeyHook;

class DefKeyFilter
  implements KeyHook
{
public boolean intercept ( KeyEvent e ) {
	if ( e.isControlDown() && (e.getKeyCode() == e.VK_F12) ){
		if ( e.id == e.KEY_RELEASED ) {
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
