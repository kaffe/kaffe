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
	private TimerClientEntry[] clients;
	private int nClients;
	private int resolution;
	private boolean stop;
	private static Timer defaultTimer;

public Timer () {
	resolution = Integer.MAX_VALUE;
	
	clients = new TimerClientEntry[5];
	for ( int i=0; i<clients.length; i++ ) {
		clients[i] = new TimerClientEntry();
	}
	
	setDaemon( true);
	start();
}

public synchronized boolean addClient ( TimerClient tc, int startWait, int interval ) {
	TimerClientEntry tce;
	int i;

	// we allow just a single instance of a client
	// (otherwise we would need client ids to distinguish
	// notifications and removes)
	for ( i=0; i<nClients; i++ ){
		if ( clients[i].client == tc )
			return false;
	}

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
	
	return true;
}

public static synchronized Timer getDefaultTimer () {
	if ( defaultTimer == null ) {
		defaultTimer = new Timer();
	}
	
	return defaultTimer;
}

public synchronized boolean removeClient ( TimerClient tc ) {
	int newres;

	for (int i=0; i < nClients; i++ ) {
		TimerClientEntry tce = clients[i];

		if ( tce.client == tc ) {
			int i1 = i+1;
			int nmax = nClients-1;

			tce.client = null;  // don't leak
			
			if (i1 < nmax)
				System.arraycopy( clients, i1, clients, i, (nmax-i));
			clients[--nClients] = tce;
			
			// adapt the timer resolution to the lowest remaining interval
			if ( (resolution == tce.interval) && (nClients > 0) ) {
				for ( i=0, newres=Integer.MAX_VALUE; i<nClients; i++ ){
					if ( clients[i].interval < newres )
						newres = clients[i].interval;
				}
				// No need to interrupt, this just increases the interval
				// and takes effect after the next tick automatically.
				// in case it was the last entry, we go to wait, anyway
				// (but NOT here, we might be in a different thread)
				if ( newres != resolution )
					resolution = newres;
			}

			return true;
		}
	}
	
	return false;
}

public void run () {
	long t;
	int  i;

	while ( !stop ) {
		try {
			while ( !stop ) {
				synchronized ( this ) {
					if ( nClients == 0 ){
						wait();
					}

					t = System.currentTimeMillis();
					for ( i=0; i<nClients; i++ ) {
						TimerClientEntry tce = clients[i];

						if ( t > tce.nextNotify ) {
							tce.client.timerExpired( this);
							tce.nextNotify = t + tce.interval;
						}
					}
				}

				if ( nClients > 0 ){ // otherwise we go waiting, anyway
					Thread.sleep(resolution);
				}
			}
		}
		catch ( InterruptedException _ ) {
			// deliberately tolerated
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
