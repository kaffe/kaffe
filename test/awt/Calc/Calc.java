//*******************************
// Java script of Calculator
// << Calc.java >>
//===============================
// Ver 0.0  96/01/19  T.Yamazaki
//-------------------------------
// email : yamaza@st.rim.or.jp
//*******************************

import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.GridLayout;
import java.awt.Panel;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

//===================
// Calculator Applet
//===================
public class Calc
  extends Frame
  implements ActionListener, WindowListener, KeyListener
{
//--------
    // member
    //--------
	TextField text;
	String sText1;
	String sText2;
	double dReg1;
	double dReg2;
	double dMem;
	int sOperator;
	boolean isFixReg;
	boolean hasDec;
	final static int MREAD = -1;
	final static int MPLUS = -2;
	final static int MMINUS = -3;
	final static int NOOP = 0;
	Button[] buttons;

//-------------
// constructor
//-------------
public Calc() {
	super( "Calc");

	Button b;

	Panel pFrame = new Panel();
	pFrame.setLayout(new FlowLayout());

	text = new TextField("");
	//text.setForeground(Color.yellow);
	text.setEditable(false);
	text.addKeyListener( this);
	
	//text.setBackground(Color.black);
	Panel pCalc = new Panel();

	pCalc.setLayout(new BorderLayout(0, 5));
	pCalc.add("North", text);

	pFrame.add("Center", pCalc);

	Dimension dSize= pCalc.getSize();
	dSize.width = dSize.width + 20;
	dSize.height = dSize.height + 20;
	pFrame.setSize(dSize);

	Panel pKey = new Panel();
	pKey.setLayout(new GridLayout(5, 4, 5, 5));
	add("Center", pKey);

	addButtons( pKey);

	pCalc.add("Center", pKey);

	setLayout(new BorderLayout(0, 0));
	add("North", pFrame);
	//setBackground(Color.darkGray);

	dReg1 = 0.0;
	dReg2 = 0.0;
	dMem = 0.0;

	sOperator = NOOP;
	text.setText("0");
	isFixReg = true;
	hasDec = false;

	addWindowListener( this);
	addKeyListener( this);

	//setBounds(50,50,150,190);
	pack();
	//setResizable(false);
	setVisible( true);
}

private double Calculation ( int op, double dReg1, double dReg2 ) {
	switch ( op ) {
		case '+':			dReg1 = dReg1 + dReg2; break;
		case '-':     dReg1 = dReg1 - dReg2; break;
		case '*':     dReg1 = dReg1 * dReg2; break;
		case '/':     dReg1 = dReg1 / dReg2; break;
		default:      dReg1 = dReg2;
	}
	return dReg1;
}

public void action ( int cmd ) {
	Double dTemp;

	switch ( cmd ) {
		case 'C':
			dReg1 = 0.0;
			dReg2 = 0.0;
			sOperator = NOOP;
			text.setText("0");
			isFixReg = true;
			break;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			cmd -= '0';
			if (isFixReg)
				sText2 = Integer.toString( cmd);
			else 
				sText2 = text.getText() + cmd;
			text.setText(sText2);
			isFixReg = false;
			break;

		case '.':
			if (hasDec == false) {
				if (isFixReg)
					sText2 = "0.";
				else
					sText2 = text.getText() + ".";
				text.setText(sText2);
				hasDec = true;
				isFixReg = false;
			}
			break;

		//
		// operations
		//
		case '+': 	case '-':	case '*': 	case '/':	case '=':
			sText1 = text.getText();
			dReg2 = (Double.valueOf(sText1)).doubleValue();
			dReg1 = Calculation( sOperator, dReg1, dReg2);
			dTemp = new Double(dReg1);
			sText2 = dTemp.toString();
			text.setText(sText2);
			sOperator = cmd;
			isFixReg = true;
			hasDec = false;
			break;

		//
		// memory read operation
		//
		case MREAD:
			dTemp = new Double(dMem);
			sText2 = dTemp.toString();
			text.setText(sText2);
			sOperator = NOOP;
			isFixReg = true;
			hasDec = false;
			break;

		//
		// memory add operation
		//
		case MPLUS:
			sText1 = text.getText();
			dReg2 = (Double.valueOf(sText1)).doubleValue();
			dReg1 = Calculation( sOperator, dReg1, dReg2);
			dTemp = new Double(dReg1);
			sText2 = dTemp.toString();
			text.setText(sText2);

			dMem = dMem + dReg1;
			sOperator = NOOP;
			isFixReg = true;
			hasDec = false;
			break;

		//
		// memory sub operation
		//
		case MMINUS:
			sText1 = text.getText();
			dReg2 = (Double.valueOf(sText1)).doubleValue();
			dReg1 = Calculation( sOperator, dReg1, dReg2);

			dTemp = new Double(dReg1);
			sText2 = dTemp.toString();
			text.setText(sText2);
			dMem = dMem - dReg1;
			sOperator = NOOP;
			isFixReg = true;
			hasDec = false;
			break;
	}
}

public void actionPerformed ( ActionEvent e ) {
	//
	// numeric key input
	//
	String arg = e.getActionCommand();
	
	if ( "MR".equals( arg) )
		action( MREAD);
	else if ( "M+".equals( arg) )
		action( MPLUS);
	else if ( "M-".equals( arg) )
		action( MMINUS);
	else if ( arg.length() == 1 )
		action( arg.charAt( 0));
}

void addButtons ( Panel pKey ) {
	String[] labels  = { "C", "MR", "M-", "M+",
                       "7", "8", "9", "/",
                       "4", "5", "6", "*",
                       "1", "2", "3", "-",
                       "0", ".", "=", "+" };

	buttons = new Button[labels.length];

	for ( int i=0; i<labels.length; i++ ) {
		buttons[i] = new Button( labels[i]);
		pKey.add( buttons[i]);
		buttons[i].addActionListener( this);
		buttons[i].addKeyListener( this);
	}
}

public void keyPressed ( KeyEvent e ) {
	char c = e.getKeyChar();
	int  o = e.getKeyCode();
	int  cmd = -1;

	switch ( c ) {
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '/': case '*': case '=':	case '.':
			cmd = c;
			break;
		case '+':
			cmd = e.isControlDown() ? MPLUS : '+';
			break;
		case '-':
			cmd = e.isControlDown() ? MMINUS : '-';
			break;
			
		default:
			switch ( o ) {
			/* interferes with Button key interface (enter, space triggers action)
				case KeyEvent.VK_SPACE:
					cmd = '=';
					break;
			*/
				case KeyEvent.VK_BACK_SPACE:
				case KeyEvent.VK_DELETE:
				case KeyEvent.VK_ESCAPE:
					cmd = 'C';
					break;
				case KeyEvent.VK_PAGE_DOWN:
					cmd = MREAD;
					break;
			}
	}
	
	if ( cmd != -1 ) {
		setFocus( cmd);
		action( cmd);
	}
}

public void keyReleased ( KeyEvent e ) {
}

public void keyTyped ( KeyEvent e ) {
}

public static void main(String args[])
	{
	Calc app = new Calc();
}

void setFocus ( int cmd ) {
	int i;

	switch ( cmd ) {
		case 'C':    i=0; break;
		case MREAD:  i=1; break;
		case MMINUS: i=2; break;
		case MPLUS:  i=3; break;
		case '7':    i=4; break;
		case '8':    i=5; break;
		case '9':    i=6; break;
		case '/':    i=7; break;
		case '4':    i=8; break;
		case '5':    i=9; break;
		case '6':    i=10; break;
		case '*':    i=11; break;
		case '1':    i=12; break;
		case '2':    i=13; break;
		case '3':    i=14; break;
		case '-':    i=15; break;
		case '0':    i=16; break;
		case '.':    i=17; break;
		case '=':    i=18; break;
		case '+':    i=19; break;
		default: i=-1;
	}

	if ( i >= 0 ) {
		buttons[i].requestFocus();
	}	
}

public void windowActivated ( WindowEvent evt ) {
}

public void windowClosed ( WindowEvent evt ){
}

public void windowClosing ( WindowEvent evt ){
	dispose();
}

public void windowDeactivated ( WindowEvent evt ){
}

public void windowDeiconified ( WindowEvent evt ){
}

public void windowIconified ( WindowEvent evt ){
}

public void windowOpened ( WindowEvent evt ){
}
}
