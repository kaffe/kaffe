/* Class : XPFileInputStream
   
  This is a cross platform version of the java.io.FileInputStream class.
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
import java.io.FileInputStream;
import java.io.FileNotFoundException;

public class XPFileInputStream extends FileInputStream {

    public XPFileInputStream(String fileName) throws FileNotFoundException
    {
	super( new XPFile(fileName) );
    }
    
    public XPFileInputStream(XPFile xpfile) throws FileNotFoundException
    {
	super( xpfile );
    }

    public XPFileInputStream(File file) throws FileNotFoundException
    {
	super( file.getPath() );
    }

    public XPFileInputStream(FileDescriptor fd)
    {
	super( fd );
    }

}
