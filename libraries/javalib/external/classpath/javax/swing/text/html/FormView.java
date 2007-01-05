/* FormView.java -- A view for a variety of HTML form elements
   Copyright (C) 2006 Free Software Foundation, Inc.

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


package javax.swing.text.html;

import java.awt.Component;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.net.MalformedURLException;
import java.net.URL;

import javax.swing.ButtonModel;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JPasswordField;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.UIManager;
import javax.swing.text.AttributeSet;
import javax.swing.text.ComponentView;
import javax.swing.text.Document;
import javax.swing.text.Element;
import javax.swing.text.StyleConstants;

/**
 * A View that renders HTML form elements like buttons and input fields.
 * This is implemented as a {@link ComponentView} that creates different Swing
 * component depending on the type and setting of the different form elements.
 *
 * Namely, this view creates the following components:
 * <table>
 * <tr><th>Element type</th><th>Swing component</th></tr>
 * <tr><td>input, button</td><td>JButton</td></tr>
 * <tr><td>input, checkbox</td><td>JButton</td></tr>
 * <tr><td>input, image</td><td>JButton</td></tr>
 * <tr><td>input, password</td><td>JButton</td></tr>
 * <tr><td>input, radio</td><td>JButton</td></tr>
 * <tr><td>input, reset</td><td>JButton</td></tr>
 * <tr><td>input, submit</td><td>JButton</td></tr>
 * <tr><td>input, text</td><td>JButton</td></tr>
 * <tr><td>select, size > 1 or with multiple attribute</td>
 * <td>JList in JScrollPane</td></tr>
 * <tr><td>select, size unspecified or == 1</td><td>JComboBox</td></tr>
 * <tr><td>textarea, text</td><td>JTextArea in JScrollPane</td></tr>
 * <tr><td>input, file</td><td>JTextField</td></tr> 
 * </table>
 *
 * @author Roman Kennke (kennke@aicas.com)
 */
public class FormView
  extends ComponentView
  implements ActionListener
{

  protected class MouseEventListener
    extends MouseAdapter
  {
    /**
     * Creates a new <code>MouseEventListener</code>.
     */
    protected MouseEventListener()
    {
      // Nothing to do here.
    }

    public void mouseReleased(MouseEvent ev)
    {
      String data = getImageData(ev.getPoint());
      imageSubmit(data);
    }
  }

  /**
   * If the value attribute of an <code>&lt;input type=&quot;submit&quot;&gt>
   * tag is not specified, then this string is used.
   * 
   * @deprecated As of JDK1.3 the value is fetched from the UIManager property
   *             <code>FormView.submitButtonText</code>.
   */
  public static final String SUBMIT =
    UIManager.getString("FormView.submitButtonText");

  /**
   * If the value attribute of an <code>&lt;input type=&quot;reset&quot;&gt>
   * tag is not specified, then this string is used.
   * 
   * @deprecated As of JDK1.3 the value is fetched from the UIManager property
   *             <code>FormView.resetButtonText</code>.
   */
  public static final String RESET =
    UIManager.getString("FormView.resetButtonText");

  /**
   * If this is true, the maximum size is set to the preferred size.
   */
  private boolean maxIsPreferred;

  /**
   * Creates a new <code>FormView</code>.
   *
   * @param el the element that is displayed by this view.
   */
  public FormView(Element el)
  {
    super(el);
  }

  /**
   * Creates the correct AWT component for rendering the form element.
   */
  protected Component createComponent()
  {
    Component comp = null;
    Element el = getElement();
    AttributeSet atts = el.getAttributes();
    Object tag = atts.getAttribute(StyleConstants.NameAttribute);
    Object model = atts.getAttribute(StyleConstants.ModelAttribute);
    if (tag.equals(HTML.Tag.INPUT))
      {
        String type = (String) atts.getAttribute(HTML.Attribute.TYPE);
        if (type.equals("button"))
          {
            String value = (String) atts.getAttribute(HTML.Attribute.VALUE);
            JButton b = new JButton(value);
            if (model != null)
              {
                b.setModel((ButtonModel) model);
                b.addActionListener(this);
              }
            comp = b;
            maxIsPreferred = true;
          }
        else if (type.equals("checkbox"))
          {
            JCheckBox c = new JCheckBox();
            if (model != null)
              {
                boolean sel = atts.getAttribute(HTML.Attribute.CHECKED) != null;
                ((JToggleButton.ToggleButtonModel) model).setSelected(sel);
                c.setModel((ButtonModel) model);
              }
            comp = c;
            maxIsPreferred = true;
          }
        else if (type.equals("image"))
          {
            String src = (String) atts.getAttribute(HTML.Attribute.SRC);
            JButton b;
            try
              {
                URL base = ((HTMLDocument) el.getDocument()).getBase();
                URL srcURL = new URL(base, src);
                ImageIcon icon = new ImageIcon(srcURL);
                b = new JButton(icon);
              }
            catch (MalformedURLException ex)
              {
                b = new JButton(src);
              }
            if (model != null)
              {
                b.setModel((ButtonModel) model);
                b.addActionListener(this);
              }
            comp = b;
            maxIsPreferred = true;
          }
        else if (type.equals("password"))
          {
            int size = HTML.getIntegerAttributeValue(atts, HTML.Attribute.SIZE,
                                                     -1);
            JTextField tf = new JPasswordField();
            if (size > 0)
              tf.setColumns(size);
            else
              tf.setColumns(20);
            if (model != null)
              tf.setDocument((Document) model);
            String value = (String) atts.getAttribute(HTML.Attribute.VALUE);
            if (value != null)
              tf.setText(value);
            tf.addActionListener(this);
            comp = tf;
            maxIsPreferred = true;
          }
        else if (type.equals("radio"))
          {
            JRadioButton c = new JRadioButton();
            if (model != null)
              {
                boolean sel = atts.getAttribute(HTML.Attribute.CHECKED) != null;
                ((JToggleButton.ToggleButtonModel) model).setSelected(sel);
                c.setModel((ButtonModel) model);
              }
            comp = c;
            maxIsPreferred = true;
          }
        else if (type.equals("reset"))
          {
            String value = (String) atts.getAttribute(HTML.Attribute.VALUE);
            if (value == null)
              value = UIManager.getString("FormView.resetButtonText");
            JButton b = new JButton(value);
            if (model != null)
              {
                b.setModel((ButtonModel) model);
                b.addActionListener(this);
              }
            comp = b;
            maxIsPreferred = true;
          }
        else if (type.equals("submit"))
          {
            String value = (String) atts.getAttribute(HTML.Attribute.VALUE);
            if (value == null)
              value = UIManager.getString("FormView.submitButtonText");
            JButton b = new JButton(value);
            if (model != null)
              {
                b.setModel((ButtonModel) model);
                b.addActionListener(this);
              }
            comp = b;
            maxIsPreferred = true;
          }
        else if (type.equals("text"))
          {
            int size = HTML.getIntegerAttributeValue(atts, HTML.Attribute.SIZE,
                                                     -1);
            JTextField tf = new JTextField();
            if (size > 0)
              tf.setColumns(size);
            else
              tf.setColumns(20);
            if (model != null)
              tf.setDocument((Document) model);
            String value = (String) atts.getAttribute(HTML.Attribute.VALUE);
            if (value != null)
              tf.setText(value);
            tf.addActionListener(this);
            comp = tf;
            maxIsPreferred = true;
          }
      }
    else if (tag == HTML.Tag.TEXTAREA)
      {
        JTextArea textArea = new JTextArea((Document) model);
        int rows = HTML.getIntegerAttributeValue(atts, HTML.Attribute.ROWS, 1);
        textArea.setRows(rows);
        int cols = HTML.getIntegerAttributeValue(atts, HTML.Attribute.COLS, 20);
        textArea.setColumns(cols);
        maxIsPreferred = true;
        comp = new JScrollPane(textArea,
                               JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                               JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
      }
    // FIXME: Implement the remaining components.
    return comp;
  }

  /**
   * Determines the maximum span for this view on the specified axis.
   *
   * @param axis the axis along which to determine the span
   *
   * @return the maximum span for this view on the specified axis
   *
   * @throws IllegalArgumentException if the axis is invalid
   */
  public float getMaximumSpan(int axis)
  {
    float span;
    if (maxIsPreferred)
      span = getPreferredSpan(axis);
    else
      span = super.getMaximumSpan(axis);
    return span;
  }

  /**
   * Processes an action from the Swing component.
   *
   * If the action comes from a submit button, the form is submitted by calling
   * {@link #submitData}. In the case of a reset button, the form is reset to
   * the original state. If the action comes from a password or text field,
   * then the input focus is transferred to the next input element in the form,
   * unless this text/password field is the last one, in which case the form
   * is submitted.
   *
   * @param ev the action event
   */
  public void actionPerformed(ActionEvent ev)
  {
    Element el = getElement();
    Object tag = el.getAttributes().getAttribute(StyleConstants.NameAttribute);
    if (tag.equals(HTML.Tag.INPUT))
      {
        AttributeSet atts = el.getAttributes();
        String type = (String) atts.getAttribute(HTML.Attribute.TYPE);
        if (type.equals("submit"))
          submitData(""); // FIXME: How to fetch the actual form data?
      }
    // FIXME: Implement the remaining actions.
  }

  /**
   * Submits the form data. A separate thread is created to do the
   * transmission.
   *
   * @param data the form data
   */
  protected void submitData(String data)
  {
    // FIXME: Implement this.
  }

  /**
   * Submits the form data in response to a click on a
   * <code>&lt;input type=&quot;image&quot;&gt;</code> element.
   *
   * @param imageData the mouse click coordinates
   */
  protected void imageSubmit(String imageData)
  {
    // FIXME: Implement this.
  }

  /**
   * Determines the image data that should be submitted in response to a
   * mouse click on a image. This is either 'x=<p.x>&y=<p.y>' if the name
   * attribute of the element is null or '' or
   * <name>.x=<p.x>&<name>.y=<p.y>' when the name attribute is not empty.
   *
   * @param p the coordinates of the mouseclick
   */
  String getImageData(Point p)
  {
    String name = (String) getElement().getAttributes()
                                            .getAttribute(HTML.Attribute.NAME);
    String data;
    if (name == null || name.equals(""))
      {
        data = "x=" + p.x + "&y=" + p.y;
      }
    else
      {
        data = name + ".x=" + p.x + "&" + name + ".y=" + p.y; 
      }
    return data;
  }
}
