package antlr.collections.impl;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.cs.usfca.edu
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: VectorEnumerator.java,v 1.2 2005/12/24 21:50:50 robilad Exp $
 */

import java.util.Enumeration;
import java.util.NoSuchElementException;

import antlr.collections.Enumerator;

// based on java.lang.Vector; returns any null indices between non-null ones.

class VectorEnumerator implements Enumeration {
    Vector vector;
    int i;


    VectorEnumerator(Vector v) {
        vector = v;
        i = 0;
    }

    public boolean hasMoreElements() {
        synchronized (vector) {
            return i <= vector.lastElement;
        }
    }

    public Object nextElement() {
        synchronized (vector) {
            if (i <= vector.lastElement) {
                return vector.data[i++];
            }
            throw new NoSuchElementException("VectorEnumerator");
        }
    }
}
