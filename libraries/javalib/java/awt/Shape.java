package java.awt;


/**
 * interface Shape - abstraction for a geometric object with a certain extend
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

import java.awt.geom.AffineTransform;
import java.awt.geom.PathIterator;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;

public interface Shape
{
    boolean contains(double x, double y);
    boolean contains(double x, double y, double w, double h);
    boolean contains(Point2D p);
    boolean contains(Rectangle2D r);
    Rectangle getBounds();
    Rectangle2D getBounds2D();
    PathIterator getPathIterator(AffineTransform at);
    PathIterator getPathIterator(AffineTransform at, double flatness);
    boolean intersects(double x, double y, double w, double h);
    boolean intersects(Rectangle2D r);
}
