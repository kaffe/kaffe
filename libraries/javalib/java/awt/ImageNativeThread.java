package java.awt;

import java.awt.image.ImageConsumer;

public class ImageNativeThread
  extends Thread
{
	private static Object lock = new Object();
	private static ImageNativeThread tid;
	private static Image queue;

public void run() {
	for (;;) {
		Image in;
		synchronized (lock) {
			if (queue == null) {
				tid = null;
				break;
			}
			in = queue;
			queue = queue.next;
			in.next = null;
		}
		ImageConsumer ic = new ImageNativeConsumer(in);
		in.getSource().startProduction(ic);
	}
}

static void startAsyncProduction(Image img) {
	synchronized (lock) {
		img.next = null;
		if (queue == null) {
			queue = img;
		}
		else {
			Image q;
			for (q = queue; q.next != null; q = q.next)
				;
			q.next = img;
		}
		if (tid == null) {
			tid = new ImageNativeThread();
			tid.start();
		}
	}
}
}
