package java.awt;


/**
 * Canvas - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J. Mehlitz
 */
public class Canvas
  extends Component
{
public Canvas() {
}

public void paint( Graphics g) {
	g.setColor( bgClr);
	g.fillRect( 0, 0, width, height);
}

}
