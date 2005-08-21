/* DefaultTreeCellEditor.java --
   Copyright (C) 2002, 2004, 2005  Free Software Foundation, Inc.

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

import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Insets;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.EventObject;

import javax.swing.Icon;
import javax.swing.JComponent;
import javax.swing.JTextField;
import javax.swing.JTree;
import javax.swing.SwingUtilities;
import javax.swing.UIDefaults;
import javax.swing.UIManager;
import javax.swing.border.Border;
import javax.swing.event.CellEditorListener;
import javax.swing.event.EventListenerList;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;

/**
 * DefaultTreeCellEditor
 * @author Andrew Selkirk
 */
public class DefaultTreeCellEditor
  implements ActionListener, TreeCellEditor, TreeSelectionListener
{
  /**
   * EditorContainer
   */
  public class EditorContainer extends Container
  {
    /**
     * Creates an <code>EditorContainer</code> object.
     */
    public EditorContainer()
    {
      // Do nothing here.
    }

    /**
     * This method only exists for API compatibility and is useless as it does
     * nothing. It got probably introduced by accident.
     */
    public void EditorContainer()
    {
      // Do nothing here.
    }

    /**
     * Returns the preferred size for the Container.
     * 
     * @return Dimension of EditorContainer
     */
    public Dimension getPreferredSize()
    {
      Dimension containerSize = super.getPreferredSize();
      containerSize.width += DefaultTreeCellEditor.this.offset;
      return containerSize;
    }

    /**
     * Overrides Container.paint to paint the node's icon and use 
     * the selection color for the background.
     * 
     * @param g - the specified Graphics window
     */
    public void paint(Graphics g)
    {
      int textIconGap = 4; // default value
      Rectangle vr = new Rectangle();
      Rectangle ir = new Rectangle();
      Rectangle tr = new Rectangle();

      FontMetrics fm = editingComponent.getToolkit().getFontMetrics(getFont());
      SwingUtilities.layoutCompoundLabel(((JComponent) editingComponent), fm,
                                         ((JTextField) editingComponent).getText(),
                                         editingIcon, (int) CENTER_ALIGNMENT,
                                         (int) LEFT_ALIGNMENT, (int) CENTER_ALIGNMENT, 
                                         (int) LEFT_ALIGNMENT, vr, ir, tr, 4);

      Rectangle cr = tr.union(ir);
      tr.width += offset;
      // paint icon
      DefaultTreeCellEditor.this.editingIcon.paintIcon(DefaultTreeCellEditor.this.editingComponent,
                                                       g, cr.x, cr.y);

      // paint background
      Insets insets = new Insets(0, 0, 0, 0);
      Border border = UIManager.getLookAndFeelDefaults().
                        getBorder("Tree.editorBorder");
      if (border != null)
        insets = border.getBorderInsets(this);

      g.setColor(UIManager.getLookAndFeelDefaults().
                 getColor("Tree.selectionBackground"));
      g.fillRect(cr.x, cr.y, cr.width, cr.height - insets.top - insets.bottom);

      // paint border
      if (borderSelectionColor != null)
        {
          g.setColor(borderSelectionColor);
          g.drawRect(cr.x, cr.y, cr.width, cr.height - insets.top
                                           - insets.bottom);
        }

      super.paint(g);
    }

    /**
     * Lays out this Container. If editing, the editor will be placed 
     * at offset in the x direction and 0 for y.
     */
    public void doLayout()
    {
      if (DefaultTreeCellEditor.this.tree.isEditing())
        setLocation(offset, 0);
      super.doLayout();
    }
  }

  /**
   * DefaultTextField
   */
  public class DefaultTextField extends JTextField
  {
    /**
     * border
     */
    protected Border border;

    /**
     * Creates a <code>DefaultTextField</code> object.
     *
     * @param border the border to use
     */
    public DefaultTextField(Border border)
    {
      this.border = border;
    }

    /**
     * Gets the font of this component.
     * @return this component's font; if a font has not been set for 
     * this component, the font of its parent is returned (if the parent
     * is not null, otherwise null is returned). 
     */
    public Font getFont()
    {
      Font font = super.getFont();
      if (font == null)
        {
          Component parent = getParent();
          if (parent != null)
            return parent.getFont();
          return null;
        }
      return font;
    }

    /**
     * Returns the border of the text field.
     *
     * @return the border
     */
    public Border getBorder()
    {
      return border;
    }

    /**
     * Overrides JTextField.getPreferredSize to return the preferred size 
     * based on current font, if set, or else use renderer's font.
     * 
     * @return the Dimension of this textfield.
     */
    public Dimension getPreferredSize()
    {
      String s = getText();

      Font f = getFont();
      FontMetrics fm = getToolkit().getFontMetrics(f);

      return new Dimension(SwingUtilities.computeStringWidth(fm, s),
                           fm.getHeight());
    }
  }

  private EventListenerList listenerList = new EventListenerList();

  /**
   * Editor handling the editing.
   */
  protected TreeCellEditor realEditor;

  /**
   * Renderer, used to get border and offsets from.
   */
  protected DefaultTreeCellRenderer renderer;

  /**
   * Editing container, will contain the editorComponent.
   */
  protected Container editingContainer;

  /**
   * Component used in editing, obtained from the editingContainer.
   */
  protected transient Component editingComponent;

  /**
   * As of Java 2 platform v1.4 this field should no longer be used. 
   * If you wish to provide similar behavior you should directly 
   * override isCellEditable.
   */
  protected boolean canEdit;

  /**
   * Used in editing. Indicates x position to place editingComponent.
   */
  protected transient int offset;

  /**
   * JTree instance listening too.
   */
  protected transient JTree tree;

  /**
   * Last path that was selected.
   */
  protected transient TreePath lastPath;

  /**
   * Used before starting the editing session.
   */
  protected transient javax.swing.Timer timer;

  /**
   * Row that was last passed into getTreeCellEditorComponent.
   */
  protected transient int lastRow;

  /**
   * True if the border selection color should be drawn.
   */
  protected Color borderSelectionColor;

  /**
   * Icon to use when editing.
   */
  protected transient Icon editingIcon;

  /**
   * Font to paint with, null indicates font of renderer is to be used.
   */
  protected Font font;

  /**
   * Constructs a DefaultTreeCellEditor object for a JTree using the 
   * specified renderer and a default editor. (Use this constructor 
   * for normal editing.)
   * 
   * @param tree - a JTree object
   * @param renderer - a DefaultTreeCellRenderer object
   */
  public DefaultTreeCellEditor(JTree tree, DefaultTreeCellRenderer renderer)
  {
    this.tree = tree;
    this.renderer = renderer;
    // FIXME: Not fully implemented.
  }

  /**
   * Constructs a DefaultTreeCellEditor  object for a JTree using the specified 
   * renderer and the specified editor. (Use this constructor 
   * for specialized editing.)
   * 
   * @param tree - a JTree object
   * @param renderer - a DefaultTreeCellRenderer object
   * @param editor - a TreeCellEditor object
   */
  public DefaultTreeCellEditor(JTree tree, DefaultTreeCellRenderer renderer,
                               TreeCellEditor editor)
  {
    this.tree = tree;
    this.renderer = renderer;
    this.realEditor = editor;
    // FIXME: Not fully implemented.
  }

  /**
   * writeObject
   * @param value0 TODO
   * @exception IOException TODO
   */
  private void writeObject(ObjectOutputStream value0) throws IOException
  {
    // TODO
  }

  /**
   * readObject
   * @param value0 TODO
   * @exception IOException TODO
   * @exception ClassNotFoundException TODO
   */
  private void readObject(ObjectInputStream value0)
    throws IOException, ClassNotFoundException
  {
    // TODO
  }

  /**
   * Sets the color to use for the border.
   * @param newColor - the new border color
   */
  public void setBorderSelectionColor(Color newColor)
  {
    this.borderSelectionColor = newColor;
  }

  /**
   * Returns the color the border is drawn.
   * @return Color
   */
  public Color getBorderSelectionColor()
  {
    return borderSelectionColor;
  }

  /**
   * Sets the font to edit with. null indicates the renderers 
   * font should be used. This will NOT override any font you have 
   * set in the editor the receiver was instantied with. If null for 
   * an editor was passed in, a default editor will be created that 
   * will pick up this font.
   * 
   * @param font - the editing Font
   */
  public void setFont(Font font)
  {
    if (font != null)
      this.font = font;
    else
      this.font = renderer.getFont();
  }

  /**
   * Gets the font used for editing.
   * 
   * @return the editing font
   */
  public Font getFont()
  {
    return font;
  }

  /**
   * Configures the editor. Passed onto the realEditor.
   * 
   * @param tree - the JTree that is asking the editor to edit; this parameter can be null
   * @param value - the value of the cell to be edited
   * @param isSelected - true is the cell is to be renderer with selection highlighting
   * @param expanded - true if the node is expanded
   * @param leaf - true if the node is a leaf node
   * @param row - the row index of the node being edited
   * 
   * @return the component for editing
   */
  public Component getTreeCellEditorComponent(JTree tree, Object value,
                                              boolean isSelected, boolean expanded,
                                              boolean leaf, int row)
  {
    return null; // TODO
  }

  /**
   * Returns the value currently being edited.
   * 
   * @return the value currently being edited
   */
  public Object getCellEditorValue()
  {
    return null; // TODO
  }

  /**
   * If the realEditor returns true to this message, prepareForEditing  
   * is messaged and true is returned.
   * 
   * @param event - the event the editor should use to consider whether to begin editing or not
   * @return true if editing can be started
   */
  public boolean isCellEditable(EventObject event)
  {
    return false; // TODO
  }

  /**
   * Messages the realEditor for the return value.
   * 
   * @param event - the event the editor should use to start editing
   * @return true if the editor would like the editing cell to be 
   * selected; otherwise returns false
   */
  public boolean shouldSelectCell(EventObject event)
  {
    return false; // TODO
  }

  /**
   * If the realEditor will allow editing to stop, the realEditor
   * is removed and true is returned, otherwise false is returned.
   * @return     true if editing was stopped; false otherwise
   */
  public boolean stopCellEditing()
  {
    return false; // TODO
  }

  /**
   * Messages cancelCellEditing to the realEditor and removes it
   * from this instance.
   */
  public void cancelCellEditing()
  {
    // TODO
  }

  /**
   * Adds a <code>CellEditorListener</code> object to this editor.
   *
   * @param listener the listener to add
   */
  public void addCellEditorListener(CellEditorListener listener)
  {
    listenerList.add(CellEditorListener.class, listener);
  }

  /**
   * Removes a <code>CellEditorListener</code> object.
   *
   * @param listener the listener to remove
   */
  public void removeCellEditorListener(CellEditorListener listener)
  {
    listenerList.remove(CellEditorListener.class, listener);
  }

  /**
   * Returns all added <code>CellEditorListener</code> objects to this editor.
   *
   * @return an array of listeners
   *
   * @since 1.4
   */
  public CellEditorListener[] getCellEditorListeners()
  {
    return (CellEditorListener[]) listenerList.getListeners(CellEditorListener.class);
  }

  /**
   * Resets lastPath.
   * 
   * @param e - the event that characterizes the change.
   */
  public void valueChanged(TreeSelectionEvent e)
  {
    // TODO
  }

  /**
   * Messaged when the timer fires, this will start the editing session.
   * 
   * @param @param e - the event that characterizes the action.
   */
  public void actionPerformed(ActionEvent e)
  {
    // TODO
  }

  /**
   * Sets the tree currently editing for. This is needed to add a 
   * selection listener.
   * 
   * @param newTree - the new tree to be edited
   */
  protected void setTree(JTree newTree)
  {
    tree = newTree;
  }

  /**
   * Returns true if event is a MouseEvent  and the click count is 1.
   * 
   * @param event - the event being studied
   * @return true if editing should start
   */
  protected boolean shouldStartEditingTimer(EventObject event)
  {
    return false; // TODO
  }

  /**
   * Starts the editing timer.
   */
  protected void startEditingTimer()
  {
    // TODO
  }

  /**
   * Returns true if event is null, or it is a MouseEvent with 
   * a click count > 2 and inHitRegion returns true.
   * 
   * @param event - the event being studied
   * @return true if event is null, or it is a MouseEvent with 
   * a click count > 2 and inHitRegion returns true 
   */
  protected boolean canEditImmediately(EventObject value0)
  {
    return false; // TODO
  }

  /**
   * Returns true if the passed in location is a valid mouse location 
   * to start editing from. This is implemented to return false if x is
   * less than or equal to the width of the icon and icon 
   * gap displayed by the renderer. In other words this returns true if 
   * the user clicks over the text part displayed by the renderer, and 
   * false otherwise.
   * 
   * @param x - the x-coordinate of the point
   * @param y - the y-coordinate of the point
   * 
   * @return true if the passed in location is a valid mouse location
   */
  protected boolean inHitRegion(int x, int y)
  {
    return false; // TODO
  }

  /**
   * determineOffset
   * @param tree -
   * @param value - 
   * @param isSelected - 
   * @param expanded - 
   * @param leaf - 
   * @param row - 
   */
  protected void determineOffset(JTree tree, Object value, boolean isSelected,
                                 boolean expanded, boolean leaf, int row)
  {
    // TODO
  }

  /**
   * Invoked just before editing is to start. Will add the 
   * editingComponent to the editingContainer.
   */
  protected void prepareForEditing()
  {
    // TODO
  }

  /**
   * Creates the container to manage placement of editingComponent.
   * 
   * @return the container to manage the placement of the editingComponent.
   */
  protected Container createContainer()
  {
    return null; // TODO
  }

  /**
   * This is invoked if a TreeCellEditor is not supplied in the constructor. 
   * It returns a TextField editor.
   * 
   * @return a new TextField editor
   */
  protected TreeCellEditor createTreeCellEditor()
  {
    return null; // TODO
  }
}
