package antlr;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.cs.usfca.edu
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: CharStreamException.java,v 1.2 2005/12/24 21:50:48 robilad Exp $
 */

/**
 * Anything that goes wrong while generating a stream of characters
 */
public class CharStreamException extends ANTLRException {
    /**
     * CharStreamException constructor comment.
     * @param s java.lang.String
     */
    public CharStreamException(String s) {
        super(s);
    }
}
