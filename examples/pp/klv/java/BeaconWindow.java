import java.awt.*;
import java.awt.event.*;

class BeaconWindow implements ActionListener, WindowListener {

  public BeaconWindow (KLVgui klvg) {
    beaconw = new Frame();
    beaconw.setTitle ("TIV_D Parameters");
    beaconw.setResizable (false);
    beaconw.setLayout (new GridBagLayout());
    GridBagConstraints beaconwgbc = new GridBagConstraints();
    beaconw.setBackground(Color.lightGray);

    Panel attrs = new Panel();
    attrs.setLayout (new GridBagLayout ());
    GridBagConstraints attgbc = new GridBagConstraints();
    attgbc.gridx = 0; attgbc.gridy = 0;
    attgbc.anchor = GridBagConstraints.WEST;
    attrs.add(new Label ("Target speed"), attgbc);
    attgbc.gridx = 0; attgbc.gridy = 1;
    attgbc.anchor = GridBagConstraints.WEST;
    targetspeed = new TextField ("0", 3);
    attrs.add (targetspeed, attgbc);

    beaconwgbc.gridx = 0; beaconwgbc.gridy = 0;
    beaconw.add(attrs, beaconwgbc);

    tivd = new Button ("TIV_D");
    tivd.addActionListener (this);
    beaconwgbc.gridx = 1; beaconwgbc.gridy = 0;
    beaconw.add(tivd, beaconwgbc);

    tive = new Button ("TIV_E");
    tive.addActionListener (this);
    beaconwgbc.gridx = 0; beaconwgbc.gridy = 1;
    beaconw.add(tive, beaconwgbc);

    tiva = new Button ("TIV_A");
    tiva.addActionListener (this);
    beaconwgbc.gridx = 0; beaconwgbc.gridy = 2;
    beaconw.add(tiva, beaconwgbc);

    fltv = new Button ("FLTV");
    fltv.addActionListener (this);
    beaconwgbc.gridx = 1; beaconwgbc.gridy = 1;
    beaconw.add(fltv, beaconwgbc);

    nobeacon = new Button ("No Beacon");
    nobeacon.addActionListener (this);
    beaconwgbc.gridx = 1; beaconwgbc.gridy = 2;
    beaconw.add(nobeacon, beaconwgbc);

    cancel = new Button ("Cancel");
    cancel.addActionListener (this);
    beaconwgbc.gridx = 1; beaconwgbc.gridy = 3;
    beaconw.add(cancel, beaconwgbc);

    klvgui = klvg;

    beaconw.pack ();
    beaconw.setVisible(false);
  }

  Frame beaconw;
  TextField targetspeed;
  Button tivd, tive, tiva, fltv, nobeacon, cancel;
  int itargetspeed = 0;
  int beaconKey = 0;

  public static final int NoValue = 0;
  public static final int TIVD = 1;
  public static final int TIVE = 2;
  public static final int TIVA = 3;
  public static final int FLTV = 4;
  public static final int NoBeacon = 5;


  KLVgui klvgui;

  // Event handling operations

  public void actionPerformed (ActionEvent ev) {
    if (ev.getSource() == tivd) {
      try {
        int ts = new Integer (targetspeed.getText()).intValue();
        int[] beacon = { TIVD, ts };
        klvgui.addBeacon (beaconKey, beacon);
      } catch (Exception ex) {                    // declivity catch
        klvgui.setMessage ("Target speed is not an integer.");
      }
      beaconw.setVisible(false);
    }
    else if (ev.getSource() == tive) {
      int[] beacon = { TIVE, NoValue };
      klvgui.addBeacon (beaconKey, beacon);
      beaconw.setVisible(false);
    }
    else if (ev.getSource() == tiva) {
      int[] beacon = { TIVA, NoValue };
      klvgui.addBeacon (beaconKey, beacon);
      beaconw.setVisible(false);
    }
    else if (ev.getSource() == fltv) {
      int[] beacon = { FLTV, NoValue };
      klvgui.addBeacon (beaconKey, beacon);
      beaconw.setVisible(false);
    }
    else if (ev.getSource() == nobeacon) {
      int[] beacon = { NoBeacon, NoValue };
      klvgui.addBeacon (beaconKey, beacon);
      beaconw.setVisible(false);
    }
    else
      beaconw.setVisible(false);
      ;
  }


  public void windowClosing (WindowEvent e) {
    beaconw.dispose(); // release the resources of the window
    System.exit (0);
  }

  // The remaining of the operations from the WindowListener interface
  public void windowActivated (WindowEvent e) {}
  public void windowClosed (WindowEvent e) {}
  public void windowDeactivated (WindowEvent e) {}
  public void windowDeiconified (WindowEvent e) {}
  public void windowIconified (WindowEvent e) {}
  public void windowOpened(WindowEvent e) {}

  void readInBeacon (int x) {
    beaconKey = x;
    beaconw.setVisible (true);
  }

  public static void main (String[] args) {
    new BeaconWindow (new KLVgui(args));
  }
    
}
