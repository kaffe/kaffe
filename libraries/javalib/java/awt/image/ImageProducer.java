package java.awt.image;


/**
 * ImageProducer - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */
public interface ImageProducer
{
public void addConsumer ( ImageConsumer consumer );

public boolean isConsumer ( ImageConsumer consumer );

public void removeConsumer ( ImageConsumer consumer );

public void requestTopDownLeftRightResend ( ImageConsumer consumer );

public void startProduction ( ImageConsumer consumer );
}
