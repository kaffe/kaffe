package kaffe.applet;

import java.applet.AudioClip;
import java.net.URL;

public class AudioPlayer
  implements AudioClip
{
	private boolean stop;
	private URL     url;

public AudioPlayer( URL url) {
	this.url = url;
}

public void loop() {
	stop = false;
	for (; !stop;) {
		play();
		try { Thread.sleep( 100); }
		catch ( InterruptedException _x ) {}
	}
}

public void play() {
	playFile( url.getFile() );
}

native static void playFile( String file );

public void stop() {
	stop = true;
}
}
