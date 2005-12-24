package antlr.collections;

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.cs.usfca.edu
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: Enumerator.java,v 1.2 2005/12/24 21:50:49 robilad Exp $
 */

public interface Enumerator {
    /**Return the element under the cursor; return null if !valid() or
     * if called before first next() call.
     */
    public Object cursor();

    /**Return the next element in the enumeration; first call to next()
     * returns the first element.
     */
    public Object next();

    /**Any more elements in the enumeration? */
    public boolean valid();
}
