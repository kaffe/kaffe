package antlr;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.cs.usfca.edu
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: TokenStream.java,v 1.2 2005/12/24 21:50:49 robilad Exp $
 */

public interface TokenStream {
    public Token nextToken() throws TokenStreamException;
}
