/*
 * tests that listener types are properly set and printed.
 * Submitted by Moses Jong <dejong@cs.umn.edu>
 */
import java.lang.reflect.*;
import java.beans.*;
import java.util.*;
 
public class BeanBug {
    public static void main(String[] argv) throws Exception {
 
        Class cls = java.awt.Button.class;
 
        BeanInfo beanInfo = Introspector.getBeanInfo(cls);
 
        EventSetDescriptor[] events = beanInfo.getEventSetDescriptors();
 
        for (int i=0; i < events.length ; i++) {
 
            Class lsnType = events[i].getListenerType();
 
            if (lsnType == null) {
                throw new NullPointerException("index " + i);
            }
	    System.out.println(events[i].getAddListenerMethod());
	    System.out.println(events[i].getRemoveListenerMethod());
	    System.out.println(lsnType);
        }
    }
}
