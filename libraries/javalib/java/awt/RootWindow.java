/*
 * RootWindow is just a dummy in case we have a native window manager
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */

package java.awt;


class RootWindow
  extends Window
{
	static RootWindow singleton;

static RootWindow getDefaultRootWindow() {
	// we don't have a RootWindow in case there is a native window manager
	return null;
}
}
