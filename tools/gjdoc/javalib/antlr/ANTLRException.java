package antlr;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.cs.usfca.edu
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: ANTLRException.java,v 1.2 2005/12/24 21:50:48 robilad Exp $
 */

public class ANTLRException extends Exception {

    public ANTLRException() {
        super();
    }

    public ANTLRException(String s) {
        super(s);
    }

	public ANTLRException(String message, Throwable cause) {
		super(message, cause);
	}
	
	public ANTLRException(Throwable cause) {
		super(cause);
	}
}
