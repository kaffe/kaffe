package java.awt.image;

import java.util.Hashtable;

public class FilteredImageSource
  implements ImageProducer
{
	ImageProducer producer;
	ImageFilter filter;
	Hashtable table = new Hashtable();

public FilteredImageSource ( ImageProducer orig, ImageFilter imgf) {
	producer = orig;
	filter = imgf;
}

public synchronized void addConsumer ( ImageConsumer ic) {
	if ( table.containsKey( ic) )
		return;
	ImageFilter flt = filter.getFilterInstance( ic);
	table.put( ic, flt);
	producer.addConsumer( flt);
}

public synchronized boolean isConsumer ( ImageConsumer ic) {
	ImageFilter flt = (ImageFilter) table.get( ic);
	if ( flt != null) 
		return producer.isConsumer( flt);
	return false;
}

public synchronized void removeConsumer ( ImageConsumer ic) {
	ImageFilter flt = (ImageFilter) table.get( ic);
	if ( flt != null) {
		producer.removeConsumer( flt);
		table.remove( ic);
	}
}

public void requestTopDownLeftRightResend ( ImageConsumer ic) {
	ImageFilter flt = (ImageFilter) table.get( ic);
	if ( flt != null)
		producer.requestTopDownLeftRightResend( flt);
}

public void startProduction ( ImageConsumer ic) {
	ImageFilter flt = (ImageFilter) table.get( ic);
	if ( flt == null) {
		flt = filter.getFilterInstance( ic);
		table.put( ic, flt);
	}
	producer.startProduction( flt);
}
}
