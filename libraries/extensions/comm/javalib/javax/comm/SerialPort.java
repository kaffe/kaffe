/*
 * Java comm library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.comm;

import java.util.TooManyListenersException;

public abstract class SerialPort
  extends CommPort {

public static final int DATABITS_5 = 1;
public static final int DATABITS_6 = 2;
public static final int DATABITS_7 = 3;
public static final int DATABITS_8 = 4;
public static final int STOPBITS_1 = 5;
public static final int STOPBITS_2 = 6;
public static final int STOPBITS_1_5 = 7;
public static final int PARITY_NONE = 8;
public static final int PARITY_ODD = 9;
public static final int PARITY_EVEN = 10;
public static final int PARITY_MARK = 11;
public static final int PARITY_SPACE = 12;
public static final int FLOWCONTROL_NONE = 13;
public static final int FLOWCONTROL_RTSCTS_IN = 14;
public static final int FLOWCONTROL_RTSCTS_OUT = 15;
public static final int FLOWCONTROL_XONXOFF_IN = 16;
public static final int FLOWCONTROL_XONXOFF_OUT = 17;

public SerialPort() {
}

public abstract int getBaudRate();
public abstract int getDataBits();
public abstract int getStopBits();
public abstract int getParity();
public abstract void sendBreak(int millis);
public abstract void setFlowControlMode(int flowcontrol) throws UnsupportedCommOperationException;
public abstract int getFlowControlMode();
public abstract void setRcvFifoTrigger(int trigger);
public abstract void setSerialPortParams(int baudrate, int dataBits, int stopBits, int parity) throws UnsupportedCommOperationException;
public abstract void setDTR(boolean dtr);
public abstract boolean isDTR();
public abstract void setRTS(boolean rts);
public abstract boolean isRTS();
public abstract boolean isCTS();
public abstract boolean isDSR();
public abstract boolean isRI();
public abstract boolean isCD();
public abstract void addEventListener(SerialPortEventListener lsnr) throws TooManyListenersException;
public abstract void removeEventListener();
public abstract void notifyOnDataAvailable(boolean enable);
public abstract void notifyOnOutputEmpty(boolean enable);
public abstract void notifyOnCTS(boolean enable);
public abstract void notifyOnDSR(boolean enable);
public abstract void notifyOnRingIndicator(boolean enable);
public abstract void notifyOnCarrierDetect(boolean enable);
public abstract void notifyOnOverrunError(boolean enable);
public abstract void notifyOnParityError(boolean enable);
public abstract void notifyOnFramingError(boolean enable);
public abstract void notifyOnBreakInterrupt(boolean enable);

}
