
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 *
 * Based on an (unrestricted) C version by: Thomas Niemann <niemannt@home.com>
 */

package java.util;

import java.io.Serializable;

// This implements a red-black tree.

public class TreeMap extends AbstractMap
		implements SortedMap, Cloneable, Serializable {
	private static final int BLACK = 0;
	private static final int RED = 1;
	private static final Node NIL;
	private final Comparator c;
	private Node insertionPoint;		// used by find() method
	private int modCount = 0;
	private Node root = NIL;
	private int size = 0;

	// Tree nodes look like this
	private static class Node implements Cloneable, Map.Entry {
		int color;
		Node left;
		Node right;
		Node parent;
		Object key;
		Object value;

		Node(Object key, Object value) {
			this.key = key;
			this.value = value;
		}

		public Object getKey() {
			return key;
		}

		public Object getValue() {
			return value;
		}

		public Object setValue(Object value) {
			Object old = value;
			this.value = value;
			return old;
		}

		public boolean equals(Object o) {
			if (!(o instanceof Map.Entry)) {
				return false;
			}
			Map.Entry me = (Map.Entry)o;
			return (this.key == null ? me.getKey() == null
				: this.key.equals(me.getKey()))
			    && (this.value == null ? me.getValue() == null
				: this.value.equals(me.getValue()));
		}

		public int hashCode() {
			return (key == null ? 0 : key.hashCode())
			    ^ (value == null ? 0 : value.hashCode());
		}

		Node cloneTree() {
			Node clone;
			try {
				clone = (Node)super.clone();
			} catch (CloneNotSupportedException e) {
				throw new Error();
			}
			if (left != NIL) {
				clone.left = left.cloneTree();
				clone.left.parent = clone;
			}
			if (right != NIL) {
				clone.right = right.cloneTree();
				clone.right.parent = clone;
			}
			return clone;
		}
	}

	// This is the NIL "sentinel" node which is the child of all leaves
	static {
		NIL = new Node(null, null);
		NIL.left = NIL;
		NIL.right = NIL;
		NIL.parent = null;
		NIL.color = BLACK;
	}

	public TreeMap() {
		c = Arrays.DEFAULT_COMPARATOR;
	}

	public TreeMap(Comparator c) {
		this.c = c;
	}

	public TreeMap(Map m) {
		c = Arrays.DEFAULT_COMPARATOR;
		for (Iterator i = m.entrySet().iterator(); i.hasNext(); ) {
			Map.Entry e = (Map.Entry)i.next();
			put(e.getKey(), e.getValue());
		}
	}

	// XXX this is not linear time like it should be..
	public TreeMap(SortedMap m) {
		Comparator c = m.comparator();
		this.c = (c != null) ? c : Arrays.DEFAULT_COMPARATOR;
		for (Iterator i = m.entrySet().iterator(); i.hasNext(); ) {
			Map.Entry e = (Map.Entry)i.next();
			put(e.getKey(), e.getValue());
		}
	}

	public int size() {
		return size;
	}

	public boolean containsKey(Object key) {
		return find(key) != null;
	}

	public boolean containsValue(Object value) {
		for (Iterator i = new NodeIterator(); i.hasNext(); ) {
			Node node = (Node)i.next();
			if (value == null ?
			    node.value == null : value.equals(node.value)) {
				return true;
			}
		}
		return false;
	}

	public Object get(Object key) {
		Node node = find(key);
		if (node == null) {
			return null;
		}
		return node.value;
	}

	public Comparator comparator() {
		return c == Arrays.DEFAULT_COMPARATOR ? null : c;
	}

	public Object firstKey() {
		if (root == NIL) {
			throw new NoSuchElementException();
		}
		Node node;
		for (node = root; node.left != NIL; node = node.left);
		return node.key;
	}

	public Object lastKey() {
		if (root == NIL) {
			throw new NoSuchElementException();
		}
		Node node;
		for (node = root; node.right != NIL; node = node.right);
		return node.key;
	}

	public void putAll(Map map) {
		for (Iterator i = map.entrySet().iterator(); i.hasNext(); ) {
			Map.Entry e = (Map.Entry)i.next();
			put(e.getKey(), e.getValue());
		}
	}

	public Object put(Object key, Object value) {
		Object rtn;
		Node node = find(key);
		if (node == null) {
			insertNode(insertionPoint, new Node(key, value));
			rtn = null;
		} else {
			rtn = node.value;
			node.value = value;
		}
		return rtn;
	}

	public Object remove(Object key) {
		Node node = find(key);
		if (node == null) {
			return null;
		}
		Object rtn = node.value;
		deleteNode(node);
		return rtn;
	}

	public void clear() {
		modCount++;
		root = NIL;
		size = 0;
	}

	public Object clone() {
		TreeMap clone = (TreeMap)super.clone();
		clone.root = root.cloneTree();
		return clone;
	}

	public Set entrySet() {
		return new AbstractMapEntrySet(this) {
			public Iterator iterator() {
				return new NodeIterator();
			}
			protected Map.Entry find(Map.Entry oent) {
				Node myent = TreeMap.this.find(oent.getKey());
				return oent.equals(myent) ? myent : null;
			}
		};
	}

	public SortedMap subMap(Object fromKey, Object toKey) {
		throw new kaffe.util.NotImplemented(Collections.class.getName()
		    + ".subMap()");
	}

	public SortedMap headMap(Object toKey) {
		throw new kaffe.util.NotImplemented(Collections.class.getName()
		    + ".headMap()");
	}

	public SortedMap tailMap(Object fromKey) {
		throw new kaffe.util.NotImplemented(Collections.class.getName()
		    + ".tailMap()");
	}

	// Find a node, or set insertionPoint to the would-be parent
	private Node find(Object key) {
		insertionPoint = null;
		for (Node node = root; node != NIL; ) {
			if (c.compare(key, node.key) == 0) {
				return node;
			}
			insertionPoint = node;
			node = (c.compare(key, node.key) < 0) ?
				node.left : node.right;
		}
		return null;
	}

	// Add a new node under given parent (null parent means at root)
	private Node insertNode(Node parent, Node node) {

		// Bump modification count
		modCount++;
		size++;

		// Make sure new node is initialized correctly
		node.parent = parent;
		node.left = NIL;
		node.right = NIL;
		node.color = RED;

		// Add node to tree
		if (parent != null) {
			if (c.compare(node.key, parent.key) < 0) {
			    parent.left = node;
			} else {
			    parent.right = node;
			}
		} else {
			root = node;
		}

		// Adjust tree
		insertFixup(node);
		return(node);
	}

	// Delete a node from tree
	private void deleteNode(Node node) {
		Node x, y;

		// Bump modification count
		modCount++;
		size--;

		// Set y to node or first successor with a NIL child
		if (node.left == NIL || node.right == NIL) {
			y = node;
		} else {
			for (y = node.right; y.left != NIL; y = y.left);
		}

		// Set x to y's only child
		if (y.left != NIL) {
			x = y.left;
		} else {
			x = y.right;
		}

		// Remove y from the parent chain
		x.parent = y.parent;
		if (y.parent != null) {
			if (y == y.parent.left) {
				y.parent.left = x;
			} else {
				y.parent.right = x;
			}
		} else {
		    	root = x;
		}

		if (y != node) {
			node.key = y.key;
			node.value = y.value;
		}

		if (y.color == BLACK) {
			deleteFixup(x);
		}
	}

	// Reestablish red/black balance after inserting node x
	private void insertFixup(Node x) {
		while (x != root && x.parent.color == RED) {
			if (x.parent == x.parent.parent.left) {
				Node y = x.parent.parent.right;
				if (y.color == RED) {
					x.parent.color = BLACK;
					y.color = BLACK;
					x.parent.parent.color = RED;
					x = x.parent.parent;
				} else {
					if (x == x.parent.right) {
						x = x.parent;
						rotateLeft(x);
					}
					x.parent.color = BLACK;
					x.parent.parent.color = RED;
					rotateRight(x.parent.parent);
				}
			} else {
				Node y = x.parent.parent.left;
				if (y.color == RED) {
					x.parent.color = BLACK;
					y.color = BLACK;
					x.parent.parent.color = RED;
					x = x.parent.parent;
				} else {
					if (x == x.parent.left) {
						x = x.parent;
						rotateRight(x);
					}
					x.parent.color = BLACK;
					x.parent.parent.color = RED;
					rotateLeft(x.parent.parent);
				}
			}
		}
	    	root.color = BLACK;
	}

	// Reestablish red/black balance after deleting node x
	private void deleteFixup(Node x) {
		while (x != root && x.color == BLACK) {
			if (x == x.parent.left) {
				Node w = x.parent.right;
				if (w.color == RED) {
					w.color = BLACK;
					x.parent.color = RED;
					rotateLeft(x.parent);
					w = x.parent.right;
				}
				if (w.left.color == BLACK
				    && w.right.color == BLACK) {
					w.color = RED;
					x = x.parent;
				} else {
					if (w.right.color == BLACK) {
						w.left.color = BLACK;
						w.color = RED;
						rotateRight(w);
						w = x.parent.right;
					}
					w.color = x.parent.color;
					x.parent.color = BLACK;
					w.right.color = BLACK;
					rotateLeft(x.parent);
					x = root;
				}
			} else {
				Node w = x.parent.left;
				if (w.color == RED) {
					w.color = BLACK;
					x.parent.color = RED;
					rotateRight (x.parent);
					w = x.parent.left;
				}
				if (w.right.color == BLACK
				    && w.left.color == BLACK) {
					w.color = RED;
					x = x.parent;
				} else {
					if (w.left.color == BLACK) {
						w.right.color = BLACK;
						w.color = RED;
						rotateLeft (w);
						w = x.parent.left;
					}
					w.color = x.parent.color;
					x.parent.color = BLACK;
					w.left.color = BLACK;
					rotateRight (x.parent);
					x = root;
				}
			}
		}
		x.color = BLACK;
	}

	// Rotate node x to the left
	private void rotateLeft(Node x) {
		Node y = x.right;
		x.right = y.left;
		if (y.left != NIL) {
			y.left.parent = x;
		}
		if (y != NIL) {
			y.parent = x.parent;
		}
		if (x.parent != null) {
			if (x == x.parent.left) {
				x.parent.left = y;
			} else {
				x.parent.right = y;
			}
		} else {
			root = y;
		}
		y.left = x;
		if (x != NIL) {
			x.parent = y;
		}
	}

	// Rotate node x to the right
	private void rotateRight(Node x) {
		Node y = x.left;
		x.left = y.right;
		if (y.right != NIL) {
			y.right.parent = x;
		}
		if (y != NIL) {
			y.parent = x.parent;
		}
		if (x.parent != null) {
			if (x == x.parent.right) {
				x.parent.right = y;
			} else {
				x.parent.left = y;
			}
		} else {
			root = y;
		}
		y.right = x;
		if (x != NIL) {
			x.parent = y;
		}
	}

	// A sorted iterator over all the Node's in this tree.
	// This iterator is "fail-fast".
	private class NodeIterator implements Iterator {
		private Node node = null;
		private Node prev = null;
		private int modCount;

		NodeIterator() {
			modCount = TreeMap.this.modCount;
			nextNode();
		}

		public boolean hasNext() {
			if (modCount != TreeMap.this.modCount) {
				throw new ConcurrentModificationException();
			}
			return node != null;
		}

		public Object next() {
			if (modCount != TreeMap.this.modCount) {
				throw new ConcurrentModificationException();
			}
			if (node == null) {
				throw new NoSuchElementException();
			}
			prev = node;
			nextNode();
			return prev;
		}

		public void remove() {
			if (modCount != TreeMap.this.modCount) {
				throw new ConcurrentModificationException();
			}
			if (prev == null) {
				throw new IllegalStateException();
			}
			Object key = null;
			if (node != null) {
				key = node.key;
			}
			TreeMap.this.deleteNode(prev);
			modCount++;
			if (node != null) {
				node = find(key);	// is this required?
			}
			prev = null;
		}

		// Starting at any node in the tree, go to the next node
		private void nextNode() {
			if (node == null) {		// first time called
				if (root == NIL) {	// tree is empty
					return;
				}
				node = root;
			} else if (node.right != NIL) { // do right subtree
				node = node.right;
			} else {			// pop back up the tree
				while (true) {
					if (node.parent == null
					    || node == node.parent.left) {
						node = node.parent;
						return;
					}
					node = node.parent;
				}
			}
			while (node.left != NIL) {
				node = node.left;
			}
		}
	}
}

