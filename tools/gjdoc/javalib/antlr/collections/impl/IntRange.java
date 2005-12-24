package antlr.collections.impl;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.cs.usfca.edu
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: IntRange.java,v 1.2 2005/12/24 21:50:50 robilad Exp $
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
