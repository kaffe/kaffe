/* Class : XPFileOutputStream
   
  This is a cross platform version of the java.io.FileOutputStream class.
  It uses the XPFile class to do cross platform file name conversion.

  Copyright : Moses DeJong, dejong@cs.umn.edu, 1998.
  Source code licensed under the GPL.
  You can get a copy of the license from www.gnu.org.

  This code is intended for use in the Kaffe project but you can use
  it in other projects as long as you follow the license rules.
*/

package kaffe.tools.jar;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class XPFileOutputStream extends FileOutputStream {

    public XPFileOutputStream(String fileName)
	throws IOException, FileNotFoundException
    {
	super( new XPFile(fileName) );
    }
    
    public XPFileOutputStream(XPFile xpfile)
	throws IOException, FileNotFoundException
    {
	super( xpfile );
    }

    public XPFileOutputStream(File file)
	throws IOException, FileNotFoundException
    {
	super( file.getPath() );
    }

    public XPFileOutputStream(FileDescriptor fd)
    {
	super( fd );
    }

}
