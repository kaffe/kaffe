/**
 * PrintJob - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */

package java.awt;

abstract public class PrintJob {

public PrintJob() {
}

abstract public void end();

abstract public Graphics getGraphics();

abstract public Dimension getPageDimension();

abstract public int getPageResolution();

abstract public boolean lastPageFirst();
}
