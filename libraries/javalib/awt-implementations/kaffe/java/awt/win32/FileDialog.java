package java.awt;

import java.awt.Dialog;
import java.awt.Frame;
import java.awt.Toolkit;
import java.io.File;
import java.io.FilenameFilter;

/**
 * FileDialog - 
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */

public class FileDialog
  extends Dialog
{
	final public static int LOAD = 0;
	final public static int SAVE = 1;
        String file;
        String dir;
        FilenameFilter filter;
	int mode;
        DlgThread dlgt;

public FileDialog( Frame parent) {
	this( parent, null, LOAD);
}

public FileDialog( Frame parent, String title) {
	this( parent, title, LOAD);
}

public FileDialog( Frame parent, String title, int mode) {
        super( parent, title, true);
	this.mode = mode;
        dlgt = new DlgThread();
}

public void addNotify() {
        flags |= IS_ADD_NOTIFIED;
}

public void removeNotify() {
        flags &= ~IS_ADD_NOTIFIED;
}

public void show() {
        addNotify();
        dlgt.start();
        Toolkit.eventThread.run( this);
}

public String getDirectory() {
        return dir;
}

public String getFile() {
        return file;
}

public FilenameFilter getFilenameFilter() {
	return filter;
}

public int getMode() {
	return mode;
}

protected String paramString() {
	return super.paramString();
}

public void setDirectory( String dir) {
        this.dir = dir;
}

public void setFile( String file) {
        this.file = file;
}

public void setFilenameFilter( FilenameFilter filter) {
	this.filter = filter;
}

public void setMode( int mode) throws IllegalArgumentException {
	if ( ( mode != LOAD) && ( mode != SAVE) )
		throw new IllegalArgumentException( String.valueOf( mode));
	this.mode = mode;
}


class DlgThread extends Thread {
public void run() {
        String sel = null;

        if ( mode == LOAD ) {
                sel = Toolkit.fdlgLoad( (owner != null) ? owner.nativeData : null,
                                         title, dir, file, ".*" );
        }
        else {
                sel = Toolkit.fdlgSave( (owner != null) ? owner.nativeData : null,
                                         title, dir, file, ".*" );
        }

        if ( sel == null ) {
                file =null;
        }
        else {
                int ls = sel.lastIndexOf( File.separatorChar) + 1;
                dir = sel.substring( 0, ls);
                file = sel.substring( ls);
        }

        WMEvent e = WMEvent.getEvent( FileDialog.this, WMEvent.WM_DESTROY);
        Toolkit.eventQueue.postEvent( e);
}
}

}
