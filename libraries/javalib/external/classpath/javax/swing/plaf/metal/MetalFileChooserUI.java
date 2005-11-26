/* MetalFileChooserUI.java --
   Copyright (C) 2005 Free Software Foundation, Inc.

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


package javax.swing.plaf.metal;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.LayoutManager;
import java.awt.Rectangle;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import java.io.File;

import javax.swing.AbstractAction;
import javax.swing.AbstractListModel;
import javax.swing.ActionMap;
import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.ComboBoxModel;
import javax.swing.DefaultListCellRenderer;
import javax.swing.Icon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.ListSelectionModel;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.filechooser.FileFilter;
import javax.swing.filechooser.FileSystemView;
import javax.swing.filechooser.FileView;
import javax.swing.plaf.ComponentUI;
import javax.swing.plaf.basic.BasicFileChooserUI;

import java.util.List;


/**
 * A UI delegate for the {@link JFileChooser} component.  This class is only
 * partially implemented and is not usable yet.
 */
public class MetalFileChooserUI 
  extends BasicFileChooserUI
{
  
  /**
   * A property change listener.
   */
  class MetalFileChooserPropertyChangeListener 
    implements PropertyChangeListener
  {
    /**
     * Default constructor.
     */
    public MetalFileChooserPropertyChangeListener()
    {
    }
    
    /**
     * Handles a property change event.
     * 
     * @param e  the event.
     */
    public void propertyChange(PropertyChangeEvent e)
    {
      JFileChooser filechooser = getFileChooser();
      
      String n = e.getPropertyName();
      if (n.equals(JFileChooser.MULTI_SELECTION_ENABLED_CHANGED_PROPERTY))
        {
          if (filechooser.isMultiSelectionEnabled())
            fileList.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
          else
            fileList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        }
      else if (n.equals(JFileChooser.SELECTED_FILE_CHANGED_PROPERTY))
        {
          File file = filechooser.getSelectedFile();
          if (file == null)
            setFileName(null);
          else
            setFileName(file.getName());
          int index = -1;
          index = getModel().indexOf(file);
          if (index >= 0)
            {
              fileList.setSelectedIndex(index);
              fileList.ensureIndexIsVisible(index);
              fileList.revalidate();
              fileList.repaint();
            }
        }
      
      else if (n.equals(JFileChooser.DIRECTORY_CHANGED_PROPERTY))
        {
          fileList.clearSelection();
          fileList.revalidate();
          fileList.repaint();
          setDirectorySelected(false);
          File currentDirectory = filechooser.getCurrentDirectory();
          setDirectory(currentDirectory);
          boolean hasParent = (currentDirectory.getParentFile() != null);
          getChangeToParentDirectoryAction().setEnabled(hasParent);
        }
      
      else if (n.equals(JFileChooser.CHOOSABLE_FILE_FILTER_CHANGED_PROPERTY))
        {
          filterModel.propertyChange(e);
        }
      else if (n.equals(JFileChooser.FILE_FILTER_CHANGED_PROPERTY))
        {
          filterModel.propertyChange(e);
        }
      else if (n.equals(JFileChooser.DIALOG_TYPE_CHANGED_PROPERTY)
                 || n.equals(JFileChooser.DIALOG_TITLE_CHANGED_PROPERTY))
        {
          Window owner = SwingUtilities.windowForComponent(filechooser);
          if (owner instanceof JDialog)
            ((JDialog) owner).setTitle(getDialogTitle(filechooser));
          approveButton.setText(getApproveButtonText(filechooser));
          approveButton.setToolTipText(
                  getApproveButtonToolTipText(filechooser));
          approveButton.setMnemonic(getApproveButtonMnemonic(filechooser));
        }
      
      else if (n.equals(JFileChooser.APPROVE_BUTTON_TEXT_CHANGED_PROPERTY))
        approveButton.setText(getApproveButtonText(filechooser));
      
      else if (n.equals(
              JFileChooser.APPROVE_BUTTON_TOOL_TIP_TEXT_CHANGED_PROPERTY))
        approveButton.setToolTipText(getApproveButtonToolTipText(filechooser));
      
      else if (n.equals(JFileChooser.APPROVE_BUTTON_MNEMONIC_CHANGED_PROPERTY))
        approveButton.setMnemonic(getApproveButtonMnemonic(filechooser));

      else if (n.equals(
              JFileChooser.CONTROL_BUTTONS_ARE_SHOWN_CHANGED_PROPERTY))
        {
          if (filechooser.getControlButtonsAreShown())
            {
              topPanel.add(controls, BorderLayout.EAST);
            }
          else
            topPanel.remove(controls);
          topPanel.revalidate();
          topPanel.repaint();
          topPanel.doLayout();
        }
      
      else if (n.equals(
              JFileChooser.ACCEPT_ALL_FILE_FILTER_USED_CHANGED_PROPERTY))
        {
          if (filechooser.isAcceptAllFileFilterUsed())
            filechooser.addChoosableFileFilter(
                    getAcceptAllFileFilter(filechooser));
          else
            filechooser.removeChoosableFileFilter(
                    getAcceptAllFileFilter(filechooser));
        }
      
      else if (n.equals(JFileChooser.ACCESSORY_CHANGED_PROPERTY))
        {
          JComponent old = (JComponent) e.getOldValue();
          if (old != null)
            getAccessoryPanel().remove(old);
          JComponent newval = (JComponent) e.getNewValue();
          if (newval != null)
            getAccessoryPanel().add(newval);
        }
      
      if (n.equals(JFileChooser.DIRECTORY_CHANGED_PROPERTY)
          || n.equals(JFileChooser.FILE_FILTER_CHANGED_PROPERTY)
          || n.equals(JFileChooser.FILE_HIDING_CHANGED_PROPERTY))
        rescanCurrentDirectory(filechooser);
      
      filechooser.revalidate();
      filechooser.repaint();
    }
  };
  
  /** 
   * A combo box model containing the selected directory and all its parent
   * directories.
   */
  protected class DirectoryComboBoxModel
    extends AbstractListModel
    implements ComboBoxModel
  {
    /** Storage for the items in the model. */
    private List items;
    
    /** The index of the selected item. */
    private int selectedIndex;
    
    /**
     * Creates a new model.
     */
    public DirectoryComboBoxModel() 
    {
      items = new java.util.ArrayList();
      selectedIndex = -1;
    }
    
    /**
     * Returns the number of items in the model.
     * 
     * @return The number of items in the model.
     */
    public int getSize()
    {
      return items.size();
    }
    
    /**
     * Returns the item at the specified index.
     * 
     * @param index  the item index.
     * 
     * @return The item.
     */
    public Object getElementAt(int index)
    {
      return items.get(index);
    }
    
    /**
     * Returns the depth of the item at the given <code>index</code>.
     * 
     * @param index  the item index.
     * 
     * @return The depth.
     */
    public int getDepth(int index)
    {
      return Math.max(index, 0);
    }

    /**
     * Returns the selected item, or <code>null</code> if no item is selected.
     * 
     * @return The selected item, or <code>null</code>.
     */
    public Object getSelectedItem()
    {
      if (selectedIndex >= 0) 
        return items.get(selectedIndex);
      else
        return null;
    }
    
    /**
     * Sets the selected item.  This clears all the directories from the
     * existing list, and repopulates it with the new selected directory
     * and all its parent directories.
     * 
     * @param selectedDirectory  the selected directory.
     */
    public void setSelectedItem(Object selectedDirectory)
    {
      items.clear();
      FileSystemView fsv = getFileChooser().getFileSystemView();
      File parent = (File) selectedDirectory;
      while (parent != null)
        {
          items.add(0, parent);
          parent = fsv.getParentDirectory(parent);
        }
      selectedIndex = items.indexOf(selectedDirectory);
      fireContentsChanged(this, 0, items.size() - 1);
    }
    
  }

  /**
   * Handles changes to the selection in the directory combo box.
   */
  protected class DirectoryComboBoxAction
    extends AbstractAction
  {
    /**
     * Creates a new action.
     */
    protected DirectoryComboBoxAction()
    {
      // Nothing to do here.
    }
    
    /**
     * Handles the action event.
     * 
     * @param e  the event.
     */
    public void actionPerformed(ActionEvent e)
    {
      JFileChooser fc = getFileChooser();
      fc.setCurrentDirectory((File) directoryModel.getSelectedItem());
    }
  }

  /**
   * A renderer for the items in the directory combo box.
   */
  class DirectoryComboBoxRenderer
    extends DefaultListCellRenderer
  {
    /**
     * Creates a new renderer.
     */
    public DirectoryComboBoxRenderer(JFileChooser fc)
    { 
    }
    
    /**
     * Returns a component that can be used to paint the given value within 
     * the list.
     * 
     * @param list  the list.
     * @param value  the value (a {@link File}).
     * @param index  the item index.
     * @param isSelected  is the item selected?
     * @param cellHasFocus  does the list cell have focus?
     * 
     * @return The list cell renderer.
     */
    public Component getListCellRendererComponent(JList list, Object value,
        int index, boolean isSelected, boolean cellHasFocus)
    {
      FileView fileView = getFileView(getFileChooser());
      File file = (File) value;
      setIcon(fileView.getIcon(file));
      setText(fileView.getName(file));
      
      if (isSelected)
        {
          setBackground(list.getSelectionBackground());
          setForeground(list.getSelectionForeground());
        }
      else
        {
          setBackground(list.getBackground());
          setForeground(list.getForeground());
        }

      setEnabled(list.isEnabled());
      setFont(list.getFont());
      return this;
    }
  }

  /**
   * A renderer for the files and directories in the file chooser.
   */
  protected class FileRenderer
    extends DefaultListCellRenderer
  {
    
    /**
     * Creates a new renderer.
     */
    protected FileRenderer()
    {
      // Nothing to do here.
    }
    
    /**
     * Returns a component that can render the specified value.
     * 
     * @param list  the list.
     * @param value  the value (a {@link File}).
     * @param index  the index.
     * @param isSelected  is the item selected?
     * @param cellHasFocus  does the item have the focus?
     * 
     * @return The renderer.
     */
    public Component getListCellRendererComponent(JList list, Object value,
        int index, boolean isSelected, boolean cellHasFocus)
    {
      FileView v = getFileView(getFileChooser());
      File f = (File) value;
      setText(v.getName(f));
      setIcon(v.getIcon(f));
      setOpaque(true);
      if (isSelected)
        {
          setBackground(list.getSelectionBackground());
          setForeground(list.getSelectionForeground());
        }
      else
        {
          setBackground(list.getBackground());
          setForeground(list.getForeground());
        }

      setEnabled(list.isEnabled());
      setFont(list.getFont());

      if (cellHasFocus)
        setBorder(UIManager.getBorder("List.focusCellHighlightBorder"));
      else
        setBorder(noFocusBorder);
      return this;
    }
  }

  /**
   * A combo box model for the file selection filters.
   */
  protected class FilterComboBoxModel
    extends AbstractListModel
    implements ComboBoxModel, PropertyChangeListener
  {

    /** Storage for the filters in the model. */
    protected FileFilter[] filters;

    /** The index of the selected file filter. */
    private Object selected;
    
    /**
     * Creates a new model.
     */
    protected FilterComboBoxModel()
    {
      filters = new FileFilter[1];
      filters[0] = getAcceptAllFileFilter(getFileChooser());
      selected = filters[0];
    }
    
    /**
     * Handles property changes.
     * 
     * @param e  the property change event.
     */
    public void propertyChange(PropertyChangeEvent e)
    {
      if (e.getPropertyName().equals(JFileChooser.FILE_FILTER_CHANGED_PROPERTY))
        {
          JFileChooser fc = getFileChooser();
          FileFilter[] choosableFilters = fc.getChoosableFileFilters();
          filters = choosableFilters;
          fireContentsChanged(this, 0, filters.length);
          selected = e.getNewValue();
          fireContentsChanged(this, -1, -1);
        }
      else if (e.getPropertyName().equals(
              JFileChooser.CHOOSABLE_FILE_FILTER_CHANGED_PROPERTY))
        {
          // repopulate list
          JFileChooser fc = getFileChooser();
          FileFilter[] choosableFilters = fc.getChoosableFileFilters();
          filters = choosableFilters;
          fireContentsChanged(this, 0, filters.length);
        }
    }
    
    /**
     * Sets the selected filter.
     * 
     * @param filter  the filter (<code>null</code> ignored).
     */
    public void setSelectedItem(Object filter)
    {
      if (filter != null)
      {
          selected = filter;
          fireContentsChanged(this, -1, -1);
      }
    }
    
    /**
     * Returns the selected file filter.
     * 
     * @return The selected file filter.
     */
    public Object getSelectedItem()
    {
      return selected;
    }
    
    /**
     * Returns the number of items in the model.
     * 
     * @return The number of items in the model.
     */
    public int getSize()
    {
      return filters.length;
    }
    
    /**
     * Returns the item at the specified index.
     * 
     * @param index  the item index.
     * 
     * @return The item at the specified index.
     */
    public Object getElementAt(int index)
    {
      return filters[index];
    }
    
  }

  /**
   * A renderer for the items in the file filter combo box.
   */
  public class FilterComboBoxRenderer
    extends DefaultListCellRenderer
  {
    /**
     * Creates a new renderer.
     */
    public FilterComboBoxRenderer()
    {
      // Nothing to do here.
    }
    
    /**
     * Returns a component that can be used to paint the given value within 
     * the list.
     * 
     * @param list  the list.
     * @param value  the value (a {@link FileFilter}).
     * @param index  the item index.
     * @param isSelected  is the item selected?
     * @param cellHasFocus  does the list cell have focus?
     * 
     * @return This component as the renderer.
     */
    public Component getListCellRendererComponent(JList list, Object value,
        int index, boolean isSelected, boolean cellHasFocus)
    {
      super.getListCellRendererComponent(list, value, index, isSelected, 
                                         cellHasFocus);
      FileFilter filter = (FileFilter) value;
      setText(filter.getDescription());
      return this;
    }
  }

  /**
   * A listener for selection events in the file list.
   * 
   * @see #createListSelectionListener(JFileChooser)
   */
  class MetalFileChooserSelectionListener 
    implements ListSelectionListener
  {
    /**
     * Creates a new <code>SelectionListener</code> object.
     */
    protected MetalFileChooserSelectionListener()
    {
      // Do nothing here.
    }

    /**
     * Makes changes to different properties when
     * a value has changed in the filechooser's selection.
     *
     * @param e - the list selection event that occured.
     */
    public void valueChanged(ListSelectionEvent e)
    {
      File f = (File) fileList.getSelectedValue();
      if (f == null)
        return;
      JFileChooser filechooser = getFileChooser();
      if (! filechooser.isTraversable(f))
        filechooser.setSelectedFile(f);
      else
        filechooser.setSelectedFile(null);
    }
  }

  /**
   * A mouse listener for the {@link JFileChooser}.
   * This listener is used for editing filenames.
   */
  protected class SingleClickListener
    extends MouseAdapter
  {
    
    /** Stores instance of the list */
    JList list;
    
    /** 
     * Stores the current file that is being edited.
     * It is null if nothing is currently being edited.
     */
    File editFile;
    
    /** The current file chooser. */
    JFileChooser fc;
    
    /** The last file selected. */
    Object lastSelected;
    
    /** The textfield used for editing. */
    JTextField editField;
    
    /**
     * Creates a new listener.
     * 
     * @param list  the directory/file list.
     */
    public SingleClickListener(JList list)
    {
      this.list = list;
      editFile = null;
      fc = getFileChooser();
      lastSelected = null;
    }
    
    /**
     * Receives notification of a mouse click event.
     * 
     * @param e  the event.
     */
    public void mouseClicked(MouseEvent e)
    {
      if (e.getClickCount() == 1)
        {
          int index = list.locationToIndex(e.getPoint());
          File[] sf = fc.getSelectedFiles();
          if ((!fc.isMultiSelectionEnabled() || (sf != null && sf.length <= 1))
              && index >= 0 && editFile == null && list.isSelectedIndex(index))
            {
              Object tmp = list.getModel().getElementAt(index);
              if (lastSelected != null && lastSelected.equals(tmp))
                editFile(index);
              lastSelected = tmp;
            }
          else if (editFile != null)
            {
              completeEditing();
              editFile = null;
              lastSelected = null;
            }
        }
    }
    
    /**
     * Sets up the text editor for the current file.
     * 
     * @param index -
     *          the current index of the item in the list to be edited.
     */
    private void editFile(int index)
    {
      list.ensureIndexIsVisible(index);
      editFile = (File) list.getModel().getElementAt(index);
      if (editFile.canWrite())
        {
          Rectangle bounds = list.getCellBounds(index, index);
          Icon icon = getFileView(fc).getIcon(editFile);
          editField = new JTextField(editFile.getName());
          // FIXME: add action listener for typing
          // FIXME: painting for textfield is messed up when typing    
          list.add(editField);
          editField.requestFocus();
          editField.selectAll();
          
          if (icon != null)
            bounds.x += icon.getIconWidth() + 4;
          editField.setBounds(bounds);
        }
      else
        {
          editField = null;
          editFile = null;
          lastSelected = null;
        }
    }
    
    /** 
     * Completes the editing.
     */
    private void completeEditing()
    {
      if (editField != null)
        {
          String text = editField.getText();
          if (text != null && !text.equals(""))
            editFile.renameTo(new File(text));
          list.remove(editField);
          list.revalidate();
          list.repaint();
        }
    }
  }

  /** The text for a label describing the directory combo box. */
  private String directoryLabel;
  
  private JComboBox directoryComboBox;
  
  /** The model for the directory combo box. */
  DirectoryComboBoxModel directoryModel;
  
  /** The text for a label describing the file text field. */
  private String fileLabel;
  
  /** The file name text field. */
  private JTextField fileTextField;
  
  /** The text for a label describing the filter combo box. */
  private String filterLabel;

  /** 
   * The top panel (contains the directory combo box and the control buttons). 
   */
  private JPanel topPanel;
  
  /** A panel containing the control buttons ('up', 'home' etc.). */
  private JPanel controls;

  /** 
   * The panel that contains the filename field and the filter combobox. 
   */
  private JPanel bottomPanel;

  /** 
   * The panel that contains the 'Open' (or 'Save') and 'Cancel' buttons. 
   */
  private JPanel buttonPanel;
  
  private JButton approveButton;
  
  /** The file list. */
  private JList fileList;
  
  /** The panel containing the file list. */
  private JPanel fileListPanel;
  
  /** The filter combo box model. */
  private FilterComboBoxModel filterModel;

  /** The action map. */
  private ActionMap actionMap;
  
  /**
   * A factory method that returns a UI delegate for the specified
   * component.
   * 
   * @param c  the component (which should be a {@link JFileChooser}).
   */
  public static ComponentUI createUI(JComponent c)
  {
    JFileChooser chooser = (JFileChooser) c;
    return new MetalFileChooserUI(chooser);
  }

  /**
   * Creates a new instance of this UI delegate.
   * 
   * @param filechooser  the file chooser component.
   */
  public MetalFileChooserUI(JFileChooser filechooser)
  {
    super(filechooser);
    bottomPanel = new JPanel(new GridLayout(3, 2));
    buttonPanel = new JPanel();
  }

  public void installUI(JComponent c)
  {
    super.installUI(c);
    actionMap = createActionMap();
  }
  
  public void uninstallUI(JComponent c)
  {
    super.uninstallUI(c);
    actionMap = null;
  }
  
  /**
   * Installs the sub-components of the file chooser.
   * 
   * @param fc  the file chooser component.
   */
  public void installComponents(JFileChooser fc)
  {
    fc.setLayout(new BorderLayout());
    topPanel = new JPanel(new BorderLayout());
    topPanel.add(new JLabel(directoryLabel), BorderLayout.WEST);
    this.controls = new JPanel();
    addControlButtons();
    
    JPanel dirPanel = new JPanel(new VerticalMidLayout());
    dirPanel.setBorder(BorderFactory.createEmptyBorder(0, 5, 0, 0));
    directoryModel = createDirectoryComboBoxModel(fc);
    directoryComboBox = new JComboBox(directoryModel);
    directoryComboBox.setRenderer(createDirectoryComboBoxRenderer(fc));
    dirPanel.add(directoryComboBox);
    topPanel.add(dirPanel);
    topPanel.add(controls, BorderLayout.EAST);
    fc.add(topPanel, BorderLayout.NORTH);
    fileListPanel = createList(fc);
    fc.add(fileListPanel);
    JPanel bottomPanel = getBottomPanel();
    filterModel = createFilterComboBoxModel();
    JComboBox fileFilterCombo = new JComboBox(filterModel);
    fileFilterCombo.setRenderer(createFilterComboBoxRenderer());
    
    fileTextField = new JTextField();
    JPanel fileNamePanel = new JPanel(new VerticalMidLayout());
    fileNamePanel.add(fileTextField);
    JPanel row1 = new JPanel(new BorderLayout());
    row1.add(new JLabel(this.fileLabel), BorderLayout.WEST);
    row1.add(fileNamePanel);
    bottomPanel.add(row1);
    
    JPanel filterPanel = new JPanel(new VerticalMidLayout());
    filterPanel.add(fileFilterCombo);    
    JPanel row2 = new JPanel(new BorderLayout());
    row2.add(new JLabel(this.filterLabel), BorderLayout.WEST);
    row2.add(filterPanel);
    bottomPanel.add(row2);
    JPanel buttonPanel = new JPanel(new ButtonLayout());
    buttonPanel.setBorder(BorderFactory.createEmptyBorder(4, 4, 4, 0));
    
    approveButton = new JButton(getApproveSelectionAction());
    approveButton.setText(getApproveButtonText(fc));
    approveButton.setToolTipText(getApproveButtonToolTipText(fc));
    approveButton.setMnemonic(getApproveButtonMnemonic(fc));
    buttonPanel.add(approveButton);
    
    JButton cancelButton = new JButton(getCancelSelectionAction());
    cancelButton.setText(cancelButtonText);
    cancelButton.setToolTipText(cancelButtonToolTipText);
    cancelButton.setMnemonic(cancelButtonMnemonic);
    buttonPanel.add(cancelButton);
    bottomPanel.add(buttonPanel);
    fc.add(bottomPanel, BorderLayout.SOUTH);
  }
  
  /**
   * Uninstalls the components added by 
   * {@link #installComponents(JFileChooser)}.
   * 
   * @param fc  the file chooser.
   */
  public void uninstallComponents(JFileChooser fc)
  {
    fc.remove(bottomPanel);
    bottomPanel = null;
    fc.remove(fileListPanel);
    fileListPanel = null;
    fc.remove(topPanel);
    topPanel = null;
    
    directoryModel = null;
    fileTextField = null;
    directoryComboBox = null;
  }
  
  /**
   * Returns the panel that contains the 'Open' (or 'Save') and 'Cancel' 
   * buttons.
   * 
   * @return The panel.
   */
  protected JPanel getButtonPanel()
  {
    return buttonPanel;    
  }
  
  /**
   * Creates and returns a new panel that will be used for the controls at
   * the bottom of the file chooser.
   * 
   * @return A new panel.
   */
  protected JPanel getBottomPanel()
  {
    if (bottomPanel == null)
      bottomPanel = new JPanel(new GridLayout(3, 2));
    return bottomPanel;
  }
  
  /**
   * Fetches localised strings for use by the labels and buttons on the
   * file chooser.
   * 
   * @param fc  the file chooser.
   */
  protected void installStrings(JFileChooser fc)
  { 
     super.installStrings(fc);
     directoryLabel = "Look In: ";
     fileLabel = "File Name: ";
     filterLabel = "Files of Type: ";
     
     this.cancelButtonMnemonic = 0;
     this.cancelButtonText = "Cancel";
     this.cancelButtonToolTipText = "Abort file chooser dialog";
     
     this.directoryOpenButtonMnemonic = 0;
     this.directoryOpenButtonText = "Open";
     this.directoryOpenButtonToolTipText = "Open selected directory";
     
     this.helpButtonMnemonic = 0;
     this.helpButtonText = "Help";
     this.helpButtonToolTipText = "Filechooser help";
     
     this.openButtonMnemonic = 0;
     this.openButtonText = "Open";
     this.openButtonToolTipText = "Open selected file";
     
     this.saveButtonMnemonic = 0;
     this.saveButtonText = "Save";
     this.saveButtonToolTipText = "Save selected file";
     
     this.updateButtonMnemonic = 0;
     this.updateButtonText = "Update";
     this.updateButtonToolTipText = "Update directory listing";   
  }
  
  /**
   * Installs the listeners required.
   * 
   * @param fc  the file chooser.
   */
  protected void installListeners(JFileChooser fc)
  {
    directoryComboBox.setAction(new DirectoryComboBoxAction());
    fileList.addListSelectionListener(createListSelectionListener(fc));
    fileList.addMouseListener(this.createDoubleClickListener(fc, fileList));
    fileList.addMouseListener(new SingleClickListener(fileList));
    fc.addPropertyChangeListener(filterModel);
    super.installListeners(fc);
  }
  
  protected void uninstallListeners(JFileChooser fc) 
  {
    super.uninstallListeners(fc);
    fc.removePropertyChangeListener(filterModel);
  }
  
  protected ActionMap getActionMap()
  {
    if (actionMap == null)
      actionMap = createActionMap();
    return actionMap;
  }
  
  /**
   * Creates and returns an action map.
   * 
   * @return The action map.
   */
  protected ActionMap createActionMap()
  {
    ActionMap map = new ActionMap();
    map.put("approveSelection", getApproveSelectionAction());
    map.put("cancelSelection", null);  // FIXME: implement this one
    map.put("Go Up", getChangeToParentDirectoryAction());
    return map;
  }

  /**
   * Creates a panel containing a list of files.
   * 
   * @param fc  the file chooser.
   * 
   * @return A panel.
   */
  protected JPanel createList(JFileChooser fc)
  {
    JPanel panel = new JPanel(new BorderLayout());
    fileList = new JList(getModel());
    fileList.setLayoutOrientation(JList.VERTICAL_WRAP);
    fileList.setVisibleRowCount(0);
    fileList.setCellRenderer(new FileRenderer());
    panel.add(new JScrollPane(fileList));
    return panel;    
  }
  
  /**
   * Creates a panel containing a table within a scroll pane.
   * 
   * @param fc  the file chooser.
   * 
   * @return The details view.
   */
  protected JPanel createDetailsView(JFileChooser fc)
  {
    // FIXME: implement this.  The details view is a panel containing a table
    // inside a JScrollPane - it gets displayed when the user clicks on the
    // "details" button.
    return new JPanel();
  }
  
  /**
   * Creates a listener that monitors selections in the directory/file list
   * and keeps the {@link JFileChooser} component up to date.
   * 
   * @param fc  the file chooser.
   * 
   * @return The listener.
   * 
   * @see #installListeners(JFileChooser)
   */
  public ListSelectionListener createListSelectionListener(JFileChooser fc)
  {
    return new MetalFileChooserSelectionListener();
  }
  
  /**
   * Returns the preferred size for the file chooser component.
   * 
   * @return The preferred size.
   */
  public Dimension getPreferredSize(JComponent c)
  {
    // FIXME: not likely to be a fixed value
    return new Dimension(500, 326);
  }
  
  /**
   * Returns the minimum size for the file chooser component.
   * 
   * @return The minimum size.
   */
  public Dimension getMinimumSize(JComponent c)
  {
    // FIXME: not likely to be a fixed value
    return new Dimension(506, 326);      
  }
  
  /**
   * Returns the maximum size for the file chooser component.
   * 
   * @return The maximum size.
   */
  public Dimension getMaximumSize(JComponent c)
  {
    return new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE);
  }
  
  /**
   * Creates a property change listener that monitors the {@link JFileChooser}
   * for property change events and updates the component display accordingly.
   * 
   * @param fc  the file chooser.
   * 
   * @return The property change listener.
   * 
   * @see #installListeners(JFileChooser)
   */
  public PropertyChangeListener createPropertyChangeListener(JFileChooser fc)
  {
    return new MetalFileChooserPropertyChangeListener();
  }

  /**
   * Creates and returns a new instance of {@link DirectoryComboBoxModel}.
   * 
   * @return A new instance of {@link DirectoryComboBoxModel}.
   */
  protected MetalFileChooserUI.DirectoryComboBoxModel 
      createDirectoryComboBoxModel(JFileChooser fc)
  {
    return new DirectoryComboBoxModel();
  }

  /**
   * Creates a new instance of the renderer used in the directory
   * combo box.
   * 
   * @param fc  the file chooser.
   * 
   * @return The renderer.
   */
  protected DirectoryComboBoxRenderer createDirectoryComboBoxRenderer(
          JFileChooser fc)
  {
    return new DirectoryComboBoxRenderer(fc);
  }

  /**
   * Creates and returns a new instance of {@link FilterComboBoxModel}.
   * 
   * @return A new instance of {@link FilterComboBoxModel}.
   */
  protected FilterComboBoxModel createFilterComboBoxModel()
  {
    return new FilterComboBoxModel();  
  }

  /**
   * Creates and returns a new instance of {@link FilterComboBoxRenderer}.
   * 
   * @return A new instance of {@link FilterComboBoxRenderer}.
   */
  protected MetalFileChooserUI.FilterComboBoxRenderer 
      createFilterComboBoxRenderer()
  {
    return new FilterComboBoxRenderer(); 
  }

  /**
   * Adds the control buttons ('up', 'home' etc.) to the panel.
   */
  protected void addControlButtons()
  {
    JButton upButton = new JButton(getChangeToParentDirectoryAction());
    upButton.setText(null);
    upButton.setIcon(this.upFolderIcon);
    upButton.setMargin(new Insets(0, 0, 0, 0));
    controls.add(upButton);
    
    JButton homeButton = new JButton(getGoHomeAction());
    homeButton.setText(null);
    homeButton.setIcon(this.homeFolderIcon);
    homeButton.setMargin(new Insets(0, 0, 0, 0));
    controls.add(homeButton);
    
    JButton newFolderButton = new JButton(getNewFolderAction());
    newFolderButton.setText(null);
    newFolderButton.setIcon(this.newFolderIcon);
    newFolderButton.setMargin(new Insets(0, 0, 0, 0));
    controls.add(newFolderButton);
    
    JToggleButton listButton = new JToggleButton();
    listButton.setIcon(this.listViewIcon);
    listButton.setMargin(new Insets(0, 0, 0, 0));
    // FIXME: this button needs an action that handles a click
    controls.add(listButton);
    
    JToggleButton detailButton = new JToggleButton(this.detailsViewIcon);
    detailButton.setMargin(new Insets(0, 0, 0, 0));
    // FIXME: this button needs an action that handles a click
    controls.add(detailButton);

    ButtonGroup buttonGroup = new ButtonGroup();
    buttonGroup.add(listButton);
    buttonGroup.add(detailButton);
  }
  
  protected void removeControlButtons()
  {
    controls.removeAll();
  }
  
  public void ensureFileIsVisible(JFileChooser fc, File f)
  {
    // FIXME: do something here - probably this figures out whether the
    // list or table view is current, and forwards the request to the 
    // appropriate one...
    super.ensureFileIsVisible(fc, f);
  }
  
  public void rescanCurrentDirectory(JFileChooser fc)
  {
    // FIXME: this will need to take into account whether the list view or
    // the table view is current
    directoryModel.setSelectedItem(fc.getCurrentDirectory());
    getModel().validateFileCache();
    fileList.revalidate();
  }
  
  /**
   * Returns the file name in the text field.
   * 
   * @return The file name.
   */
  public String getFileName()
  {
    String result = null;
    if (fileTextField != null) 
      result = fileTextField.getText();
    return result;
  }
  
  /**
   * Sets the file name in the text field.
   * 
   * @param filename  the file name.
   */
  public void setFileName(String filename)
  {
    fileTextField.setText(filename);
  }

  protected void setDirectorySelected(boolean directorySelected)
  {
    // FIXME: do something here
    super.setDirectorySelected(directorySelected);
  }
  
  public String getDirectoryName()
  {
    // FIXME: do something here
    return super.getDirectoryName();      
  }

  public void setDirectoryName(String dirname)
  {
    // FIXME: do something here
    super.setDirectoryName(dirname);    
  }
  
  public void valueChanged(ListSelectionEvent e)
  {
    // FIXME: implement
  }
  
  /**
   * Returns the approve button.
   * 
   * @return The approve button.
   */
  protected JButton getApproveButton(JFileChooser fc)
  {
    return approveButton;
  }

  /**
   * A layout manager that is used to arrange the subcomponents of the
   * {@link JFileChooser}.
   */
  class VerticalMidLayout implements LayoutManager
  {
    /**
     * Performs the layout.
     * 
     * @param parent  the container.
     */
    public void layoutContainer(Container parent) 
    {
      int count = parent.getComponentCount();
      if (count > 0)
        {
          Insets insets = parent.getInsets();
          Component c = parent.getComponent(0);
          Dimension prefSize = c.getPreferredSize();
          int h = parent.getHeight() - insets.top - insets.bottom;
          int adj = Math.max(0, (h - prefSize.height) / 2);
          c.setBounds(insets.left, insets.top + adj, parent.getWidth() 
              - insets.left - insets.right, 
              (int) Math.min(prefSize.getHeight(), h));
        }
    }
    
    /**
     * Returns the minimum layout size.
     * 
     * @param parent  the container.
     * 
     * @return The minimum layout size.
     */
    public Dimension minimumLayoutSize(Container parent) 
    {
      return preferredLayoutSize(parent);
    }
    
    /**
     * Returns the preferred layout size.
     * 
     * @param parent  the container.
     * 
     * @return The preferred layout size.
     */
    public Dimension preferredLayoutSize(Container parent) 
    {
      if (parent.getComponentCount() > 0)
        {
          return parent.getComponent(0).getPreferredSize();
        }
      else return null;
    }
    
    /**
     * This layout manager does not need to track components, so this 
     * method does nothing.
     * 
     * @param name  the name the component is associated with.
     * @param component  the component.
     */
    public void addLayoutComponent(String name, Component component) 
    {
      // do nothing
    }
    
    /**
     * This layout manager does not need to track components, so this 
     * method does nothing.
     * 
     * @param component  the component.
     */
    public void removeLayoutComponent(Component component) {
      // do nothing
    }
  }

  /**
   * A layout manager that is used to arrange buttons for the
   * {@link JFileChooser}.
   */
  class ButtonLayout implements LayoutManager
  {
    static final int GAP = 4;
      
    /**
     * Performs the layout.
     * 
     * @param parent  the container.
     */
    public void layoutContainer(Container parent) 
    {
      int count = parent.getComponentCount();
      if (count > 0)
        {
          // first find the widest button
          int maxW = 0;
          for (int i = 0; i < count; i++)
            {
              Component c = parent.getComponent(i);
              Dimension prefSize = c.getPreferredSize();
              maxW = Math.max(prefSize.width, maxW);
            }
  
          // then position the buttons
          Insets insets = parent.getInsets();
          int availableH = parent.getHeight() - insets.top - insets.bottom;
          int currentX = parent.getWidth() - insets.right;
          for (int i = count - 1; i >= 0; i--)
            {
              Component c = parent.getComponent(i);
              Dimension prefSize = c.getPreferredSize();      
              int adj = Math.max(0, (availableH - prefSize.height) / 2);
              currentX = currentX - prefSize.width;
              c.setBounds(currentX, insets.top + adj, prefSize.width, 
                  (int) Math.min(prefSize.getHeight(), availableH));
              currentX = currentX - GAP;
            }
        }
    }
    
    /**
     * Returns the minimum layout size.
     * 
     * @param parent  the container.
     * 
     * @return The minimum layout size.
     */
    public Dimension minimumLayoutSize(Container parent) 
    {
      return preferredLayoutSize(parent);
    }
    
    /**
     * Returns the preferred layout size.
     * 
     * @param parent  the container.
     * 
     * @return The preferred layout size.
     */
    public Dimension preferredLayoutSize(Container parent) 
    {
      Insets insets = parent.getInsets();
      int maxW = 0;
      int maxH = 0;
      int count = parent.getComponentCount();
      if (count > 0) 
        {
          for (int i = 0; i < count; i++)
            {
              Component c = parent.getComponent(i);
              Dimension d = c.getPreferredSize();
              maxW = Math.max(d.width, maxW);
              maxH = Math.max(d.height, maxH);
            }
        }
      return new Dimension(maxW * count + GAP * (count - 1) + insets.left 
              + insets.right, maxH + insets.top + insets.bottom);
    }
    
    /**
     * This layout manager does not need to track components, so this 
     * method does nothing.
     * 
     * @param name  the name the component is associated with.
     * @param component  the component.
     */
    public void addLayoutComponent(String name, Component component) 
    {
      // do nothing
    }
    
    /**
     * This layout manager does not need to track components, so this 
     * method does nothing.
     * 
     * @param component  the component.
     */
    public void removeLayoutComponent(Component component) {
      // do nothing
    }
  }

}
