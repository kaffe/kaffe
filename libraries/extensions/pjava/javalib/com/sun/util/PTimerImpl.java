/*
 * PTimerImpl.java
 *
 * PersonalJava API 1.1
 *  This class interfaces the PersonalJava timers to the Kaffe timer system.
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */

package com.sun.util;

import java.util.Hashtable;
import kaffe.util.Timer;
import kaffe.util.TimerClient;

public class PTimerImpl
  extends PTimer {

private static PTimer singleton;

private Timer timer;
private Hashtable clients;

public PTimerImpl() {
	timer = Timer.getDefaultTimer();
	clients = new Hashtable();
}

public static PTimer getTimer() {
	if (singleton == null) {
		singleton = new PTimerImpl();
	}
	return (singleton);
}

public void schedule(PTimerSpec t) {
	long startWait = t.getTime();
	if (t.isAbsolute()) {
		startWait -= System.currentTimeMillis();
	}
	long interval = 0;
	if (t.isRepeat()) {
		interval = startWait;
	}
	Client tc = new Client(this, t);
	clients.put(t, tc);
	timer.addClient(tc, (int)startWait, (int)interval);
}

public void deschedule(PTimerSpec t) {
	timer.removeClient((Client)clients.get(t));
}

class Client implements TimerClient {

private PTimer ptimer;
private PTimerSpec spec;

Client(PTimer t, PTimerSpec s) {
	ptimer = t;
	spec = s;
}

public void timerExpired(Timer t) {
	spec.notifyListeners(ptimer);
}

}

}
