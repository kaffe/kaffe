/*
 * Timer - simple Thread based Timer support
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.Mehlitz
 */

package kaffe.util;


public class Timer
  extends Thread
{
	TimerClientEntry[] clients;
	int nClients;
	int resolution;
	boolean stop;
	static Timer defaultTimer;

public Timer () {
	resolution = Integer.MAX_VALUE;
	
	clients = new TimerClientEntry[5];
	for ( int i=0; i<clients.length; i++ ) {
		clients[i] = new TimerClientEntry();
	}
	
	start();
}

public synchronized void addClient ( TimerClient tc, int startWait, int interval ) {
	TimerClientEntry tce;
	int i;

	if ( nClients == clients.length ){
		TimerClientEntry[] newClients = new TimerClientEntry[clients.length+10];
		System.arraycopy( clients, 0, newClients, 0, nClients);
		clients = newClients;
		for ( i=nClients; i<clients.length; i++ )
			clients[i] = new TimerClientEntry();
	}
	
	tce = clients[nClients++];
	tce.client = tc;
	tce.nextNotify = System.currentTimeMillis() + startWait;
	tce.interval = interval;

	// If interval is smaller than current resolution, drop current
	// resolution and wake the timer thread.
	if (interval < resolution) {
		resolution = interval;
		interrupt();
	}
	
	if ( nClients == 1 ) {
		notify();
	}
}

public static Timer getDefaultTimer () {
	if ( defaultTimer == null ) {
		defaultTimer = new Timer();
	}
	
	return defaultTimer;
}

public synchronized void removeClient ( TimerClient tc ) {

	int newres = Integer.MAX_VALUE;

	nClients--;
	for (int i=0; i <= nClients; i++ ) {
		TimerClientEntry tce = clients[i];
		if (tce.interval < newres) {
			newres = tce.interval;
		}
		if ( tce.client == tc ) {
			tce.client = null;
			int i1 = i+1;
			if (i1 < nClients) {
				System.arraycopy( clients, i1, clients, i, (nClients-i));
				clients[nClients] = tce;
			}
			break;
		}
	}

	// Reset the resultion - takes effect after next timer tick
	resolution = newres;
}

public void run () {
	long t;
	int  i;

	while ( !stop ) {
		try {
			while ( !stop ) {
				synchronized ( this ) {
					if ( nClients == 0 ) wait();

					t = System.currentTimeMillis();
					for ( i=0; i<nClients; i++ ) {
						TimerClientEntry tce = clients[i];

						if ( t > tce.nextNotify ) {
							tce.client.timerExpired( this);
							tce.nextNotify = t + tce.interval;
						}
					}
				}
				try {
					Thread.sleep(resolution);
				}
				catch (InterruptedException _) {
				}
			}
		}
		catch ( Exception x ) {
			x.printStackTrace();
		}
	}
}

public void startNotify () {
	stop = false;
	start();
}

public void stopNotify () {
	stop = true;
}
}

class TimerClientEntry
{
	TimerClient client;
	long nextNotify;
	int interval;

}
