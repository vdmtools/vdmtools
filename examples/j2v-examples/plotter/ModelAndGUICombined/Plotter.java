/*
 * Curve Plotter is a Java application/applet allowing to draw
 * an arbitrary 2-dimension parametric curve in
 * the Cartesian coordinate system.
 * 
 * A curve is given as 
 * 
 * x(arg) = <expression>
 * y(arg) = <expression>
 * Min = <expression>
 * Max = <expression>
 * Delta = <expression>
 * X0 - the x-coordinate of the center of the drawing area
 * Y0 - the y-coordinate of the center of the drawing area
 * Scale - the number of screen dots in the coordinate unit
 * 
 * where 
 * 
 * arg is in {t| t in [Min, Max] : Exists k in Z : t = Min+k*Delta}
 * 
 * <expression> ::= {-} <summand> {+ <summand>} | {-} <summand> {- <summand>}
 * <summand>    ::= <multiplicand> {* <multiplicand>} | <multiplicand> {/ <multiplicand>}
 * <term>       ::= <atom> {^ <atom>}
 * <atom>       ::= <number> | arg | <function> ( <expression> ) | ( <expression> )
 * <function>   ::= E | PI | abs | acos | asin | atan | cos | ceil | cos |
 * 						  exp |  log |  sin | sqrt | tan | floor 
 * <number>     ::= <integer> [. <integer> ]	
 * <integer>    ::= <digit> { <digit> }
 * 
 * The application window contains the drawing area,
 * the text field to type x(arg), y(arg), Min, Max, Delta, and
 * buttons to save the current curve, read a saved curve, draw the current curve,
 * clear the drawing area.
 * 
 * A curve is stored in a plain text file containing textual presentation for
 * x(arg), y(arg), Min, Max, Delta, X0, Y0, Scale.
 *   
 * Author.		M.Lebedev (lebedev@cecmow.enet.dec.com) Date.		06.04.97
 * Updated.		M.Lebedev (Mikhail.Lebedev@ifad.dk)     Date.		19.11.2001
 *
 */

package gui;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.applet.*;

import core.*;
import TB.*;

public class Plotter extends Applet implements Runnable {
	Button startButton = null;
	PlotterFrame f = null;
	Thread t = null;

  public static void main(String args[]) {

 		TB tb = new TB(args);

    Plotter g = new Plotter();
	  g.f = new PlotterFrame();
  }

  public void init () {
    startButton = new Button("Start");
    setLayout(new BorderLayout());
    add("Center", startButton);
  }

	public void start() {
		if(t==null) t = new Thread(this);
		t.start();
	}

	public synchronized void run() {
		try {
			wait();
			System.out.println("after wait");
			f = new PlotterFrame();
		}
		catch(InterruptedException e) {
		}
	}

  public synchronized boolean action(Event event, Object what) {
    if (event.target instanceof Button) {
        notify();
    } 
    return true;
  }
}

class PlotterFrame extends Frame { // implements Runnable {
	Parser parser = null;

	public PlotterFrame() {
        super("Plotter window");

        //{{INIT_CONTROLS
        setLayout(new BorderLayout());
        addNotify();
        resize(insets().left + insets().right + 511, insets().top + insets().bottom + 604);
        drawPanel=new DrawPanel();
        add("Center", drawPanel);
        drawPanel.reshape(insets().left + 2,insets().top + 2,502,317);
        ControlPanel=new Panel();
        ControlPanel.setLayout(null);
        add("South", ControlPanel);
        ControlPanel.reshape(insets().left + 0,insets().top + 319,504,253);
        xtLabel=new Label("x(arg)");
        ControlPanel.add(xtLabel);
        xtLabel.reshape(7,13,34,25);
        xtEdit=new TextField(53);
        ControlPanel.add(xtEdit);
        xtEdit.reshape(56,13,441,25);
        label1=new Label("y(arg)");
        ControlPanel.add(label1);
        label1.reshape(7,45,34,26);
        ytEdit=new TextField(53);
        ControlPanel.add(ytEdit);
        ytEdit.reshape(56,45,441,26);
        tminLabel=new Label("Min");
        ControlPanel.add(tminLabel);
        tminLabel.reshape(7,78,35,25);
        tminEdit=new TextField(11);
        ControlPanel.add(tminEdit);
        tminEdit.reshape(56,78,98,25);
        tmaxLabel=new Label("Max");
        ControlPanel.add(tmaxLabel);
        tmaxLabel.reshape(7,110,35,26);
        tmaxEdit=new TextField(11);
        ControlPanel.add(tmaxEdit);
        tmaxEdit.reshape(56,110,98,26);
        label2=new Label("Delta");
        ControlPanel.add(label2);
        label2.reshape(7,142,35,27);
        dtEdit=new TextField(11);
        ControlPanel.add(dtEdit);
        dtEdit.reshape(56,142,98,27);
        x0Label=new Label("X0");
        ControlPanel.add(x0Label);
        x0Label.reshape(189,78,28,25);
        x0Edit=new TextField(13);
        ControlPanel.add(x0Edit);
        x0Edit.reshape(231,78,112,25);
        y0Label=new Label("Y0");
        ControlPanel.add(y0Label);
        y0Label.reshape(189,110,28,26);
        y0Edit=new TextField(13);
        ControlPanel.add(y0Edit);
        y0Edit.reshape(231,110,112,26);
        scaleLabel=new Label("Scale");
        ControlPanel.add(scaleLabel);
        scaleLabel.reshape(175,142,42,27);
        scaleEdit=new TextField(13);
        ControlPanel.add(scaleEdit);
        scaleEdit.reshape(231,142,112,27);
        openButton=new Button("Open");
        ControlPanel.add(openButton);
        openButton.reshape(370,78,45,25);
        saveButton=new Button("Save");
        ControlPanel.add(saveButton);
        saveButton.reshape(370,110,45,25);
        drawButton=new Button("Draw");
        ControlPanel.add(drawButton);
        drawButton.reshape(425,78,45,25);
        clearButton=new Button("Clear");
        ControlPanel.add(clearButton);
        clearButton.reshape(425,110,45,25);
        logEdit=new TextArea();
        ControlPanel.add(logEdit);
        logEdit.reshape(5,182,492,68);
        //}}

        //{{INIT_MENUS
        //}}

		xtEdit.setText("5*cos(arg)");
		ytEdit.setText("5*sin(3*arg)");
		x0Edit.setText("0.");
		y0Edit.setText("0.");
		scaleEdit.setText("10");
		tminEdit.setText("0.");
		tmaxEdit.setText("6.");
		dtEdit.setText("0.1");

		show();
    }

    public boolean handleEvent(Event event) {
//		System.out.println(event.toString());
    	if (event.id == Event.ACTION_EVENT && event.target == drawButton) {
    	    	clickedDrawButton();
    	    	return true;
    	}
    	else 
    	if (event.id == Event.ACTION_EVENT && event.target == clearButton) {
    	    	clickedClearButton();
    	    	return true;
    	}
    	else 
    	if (event.id == Event.ACTION_EVENT && event.target == saveButton) {
    	    	clickedSaveButton();
    	    	return true;
    	}
    	else 
    	if (event.id == Event.ACTION_EVENT && event.target == openButton) {
    	    	clickedOpenButton();
    	    	return true;
    	}
    	else 

    	if (event.id == Event.WINDOW_DESTROY) {
    	    System.exit(0);
    	    return true;
    	}
    	return super.handleEvent(event);
    }

    //{{DECLARE_MENUS
    //}}

    //{{DECLARE_CONTROLS
    DrawPanel drawPanel;
    Panel ControlPanel;
    Label xtLabel;
    TextField xtEdit;
    Label label1;
    TextField ytEdit;
    Label tminLabel;
    TextField tminEdit;
    Label tmaxLabel;
    TextField tmaxEdit;
    Label label2;
    TextField dtEdit;
    Label x0Label;
    TextField x0Edit;
    Label y0Label;
    TextField y0Edit;
    Label scaleLabel;
    TextField scaleEdit;
    Button openButton;
    Button saveButton;
    Button drawButton;
    Button clearButton;
    TextArea logEdit;
    //}}
    public void clickedOpenButton() {
        FileDialog fd
		= new FileDialog(
			this,
			"Open file with curve",
			FileDialog.LOAD);

	    fd.show();
	    if( fd.getFile() != null ) {
    		DataInputStream in;
    		FileInputStream fis = null;
			try {
    			fis = new FileInputStream(fd.getDirectory()+fd.getFile());
    			in = new DataInputStream(new BufferedInputStream(fis));
				xtEdit.setText(in.readLine());
				ytEdit.setText(in.readLine());
				x0Edit.setText(in.readLine());
				y0Edit.setText(in.readLine());
				scaleEdit.setText(in.readLine());
				tminEdit.setText(in.readLine());
				tmaxEdit.setText(in.readLine());
				dtEdit.setText(in.readLine());

                fis.close();
                in.close();
    		}
    		catch(FileNotFoundException e) {
    			log("FileNotFoundException: " + e.getMessage());
    		}
    		catch(IOException e) {
    			log("IOException: " + e.getMessage());
   			}
		}
    }
    public void clickedSaveButton() {
        FileDialog fd
		= new FileDialog(
			this,
			"Save curve nto a file",
			FileDialog.SAVE);

		fd.show();
		if( fd.getFile() != null ) {
   			FileOutputStream fos = null;
   			PrintStream out = null;
			try {
    			fos = new FileOutputStream(fd.getDirectory()+fd.getFile());
 				out = new PrintStream(new BufferedOutputStream(fos));

				out.println(xtEdit.getText());
				out.println(ytEdit.getText());
				out.println(x0Edit.getText());
				out.println(y0Edit.getText());
				out.println(scaleEdit.getText());
				out.println(tminEdit.getText());
				out.println(tmaxEdit.getText());
				out.println(dtEdit.getText());

                out.close();
				fos.close();
			}
			catch(FileNotFoundException e) {
    			log("FileNotFoundException: " + e.getMessage());
    		}
    		catch(IOException e) {
    			log("IOException: " + e.getMessage());
   			}
		}
    }
    public void clickedDrawButton() {
		if(drawButton.getLabel().equals("Stop")) {
			stopDraw();
		} else {
			drawButton.setLabel("Stop");
			try {
				drawPanel.onDraw(
					scaleEdit.getText(),
					x0Edit.getText(),
					y0Edit.getText(),
					tminEdit.getText(),
					tmaxEdit.getText(),
					dtEdit.getText(),
					xtEdit.getText(),
					ytEdit.getText()
				);
			}
			catch(MyException e) {
				log("Parsing. MyException: " + e.getMessage());
			}
			catch(NumberFormatException e) {
				log("Parsing. NumberFormatException: " + e.getMessage());
			}
			drawButton.setLabel("Draw");
		}
	}
    public void clickedClearButton() {
		drawPanel.icurve = null;
		drawPanel.repaint();
	}
	private void stopDraw() {
		drawButton.setLabel("Draw");
		drawPanel.icurve = null;
	}
	protected void log(String str)
    {
        logEdit.appendText(str + "\n");
    }
}

class DrawPanel extends Canvas {
	public double tmin, tmax, dt;
	public R rmid = null;
	R irmid = null;
	public int scale;
	Dimension d = null;
	int color = 9;

	static final Color colors[] = {
		Color.black,
		Color.blue, 
		Color.cyan, 
		Color.darkGray,
		Color.gray, 
		Color.green, 
		Color.lightGray, 
		Color.magenta, 
		Color.orange, 
		Color.pink, 
		Color.red, 
		Color.yellow
	};

	public Curve icurve = null;

	R ri0 = null;
	
	public DrawPanel() {
		setBackground(Color.white);
    }

	R r2ir(R r) {
		return r.sub(ri0).mul(scale);
	}

	R ri2r(R ir) {
		return ir.mul((double)(1./scale)).add(ri0);
	}
    
	public void onDraw(
		String scaleText,
		String x0Text,
		String y0Text,
		String tminText,
		String tmaxText,
		String dtText,
		String xtText,
		String ytText) throws MyException
	{
		scale = Integer.valueOf(scaleText).intValue();
		rmid = new R(Double.valueOf(x0Text).doubleValue(),
					 Double.valueOf(y0Text).doubleValue());
		
		Parser parser = new Parser();
		Calculator c = new Calculator();

		tmin = c.calculate(parser.parse(tminText), 0.);
		tmax = c.calculate(parser.parse(tmaxText), 0.);
		dt   = Double.valueOf(dtText).doubleValue();

		String programmX = parser.parse(xtText);
		String programmY = parser.parse(ytText);
		icurve = new Curve(programmX, programmY);

		repaint();
    }

	public void paint(Graphics g) {
		if( icurve != null ) {
			drawGraph(g);
		} else {
			d = size();
			g.setColor(getBackground());
			g.setPaintMode();
			g.clearRect(0, 0, d.width, d.height);
		}
	}

	public void update(Graphics g) {
		if( icurve != null ) {
			drawGraph(g);
		} else {
			d = size();
			g.setColor(getBackground());
			g.setPaintMode();
			g.clearRect(0, 0, d.width, d.height);
		}
	}
/*
    public boolean handleEvent(Event event) {
		System.out.println(event.toString());
		return true;
	}

	public synchronized void run() {
		repaint();
		try {
			wait();
		}
		catch(InterruptedException e) {
		}
	}
*/	
    public void drawGraph(Graphics g) {
		d = size();
		irmid = new R(d.width/2, -d.height/2);
		ri0 = rmid.sub(irmid.mul((double)(1./scale)));		

		color = (color + 1) % colors.length;
		if(colors[color].equals(getBackground()) || colors[color].equals(Color.blue))  {
			color = (color + 1) % colors.length;
		}
		
		g.setColor(Color.blue);
		g.setPaintMode();

//	draw axes...

		R ir0 = r2ir (new R(0., 0.)); 

		int ix0 = (int)ir0.x;
		int iy0 = -(int)ir0.y; 

		g.drawLine(ix0, 0, ix0, d.height);
		g.drawLine(0, iy0, d.width, iy0);

		int ix, iy;
		for(int i = 1; true; i++) {
			int ixmin = ix0-i*scale;
			int ixmax = ix0+i*scale;
			int iymin = iy0-i*scale;
			int iymax = iy0+i*scale;
			g.drawLine(ix0-2, iymax, ix0+2, iymax);
			g.drawLine(ix0-2, iymin, ix0+2, iymin);
			g.drawLine(ixmax, iy0-2, ixmax, iy0+2);
			g.drawLine(ixmin, iy0-2, ixmin, iy0+2);

			boolean b =	(ixmin < 0 || ixmin > d.width) &&
						(ixmax < 0 || ixmax > d.width) &&
						(iymax < 0 || iymax > d.height) &&
						(iymin < 0 || iymax > d.height);

			if(b) break;
		}

		g.setColor(colors[color]);

		drawCurve(g, tmin);
    }

	protected void drawCurve(Graphics g, double tt) {
		if(tt > tmax) return;

		double t = tt;

		try {
			R r = new R(icurve.x(t), icurve.y(t));
			R irnew = r2ir(r);
			R ir = irnew;
			for(t=t+dt; t <= tmax; t+=dt) {
				if(!isInside(irnew)) throw new ArithmeticException("Outside view");
				irnew = r2ir(r.setR(icurve.x(t), icurve.y(t)));
				g.drawLine((int)ir.x, -(int)ir.y, (int)irnew.x, -(int)irnew.y);
				ir = irnew;
			}
		}
		catch(MyException e) {
			System.out.println("Exception: "+e.getMessage());
			return;
		}
		catch(ArithmeticException  e) {
			drawCurve(g, t+dt);
		}
	}

	protected boolean isInside(R ir) {
		return (0<=ir.x && ir.x<=d.width) && (0<=-ir.y && -ir.y<=d.height);
	}
}

class R {
	public double x;
	public double y;

	public R() {
		x=0.;
		y=0.;
	}

	public R(double anX, double anY) {
		x=anX;
		y=anY;
	}

	public R(int anX, int anY) {
		x=(double)anX;
		y=(double)anY;
	}

	public R(R r) {
		x=r.x;
		y=r.y;
	}

	public R setR(double anX, double anY) {
		x=anX;
		y=anY;
		return this;
	}

	public R add(R r) {
		return new R(x+r.x, y+r.y);
	}

	public R sub(R r) {
		return new R(x-r.x, y-r.y);
	}

	public R mul(double s) {
		return new R(s*x, s*y);
	}
}


