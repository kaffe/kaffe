/* Class : XPFile
   
  This is a cross platform version of the java.io.File class.
  The regular File object does not understand how to convert
  filenames from UNIX to DOS to Mac style paths and vice versa.
  The XPFile object extends the File object so that it can provide
  this functionality transparently. Just allocate a XPFile
  instead of a File and use it just like a regular File object.

  Copyright : Moses DeJong, dejong@cs.umn.edu, 1998.
  Source code licensed under the GPL.
  You can get a copy of the license from www.gnu.org.

  This code is intended for use in the Kaffe project but you can use
  it in other projects as long as you follow the license rules.
*/

package kaffe.tools.jar;

import java.io.File;

public class XPFile extends File {

    public XPFile(String path) {
        super( convertSeparator(null,path) );
    }

    public XPFile(String path, String name) {
        super( convertSeparator(path,name) );
    }

    public XPFile(File dir, String name) {
        this( (dir == null) ? (String) null : dir.getPath(), name);
    }

    private static String convertSeparator(String path, String name) {
        StringBuffer sb = new StringBuffer();

        if (name == null) {
            throw new NullPointerException();
        }

        if (path == null) {
            convertAppend(sb,name,false);
        } else {
            convertAppend(sb,path,true);
            sb.append(separator);
            convertAppend(sb,name,false);
        }

        return sb.toString();
    }

    private static void convertAppend(StringBuffer sb,
        String name,
        boolean rem_end_sep) {

        int i = 0;
        int len = name.length();
        char c;

        if (len == 0) {
            return; // zero length string needs no processing
        } else {
            len--; // end for loop one char before last char
        }

        for ( ; i < len ; i++) {
            c = name.charAt(i);
            switch (c) {
                case '\\':
                case '/':
                    sb.append(separator);
                    break;
                default:
                    sb.append(c);
            }
        }

        // don't add a separator to the end of the name
        // if we were called with rem_end_sep set to true

        c = name.charAt(i);

        if (c != '/' && c != '\\') {
            sb.append(c);
        } else {
            if (! rem_end_sep) {
                sb.append(separator);
            }
        }

        //System.out.println("sb is \"" + sb + "\"");

    }

}
