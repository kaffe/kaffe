package kaffe.awt;

import java.awt.AWTEvent;

/**
 * HookInstaller - installs hooks into normal event processing chain
 * 
 * Note that method names have to be different because a hook object
 * might implement more than one hook interface.
 * 
 * There is no automatic chaining of hooks. If a hook has to be chained,
 * the install sequence should read:
 *
 *   MyHook myHook = new XYZHook(..)
 *   XYZHook oldHook = HookInstaller.installXYZHook( myHook);
 *   myHook.setPrevHook( oldHook);
 */
public class HookInstaller
  extends AWTEvent
{
private HookInstaller () {
	// never to be instanciated
	super( null, 0);
}

public static FocusHook installFocusHook ( FocusHook newHook ) {
	FocusHook oldHook = focusHook;
	focusHook = newHook;
	return oldHook;
}

public static KeyHook installKey ( KeyHook newHook ) {
	KeyHook oldHook = keyHook;
	keyHook = newHook;
	return oldHook;
}

public static MouseHook installMouseHook ( MouseHook newHook ) {
	MouseHook oldHook = mouseHook;
	mouseHook = newHook;
	return oldHook;
}
}
