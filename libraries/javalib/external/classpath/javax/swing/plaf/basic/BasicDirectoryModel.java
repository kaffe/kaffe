/* BasicDirectoryModel.java --
   Copyright (C) 2005, 2006  Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

package javax.swing.plaf.basic;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.File;
import java.util.Collections;
import java.util.Comparator;
import java.util.Enumeration;
import java.util.Vector;
import javax.swing.AbstractListModel;
import javax.swing.JFileChooser;
import javax.swing.event.ListDataEvent;
import javax.swing.filechooser.FileSystemView;


/**
 * Implements an AbstractListModel for directories where the source
 * of the files is a JFileChooser object. 
 *
 * This class is used for sorting and ordering the file list in
 * a JFileChooser L&F object.
 */
public class BasicDirectoryModel extends AbstractListModel
  implements PropertyChangeListener
{
  /** The list of files itself */
  private Vector contents;

  /** The number of directories in the list */
  private int directories;

  /** The listing mode of the associated JFileChooser,
      either FILES_ONLY, DIRECTORIES_ONLY or FILES_AND_DIRECTORIES */
  private int listingMode;

  /** The JFileCooser associated with this model */
  private JFileChooser filechooser;

  /** A Comparator class/object for sorting the file list. */
  private Comparator comparator = new Comparator()
    {
      public int compare(Object o1, Object o2)
      {
	if (lt((File) o1, (File) o2))
	  return -1;
	else
	  return 1;
      }
    };

  /**
   * Creates a new BasicDirectoryModel object.
   *
   * @param filechooser DOCUMENT ME!
   */
  public BasicDirectoryModel(JFileChooser filechooser)
  {
    this.filechooser = filechooser;
    filechooser.addPropertyChangeListener(this);
    listingMode = filechooser.getFileSelectionMode();
    contents = new Vector();
    validateFileCache();
  }

  /**
   * Returns whether a given (File) object is included in the list.
   *
   * @param o - The file object to test.
   *
   * @return <code>true</code> if the list contains the given object.
   */
  public boolean contains(Object o)
  {
    return contents.contains(o);
  }

  /**
   * Fires a content change event. 
   */
  public void fireContentsChanged()
  {
    fireContentsChanged(this, 0, getSize() - 1);
  }

  /**
   * Returns a Vector of (java.io.File) objects containing
   * the directories in this list.
   *
   * @return a Vector
   */
  public Vector getDirectories()
  {
    Vector tmp = new Vector();
    for (int i = 0; i < directories; i++)
      tmp.add(contents.get(i));
    return tmp;
  }

  /**
   * Returns the (java.io.File) object at 
   * an index in the list.
   *
   * @param index The list index
   * @return a File object
   */
  public Object getElementAt(int index)
  {
    if (index > getSize() - 1)
      return null;
    return contents.elementAt(index);
  }

  /**
   * Returns a Vector of (java.io.File) objects containing
   * the files in this list.
   *
   * @return a Vector
   */
  public Vector getFiles()
  {
    Vector tmp = new Vector();
    for (int i = directories; i < getSize(); i++)
      tmp.add(contents.get(i));
    return tmp;
  }

  /**
   * Returns the size of the list, which only includes directories 
   * if the JFileChooser is set to DIRECTORIES_ONLY.
   *
   * Otherwise, both directories and files are included in the count.
   *
   * @return The size of the list.
   */
  public int getSize()
  {
    if (listingMode == JFileChooser.DIRECTORIES_ONLY)
      return directories;
    return contents.size();
  }

  /**
   * Returns the index of an (java.io.File) object in the list.
   *
   * @param o The object - normally a File.
   *
   * @return the index of that object, or -1 if it is not in the list.
   */
  public int indexOf(Object o)
  {
    return contents.indexOf(o);
  }

  /**
   * Obsoleted method which does nothing.
   */
  public void intervalAdded(ListDataEvent e)
  {
    // obsoleted
  }

  /**
   * Obsoleted method which does nothing.
   */
  public void intervalRemoved(ListDataEvent e)
  {
    // obsoleted
  }

  /**
   * Obsoleted method which does nothing.
   */
  public void invalidateFileCache()
  {
    // obsoleted
  }

  /**
   * Less than, determine the relative order in the list of two files
   * for sorting purposes.
   *
   * The order is: directories < files, and thereafter alphabetically,
   * using the default locale collation.
   *
   * @param a the first file
   * @param b the second file
   *
   * @return <code>true</code> if a > b, <code>false</code> if a < b.
   */
  protected boolean lt(File a, File b)
  {
    boolean aTrav = filechooser.isTraversable(a);
    boolean bTrav = filechooser.isTraversable(b);

    if (aTrav == bTrav)
      {
        String aname = a.getName().toLowerCase();
        String bname = b.getName().toLowerCase();
        return (aname.compareTo(bname) < 0) ? true : false;
      }
    else
      {
        if (aTrav)
          return true;
        else
          return false;
      }
  }

  /**
   * Listens for a property change; the change in file selection mode of the
   * associated JFileChooser. Reloads the file cache on that event.
   *
   * @param e - A PropertyChangeEvent.
   */
  public void propertyChange(PropertyChangeEvent e)
  {
    if (e.getPropertyName().equals(JFileChooser.FILE_SELECTION_MODE_CHANGED_PROPERTY))
      {
	listingMode = filechooser.getFileSelectionMode();
	validateFileCache();
      }
  }

  /**
   * Renames a file - However, does <I>not</I> re-sort the list 
   * or replace the old file with the new one in the list.
   *
   * @param oldFile The old file
   * @param newFile The new file name
   *
   * @return <code>true</code> if the rename succeeded
   */
  public boolean renameFile(File oldFile, File newFile)
  {
    return oldFile.renameTo( newFile );
  }

  /**
   * Sorts a Vector of File objects.
   *
   * @param v The Vector to sort.
   */
  protected void sort(Vector v)
  {
    Collections.sort(v, comparator);
    Enumeration e = Collections.enumeration(v);
    Vector tmp = new Vector();
    for (; e.hasMoreElements();)
      tmp.add(e.nextElement());

    contents = tmp;
  }

  /**
   * Re-loads the list of files
   */
  public void validateFileCache()
  {
    // FIXME: Get the files and sort them in a seperate thread and deliver
    // them a few at a time to be filtered, so that the file selector is
    // responsive even with long file lists.
    contents.clear();
    directories = 0;
    FileSystemView fsv = filechooser.getFileSystemView();
    File[] list = fsv.getFiles(filechooser.getCurrentDirectory(),
                               filechooser.isFileHidingEnabled());

    if (list == null)
      return;

    for (int i = 0; i < list.length; i++)
      {
	if (list[i] == null)
	  continue;
	boolean isDir = filechooser.isTraversable(list[i]);

	if( listingMode != JFileChooser.DIRECTORIES_ONLY || isDir )
	  if (filechooser.accept(list[i]))
	    {
	      contents.add(list[i]);
	      if (isDir)
		directories++;
	    }
      }
    sort(contents);
    filechooser.revalidate();
    filechooser.repaint();
  }
}

