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
float getLayoutAlignmentX ( Container parent );
float getLayoutAlignmentY ( Container parent );
void invalidateLayout ( Container parent );
Dimension maximumLayoutSize ( Container parent );

}
