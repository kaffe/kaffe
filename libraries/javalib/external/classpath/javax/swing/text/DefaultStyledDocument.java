/* DefaultStyledDocument.java --
   Copyright (C) 2004, 2005 Free Software Foundation, Inc.

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


package javax.swing.text;

import java.awt.Color;
import java.awt.Font;
import java.io.Serializable;
import java.util.Enumeration;
import java.util.Stack;
import java.util.Vector;

import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.DocumentEvent;
import javax.swing.undo.AbstractUndoableEdit;
import javax.swing.undo.UndoableEdit;

/**
 * The default implementation of {@link StyledDocument}.
 *
 * The document is modeled as an {@link Element} tree, which has
 * a {@link SectionElement} as single root, which has one or more
 * {@link AbstractDocument.BranchElement}s as paragraph nodes
 * and each paragraph node having one or more
 * {@link AbstractDocument.LeafElement}s as content nodes.
 *
 * @author Michael Koch (konqueror@gmx.de)
 * @author Roman Kennke (roman@kennke.org)
 */
public class DefaultStyledDocument extends AbstractDocument
  implements StyledDocument
{
  /**
   * An {@link UndoableEdit} that can undo attribute changes to an element.
   *
   * @author Roman Kennke (kennke@aicas.com)
   */
  public static class AttributeUndoableEdit
    extends AbstractUndoableEdit
  {
    /**
     * A copy of the old attributes.
     */
    protected AttributeSet copy;

    /**
     * The new attributes.
     */
    protected AttributeSet newAttributes;

    /**
     * If the new attributes replaced the old attributes or if they only were
     * added to them.
     */
    protected boolean isReplacing;

    /**
     * The element that has changed.
     */
    protected Element element;

    /**
     * Creates a new <code>AttributeUndoableEdit</code>.
     *
     * @param el the element that changes attributes
     * @param newAtts the new attributes
     * @param replacing if the new attributes replace the old or only append to
     *        them
     */
    public AttributeUndoableEdit(Element el, AttributeSet newAtts,
                                 boolean replacing)
    {
      element = el;
      newAttributes = newAtts;
      isReplacing = replacing;
      copy = el.getAttributes().copyAttributes();
    }

    /**
     * Undos the attribute change. The <code>copy</code> field is set as
     * attributes on <code>element</code>.
     */
    public void undo()
    {
      super.undo();
      AttributeSet atts = element.getAttributes();
      if (atts instanceof MutableAttributeSet)
        {
          MutableAttributeSet mutable = (MutableAttributeSet) atts;
          mutable.removeAttributes(atts);
          mutable.addAttributes(copy);
        }
    }

    /**
     * Redos an attribute change. This adds <code>newAttributes</code> to the
     * <code>element</code>'s attribute set, possibly clearing all attributes
     * if <code>isReplacing</code> is true.
     */
    public void redo()
    {
      super.undo();
      AttributeSet atts = element.getAttributes();
      if (atts instanceof MutableAttributeSet)
        {
          MutableAttributeSet mutable = (MutableAttributeSet) atts;
          if (isReplacing)
            mutable.removeAttributes(atts);
          mutable.addAttributes(newAttributes);
        }
    }
  }

  /**
   * Carries specification information for new {@link Element}s that should
   * be created in {@link ElementBuffer}. This allows the parsing process
   * to be decoupled from the <code>Element</code> creation process.
   */
  public static class ElementSpec
  {
    /**
     * This indicates a start tag. This is a possible value for
     * {@link #getType}.
     */
    public static final short StartTagType = 1;

    /**
     * This indicates an end tag. This is a possible value for
     * {@link #getType}.
     */
    public static final short EndTagType = 2;

    /**
     * This indicates a content element. This is a possible value for
     * {@link #getType}.
     */
    public static final short ContentType = 3;

    /**
     * This indicates that the data associated with this spec should be joined
     * with what precedes it. This is a possible value for
     * {@link #getDirection}.
     */
    public static final short JoinPreviousDirection = 4;

    /**
     * This indicates that the data associated with this spec should be joined
     * with what follows it. This is a possible value for
     * {@link #getDirection}.
     */
    public static final short JoinNextDirection = 5;

    /**
     * This indicates that the data associated with this spec should be used
     * to create a new element. This is a possible value for
     * {@link #getDirection}.
     */
    public static final short OriginateDirection = 6;

    /**
     * This indicates that the data associated with this spec should be joined
     * to the fractured element. This is a possible value for
     * {@link #getDirection}.
     */
    public static final short JoinFractureDirection = 7;

    /**
     * The type of the tag.
     */
    short type;

    /**
     * The direction of the tag.
     */
    short direction;

    /**
     * The offset of the content.
     */
    int offset;

    /**
     * The length of the content.
     */
    int length;

    /**
     * The actual content.
     */
    char[] content;

    /**
     * The attributes for the tag.
     */
    AttributeSet attributes;

    /**
     * Creates a new <code>ElementSpec</code> with no content, length or
     * offset. This is most useful for start and end tags.
     *
     * @param a the attributes for the element to be created
     * @param type the type of the tag
     */
    public ElementSpec(AttributeSet a, short type)
    {
      this(a, type, 0);
    }

    /**
     * Creates a new <code>ElementSpec</code> that specifies the length but
     * not the offset of an element. Such <code>ElementSpec</code>s are
     * processed sequentially from a known starting point.
     *
     * @param a the attributes for the element to be created
     * @param type the type of the tag
     * @param len the length of the element
     */
    public ElementSpec(AttributeSet a, short type, int len)
    {
      this(a, type, null, 0, len);
    }
 
    /**
     * Creates a new <code>ElementSpec</code> with document content.
     *
     * @param a the attributes for the element to be created
     * @param type the type of the tag
     * @param txt the actual content
     * @param offs the offset into the <code>txt</code> array
     * @param len the length of the element
     */
    public ElementSpec(AttributeSet a, short type, char[] txt, int offs,
                       int len)
    {
      attributes = a;
      this.type = type;
      offset = offs;
      length = len;
      content = txt;
      direction = OriginateDirection;
    }

    /**
     * Sets the type of the element.
     *
     * @param type the type of the element to be set
     */
    public void setType(short type)
    {
      this.type = type;
    }

    /**
     * Returns the type of the element.
     *
     * @return the type of the element
     */
    public short getType()
    {
      return type;
    }

    /**
     * Sets the direction of the element.
     *
     * @param dir the direction of the element to be set
     */
    public void setDirection(short dir)
    {
      direction = dir;
    }

    /**
     * Returns the direction of the element.
     *
     * @return the direction of the element
     */
    public short getDirection()
    {
      return direction;
    }

    /**
     * Returns the attributes of the element.
     *
     * @return the attributes of the element
     */
    public AttributeSet getAttributes()
    {
      return attributes;
    }

    /**
     * Returns the actual content of the element.
     *
     * @return the actual content of the element
     */
    public char[] getArray()
    {
      return content;
    }

    /**
     * Returns the offset of the content.
     *
     * @return the offset of the content
     */
    public int getOffset()
    {
      return offset;
    }

    /**
     * Returns the length of the content.
     *
     * @return the length of the content
     */
    public int getLength()
    {
      return length;
    }

    /**
     * Returns a String representation of this <code>ElementSpec</code>
     * describing the type, direction and length of this
     * <code>ElementSpec</code>.
     *
     * @return a String representation of this <code>ElementSpec</code>
     */
    public String toString()
    {
      StringBuilder b = new StringBuilder();
      b.append('<');
      switch (type)
        {
        case StartTagType:
          b.append("StartTag");
          break;
        case EndTagType:
          b.append("EndTag");
          break;
        case ContentType:
          b.append("Content");
          break;
        default:
          b.append("??");
          break;
        }

      b.append(':');

      switch (direction)
        {
        case JoinPreviousDirection:
          b.append("JoinPrevious");
          break;
        case JoinNextDirection:
          b.append("JoinNext");
          break;
        case OriginateDirection:
          b.append("Originate");
          break;
        case JoinFractureDirection:
          b.append("Fracture");
          break;
        default:
          b.append("??");
          break;
        }

      b.append(':');
      b.append(length);

      return b.toString();
    }
  }

  /**
   * Performs all <em>structural</code> changes to the <code>Element</code>
   * hierarchy.
   */
  public class ElementBuffer implements Serializable
  {
    /** The serialization UID (compatible with JDK1.5). */
    private static final long serialVersionUID = 1688745877691146623L;

    /** The root element of the hierarchy. */
    private Element root;

    /** Holds the offset for structural changes. */
    private int offset;

    /** Holds the length of structural changes. */
    private int length;

    /**
     * The number of inserted end tags. This is a counter which always gets
     * incremented when an end tag is inserted. This is evaluated before
     * content insertion to go up the element stack.
     */
    private int numEndTags;

    /**
     * The number of inserted start tags. This is a counter which always gets
     * incremented when an end tag is inserted. This is evaluated before
     * content insertion to go up the element stack.
     */
    private int numStartTags;

    /**
     * The current position in the element tree. This is used for bulk inserts
     * using ElementSpecs.
     */
    private Stack elementStack;

    /**
     * Holds fractured elements during insertion of end and start tags.
     * Inserting an end tag may lead to fracturing of the current paragraph
     * element. The elements that have been cut off may be added to the
     * next paragraph that is created in the next start tag.
     */
    Element[] fracture;

    /**
     * The ElementChange that describes the latest changes.
     */
    DefaultDocumentEvent documentEvent;

    /**
     * Creates a new <code>ElementBuffer</code> for the specified
     * <code>root</code> element.
     *
     * @param root the root element for this <code>ElementBuffer</code>
     */
    public ElementBuffer(Element root)
    {
      this.root = root;
      elementStack = new Stack();
    }

    /**
     * Returns the root element of this <code>ElementBuffer</code>.
     *
     * @return the root element of this <code>ElementBuffer</code>
     */
    public Element getRootElement()
    {
      return root;
    }

    /**
     * Modifies the element structure so that the specified interval starts
     * and ends at an element boundary. Content and paragraph elements
     * are split and created as necessary.
     *
     * This also updates the <code>DefaultDocumentEvent</code> to reflect the
     * structural changes.
     *
     * The bulk work is delegated to {@link #changeUpdate()}.
     *
     * @param offset the start index of the interval to be changed
     * @param length the length of the interval to be changed
     * @param ev the <code>DefaultDocumentEvent</code> describing the change
     */
    public void change(int offset, int length, DefaultDocumentEvent ev)
    {
      this.offset = offset;
      this.length = length;
      documentEvent = ev;
      changeUpdate();
    }

    /**
     * Performs the actual work for {@link #change}.
     * The elements at the interval boundaries are split up (if necessary)
     * so that the interval boundaries are located at element boundaries.
     */
    protected void changeUpdate()
    {
      // Split up the element at the start offset if necessary.
      Element el = getCharacterElement(offset);
      Element[] res = split(el, offset, 0);
      BranchElement par = (BranchElement) el.getParentElement();
      if (res[1] != null)
        {
          int index = par.getElementIndex(offset);
          Element[] removed;
          Element[] added;
          if (res[0] == null)
            {
              removed = new Element[0];
              added = new Element[]{ res[1] };
              index++;
            }
          else
            {
              removed = new Element[]{ el };
              added = new Element[]{ res[0], res[1] };
            }
          par.replace(index, removed.length, added);
          addEdit(par, index, removed, added);
        }

      int endOffset = offset + length;
      el = getCharacterElement(endOffset);
      res = split(el, endOffset, 0);
      par = (BranchElement) el.getParentElement();
      if (res[1] != null)
        {
          int index = par.getElementIndex(offset);
          Element[] removed;
          Element[] added;
          if (res[1] == null)
            {
              removed = new Element[0];
              added = new Element[]{ res[1] };
            }
          else
            {
              removed = new Element[]{ el };
              added = new Element[]{ res[0], res[1] };
            }
          par.replace(index, removed.length, added);
          addEdit(par, index, removed, added);
        }
    }

    /**
     * Splits an element if <code>offset</code> is not alread at its boundary.
     *
     * @param el the Element to possibly split
     * @param offset the offset at which to possibly split
     * @param space the amount of space to create between the splitted parts
     *
     * @return An array of elements which represent the split result. This
     *         array has two elements, the two parts of the split. The first
     *         element might be null, which means that the element which should
     *         be splitted can remain in place. The second element might also
     *         be null, which means that the offset is already at an element
     *         boundary and the element doesn't need to be splitted.
     *          
     */
    private Element[] split(Element el, int offset, int space)
    {
      // If we are at an element boundary, then return an empty array.
      if ((offset == el.getStartOffset() || offset == el.getEndOffset())
          && space == 0 && el.isLeaf())
        return new Element[2];

      // If the element is an instance of BranchElement, then we recursivly
      // call this method to perform the split.
      Element[] res = new Element[2];
      if (el instanceof BranchElement)
        {
          int index = el.getElementIndex(offset);
          Element child = el.getElement(index);
          Element[] result = split(child, offset, space);
          Element[] removed;
          Element[] added;
          Element[] newAdded;

          int count = el.getElementCount();
          if (!(result[1] == null))
            {
              // This is the case when we can keep the first element.
              if (result[0] == null)
                {
                  removed = new Element[count - index - 1];
                  newAdded = new Element[count - index - 1];
                  added = new Element[]{};
                }
              // This is the case when we may not keep the first element.
              else
                {
                  removed = new Element[count - index];
                  newAdded = new Element[count - index];
                  added = new Element[]{result[0]};
                }
              newAdded[0] = result[1];
              for (int i = index; i < count; i++)
                {
                  Element el2 = el.getElement(i);
                  int ind = i - count + removed.length;
                  removed[ind] = el2;
                  if (ind != 0)
                    newAdded[ind] = el2;
                }

              ((BranchElement) el).replace(index, removed.length, added);
              addEdit(el, index, removed, added);
              BranchElement newPar =
                (BranchElement) createBranchElement(el.getParentElement(),
                                                    el.getAttributes());
              newPar.replace(0, 0, newAdded);
              res = new Element[]{ null, newPar };
            }
          else
            {
              removed = new Element[count - index];
              for (int i = index; i < count; ++i)
                removed[i - index] = el.getElement(i);
              added = new Element[0];
              ((BranchElement) el).replace(index, removed.length,
                                           added);
              addEdit(el, index, removed, added);
              BranchElement newPar =
                (BranchElement) createBranchElement(el.getParentElement(),
                                                    el.getAttributes());
              newPar.replace(0, 0, removed);
              res = new Element[]{ null, newPar };
            }
        }
      else if (el instanceof LeafElement)
        {
          BranchElement par = (BranchElement) el.getParentElement();
          Element el1 = createLeafElement(par, el.getAttributes(),
                                          el.getStartOffset(), offset);
          Element el2 = createLeafElement(par, el.getAttributes(),
                                          offset + space, el.getEndOffset());
          res = new Element[]{ el1, el2 };
        }
      return res;
    }

    /**
     * Inserts new <code>Element</code> in the document at the specified
     * position.
     *
     * Most of the work is done by {@link #insertUpdate}, after some fields
     * have been prepared for it.
     *
     * @param offset the location in the document at which the content is
     *        inserted
     * @param length the length of the inserted content
     * @param data the element specifications for the content to be inserted
     * @param ev the document event that is updated to reflect the structural
     *        changes
     */
    public void insert(int offset, int length, ElementSpec[] data,
                       DefaultDocumentEvent ev)
    {
      this.offset = offset;
      this.length = length;
      documentEvent = ev;
      // Push the root and the paragraph at offset onto the element stack.
      elementStack.clear();
      elementStack.push(root);
      elementStack.push(root.getElement(root.getElementIndex(offset)));
      numEndTags = 0;
      numStartTags = 0;
      insertUpdate(data);
    }

    /**
     * Performs the actual structural change for {@link #insert}. This
     * creates a bunch of {@link Element}s as specified by <code>data</code>
     * and inserts it into the document as specified in the arguments to
     * {@link #insert}.
     *
     * @param data the element specifications for the elements to be inserte
     */
    protected void insertUpdate(ElementSpec[] data)
    {
      for (int i = 0; i < data.length; i++)
        {
          switch (data[i].getType())
            {
            case ElementSpec.StartTagType:
              numStartTags++;
              break;
            case ElementSpec.EndTagType:
              numEndTags++;
              break;
            default:
              insertContentTag(data[i]);
              break;
            }
        }
      endEdit();
    }

    /**
     * Finishes an insertion by possibly evaluating the outstanding start and
     * end tags. However, this is only performed if the event has received any
     * modifications.
     */
    private void endEdit()
    {
      if (documentEvent.modified)
        prepareContentInsertion();
    }

    /**
     * Evaluates the number of inserted end tags and performs the corresponding
     * structural changes.
     */
    private void prepareContentInsertion()
    {
      while (numEndTags > 0)
        {
          elementStack.pop();
          numEndTags--;
        }

      while (numStartTags > 0)
        {
          Element current = (Element) elementStack.peek();
          Element newParagraph =
            insertParagraph((BranchElement) current, offset);
          elementStack.push(newParagraph);
          numStartTags--;
        }
    }

    private Element insertParagraph(BranchElement par, int offset)
    {
      Element current = par.getElement(par.getElementIndex(offset));
      Element[] res = split(current, offset, 0);
      int index = par.getElementIndex(offset);
      Element ret;
      if (res[1] != null)
        {
          Element[] removed;
          Element[] added;
          if (res[0] == null)
            {
              removed = new Element[0];
              if (res[1] instanceof BranchElement)
                {
                  added = new Element[]{ res[1] };
                  ret = res[1];
                }
              else
                {
                  ret = createBranchElement(par, null);
                  added = new Element[]{ ret, res[1] };
                }
              index++;
            }
          else
            {
              removed = new Element[]{ current };
              if (res[1] instanceof BranchElement)
                {
                  ret = res[1];
                  added = new Element[]{ res[0], res[1] };
                }
              else
                {
                  ret = createBranchElement(par, null);
                  added = new Element[]{ res[0], ret, res[1] };
                }
            }
          par.replace(index, removed.length, added);
          addEdit(par, index, removed, added);
        }
      else
        {
          ret = createBranchElement(par, null);
          Element[] added = new Element[]{ ret };
          par.replace(index, 0, added);
          addEdit(par, index, new Element[0], added);
        }
      return ret;
    }

    /**
     * Inserts a content element into the document structure.
     *
     * @param tag the element spec
     */
    private void insertContentTag(ElementSpec tag)
    {
      prepareContentInsertion();
      int len = tag.getLength();
      int dir = tag.getDirection();
      if (dir == ElementSpec.JoinPreviousDirection)
        {
          // The mauve tests to this class show that a JoinPrevious insertion
          // does not add any edits to the document event. To me this means
          // that nothing is done here. The previous element naturally should
          // expand so that it covers the new characters.
        }
      else if (dir == ElementSpec.JoinNextDirection)
        {
          BranchElement paragraph = (BranchElement) elementStack.peek();
          int currentIndex = paragraph.getElementIndex(offset);
          Element current = paragraph.getElement(currentIndex);
          Element next = paragraph.getElement(currentIndex + 1);

          Element newEl1 = createLeafElement(paragraph,
                                             current.getAttributes(),
                                             current.getStartOffset(),
                                             offset);
          Element newEl2 = createLeafElement(paragraph,
                                             current.getAttributes(),
                                             offset,
                                             next.getEndOffset());

          Element[] add = new Element[] {newEl1, newEl2};
          Element[] remove = new Element[] {current, next};
          paragraph.replace(currentIndex, 2, add);

          // Add this action to the document event.
          addEdit(paragraph, currentIndex, remove, add);
        }
      else
        {
          BranchElement paragraph = (BranchElement) elementStack.peek();
          int index = paragraph.getElementIndex(offset);
          Element current = paragraph.getElement(index);

          Element[] added;
          Element[] removed;
          Element[] splitRes = split(current, offset, length);
          // Special case for when offset == startOffset or offset == endOffset.
          if (splitRes[0] == null)
            {
              added = new Element[2];
              added[0] = createLeafElement(paragraph, tag.getAttributes(),
                                           offset, offset + length);
              added[1] = splitRes[1];
              removed = new Element[0];
              index++;
            }
          else if (current.getStartOffset() == offset)
            {
              added = new Element[2];
              added[0] = createLeafElement(paragraph, tag.getAttributes(),
                                           offset, offset + length);
              added[1] = splitRes[1];
              removed = new Element[] { current };
            }
          else if (current.getEndOffset() - length == offset)
            {
              added = new Element[2];
              added[0] = splitRes[0];
              added[1] = createLeafElement(paragraph, tag.getAttributes(),
                                           offset, offset + length);
              removed = new Element[] { current };
            }
          else
            {
              added = new Element[3];
              added[0] = splitRes[0];
              added[1] = createLeafElement(paragraph, tag.getAttributes(),
                                           offset, offset + length);
              added[2] = splitRes[1];
              removed = new Element[] { current };
            }
          paragraph.replace(index, removed.length, added);
          addEdit(paragraph, index, removed, added);
        }
      offset += len;
    }
    
    /**
     * Creates a copy of the element <code>clonee</code> that has the parent
     * <code>parent</code>.
     * @param parent the parent of the newly created Element
     * @param clonee the Element to clone
     * @return the cloned Element
     */
    public Element clone (Element parent, Element clonee)
    {
      // If the Element we want to clone is a leaf, then simply copy it
      if (clonee.isLeaf())
        return createLeafElement(parent, clonee.getAttributes(),
                                 clonee.getStartOffset(), clonee.getEndOffset());
      
      // Otherwise create a new BranchElement with the desired parent and 
      // the clonee's attributes
      BranchElement result = (BranchElement) createBranchElement(parent, clonee.getAttributes());
      
      // And clone all the of clonee's children
      Element[] children = new Element[clonee.getElementCount()];
      for (int i = 0; i < children.length; i++)
        children[i] = clone(result, clonee.getElement(i));
      
      // Make the cloned children the children of the BranchElement
      result.replace(0, 0, children);
      return result;
    }

    /**
     * Adds an ElementChange for a given element modification to the document
     * event. If there already is an ElementChange registered for this element,
     * this method tries to merge the ElementChanges together. However, this
     * is only possible if the indices of the new and old ElementChange are
     * equal.
     *
     * @param e the element
     * @param i the index of the change
     * @param removed the removed elements, or <code>null</code>
     * @param added the added elements, or <code>null</code>
     */
    private void addEdit(Element e, int i, Element[] removed, Element[] added)
    {
      // Perform sanity check first.
      DocumentEvent.ElementChange ec = documentEvent.getChange(e);

      // Merge the existing stuff with the new stuff.
      Element[] oldAdded = ec == null ? null: ec.getChildrenAdded();
      Element[] newAdded;
      if (oldAdded != null && added != null)
        {
          if (ec.getIndex() <= i)
            {
              int index = i - ec.getIndex();
              // Merge adds together.
              newAdded = new Element[oldAdded.length + added.length];
              System.arraycopy(oldAdded, 0, newAdded, 0, index);
              System.arraycopy(added, 0, newAdded, index, added.length);
              System.arraycopy(oldAdded, index, newAdded, index + added.length,
                               oldAdded.length - index);
              i = ec.getIndex();
            }
          else
            throw new AssertionError("Not yet implemented case.");
        }
      else if (added != null)
        newAdded = added;
      else if (oldAdded != null)
        newAdded = oldAdded;
      else
        newAdded = new Element[0];

      Element[] oldRemoved = ec == null ? null: ec.getChildrenRemoved();
      Element[] newRemoved;
      if (oldRemoved != null && removed != null)
        {
          if (ec.getIndex() <= i)
            {
              int index = i - ec.getIndex();
              // Merge removes together.
              newRemoved = new Element[oldRemoved.length + removed.length];
              System.arraycopy(oldAdded, 0, newRemoved, 0, index);
              System.arraycopy(removed, 0, newRemoved, index, removed.length);
              System.arraycopy(oldRemoved, index, newRemoved,
                               index + removed.length,
                               oldRemoved.length - index);
              i = ec.getIndex();
            }
          else
            throw new AssertionError("Not yet implemented case.");
        }
      else if (removed != null)
        newRemoved = removed;
      else if (oldRemoved != null)
        newRemoved = oldRemoved;
      else
        newRemoved = new Element[0];

      // Replace the existing edit for the element with the merged.
      documentEvent.addEdit(new ElementEdit(e, i, newRemoved, newAdded));
    }
  }

  /**
   * An element type for sections. This is a simple BranchElement with
   * a unique name.
   */
  protected class SectionElement extends BranchElement
  {
    /**
     * Creates a new SectionElement.
     */
    public SectionElement()
    {
      super(null, null);
    }

    /**
     * Returns the name of the element. This method always returns
     * &quot;section&quot;.
     *
     * @return the name of the element
     */
    public String getName()
    {
      return "section";
    }
  }

  /**
   * Receives notification when any of the document's style changes and calls
   * {@link DefaultStyledDocument#styleChanged(Style)}.
   *
   * @author Roman Kennke (kennke@aicas.com)
   */
  private class StyleChangeListener
    implements ChangeListener
  {

    /**
     * Receives notification when any of the document's style changes and calls
     * {@link DefaultStyledDocument#styleChanged(Style)}.
     *
     * @param event the change event
     */
    public void stateChanged(ChangeEvent event)
    {
      Style style = (Style) event.getSource();
      styleChanged(style);
    }
  }

  /** The serialization UID (compatible with JDK1.5). */
  private static final long serialVersionUID = 940485415728614849L;

  /**
   * The default size to use for new content buffers.
   */
  public static final int BUFFER_SIZE_DEFAULT = 4096;

  /**
   * The <code>EditorBuffer</code> that is used to manage to
   * <code>Element</code> hierarchy.
   */
  protected DefaultStyledDocument.ElementBuffer buffer;

  /**
   * Listens for changes on this document's styles and notifies styleChanged().
   */
  private StyleChangeListener styleChangeListener;

  /**
   * Creates a new <code>DefaultStyledDocument</code>.
   */
  public DefaultStyledDocument()
  {
    this(new GapContent(BUFFER_SIZE_DEFAULT), new StyleContext());
  }

  /**
   * Creates a new <code>DefaultStyledDocument</code> that uses the
   * specified {@link StyleContext}.
   *
   * @param context the <code>StyleContext</code> to use
   */
  public DefaultStyledDocument(StyleContext context)
  {
    this(new GapContent(BUFFER_SIZE_DEFAULT), context);
  }

  /**
   * Creates a new <code>DefaultStyledDocument</code> that uses the
   * specified {@link StyleContext} and {@link Content} buffer.
   *
   * @param content the <code>Content</code> buffer to use
   * @param context the <code>StyleContext</code> to use
   */
  public DefaultStyledDocument(AbstractDocument.Content content,
			       StyleContext context)
  {
    super(content, context);
    buffer = new ElementBuffer(createDefaultRoot());
    setLogicalStyle(0, context.getStyle(StyleContext.DEFAULT_STYLE));
  }

  /**
   * Adds a style into the style hierarchy. Unspecified style attributes
   * can be resolved in the <code>parent</code> style, if one is specified.
   *
   * While it is legal to add nameless styles (<code>nm == null</code),
   * you must be aware that the client application is then responsible
   * for managing the style hierarchy, since unnamed styles cannot be
   * looked up by their name.
   *
   * @param nm the name of the style or <code>null</code> if the style should
   *           be unnamed
   * @param parent the parent in which unspecified style attributes are
   *           resolved, or <code>null</code> if that is not necessary
   *
   * @return the newly created <code>Style</code>
   */
  public Style addStyle(String nm, Style parent)
  {
    StyleContext context = (StyleContext) getAttributeContext();
    Style newStyle = context.addStyle(nm, parent);

    // Register change listener.
    if (styleChangeListener == null)
      styleChangeListener = new StyleChangeListener();
    newStyle.addChangeListener(styleChangeListener);

    return newStyle;
  }

  /**
   * Create the default root element for this kind of <code>Document</code>.
   *
   * @return the default root element for this kind of <code>Document</code>
   */
  protected AbstractDocument.AbstractElement createDefaultRoot()
  {
    Element[] tmp;
    // FIXME: Create a SecionElement here instead of a BranchElement.
    // Use createBranchElement() and createLeafElement instead.
    SectionElement section = new SectionElement();

    BranchElement paragraph =
      (BranchElement) createBranchElement(section, null);
    paragraph.setResolveParent(getStyle(StyleContext.DEFAULT_STYLE));
    tmp = new Element[1];
    tmp[0] = paragraph;
    section.replace(0, 0, tmp);

    LeafElement leaf = new LeafElement(paragraph, null, 0, 1);
    tmp = new Element[1];
    tmp[0] = leaf;
    paragraph.replace(0, 0, tmp);

    return section;
  }

  /**
   * Returns the <code>Element</code> that corresponds to the character
   * at the specified position.
   *
   * @param position the position of which we query the corresponding
   *        <code>Element</code>
   *
   * @return the <code>Element</code> that corresponds to the character
   *         at the specified position
   */
  public Element getCharacterElement(int position)
  {
    Element element = getDefaultRootElement();

    while (!element.isLeaf())
      {
        int index = element.getElementIndex(position);
        element = element.getElement(index);
      }
    
    return element;
  }

  /**
   * Extracts a background color from a set of attributes.
   *
   * @param attributes the attributes from which to get a background color
   *
   * @return the background color that correspond to the attributes
   */
  public Color getBackground(AttributeSet attributes)
  {
    StyleContext context = (StyleContext) getAttributeContext();
    return context.getBackground(attributes);
  }

  /**
   * Returns the default root element.
   *
   * @return the default root element
   */
  public Element getDefaultRootElement()
  {
    return buffer.getRootElement();
  }

  /**
   * Extracts a font from a set of attributes.
   *
   * @param attributes the attributes from which to get a font
   *
   * @return the font that correspond to the attributes
   */
  public Font getFont(AttributeSet attributes)
  {
    StyleContext context = (StyleContext) getAttributeContext();
    return context.getFont(attributes);
  }
  
  /**
   * Extracts a foreground color from a set of attributes.
   *
   * @param attributes the attributes from which to get a foreground color
   *
   * @return the foreground color that correspond to the attributes
   */
  public Color getForeground(AttributeSet attributes)
  {
    StyleContext context = (StyleContext) getAttributeContext();
    return context.getForeground(attributes);
  }

  /**
   * Returns the logical <code>Style</code> for the specified position.
   *
   * @param position the position from which to query to logical style
   *
   * @return the logical <code>Style</code> for the specified position
   */
  public Style getLogicalStyle(int position)
  {
    Element paragraph = getParagraphElement(position);
    AttributeSet attributes = paragraph.getAttributes();
    return (Style) attributes.getResolveParent();
  }

  /**
   * Returns the paragraph element for the specified position.
   * If the position is outside the bounds of the document's root element,
   * then the closest element is returned. That is the last paragraph if
   * <code>position >= endIndex</code> or the first paragraph if
   * <code>position < startIndex</code>.
   *
   * @param position the position for which to query the paragraph element
   *
   * @return the paragraph element for the specified position
   */
  public Element getParagraphElement(int position)
  {
    BranchElement root = (BranchElement) getDefaultRootElement();
    int start = root.getStartOffset();
    int end = root.getEndOffset();
    if (position >= end)
      position = end - 1;
    else if (position < start)
      position = start;

    Element par = root.positionToElement(position);

    assert par != null : "The paragraph element must not be null";
    return par;
  }

  /**
   * Looks up and returns a named <code>Style</code>.
   *
   * @param nm the name of the <code>Style</code>
   *
   * @return the found <code>Style</code> of <code>null</code> if no such
   *         <code>Style</code> exists
   */
  public Style getStyle(String nm)
  {
    StyleContext context = (StyleContext) getAttributeContext();
    return context.getStyle(nm);
  }

  /**
   * Removes a named <code>Style</code> from the style hierarchy.
   *
   * @param nm the name of the <code>Style</code> to be removed
   */
  public void removeStyle(String nm)
  {
    StyleContext context = (StyleContext) getAttributeContext();
    context.removeStyle(nm);
  }

  /**
   * Sets text attributes for the fragment specified by <code>offset</code>
   * and <code>length</code>.
   *
   * @param offset the start offset of the fragment
   * @param length the length of the fragment
   * @param attributes the text attributes to set
   * @param replace if <code>true</code>, the attributes of the current
   *     selection are overridden, otherwise they are merged
   */
  public void setCharacterAttributes(int offset, int length,
				     AttributeSet attributes,
				     boolean replace)
  {
    DefaultDocumentEvent ev =
      new DefaultDocumentEvent(offset, length,
			       DocumentEvent.EventType.CHANGE);

    // Modify the element structure so that the interval begins at an element
    // start and ends at an element end.
    buffer.change(offset, length, ev);

    Element root = getDefaultRootElement();
    // Visit all paragraph elements within the specified interval
    int paragraphCount =  root.getElementCount();
    for (int pindex = 0; pindex < paragraphCount; pindex++)
      {
        Element paragraph = root.getElement(pindex);
        // Skip paragraphs that lie outside the interval.
        if ((paragraph.getStartOffset() > offset + length)
            || (paragraph.getEndOffset() < offset))
          continue;

        // Visit content elements within this paragraph
        int contentCount = paragraph.getElementCount();
        for (int cindex = 0; cindex < contentCount; cindex++)
          {
            Element content = paragraph.getElement(cindex);
            // Skip content that lies outside the interval.
            if ((content.getStartOffset() > offset + length)
                || (content.getEndOffset() < offset))
              continue;

            if (content instanceof AbstractElement)
              {
                AbstractElement el = (AbstractElement) content;
                if (replace)
                  el.removeAttributes(el);
                el.addAttributes(attributes);
              }
            else
              throw new AssertionError("content elements are expected to be"
                                       + "instances of "
		       + "javax.swing.text.AbstractDocument.AbstractElement");
          }
      }

    fireChangedUpdate(ev);
  }
  
  /**
   * Sets the logical style for the paragraph at the specified position.
   *
   * @param position the position at which the logical style is added
   * @param style the style to set for the current paragraph
   */
  public void setLogicalStyle(int position, Style style)
  {
    Element el = getParagraphElement(position);
    if (el instanceof AbstractElement)
      {
        AbstractElement ael = (AbstractElement) el;
        ael.setResolveParent(style);
      }
    else
      throw new AssertionError("paragraph elements are expected to be"
         + "instances of javax.swing.text.AbstractDocument.AbstractElement");
  }

  /**
   * Sets text attributes for the paragraph at the specified fragment.
   *
   * @param offset the beginning of the fragment
   * @param length the length of the fragment
   * @param attributes the text attributes to set
   * @param replace if <code>true</code>, the attributes of the current
   *     selection are overridden, otherwise they are merged
   */
  public void setParagraphAttributes(int offset, int length,
                                     AttributeSet attributes,
                                     boolean replace)
  {
    int index = offset;
    while (index < offset + length)
      {
        AbstractElement par = (AbstractElement) getParagraphElement(index);
        AttributeContext ctx = getAttributeContext();
        if (replace)
          par.removeAttributes(par);
        par.addAttributes(attributes);
        index = par.getElementCount();
      }
  }

  /**
   * Called in response to content insert actions. This is used to
   * update the element structure.
   *
   * @param ev the <code>DocumentEvent</code> describing the change
   * @param attr the attributes for the change
   */
  protected void insertUpdate(DefaultDocumentEvent ev, AttributeSet attr)
  {
    super.insertUpdate(ev, attr);
    int offset = ev.getOffset();
    int length = ev.getLength();
    int endOffset = offset + length;
    Segment txt = new Segment();
    try
      {
        getText(offset, length, txt);
      }
    catch (BadLocationException ex)
      {
        AssertionError ae = new AssertionError("Unexpected bad location");
        ae.initCause(ex);
        throw ae;
      }

    int len = 0;
    Vector specs = new Vector();

    Element prev = getCharacterElement(offset);
    Element next = getCharacterElement(endOffset);

    for (int i = offset; i < endOffset; ++i)
      {
        len++;
        if (txt.array[i] == '\n')
          {
            ElementSpec spec = new ElementSpec(attr, ElementSpec.ContentType,
                                               len);

            // If we are at the last index, then check if we could probably be
            // joined with the next element.
            if (i == endOffset - 1)
              {
                if (next.getAttributes().isEqual(attr))
                  spec.setDirection(ElementSpec.JoinNextDirection);
              }
            // If we are at the first new element, then check if it could be
            // joined with the previous element.
            else if (specs.size() == 0)
              {
                if (prev.getAttributes().isEqual(attr))
                    spec.setDirection(ElementSpec.JoinPreviousDirection);
              }

            specs.add(spec);

            // Add ElementSpecs for the newline.
            ElementSpec endTag = new ElementSpec(null, ElementSpec.EndTagType);
            specs.add(endTag);
            ElementSpec startTag = new ElementSpec(null,
                                                   ElementSpec.StartTagType);
            startTag.setDirection(ElementSpec.JoinFractureDirection);
            specs.add(startTag);

            len = 0;
            offset += len;
          }
      }

    // Create last element if last character hasn't been a newline.
    if (len > 0)
      {
        ElementSpec spec = new ElementSpec(attr, ElementSpec.ContentType, len);
        // If we are at the first new element, then check if it could be
        // joined with the previous element.
        if (specs.size() == 0)
          {
            if (prev.getAttributes().isEqual(attr))
              spec.setDirection(ElementSpec.JoinPreviousDirection);
          }
        // Check if we could probably be joined with the next element.
        else if (next.getAttributes().isEqual(attr))
          spec.setDirection(ElementSpec.JoinNextDirection);

        specs.add(spec);
      }

    ElementSpec[] elSpecs =
      (ElementSpec[]) specs.toArray(new ElementSpec[specs.size()]);

    buffer.insert(offset, length, elSpecs, ev);
  }

  /**
   * Returns an enumeration of all style names.
   *
   * @return an enumeration of all style names
   */
  public Enumeration getStyleNames()
  {
    StyleContext context = (StyleContext) getAttributeContext();
    return context.getStyleNames();
  }

  /**
   * Called when any of this document's styles changes.
   *
   * @param style the style that changed
   */
  protected void styleChanged(Style style)
  {
    // Nothing to do here. This is intended to be overridden by subclasses.
  }

  /**
   * Inserts a bulk of structured content at once.
   *
   * @param offset the offset at which the content should be inserted
   * @param data the actual content spec to be inserted
   */
  protected void insert(int offset, ElementSpec[] data)
    throws BadLocationException
  {
    writeLock();
    // First we insert the content.
    int index = offset;
    for (int i = 0; i < data.length; i++)
      {
        ElementSpec spec = data[i];
        if (spec.getArray() != null && spec.getLength() > 0)
          {
            String insertString = new String(spec.getArray(), spec.getOffset(),
                                             spec.getLength());
            content.insertString(index, insertString);
          }
        index += spec.getLength();
      }
    // Update the view structure.
    DefaultDocumentEvent ev = new DefaultDocumentEvent(offset, index - offset,
                                               DocumentEvent.EventType.INSERT);
    for (int i = 0; i < data.length; i++)
      {
        ElementSpec spec = data[i];
        AttributeSet atts = spec.getAttributes();
        if (atts != null)
          insertUpdate(ev, atts);
      }

    // Finally we must update the document structure and fire the insert update
    // event.
    buffer.insert(offset, index - offset, data, ev);
    if (ev.modified)
      fireInsertUpdate(ev);
    writeUnlock();
  }

  /**
   * Initializes the <code>DefaultStyledDocument</code> with the specified
   * data.
   *
   * @param data the specification of the content with which the document is
   *        initialized
   */
  protected void create(ElementSpec[] data)
  {
    try
      {
        // Clear content.
        content.remove(0, content.length());
        // Clear buffer and root element.
        buffer = new ElementBuffer(createDefaultRoot());
        // Insert the data.
        insert(0, data);
      }
    catch (BadLocationException ex)
      {
        AssertionError err = new AssertionError("Unexpected bad location");
        err.initCause(ex);
        throw err;
      }
  }
}
