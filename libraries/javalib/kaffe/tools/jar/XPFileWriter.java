/* Class : XPFileWriter
   
  This is a cross platform version of the java.io.FileWriter class.
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
import java.io.FileWriter;
import java.io.IOException;

public class XPFileWriter extends FileWriter {

    public XPFileWriter(String fileName)
	throws IOException, FileNotFoundException
    {
	super( new XPFile(fileName) );
    }
    
    public XPFileWriter(XPFile xpfile)
	throws IOException, FileNotFoundException
    {
	super( xpfile );
    }

    public XPFileWriter(File file)
	throws IOException, FileNotFoundException
    {
	super( file.getPath() );
    }

    public XPFileWriter(FileDescriptor fd)
    {
	super( fd );
    }

}
