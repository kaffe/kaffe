package java.awt;


/**
 * interface LayoutManager - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public interface LayoutManager
{
void addLayoutComponent ( String name, Component child );

void layoutContainer ( Container parent );

Dimension minimumLayoutSize ( Container parent );

Dimension preferredLayoutSize ( Container parent );

void removeLayoutComponent ( Component child );
}
