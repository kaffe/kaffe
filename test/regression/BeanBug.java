/*
 * tests that listener types are properly set and printed.
 * Submitted by Moses DeJong <dejong@cs.umn.edu>
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
	System.out.flush();
    }
}

// Sort output
/* Expected Output:
interface java.awt.event.ActionListener
interface java.awt.event.ComponentListener
interface java.awt.event.FocusListener
interface java.awt.event.KeyListener
interface java.awt.event.MouseListener
interface java.awt.event.MouseMotionListener
public void java.awt.Button.addActionListener(java.awt.event.ActionListener)
public void java.awt.Button.removeActionListener(java.awt.event.ActionListener)
public void java.awt.Component.addComponentListener(java.awt.event.ComponentListener)
public void java.awt.Component.addFocusListener(java.awt.event.FocusListener)
public void java.awt.Component.addKeyListener(java.awt.event.KeyListener)
public void java.awt.Component.addMouseListener(java.awt.event.MouseListener)
public void java.awt.Component.addMouseMotionListener(java.awt.event.MouseMotionListener)
public void java.awt.Component.removeComponentListener(java.awt.event.ComponentListener)
public void java.awt.Component.removeFocusListener(java.awt.event.FocusListener)
public void java.awt.Component.removeKeyListener(java.awt.event.KeyListener)
public void java.awt.Component.removeMouseListener(java.awt.event.MouseListener)
public void java.awt.Component.removeMouseMotionListener(java.awt.event.MouseMotionListener)
*/
