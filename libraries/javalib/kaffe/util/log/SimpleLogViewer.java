/**
 * SimpleLogViewer - a very simple console window, which can be used to display
 *                   the contents of a LogStream (no scrolling or input supported yet)
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */

package kaffe.util.log;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Insets;
import java.awt.event.WindowEvent;
import java.io.PrintStream;

import kaffe.util.DoubleLinkedObject;

public class SimpleLogViewer
  extends Frame
  implements LogClient
{
	private int lineHeight;
	private FontMetrics fm;
	private LogStream logStream;
	private boolean isEnabled;

public SimpleLogViewer () {	
	super( "Log");

	Dimension d = getToolkit().getScreenSize();
	setBounds( 0, d.height / 2, 2*d.width /3, d.height / 2);
	
	setBackground( Color.black);
	setForeground( Color.green);
	
	Font fnt = new Font( "Monospaced", Font.PLAIN, 10);
	fm = getToolkit().getFontMetrics( fnt);
	
	setFont( fnt);
	lineHeight = fm.getHeight();
}

public void disable () {
	isEnabled = false;
}

public void enable () {
	setVisible( true);
	isEnabled = true;
}

public static void main ( String[] args ) {
	// just a testbed

	LogClient lv = new SimpleLogViewer();
	LogStream ls = new LogStream( 30, lv);
	lv.enable();
	
	System.setOut( new PrintStream( ls) );
	System.setErr( System.out);
}

public void newLogLine ( LogStream ls ) {
	if ( ! isEnabled )
		return;

	logStream = ls;

/** this is the fast version, but it does work properly just in case we are not obscured
	DoubleLinkedObject ll = ls.getLastLine();
	Dimension d = getSize();
	Insets    in = getInsets();

	Graphics g = getGraphics();
	if ( g != null ) {
		int h = d.height - (in.top + in.bottom + lineHeight);
		int w = d.width - (in.left + in.right);
		int v = in.top + lineHeight;

		g.copyArea( in.left, v, w, h, 0, -lineHeight);		
		g.clearRect( in.left, d.height - in.bottom - lineHeight, w, lineHeight);
		g.drawString( ll.data.toString(), in.left + 2,
		              d.height - in.bottom - fm.getDescent());
		g.dispose();
	}
**/
	
	repaint();
}

public void paint ( Graphics g ) {
	Insets    in = getInsets();
	Dimension d = getSize();
	DoubleLinkedObject ll = null;

	int u = in.left + 2;
	int v = d.height - in.bottom;
	int w = d.width - (in.left + in.right);

	if ( logStream != null ) {
		ll = logStream.getLastLine();
		v -= fm.getDescent();
		
		do {
			g.clearRect( in.left, v - fm.getAscent(), w, lineHeight);		
			g.drawString( ll.data.toString(), u, v);
					ll = ll.prev;
			v -= lineHeight;	
		} while ( (v > in.top) && (ll != null) );
	}
	
	if ( ll == null ) {
		g.clearRect( in.left, in.top, w, v - in.top);
	}
}

protected void processWindowEvent ( WindowEvent e ) {
	super.processWindowEvent( e);

	switch ( e.getID() ) {
	case WindowEvent.WINDOW_CLOSING:
		isEnabled = false;
		dispose();
		break;
	case WindowEvent.WINDOW_ICONIFIED:
		isEnabled = false;
		break;
	case WindowEvent.WINDOW_DEICONIFIED:
		isEnabled = true;
		break;
	}
}

public void update ( Graphics g ) {
	// no background blanking required
	paint( g);
}
}
