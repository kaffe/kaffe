package java.awt;

import java.awt.Toolkit;
import java.awt.image.ImageConsumer;
import kaffe.util.Ptr;

public class ImageDataProducer
  extends ImageNativeProducer
{
	private byte[] buf;
	private int off;
	private int len;

public ImageDataProducer(byte[] b, int o, int l) {
	super();
	buf = b;
	off = o;
	len = l;
}

public void startProduction (ImageConsumer consumer) {
	addConsumer( consumer);

	Ptr data = Toolkit.imgCreateFromData(buf, off, len);
	if (data != null) {
		setRawData(data);
		imageComplete(ImageConsumer.STATICIMAGEDONE);
	}
	else {
		imageComplete(ImageConsumer.IMAGEERROR|ImageConsumer.IMAGEABORTED);
	}
}
}
