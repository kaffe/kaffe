package antlr;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: TokenStream.java,v 1.1 2005/09/17 21:38:44 robilad Exp $
 */

public interface TokenStream {
    public Token nextToken() throws TokenStreamException;
}
