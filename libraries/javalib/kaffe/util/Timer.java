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

public Timer ( int msecInterval ) {
	resolution = msecInterval;
	
	clients = new TimerClientEntry[5];
	for ( int i=0; i<clients.length; i++ )
		clients[i] = new TimerClientEntry();
	
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
	
	if ( nClients == 1 )
		notify();
}

public static Timer getDefaultTimer () {
	if ( defaultTimer == null ) {
		defaultTimer = new Timer( 100);
	}
	
	return defaultTimer;
}

public synchronized void removeClient ( TimerClient tc ) {
	TimerClientEntry tce;
	int i, i1;

	for ( i=0; i<nClients; i++ ) {
		tce = clients[i];
		if ( tce.client == tc ) {
			i1 = i+1;
			if ( i1 < nClients-- ){
				System.arraycopy( clients, i1, clients, i, (nClients-i));
				clients[nClients] = tce;
			}
			tce.client = null;

			return;
		}
	}
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
				Thread.sleep( resolution);
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
