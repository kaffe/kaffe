package java.awt;

import java.util.Properties;

/**
 * class PSPrintJob - a PrintJob PostScript implementation
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author J.Mehlitz
 */
public class PSPrintJob
  extends PrintJob
{
	PSGraphics psg;

public PSPrintJob( Frame fr, String jobTitle, Properties props) {
	psg = new PSGraphics( jobTitle + ".ps");
	psg.pj = this;
}

public void end() {
	psg.dispose();
}

public Graphics getGraphics() {
	return psg;
}

public Dimension getPageDimension() {
	return Toolkit.singleton.getScreenSize();
}

public int getPageResolution() {
	//pixels per inch
	return 300;
}

public boolean lastPageFirst() {
	return false;
}
}
