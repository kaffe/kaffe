import java.awt.Toolkit;

public class DemoImageDict
  extends kaffe.awt.ImageDict
{
public DemoImageDict() {
	super();
	putImage( "Add", "back.jpg", false);
	putImage( "Remove", "forward.jpg", false);
	putImage( "Modify", "transfer.jpg", false);
	putImage( "Close", "hand.up.jpg", false);
}
}
