package java.awt;

import java.awt.Toolkit;
import java.awt.image.ImageConsumer;
import java.io.InputStream;
import java.net.URL;
import kaffe.io.AccessibleBAOStream;
import kaffe.util.Ptr;

public class ImageURLProducer
  extends ImageNativeProducer
{
	private URL url;

public ImageURLProducer(URL u) {
	url = u;
}

public void startProduction (ImageConsumer consumer ) {
	addConsumer( consumer);

	byte[] buf = new byte[1024];
	int n;

	// since this is most likely used in a browser context (no file
	// system), the onlything we can do (in the absence of a suspendable
	// native image production) is to temporarily store the data in
	// memory. Note that this is done via kaffe.io.AccessibleBAOStream,
	// to prevent the inacceptable memory consumption duplication of
	// "toByteArray()".
	// Ideally, we would have a suspendable image production (that can
	// deal with reading and processing "incomplete" data), but that
	// simply isn't supported by many native image conversion libraries.
	// Some could be done in Java (at the expense of a significant speed
	// degradation - this is the classical native functionality), but
	// things like Jpeg ?
	try {
		InputStream in = url.openStream();
		if ( in == null ) {
			throw new Exception();
		}
		AccessibleBAOStream out = new AccessibleBAOStream(8192);
		while ((n = in.read(buf)) >= 0) {
			out.write(buf, 0, n);
		}
		in.close();
		Ptr data = Toolkit.imgCreateFromData(out.getBuffer(), 0, out.size());
		setRawData(data);
		imageComplete(ImageConsumer.STATICIMAGEDONE);
	}
	catch ( Exception x ) {
		imageComplete(ImageConsumer.IMAGEERROR|ImageConsumer.IMAGEABORTED);
	}
}
}
