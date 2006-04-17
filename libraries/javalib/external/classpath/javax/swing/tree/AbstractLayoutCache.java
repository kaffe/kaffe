/* AbstractLayoutCache.java --
   Copyright (C) 2002, 2004  Free Software Foundation, Inc.

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


package javax.swing.tree;

import gnu.classpath.NotImplementedException;

import java.awt.Rectangle;
import java.util.Enumeration;

import javax.swing.event.TreeModelEvent;

/**
 * class AbstractLayoutCache
 * 
 * @author Andrew Selkirk
 */
public abstract class AbstractLayoutCache
  implements RowMapper
{
  /**
   * class NodeDimensions
   */
  public abstract static class NodeDimensions
  {
    /**
     * Creates <code>NodeDimensions</code> object.
     */
    public NodeDimensions()
    {
      // Do nothing here.
    }

    /**
     * getNodeDimensions
     * 
     * @param value0 TODO
     * @param value1 TODO
     * @param value2 TODO
     * @param value3 TODO
     * @param value4 TODO
     * @return Rectangle
     */
    public abstract Rectangle getNodeDimensions(Object value0, int value1,
                                                int value2, boolean value3,
                                                Rectangle value4);
  }

  /**
   * nodeDimensions
   */
  protected NodeDimensions nodeDimensions;

  /**
   * treeModel
   */
  protected TreeModel treeModel;

  /**
   * treeSelectionModel
   */
  protected TreeSelectionModel treeSelectionModel;

  /**
   * rootVisible
   */
  protected boolean rootVisible;

  /**
   * rowHeight
   */
  protected int rowHeight;

  /**
   * Constructor AbstractLayoutCache
   */
  public AbstractLayoutCache()
  {
    // Do nothing here.
  }

  /**
   * setNodeDimensions
   * 
   * @param dimensions TODO
   */
  public void setNodeDimensions(NodeDimensions dimensions)
  {
    nodeDimensions = dimensions;
  }

  /**
   * getNodeDimensions
   * 
   * @return NodeDimensions
   */
  public NodeDimensions getNodeDimensions()
  {
    return nodeDimensions;
  }

 /**
  * Get the node dimensions. The NodeDimensions property must be set
  * (unless the method is overridden, like if
  * {@link FixedHeightLayoutCache}. If the method is not overridden and
  * the property is not set, the InternalError is thrown.
  * 
  * @param value the last node in the path
  * @param row the node row
  * @param depth the indentation depth
  * @param expanded true if this node is expanded, false otherwise
  * @param bounds the area where the tree is displayed
  */
  protected Rectangle getNodeDimensions(Object value, int row, int depth,
                                        boolean expanded, Rectangle bounds)
  {
    if (nodeDimensions == null)
      throw new InternalError("The NodeDimensions are not set");
    return nodeDimensions.getNodeDimensions(value, row, depth, expanded, bounds);
  }

  /**
   * Sets the model that provides the tree data.
   * 
   * @param model the model
   */
  public void setModel(TreeModel model)
  {
    treeModel = model;
  }

  /**
   * Returns the model that provides the tree data.
   * 
   * @return the model
   */
  public TreeModel getModel()
  {
    return treeModel;
  }

  /**
   * setRootVisible
   * 
   * @param visible <code>true</code> if root should be visible,
   * <code>false</code> otherwise
   */
  public void setRootVisible(boolean visible)
  {
    rootVisible = visible;
  }

  /**
   * isRootVisible
   * 
   * @return <code>true</code> if root is visible,
   * <code>false</code> otherwise
   */
  public boolean isRootVisible()
  {
    return rootVisible;
  }

  /**
   * setRowHeight
   * 
   * @param height the row height
   */
  public void setRowHeight(int height)
  {
    rowHeight = height;
  }

  /**
   * getRowHeight
   * 
   * @return the row height
   */
  public int getRowHeight()
  {
    return rowHeight;
  }

  /**
   * setSelectionModel
   * 
   * @param model the model
   */
  public void setSelectionModel(TreeSelectionModel model)
  {
    treeSelectionModel = model;
  }

  /**
   * getSelectionModel
   * 
   * @return the model
   */
  public TreeSelectionModel getSelectionModel()
  {
    return treeSelectionModel;
  }

  /**
   * getPreferredHeight
   * 
   * @return int
   */
  public int getPreferredHeight() 
    throws NotImplementedException
  {
    return 0; // TODO
  }

  /**
   * getPreferredWidth
   * 
   * @param value0 TODO
   * 
   * @return int
   */
  public int getPreferredWidth(Rectangle value0) 
    throws NotImplementedException
  {
    return 0; // TODO
  }

  /**
   * isExpanded
   * 
   * @param value0 TODO
   * 
   * @return boolean
   */
  public abstract boolean isExpanded(TreePath value0);

  /**
   * getBounds
   * 
   * @param value0 TODO
   * @param value1 TODO
   * 
   * @return Rectangle
   */
  public abstract Rectangle getBounds(TreePath value0, Rectangle value1);

  /**
   * getPathForRow
   * 
   * @param row the row
   * 
   * @return the tree path
   */
  public abstract TreePath getPathForRow(int row);

  /**
   * getRowForPath
   * 
   * @param path the tree path
   * 
   * @return the row
   */
  public abstract int getRowForPath(TreePath path);

  /**
   * getPathClosestTo
   * 
   * @param value0 TODO
   * @param value1 TODO
   * 
   * @return the tree path
   */
  public abstract TreePath getPathClosestTo(int value0, int value1);

  /**
   * getVisiblePathsFrom
   * 
   * @param path the tree path
   * 
   * @return Enumeration
   */
  public abstract Enumeration getVisiblePathsFrom(TreePath path);

  /**
   * getVisibleChildCount
   * 
   * @param path the tree path
   * 
   * @return int
   */
  public abstract int getVisibleChildCount(TreePath path);

  /**
   * setExpandedState
   * 
   * @param value0 TODO
   * 
   * @param value1 TODO
   */
  public abstract void setExpandedState(TreePath value0, boolean value1);

  /**
   * getExpandedState
   * 
   * @param path the tree path
   * 
   * @return boolean
   */
  public abstract boolean getExpandedState(TreePath path);

  /**
   * getRowCount
   * 
   * @return the number of rows
   */
  public abstract int getRowCount();

  /**
   * invalidateSizes
   */
  public abstract void invalidateSizes();

  /**
   * invalidatePathBounds
   * 
   * @param path the tree path
   */
  public abstract void invalidatePathBounds(TreePath path);

  /**
   * treeNodesChanged
   * 
   * @param event the event to send
   */
  public abstract void treeNodesChanged(TreeModelEvent event);

  /**
   * treeNodesInserted
   * 
   * @param event the event to send
   */
  public abstract void treeNodesInserted(TreeModelEvent event);

  /**
   * treeNodesRemoved
   * 
   * @param event the event to send
   */
  public abstract void treeNodesRemoved(TreeModelEvent event);

  /**
   * treeStructureChanged
   * 
   * @param event the event to send
   */
  public abstract void treeStructureChanged(TreeModelEvent event);

  /**
   * Get the tree row numbers for the given pathes. This method performs
   * the "bulk" conversion that may be faster than mapping pathes one by
   * one. To have the benefit from the bulk conversion, the method must be
   * overridden in the derived classes. The default method delegates work
   * to the {@link #getRowForPath(TreePath)}.
   * 
   * @param paths the tree paths the array of the tree pathes.
   * @return the array of the matching tree rows.
   */
  public int[] getRowsForPaths(TreePath[] paths)
  {
    int[] rows = new int[paths.length];
    for (int i = 0; i < rows.length; i++)
      rows[i] = getRowForPath(paths[i]);
    return rows;
  }

  /**
   * isFixedRowHeight
   * 
   * @return boolean
   */
  protected boolean isFixedRowHeight()
    throws NotImplementedException
  {
    return false; // TODO
  }
}
