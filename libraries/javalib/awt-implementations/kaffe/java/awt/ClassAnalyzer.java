package java.awt;

import java.lang.reflect.Method;
import java.util.Hashtable;

class ClassAnalyzer
{
	private static Hashtable dict = new Hashtable();
	private static Class[] sig_AWTEvent = new Class[] { AWTEvent.class };
	private static Class[] sig_Event = new Class[] { Event.class };
	private static Class[] sig_Event_Object = new Class[] { Event.class, Object.class };
	private static Class[] sig_Event_int = new Class[] { Event.class, Integer.TYPE };
	private static Class[] sig_Event_int_int = new Class[] { Event.class, Integer.TYPE, Integer.TYPE };
	private static String[] oldEventMethodName = {
	"handleEvent",
	"action",
	"keyDown",
	"keyUp",
	"mouseDown",
	"mouseUp",
	"mouseMove",
	"gotFocus",
	"lostFocus",
	"mouseEnter",
	"mouseExit",
	"mouseDrag",
	"postEvent"
};
	private static Class[][] oldEventMethodSig = {
	sig_Event,
	sig_Event_Object,
	sig_Event_int,
	sig_Event_int,
	sig_Event_int_int,
	sig_Event_int_int,
	sig_Event_int_int,
	sig_Event_Object,
	sig_Event_Object,
	sig_Event_int_int,
	sig_Event_int_int,
	sig_Event_int_int,
	sig_Event
};

static ClassProperties analyzeAll ( Class clazz, boolean isNativeLike ) {
	ClassProperties props = (ClassProperties) dict.get( clazz);
	
	if ( props == null ) {
		props = new ClassProperties();
	
		props.hasProcessEvent = checkProcessEvent( clazz);
		props.useOldEvents    = isNativeLike ? checkOldEventMethods( clazz) : false;
		props.isNativeLike    = isNativeLike;

		dict.put( clazz, props);
//System.out.println( "" + clazz + ", oe: " + props.useOldEvents +
//                        ", pe: " + props.hasProcessEvent);
	}

	return props;
}

static ClassProperties analyzePostEvent ( Class clazz ) {
	ClassProperties props = (ClassProperties) dict.get( clazz);
	
	if ( props == null ) {
		props = new ClassProperties();
	
		props.useOldEvents    = checkOldEventMenuMethods( clazz);
		props.hasProcessEvent = false;
		props.isNativeLike    = false;

		dict.put( clazz, props);
//System.out.println( "" + clazz + ", oe: " + props.useOldEvents +
//                        ", pe: " + props.hasProcessEvent);
	}

	return props;
}

static ClassProperties analyzeProcessEvent ( Class clazz, boolean isNativeLike ) {
	ClassProperties props = (ClassProperties) dict.get( clazz);
	
	if ( props == null ) {
		props = new ClassProperties();
	
		props.useOldEvents    = false;
		props.hasProcessEvent = checkProcessEvent( clazz);
		props.isNativeLike    = isNativeLike;

		dict.put( clazz, props);
//System.out.println( "" + clazz + ", oe: " + props.useOldEvents +
//                        ", pe: " + props.hasProcessEvent);
	}

	return props;
}

static boolean checkOldEventMenuMethods ( Class clazz ) {
	Method m;

	try {
	  m = clazz.getMethod( "postEvent", sig_Event);
	  if ( m.getDeclaringClass() != MenuComponent.class )
		return true; // Another zombie
	}
	catch ( NoSuchMethodException _ ) {
System.err.println( "ouch!! " + clazz + " has no postEvent");
		return false; // no MenuComponent subclass!
	}
	
	return false;
}

static boolean checkOldEventMethods ( Class clazz ) {
	int i=0;
	Method m;

	// we just look for the first old-tyme method - if there is one
	// of them, we more than likely have a completely 1.0.2 app
	try {
		for ( i=0; i<oldEventMethodName.length; i++ ) {
			m = clazz.getMethod( oldEventMethodName[i], oldEventMethodSig[i]);
			if ( m.getDeclaringClass() != Component.class )
				return true; // OK, we got one
		}	
	}
	catch ( NoSuchMethodException _ ) {
System.err.println( "ouch!! " + clazz + " has no " + oldEventMethodName[i]);
		return false; // no Component subclass!
	}
	
	return false;
}

static boolean checkProcessEvent ( Class clazz ) {
	// "processEvent" is protected - we have to use an Exception for detection
	
	try {
		Method m = clazz.getDeclaredMethod( "processEvent", sig_AWTEvent);
		return true; // OK, we have a overloaded "processEvent"
	}
	catch ( NoSuchMethodException _ ) {
		return false; // no Component subclass!
	}
}
}
