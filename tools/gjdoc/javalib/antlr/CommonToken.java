package antlr;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.cs.usfca.edu
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: CommonToken.java,v 1.2 2005/12/24 21:50:48 robilad Exp $
 */

public class CommonToken extends Token {
    // most tokens will want line and text information
    protected int line;
    protected String text = null;
    protected int col;

    public CommonToken() {
    }

    public CommonToken(int t, String txt) {
        type = t;
        setText(txt);
    }

    public CommonToken(String s) {
        text = s;
    }

    public int getLine() {
        return line;
    }

    public String getText() {
        return text;
    }

    public void setLine(int l) {
        line = l;
    }

    public void setText(String s) {
        text = s;
    }

    public String toString() {
        return "[\"" + getText() + "\",<" + type + ">,line=" + line + ",col=" + col + "]";
    }

    /** Return token's start column */
    public int getColumn() {
        return col;
    }

    public void setColumn(int c) {
        col = c;
    }
}
