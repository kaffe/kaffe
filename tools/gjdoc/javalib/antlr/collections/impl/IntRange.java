package antlr.collections.impl;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: IntRange.java,v 1.1 2005/09/17 21:38:45 robilad Exp $
 */

public class IntRange {
    int begin, end;


    public IntRange(int begin, int end) {
        this.begin = begin;
        this.end = end;
    }

    public String toString() {
        return begin + ".." + end;
    }
}
