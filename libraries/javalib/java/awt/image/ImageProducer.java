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

package java.awt.image;

public interface ImageProducer {

void addConsumer ( ImageConsumer consumer );
boolean isConsumer ( ImageConsumer consumer );
void removeConsumer ( ImageConsumer consumer );
void requestTopDownLeftRightResend ( ImageConsumer consumer );
void startProduction ( ImageConsumer consumer );

}
