/* FixedHeightLayoutCache.java -- Fixed cell height tree layout cache
Copyright (C) 2002, 2004, 2006,  Free Software Foundation, Inc.

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

import java.awt.Rectangle;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.Set;
import java.util.Vector;

import javax.swing.UIManager;
import javax.swing.event.TreeModelEvent;

/**
 * The fixed height tree layout. This class assumes that all cells in the tree
 * have the same fixed height. This may be not the case, for instance, if leaves
 * and branches have different height, of if the tree rows may have arbitrary
 * variable height. This class will also work if the NodeDimensions are not
 * set. If they are set, the size calculations are just forwarded to the set
 * instance. 
 * 
 * @author Audrius Meskauskas
 * @author Andrew Selkirk 
 */
public class FixedHeightLayoutCache
		extends AbstractLayoutCache
{
  /**
   * The cached node record.
   */
  class NodeRecord
  {
    NodeRecord(int aRow, int aDepth, Object aNode, Object aParent)
    {
      row = aRow;
      depth = aDepth;
      parent = aParent;
      node = aNode;
      
      isExpanded = expanded.contains(aNode); 
    }
    
    /**
     * The row, where the tree node is displayed.
     */
    final int row;    
    
    /**
     * The nesting depth
     */
    final int depth;
    
    /**
     * The parent of the given node, null for the root node.
     */
    final Object parent;
    
    /**
     * This node.
     */
    final Object node;
    
    /**
     * True for the expanded nodes. The value is calculated in constructor.
     * Using this field saves one hashtable access operation.
     */
    final boolean isExpanded;
    
    /**
     * The cached bounds of the tree row.
     */
    Rectangle bounds;
    
    /**
     * The path from the tree top to the given node (computed under first
     * demand)
     */
    private TreePath path;
    
    TreePath getPath()
    {
      if (path == null)
        {
          LinkedList lpath = new LinkedList();
          NodeRecord rp = this;
          while (rp != null)
            {
              lpath.addFirst(rp.node);
              if (rp.parent != null)
                rp = (NodeRecord) nodes.get(rp.parent);
              else
                rp = null;
            }
          path = new TreePath(lpath.toArray());
        }
      return path;
    }
  }

  /**
   * The set of all expanded tree nodes.
   */
  Set expanded = new HashSet();
  
  /**
   * Maps nodes to the row numbers.
   */
  Hashtable nodes = new Hashtable();
  
  /**
   * Maps row numbers to nodes.
   */
  Hashtable row2node = new Hashtable();
  
  /**
   * If true, the row map must be recomputed before using.
   */
  boolean dirty;

  /**
   * Creates the unitialised instance. Before using the class, the row height
   * must be set with the {@link #setRowHeight(int)} and the model must be set
   * with {@link #setModel(TreeModel)}. The node dimensions may not be set.
   */
  public FixedHeightLayoutCache()
  {
    // Nothing to do here.
  } 

  /**
   * Get the total number of rows in the tree. Every displayed node occupies the
   * single row. The root node row is included if the root node is set as
   * visible (false by default).
   * 
   * @return int the number of the displayed rows.
   */
  public int getRowCount()
  {
    if (dirty) update();
    return row2node.size();
  } 
  
  /**
   * Refresh the row map.
   */
  private final void update()
  {
    nodes.clear();
    row2node.clear();

    Object root = treeModel.getRoot();

    if (rootVisible)
      {
        countRows(root, null, 0);
      }
    else
      {
        int sc = treeModel.getChildCount(root);
        for (int i = 0; i < sc; i++)
          {
            Object child = treeModel.getChild(root, i);
            countRows(child, root, 1);
          }
      }
    dirty = false;
  }
  
  /**
   * Recursively counts all rows in the tree.
   */
  private final void countRows(Object node, Object parent, int depth)
  {
    Integer n = new Integer(row2node.size());
    row2node.put(n, node);
    
    nodes.put(node, new NodeRecord(n.intValue(), depth, node, parent));

    if (expanded.contains(node) || parent == null)
      {
        int sc = treeModel.getChildCount(node);
        int deeper = depth+1;
        for (int i = 0; i < sc; i++)
          {
            Object child = treeModel.getChild(node, i);
            countRows(child, node, deeper);
          }
      }
  }

  /**
   * This should invalidate the width of the last path component, but 
   * following the JDK 1.4 API it is not cached and the method should return
   * without action.
   * @param path the path being invalidated, ignored.
   */
  public void invalidatePathBounds(TreePath path)
  {
    // Following JDK 1.4 API, should return without action.
  } 

  /**
   * Mark all cached information as invalid.
   */
  public void invalidateSizes()
  {
    dirty = true;
  } 

  /**
   * Set the expanded state of the given path. The expansion states must be
   * always updated when expanding and colapsing the tree nodes. Otherwise 
   * other methods will not work correctly after the nodes are collapsed or
   * expanded.
   *
   * @param path the tree path, for that the state is being set.
   * @param isExpanded the expanded state of the given path.
   */
  public void setExpandedState(TreePath path, boolean isExpanded)
  {
    if (isExpanded)
      expanded.add(path.getLastPathComponent());
    else
      expanded.remove(path.getLastPathComponent());
    
    dirty = true;
  }
  
  /**
   * Get the expanded state for the given tree path.
   * 
   * @return true if the given path is expanded, false otherwise.
   */
  public boolean isExpanded(TreePath path)
  {
    return expanded.contains(path.getLastPathComponent());
  } 

  /**
   * Get bounds for the given tree path.
   * 
   * @param path the tree path
   * @param rect the rectangle, specifying the area where the path should be
   *          displayed.
   * @return Rectangle the bounds of the last line, defined by the given path.
   */
  public Rectangle getBounds(TreePath path, Rectangle rect)
  {
    if (dirty)
      update();
    Object last = path.getLastPathComponent();
    NodeRecord r = (NodeRecord) nodes.get(last);
    if (r == null)
      // This node is not visible.
      return new Rectangle();
    else
      {
        if (r.bounds == null)
          {
            Rectangle dim = getNodeDimensions(last, r.row, r.depth, r.isExpanded,
                                              rect);
            r.bounds = dim;
          }
        return r.bounds;
      }
  } 

  /**
   * Get the path, the last element of that is displayed in the given row.
   * 
   * @param row the row
   * @return TreePath the path
   */
  public TreePath getPathForRow(int row)
  {
    if (dirty)
      update();
    Object last = row2node.get(new Integer(row));
    if (last == null)
      return null;
    else
      {
        NodeRecord r = (NodeRecord) nodes.get(last);
        return r.getPath();
      }
  } 

  /**
   * Get the row, displaying the last node of the given path.
   * 
   * @param path the path
   * @return int the row number or -1 if the end of the path is not visible.
   */
  public int getRowForPath(TreePath path)
  {
    if (dirty) update();

    NodeRecord r = (NodeRecord) nodes.get(path.getLastPathComponent());
    if (r == null)
      return - 1;
    else
      return r.row;
  } 

  /**
   * Get the path, closest to the given point.
   * 
   * @param x the point x coordinate
   * @param y the point y coordinate
   * @return the tree path, closest to the the given point
   */
  public TreePath getPathClosestTo(int x, int y)
  {
    if (dirty)
      update();

    // We do not need to iterate because all rows have the same height.
    int row = y / rowHeight;
    if (row < 0)
      row = 0;
    if (row > getRowCount())
      row = getRowCount() - 1;
    
    if (row < 0)
      return null; // Empty tree - nothing to return.
    
    Object node = row2node.get(new Integer(row));
    NodeRecord nr = (NodeRecord) nodes.get(node);
    return nr.getPath();
  } 

  /**
   * Get the number of the visible childs for the given tree path. If the
   * node is not expanded, 0 is returned. Otherwise, the number of children
   * is obtained from the model as the number of children for the last path
   * component.
   * 
   * @param path the tree path 
   * @return int the number of the visible childs (for row).
   */
  public int getVisibleChildCount(TreePath path)  
  {
    if (isExpanded(path))
      return 0; 
    else
      return treeModel.getChildCount(path.getLastPathComponent());
  } 

  /**
   * Get the enumeration over all visible pathes that start from the given
   * parent path.
   * 
   * @param parentPath the parent path
   * @return the enumeration over pathes
   */
  public Enumeration getVisiblePathsFrom(TreePath parentPath)
  {
    if (dirty)
      update();
    Vector p = new Vector(parentPath.getPathCount());
    Object node;
    NodeRecord nr;

    for (int i = 0; i < parentPath.getPathCount(); i++)
      {
        node = parentPath.getPathComponent(i);
        nr = (NodeRecord) nodes.get(node);
        if (nr.row >= 0)
          p.add(node);
      }
    return p.elements();
  }

  /**
   * Return the expansion state of the given tree path. The expansion state
   * must be previously set with the 
   * {@link #setExpandedState(TreePath, boolean)}
   * 
   * @param path the path being checked
   * @return true if the last node of the path is expanded, false otherwise.
   */
  public boolean getExpandedState(TreePath path)
  {
    return expanded.contains(path.getLastPathComponent());
  }

  /**
   * The listener method, called when the tree nodes are changed.
   * 
   * @param event the change event
   */
  public void treeNodesChanged(TreeModelEvent event)
  {
    dirty = true;
  } 

  /**
   * The listener method, called when the tree nodes are inserted.
   * 
   * @param event the change event
   */
  public void treeNodesInserted(TreeModelEvent event)
  {
    dirty = true;
  } 

  /**
   * The listener method, called when the tree nodes are removed.
   * 
   * @param event the change event
   */
  public void treeNodesRemoved(TreeModelEvent event)
  {
    dirty = true;
  } 

  /**
   * Called when the tree structure has been changed. 
   * 
   * @param event the change event
   */
  public void treeStructureChanged(TreeModelEvent event)
  {
    dirty = true;
  } 
  
  /**
   * Set the tree model that will provide the data.
   */
  public void setModel(TreeModel newModel)
  {
    super.setModel(newModel);
    dirty = true;
  }
  
  /**
   * Inform the instance if the tree root node is visible. If this method
   * is not called, it is assumed that the tree root node is not visible.
   * 
   * @param visible true if the tree root node is visible, false
   * otherwise.
   */
  public void setRootVisible(boolean visible)
  {
    rootVisible = visible;
    dirty = true;
  }

  /**
   * Get the node dimensions. If the NodeDimensions are not set, this method
   * calculates dimensions assuming the fixed row height.
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
    if (nodeDimensions != null)
      return nodeDimensions.getNodeDimensions(value, row, depth, expanded,
                                              bounds);
    else
      {
        Rectangle r = new Rectangle(bounds);

        int indent = depth * UIManager.getInt("Tree.rightChildIndent");

        r.x = indent;
        r.y = row * getRowHeight();
        r.width = bounds.width = r.x;
        r.height = getRowHeight();
        return r;
      }
  }
}
