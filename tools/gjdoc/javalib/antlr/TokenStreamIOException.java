package antlr;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.cs.usfca.edu
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: TokenStreamIOException.java,v 1.2 2005/12/24 21:50:49 robilad Exp $
 */

import java.io.IOException;

/**
 * Wraps an IOException in a TokenStreamException
 */
public class TokenStreamIOException extends TokenStreamException {
    public IOException io;

    /**
     * TokenStreamIOException constructor comment.
     * @param s java.lang.String
     */
    public TokenStreamIOException(IOException io) {
        super(io.getMessage());
        this.io = io;
    }
}
