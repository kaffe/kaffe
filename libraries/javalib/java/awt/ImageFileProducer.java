package java.awt;

import java.lang.String;
import java.awt.Toolkit;
import java.awt.image.ImageConsumer;
import java.io.File;
import kaffe.util.Ptr;

public class ImageFileProducer
  extends ImageNativeProducer
{
	private File file;

public ImageFileProducer(File f) {
	super();
	file = f;
}

public void startProduction (ImageConsumer consumer ) {
	addConsumer( consumer);

	String fileName = file.getAbsolutePath();
	if (!file.exists()) {
		imageComplete(ImageConsumer.IMAGEERROR);
	}
	else {
		Ptr data = Toolkit.imgCreateFromFile(fileName);
		if (data != null) {
			setRawData(data);
			imageComplete(ImageConsumer.STATICIMAGEDONE);
		}
		else {
			imageComplete(ImageConsumer.IMAGEERROR|ImageConsumer.IMAGEABORTED);
		}
	}
}
}
