/**
 * simple test for UDP send/receive.
 */
import java.io.*;
import java.net.*;

public class UDPTest {

    static final String msg = "UDPTest: Success.";

    static class UDPSend implements Runnable {
	int port;
	UDPSend(int port) {
	    this.port = port;
	}

	public void run() {
	    
	    try {
		// wait for receiver to get in receive
		Thread.sleep(2000);
		InetAddress address = InetAddress.getByName("127.0.0.1");
		byte[] message = msg.getBytes();
		int msglen = message.length;

		DatagramPacket packet = new DatagramPacket(message, msglen, 
				       address, port);
		DatagramSocket socket = new DatagramSocket();
		socket.send(packet);
	    } catch (Exception e) {
		System.out.println(e);
		e.printStackTrace(System.out);
	    }
	}
    }

    static class UDPReceive implements Runnable {
	public void run()
	{
	    try {
		byte[] buffer = new byte[1024];
		String s;
		// Create a socket to listen on the port.
		DatagramSocket socket = new DatagramSocket(0);
	    
		DatagramPacket packet = 
		    new DatagramPacket(buffer, buffer.length);

		new Thread(new UDPSend(socket.getLocalPort())).start();
		socket.receive(packet);
		s = new String(buffer, 0, packet.getLength());
		if (s.equals(msg))
		    System.out.println(s);
		else
		    System.out.println("Failure, got `" 
			+ s + "', expected `" + msg + "'");
	    } catch (Exception e) {
		System.out.println(e);
		e.printStackTrace(System.out);
	    }
	}
    }

    public static void main(String av[]) throws Exception
    {
	new Thread(new UDPReceive()).start();
    }
}

/* Expected Output:
UDPTest: Success.
*/
