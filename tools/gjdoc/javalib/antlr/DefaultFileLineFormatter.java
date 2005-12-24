package antlr;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.cs.usfca.edu
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: DefaultFileLineFormatter.java,v 1.2 2005/12/24 21:50:48 robilad Exp $
 */

public class DefaultFileLineFormatter extends FileLineFormatter {
    public String getFormatString(String fileName, int line, int column) {
        StringBuffer buf = new StringBuffer();

        if (fileName != null)
            buf.append(fileName + ":");

        if (line != -1) {
            if (fileName == null)
                buf.append("line ");

            buf.append(line);

            if (column != -1)
                buf.append(":" + column);

            buf.append(":");
        }

        buf.append(" ");

        return buf.toString();
    }
}
