package antlr;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.cs.usfca.edu
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: ParserSharedInputState.java,v 1.2 2005/12/24 21:50:49 robilad Exp $
 */

/** This object contains the data associated with an
 *  input stream of tokens.  Multiple parsers
 *  share a single ParserSharedInputState to parse
 *  the same stream of tokens.
 */
public class ParserSharedInputState {
    /** Where to get token objects */
    protected TokenBuffer input;

    /** Are we guessing (guessing>0)? */
    public int guessing = 0;

    /** What file (if known) caused the problem? */
    protected String filename;

    public void reset() {
         guessing = 0;
         filename = null;
         input.reset();
     }
    
    public String getFilename() {
		return filename;
	}

	public TokenBuffer getInput() {
		return input;
	}
}
