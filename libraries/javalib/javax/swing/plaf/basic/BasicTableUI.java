package javax.swing.plaf.basic;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.MouseEvent;
import javax.swing.CellRendererPane;
import javax.swing.JComponent;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.event.MouseInputListener;
import javax.swing.plaf.ComponentUI;
import javax.swing.plaf.TableUI;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;
import javax.swing.UIDefaults;
import javax.swing.UIManager;


public class BasicTableUI
  extends TableUI
{

  public static ComponentUI createUI(JComponent comp) 
  {
    return new BasicTableUI();
  }

  protected FocusListener focusListener;  
  protected KeyListener keyListener;   
  protected MouseInputListener	mouseInputListener;   
  protected CellRendererPane rendererPane;   
  protected JTable table;

  class FocusHandler implements FocusListener
  {
    public void focusGained(FocusEvent e) 
    {
    }
    public void focusLost(FocusEvent e) 
    {
    }
  }

  class KeyHandler implements KeyListener
  {
    public void keyPressed(KeyEvent e) 
    {
    }
    public void keyReleased(KeyEvent e) 
    {
    }
    public void keyTyped(KeyEvent e) 
    {
    }
  }

  class MouseInputHandler implements MouseInputListener
  {
    Point begin, curr;

    private int getRowForPoint(Point p)
    {      
      int y0 = table.getLocation().y;
      int nrows = table.getRowCount();
      Dimension gap = table.getInterCellSpacing();
      int height = table.getRowHeight() + (gap == null ? 0 : gap.height);
      int y = p.y;
      for (int i = 0; i < nrows; ++i)
        {
          if (0 <= y && y < height)
            return i;
          y -= height;
        }
      return -1;
    }

    private int getColForPoint(Point p)
    {
      int x0 = table.getLocation().x;
      int ncols = table.getColumnCount();
      Dimension gap = table.getInterCellSpacing();
      TableColumnModel cols = table.getColumnModel();      
      int x = p.x;
      for (int i = 0; i < ncols; ++i)
        {
          int width = cols.getColumn(i).getWidth() + (gap == null ? 0 : gap.width);
          if (0 <= x && x < width)
            return i;
          x -= width;
        }
      return -1;
    }

    private void updateSelection()
    {
      if (table.getRowSelectionAllowed())
        {
          int lo_row = getRowForPoint(begin);
          int hi_row  = getRowForPoint(curr);
          ListSelectionModel rowModel = table.getSelectionModel();
          if (lo_row != -1 && hi_row != -1)
            rowModel.setSelectionInterval(lo_row, hi_row);
        }

      if (table.getColumnSelectionAllowed())
        {
          int lo_col = getColForPoint(begin);
          int hi_col = getColForPoint(curr);
          ListSelectionModel colModel = table.getColumnModel().getSelectionModel();
          if (lo_col != -1 && hi_col != -1)
            colModel.setSelectionInterval(lo_col, hi_col);
        }
    }

    public void mouseClicked(MouseEvent e) 
    {
    }
    public void mouseDragged(MouseEvent e) 
    {
      curr = new Point(e.getX(), e.getY());
      updateSelection();      
    }
    public void mouseEntered(MouseEvent e) 
    {
    }
    public void mouseExited(MouseEvent e) 
    {
    }
    public void mouseMoved(MouseEvent e) 
    {
    }
    public void mousePressed(MouseEvent e) 
    {
      begin = new Point(e.getX(), e.getY());
      curr = new Point(e.getX(), e.getY());
      updateSelection();
    }
    public void mouseReleased(MouseEvent e) 
    {
      begin = null;
      curr = null;
    }
  }

  protected FocusListener createFocusListener() 
  {
    return new FocusHandler();
  }
  protected KeyListener createKeyListener() 
  {
    return new KeyHandler();
  }
  protected MouseInputListener createMouseInputListener() 
  {
    return new MouseInputHandler();
  }

  public Dimension getMaximumSize(JComponent comp) 
  {
    return getPreferredSize(comp);
  }

  public Dimension getMinimumSize(JComponent comp) 
  {
    return getPreferredSize(comp);
  }

  public Dimension getPreferredSize(JComponent comp) 
  {
    int width = table.getColumnModel().getTotalColumnWidth();
    int height = table.getRowCount() * table.getRowHeight();
    return new Dimension(width, height);
  }

  protected void installDefaults() 
  {
    UIDefaults defaults = UIManager.getLookAndFeelDefaults();
    table.setFont(defaults.getFont("Table.font"));
    table.setGridColor(defaults.getColor("Table.gridColor"));
    table.setForeground(defaults.getColor("Table.foreground"));
    table.setBackground(defaults.getColor("Table.background"));
    table.setSelectionForeground(defaults.getColor("Table.selectionForeground"));
    table.setSelectionBackground(defaults.getColor("Table.selectionBackground"));
    table.setOpaque(true);
  }
  protected void installKeyboardActions() 
  {
  }

  protected void installListeners() 
  {
    table.addFocusListener(focusListener);  
    table.addKeyListener(keyListener);
    table.addMouseListener(mouseInputListener);    
  }

  protected void uninstallDefaults() 
  {
    table.setFont(null);
    table.setGridColor(null);
    table.setForeground(null);
    table.setBackground(null);
    table.setSelectionForeground(null);
    table.setSelectionBackground(null);
  }

  protected void uninstallKeyboardActions() 
  {
  }

  protected void uninstallListeners() 
  {
    table.removeFocusListener(focusListener);  
    table.removeKeyListener(keyListener);
    table.removeMouseListener(mouseInputListener);    
  }

  public void installUI(JComponent comp) 
  {
    table = (JTable)comp;
    focusListener = createFocusListener();  
    keyListener = createKeyListener();
    mouseInputListener = createMouseInputListener();
    installDefaults();
    installKeyboardActions();
    installListeners();
  }

  public void uninstallUI(JComponent c) 
  {
    uninstallListeners();
    uninstallKeyboardActions();
    uninstallDefaults();    
  }

  public void paint(Graphics gfx, JComponent ignored) 
  {
    int ncols = table.getColumnCount();
    int nrows = table.getRowCount();
    if (nrows == 0 || ncols == 0)
      return;

    Rectangle clip = gfx.getClipBounds();
    TableColumnModel cols = table.getColumnModel();

    int height = table.getRowHeight();
    int x0 = 0, y0 = 0;
    int x = x0;
    int y = y0;

    Dimension gap = table.getInterCellSpacing();
    int ymax = clip.y + clip.height;
    int xmax = clip.x + clip.width;

    // paint the cell contents
    for (int c = 0; c < ncols && x < xmax; ++c)
      {
        y = y0;
        TableColumn col = cols.getColumn(c);
        int width = col.getWidth();
        int modelCol = col.getModelIndex();

        for (int r = 0; r < nrows && y < ymax; ++r)
          {
            Rectangle bounds = new Rectangle(x, y, width, height);
              if (bounds.intersects(clip))
              {
                TableCellRenderer rend = table.getCellRenderer(r, c);
                Component comp = table.prepareRenderer(rend, r, c);
                gfx.translate(x, y);
                comp.setBounds(new Rectangle(0, 0, width, height));
                comp.paint(gfx);
                gfx.translate(-x, -y);
              }
              y += height;
              if (gap != null)
                y += gap.height;
          }
        x += width;
        if (gap != null)
          x += gap.width;
      }

    // tighten up the x and y max bounds
    ymax = y;
    xmax = x;

    Color grid = table.getGridColor();    

    // paint vertical grid lines    
    if (grid != null && table.getShowVerticalLines())
      {    
        x = x0;
        Color save = gfx.getColor();
        gfx.setColor(grid);
        boolean paintedLine = false;
        for (int c = 0; c < ncols && x < xmax; ++c)
          {
            x += cols.getColumn(c).getWidth();;
            if (gap != null)
              x += gap.width;
            gfx.drawLine(x, y0, x, ymax);
            paintedLine = true;
          }
        gfx.setColor(save);
      }

    // paint horizontal grid lines    
    if (grid != null && table.getShowHorizontalLines())
      {    
        y = y0;
        Color save = gfx.getColor();
        gfx.setColor(grid);
        boolean paintedLine = false;
        for (int r = 0; r < nrows && y < ymax; ++r)
          {
            y += height;
            if (gap != null)
              y += gap.height;
            gfx.drawLine(x0, y, xmax, y);
            paintedLine = true;
          }
        gfx.setColor(save);
      }

  }

}
