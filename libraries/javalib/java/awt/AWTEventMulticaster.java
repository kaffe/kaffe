package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.ContainerEvent;
import java.awt.event.ContainerListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.TextEvent;
import java.awt.event.TextListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.util.EventListener;

/**
 * class AWTEventMulticaster - transparent container factory for EventListeners
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class AWTEventMulticaster
  implements ComponentListener, ContainerListener, FocusListener, KeyListener, MouseListener, MouseMotionListener, WindowListener, ActionListener, ItemListener, AdjustmentListener, TextListener
{
	protected EventListener a;
	protected EventListener b;

protected AWTEventMulticaster ( EventListener head, EventListener tail ) {
	a = head;
	b = tail;
}

public void actionPerformed ( ActionEvent evt ) {
	((ActionListener)a).actionPerformed( evt);
	((ActionListener)b).actionPerformed( evt);
}

public static ActionListener add ( ActionListener listeners, 
                     ActionListener newListener) {
	return (ActionListener)addInternal( listeners, newListener);
}

public static AdjustmentListener add ( AdjustmentListener listeners,
                         AdjustmentListener newListener) {
	return (AdjustmentListener)addInternal( listeners, newListener);
}

public static ComponentListener add ( ComponentListener listeners,
                        ComponentListener newListener ) {
	return (ComponentListener)addInternal( listeners, newListener);
}

public static ContainerListener add ( ContainerListener listeners,
                        ContainerListener newListener ) {
	return (ContainerListener)addInternal( listeners, newListener);
}

public static FocusListener add ( FocusListener listeners,
                    FocusListener newListener) {
	return (FocusListener)addInternal( listeners, newListener);
}

public static ItemListener add ( ItemListener listeners, 
                   ItemListener newListener) {
	return (ItemListener)addInternal( listeners, newListener);
}

public static KeyListener add ( KeyListener listeners, KeyListener newListener ) {
	return (KeyListener) addInternal( listeners, newListener);
}

public static MouseListener add ( MouseListener listeners,
                    MouseListener newListener) {
	return (MouseListener)addInternal( listeners, newListener);
}

public static MouseMotionListener add ( MouseMotionListener listeners,
                          MouseMotionListener newListener) {
	return (MouseMotionListener)addInternal( listeners, newListener);
}

public static TextListener add ( TextListener listeners,
                   TextListener newListener) {
	return (TextListener)addInternal( listeners, newListener);
}

public static WindowListener add ( WindowListener listeners,
                     WindowListener newListener) {
	return (WindowListener)addInternal( listeners, newListener);
}

protected static EventListener addInternal ( EventListener listeners,
                            EventListener newListener ) {
	EventListener       l;
	AWTEventMulticaster mc;

	if ( listeners == null )            // first time
		return newListener;
	if ( newListener == listeners )     // don't add same listener twice consecutively
		return listeners;
	if ( newListener == null )            // strange, but check it (wrong order of args?)
		return listeners;

	// check if newListener is already contained in the list
	if ( listeners instanceof AWTEventMulticaster ) {
		mc = (AWTEventMulticaster) listeners;
		if ( mc.a == newListener )
			return listeners;
		l = mc.b;

		while ( true ) {
			if ( l == newListener )
				return listeners;
			if ( l instanceof AWTEventMulticaster ) {
				mc = (AWTEventMulticaster) l;
				if ( mc.a == newListener )
					return listeners;
				l = mc.b;
			}
			else
				break;
		}
	}

	return new AWTEventMulticaster( newListener, listeners);
}

public void adjustmentValueChanged ( AdjustmentEvent evt ) {
	((AdjustmentListener)a).adjustmentValueChanged( evt);
	((AdjustmentListener)b).adjustmentValueChanged( evt);
}

public void componentAdded ( ContainerEvent evt ) {
	((ContainerListener)a).componentAdded( evt);
	((ContainerListener)b).componentAdded( evt);
}

public void componentHidden ( ComponentEvent evt ) {
	((ComponentListener)a).componentHidden( evt);
	((ComponentListener)b).componentHidden( evt);
}

public void componentMoved ( ComponentEvent evt ) {
	((ComponentListener)a).componentMoved( evt);
	((ComponentListener)b).componentMoved( evt);
}

public void componentRemoved ( ContainerEvent evt ) {
	((ContainerListener)a).componentRemoved( evt);
	((ContainerListener)b).componentRemoved( evt);
}

public void componentResized ( ComponentEvent evt ) {
	((ComponentListener)a).componentResized( evt);
	((ComponentListener)b).componentResized( evt);
}

public void componentShown ( ComponentEvent evt ) {
	((ComponentListener)a).componentShown( evt);
	((ComponentListener)b).componentShown( evt);
}

void dump () {
	System.out.print( "[");
	System.out.print( a);
	System.out.print( ",");
	if ( b instanceof AWTEventMulticaster )
		((AWTEventMulticaster)b).dump();
	else
		System.out.print( b);
	System.out.print( "]");
}

public void focusGained ( FocusEvent evt ) {
	((FocusListener)a).focusGained( evt);
	((FocusListener)b).focusGained( evt);
}

public void focusLost ( FocusEvent evt ) {
	((FocusListener)a).focusLost( evt);
	((FocusListener)b).focusLost( evt);
}

public void itemStateChanged ( ItemEvent evt ) {
	((ItemListener)a).itemStateChanged( evt);
	((ItemListener)b).itemStateChanged( evt);
}

public void keyPressed ( KeyEvent evt ) {
	((KeyListener)a).keyPressed( evt);
	((KeyListener)b).keyPressed( evt);
}

public void keyReleased ( KeyEvent evt ) {
	((KeyListener)a).keyReleased( evt);
	((KeyListener)b).keyReleased( evt);
}

public void keyTyped ( KeyEvent evt ) {
	((KeyListener)a).keyTyped( evt);
	((KeyListener)b).keyTyped( evt);
}

public static void main ( String[] args ) {
/********
	class KeyLi implements KeyListener {
		int id;
		KeyLi ( int i ) { id = i; }
		public void keyPressed ( KeyEvent evt ) {}
		public void keyReleased ( KeyEvent evt ) {}
		public void keyTyped ( KeyEvent evt ) { System.out.println( "keyTyped: " + id); }
		public String toString () { return "KeyLi:" + id; }
	}
	
	KeyLi       k1, k2, k3;
	KeyListener kl = null;
	
	kl = add( kl, (k1 = new KeyLi( 1)));
	System.out.println( "+1:  " + kl.getClass().getName());
	
	kl = add( kl, (k2 = new KeyLi( 2)));
	System.out.print( "+2:  " + kl.getClass().getName());
	if ( kl instanceof AWTEventMulticaster ) ((AWTEventMulticaster)kl).dump();
	System.out.println();
	
	kl = add( kl, k2);
	System.out.print( "+2: " + kl.getClass().getName());
	if ( kl instanceof AWTEventMulticaster ) ((AWTEventMulticaster)kl).dump();
	System.out.println();
	
	kl = add( kl, (k3 = new KeyLi(3)));
	System.out.print( "+3: " + kl.getClass().getName());
	if ( kl instanceof AWTEventMulticaster ) ((AWTEventMulticaster)kl).dump();
	System.out.println();
	
	kl.keyTyped( new KeyEvent( new Frame("test"), 0,0,0,0));
	
	kl = remove( kl, k2);
	System.out.print( "-2: " + kl);
	if ( kl instanceof AWTEventMulticaster ) ((AWTEventMulticaster)kl).dump();
	System.out.println();
	
	kl = remove( kl, k1);
	System.out.print( "-1: " + kl);
	if ( kl instanceof AWTEventMulticaster ) ((AWTEventMulticaster)kl).dump();
	System.out.println();

	kl = remove( kl, k1);
	System.out.print( "-1: " + kl);
	if ( kl instanceof AWTEventMulticaster ) ((AWTEventMulticaster)kl).dump();
	System.out.println();

	kl = remove( kl, k3);
	System.out.print( "-3: " + kl);
	System.out.println();
*******/
}

public void mouseClicked ( MouseEvent evt ) {
	((MouseListener)a).mouseClicked( evt);
	((MouseListener)b).mouseClicked( evt);
}

public void mouseDragged ( MouseEvent evt ) {
	((MouseMotionListener)a).mouseDragged( evt);
	((MouseMotionListener)b).mouseDragged( evt);
}

public void mouseEntered ( MouseEvent evt ) {
	((MouseListener)a).mouseEntered( evt);
	((MouseListener)b).mouseEntered( evt);
}

public void mouseExited ( MouseEvent evt ) {
	((MouseListener)a).mouseExited( evt);
	((MouseListener)b).mouseExited( evt);
}

public void mouseMoved ( MouseEvent evt ) {
	((MouseMotionListener)a).mouseMoved( evt);
	((MouseMotionListener)b).mouseMoved( evt);
}

public void mousePressed ( MouseEvent evt ) {
	((MouseListener)a).mousePressed( evt);
	((MouseListener)b).mousePressed( evt);
}

public void mouseReleased ( MouseEvent evt ) {
	((MouseListener)a).mouseReleased( evt);
	((MouseListener)b).mouseReleased( evt);
}

public static ActionListener remove ( ActionListener listeners,
                        ActionListener remListener) {
	return (ActionListener) removeInternal( listeners, remListener);
}

public static AdjustmentListener remove ( AdjustmentListener listeners,
                            AdjustmentListener remListener) {
	return (AdjustmentListener) removeInternal( listeners, remListener);
}

public static ComponentListener remove ( ComponentListener listeners,
                           ComponentListener remListener) {
	return (ComponentListener) removeInternal( listeners, remListener);
}

public static ContainerListener remove ( ContainerListener listeners,
                           ContainerListener remListener) {
	return (ContainerListener) removeInternal( listeners, remListener);
}

public static FocusListener remove ( FocusListener listeners,
                       FocusListener remListener) {
	return (FocusListener) removeInternal( listeners, remListener);
}

public static ItemListener remove ( ItemListener listeners,
                      ItemListener remListener) {
	return (ItemListener) removeInternal( listeners, remListener);
}

public static KeyListener remove ( KeyListener listeners,
                     KeyListener remListener) {
	return (KeyListener) removeInternal( listeners, remListener);
}

public static MouseListener remove ( MouseListener listeners,
                       MouseListener remListener) {
	return (MouseListener) removeInternal( listeners, remListener);
}

public static MouseMotionListener remove ( MouseMotionListener listeners,
                             MouseMotionListener remListener) {
	return (MouseMotionListener) removeInternal( listeners, remListener);
}

public static TextListener remove ( TextListener listeners,
                      TextListener remListener) {
	return (TextListener) removeInternal( listeners, remListener);
}

public static WindowListener remove ( WindowListener listeners,
                        WindowListener remListener) {
	return (WindowListener) removeInternal( listeners, remListener);
}

protected static EventListener removeInternal ( EventListener list,
                               EventListener remListener ) {
	AWTEventMulticaster mc, mcLast;

	if ( (list == null) || (list == remListener) ) // empty list or only listener
		return null;

	if ( list instanceof AWTEventMulticaster ) {   // traverse the list
		mc = (AWTEventMulticaster) list;
		if ( mc.a == remListener )
			return mc.b;
		if ( mc.b == remListener )
			return mc.a;

		while ( mc.b instanceof AWTEventMulticaster ) {
			mcLast = mc;
			mc = (AWTEventMulticaster) mc.b;

			if ( mc.a == remListener ){
				mcLast.b = mc.b;
				return list;
			}
			if ( mc.b == remListener ) {
				mcLast.b = mc.a;
				return list;
			}
		}
	}
	
	return list;
}

public void textValueChanged ( TextEvent evt ) {
	((TextListener)a).textValueChanged( evt);
	((TextListener)b).textValueChanged( evt);
}

public void windowActivated ( WindowEvent evt ) {
	((WindowListener)a).windowActivated( evt);
	((WindowListener)b).windowActivated( evt);
}

public void windowClosed ( WindowEvent evt ) {
	((WindowListener)a).windowClosed( evt);
	((WindowListener)b).windowClosed( evt);
}

public void windowClosing ( WindowEvent evt ) {
	((WindowListener)a).windowClosing( evt);
	((WindowListener)b).windowClosing( evt);
}

public void windowDeactivated ( WindowEvent evt ) {
	((WindowListener)a).windowDeactivated( evt);
	((WindowListener)b).windowDeactivated( evt);
}

public void windowDeiconified ( WindowEvent evt ) {
	((WindowListener)a).windowDeiconified( evt);
	((WindowListener)b).windowDeiconified( evt);
}

public void windowIconified ( WindowEvent evt ) {
	((WindowListener)a).windowIconified( evt);
	((WindowListener)b).windowIconified( evt);
}

public void windowOpened ( WindowEvent evt ) {
	((WindowListener)a).windowOpened( evt);
	((WindowListener)b).windowOpened( evt);
}
}
