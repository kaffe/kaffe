/**
 * LayoutManager2 - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */

package java.awt;

public interface LayoutManager2
  extends LayoutManager {

void addLayoutComponent ( Component child, Object constraints );
public float getLayoutAlignmentX ( Container parent );
public float getLayoutAlignmentY ( Container parent );
public void invalidateLayout ( Container parent );
public Dimension maximumLayoutSize ( Container parent );

}
