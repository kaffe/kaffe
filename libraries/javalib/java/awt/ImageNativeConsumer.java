package java.awt;

import java.awt.Image;
import java.awt.image.ColorModel;
import java.awt.image.DirectColorModel;
import java.awt.image.ImageConsumer;
import java.awt.image.ImageObserver;
import java.awt.image.IndexColorModel;
import java.util.Hashtable;
import kaffe.util.Ptr;

public class ImageNativeConsumer
  implements ImageConsumer
{
	private Image img;

public ImageNativeConsumer(Image img) {
	this.img = img;
}

public void imageComplete ( int status ) {
	// System.out.println("ImageNativeConsumer: " + this);
	// System.out.println("imageComplete: status="+status);
	int s = 0;
	if ( status == SINGLEFRAMEDONE) {
		s |= ImageObserver.FRAMEBITS;
	}
	if ( status == STATICIMAGEDONE) {
		s |= ImageObserver.ALLBITS;
	}
	if ( status == IMAGEERROR) {
		s |= ImageObserver.ERROR;
	}
	if ( status == IMAGEABORTED) {
		s |= ImageObserver.ABORT;
	}
	img.stateChange(s, 0, 0, 0, 0);

	img.producer.removeConsumer(this);
}

public void setColorModel ( ColorModel model ) {
	/* Does nothing */
}

public void setDimensions ( int w, int h ) {
	if (w != img.width || h != img.height) {
		img.width = w;
		img.height = h;

		// we need a target for subsequent setPixel() calls
		img.nativeData = Toolkit.imgCreateImage( w, h);

		img.stateChange(ImageObserver.WIDTH|ImageObserver.HEIGHT, 0, 0, w, h);
	}
}

public void setHints ( int hints ) {
	/* Does nothing */
}

public void setPixels ( int x, int y, int w, int h, ColorModel model, byte[] pels, int off, int scans ) {
	// System.out.println("ImageNativeConsumer: " + this);
	// System.out.println("setPixels: x="+x+", y="+y+", w="+w+", h="+h+", off="+off+", scans="+scans);
	// System.out.println("byte[] len="+pels.length);
	if ( img.nativeData == null ) {
		// error, we did not get a setDimension call
		return;
	}

	if ( model instanceof IndexColorModel ) {
		IndexColorModel icm = (IndexColorModel) model;
		//{
		// for (int j = y; j < y+h; j += 4) {
		//  for (int i = x; i < x+w; i += 4) {
		//   byte v = pels[i+j*scans+off];
		//   System.out.print((char)((v&15)+'A'));
		//  }
		//  System.out.println();
		// }
		//}
		Toolkit.imgSetIdxPels(img.nativeData, x, y, w, h, icm.rgbs, pels, icm.trans, off, scans);
		img.stateChange(ImageObserver.SOMEBITS, x, y, w, h);
	}
	else {
		System.err.println("Unhandled colorModel: " + model.getClass());
	}
}

public void setPixels ( int x, int y, int w, int h, ColorModel model, int[] pels, int off, int scans) {
	// System.out.println("ImageNativeConsumer: " + this);
	// System.out.println("setPixels: x="+x+", y="+y+", w="+w+", h="+h+", off="+off+", scans="+scans);
	// System.out.println("int[] len="+pels.length);
	if ( img.nativeData == null ) {
		// error, we did not get a setDimension call
		return;
	}

	if ( model instanceof DirectColorModel ) {
		if ( model != ColorModel.getRGBdefault() ){
				int xw = x + w;
				int yh = y + h;
				for (int j = y; j < yh; j++) {
					for (int i = x; i < xw; i++) {
						int idx = i+j*scans+off;
						pels[idx] = model.getRGB(pels[idx]);
					}
				}
			}

			Toolkit.imgSetRGBPels(img.nativeData, x, y, w, h, pels, off, scans);
			img.stateChange(ImageObserver.SOMEBITS, x, y, w, h);
		}
		else {
			System.out.println("Unhandled colorModel: " + model.getClass());
		}
	}

public void setProperties ( Hashtable properties ) {
	img.stateChange(ImageObserver.PROPERTIES, 0, 0, 0, 0);
}

void setRawData(Ptr ptr) {
	img.nativeData = ptr;
	if (ptr != null) {
		/* Set the height by hand so we don't overwrite the image
		 * by using setDimensions.
		 */
		img.width = Toolkit.imgGetWidth(ptr);
		img.height = Toolkit.imgGetHeight(ptr);
		img.stateChange(ImageObserver.WIDTH|ImageObserver.HEIGHT, 0, 0, img.width, img.height);
	}
}
}
