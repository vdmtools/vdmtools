import java.awt.*;
import java.awt.event.*;

import java.rmi.RemoteException;


class KLVgui implements ActionListener, WindowListener {

  public KLVgui (String[] args) {
    // Overall KLV frame
    klv = new Frame();
    klv.setTitle ("KLV System");
    klv.setResizable (false); // maybe this should be true!
    klv.setLayout (new GridBagLayout ());
    GridBagConstraints klvgbc = new GridBagConstraints();
    klv.setBackground(Color.lightGray);

    MenuBar mb = new MenuBar();
    Menu options = new Menu ("Actions");
    resetmenu = new MenuItem ("Reset");
    resetmenu.addActionListener (this);
    quitmenu = new MenuItem ("Quit");
    quitmenu.addActionListener (this);
    options.add (resetmenu);
    options.add (quitmenu);
    mb.add (options);
    klv.setMenuBar (mb);

    // Canvas with train
    traingui = new TrainGui (this, trainLocation);
    klvgbc.gridx = 0; klvgbc.gridy = 0;
    klv.add(traingui, klvgbc);
    
    // Events panel
    Panel events = new Panel();
    events.setLayout (new GridBagLayout());
    GridBagConstraints evgbc = new GridBagConstraints();

    // Train length fields
    trainlenb = new Button ("Train Length");
    trainlenb.addActionListener (this);
    evgbc.gridx = 0; evgbc.gridy = 0;
    events.add (trainlenb, evgbc);
    trainlength = new TextField ("4",3);
    evgbc.gridx = 1; evgbc.gridy = 0;
    evgbc.insets = new Insets (0,0,0,10);
    events.add (trainlength, evgbc);
    
    // Train speed fields
    checkspeed = new Button ("Check Speed");
    checkspeed.addActionListener (this);
    evgbc.gridx = 2; evgbc.gridy = 0;
    evgbc.insets = new Insets (0,10,0,0);
    events.add(checkspeed, evgbc);
    speedinput = new TextField ("0",3);
    evgbc.gridx = 3; evgbc.gridy = 0;
    evgbc.insets = new Insets (0,0,0,10);
    events.add(speedinput, evgbc);

    // Step field
    step = new Button ("Step");
    step.addActionListener (this);
    evgbc.gridx = 4; evgbc.gridy = 0;
    evgbc.insets = new Insets (0,10,0,10);
    events.add (step, evgbc);

    // Release Emergency Break field
    releaseEB = new Button ("Release Emergency Break");
    releaseEB.addActionListener (this);
    evgbc.gridx = 5; evgbc.gridy = 0;
    evgbc.insets = new Insets (0,10,0,10);
    events.add(releaseEB, evgbc);
    
    klvgbc.gridx = 0; klvgbc.gridy = 1;
    klv.add (events, klvgbc);
    
    // Display panel
    Panel display = new Panel();
    display.setLayout (new GridBagLayout ());
    GridBagConstraints dpgbc = new GridBagConstraints();
    
    // Cab Display panel of Display panel
    Panel cabdisplay = new Panel();
    cabdisplay.setLayout (new GridLayout (4,1));
    cabdisplay.add (new Label("Cab Display")); //, cabdgbc);

    alarmlamp = new Button ("Alarm");
    alarmlamp.setBackground (Color.white);
    cabdisplay.add(alarmlamp);

    emerbreaklamp = new Button("Emergency Break");
    emerbreaklamp.setBackground (Color.white);
    cabdisplay.add(emerbreaklamp);

    grfaultlamp = new Button ("Ground Fault");
    grfaultlamp.setBackground (Color.white);
    cabdisplay.add(grfaultlamp);
    
    dpgbc.gridx = 0; dpgbc.gridy = 0;
    dpgbc.insets = new Insets (10,10,10,10);
    display.add(cabdisplay, dpgbc);
    
    // Max Speed panel of Display panel
    Panel maxspeedp = new Panel();
    maxspeedp.setLayout (new GridLayout(2,1));
    maxspeedp.add (new Label("Max Speed"));
    maxspeed = new Label ("0");  // maybe larger?
    maxspeedp.add(maxspeed);
    dpgbc.gridx = 1; dpgbc.gridy = 0;
    dpgbc.insets = new Insets (25,10,25,10);
    display.add (maxspeedp, dpgbc);
    
    // Message text area of Display panel
    Panel messagep = new Panel();
    messagep.setLayout (new GridBagLayout ());
    GridBagConstraints mesgbc = new GridBagConstraints ();
    mesgbc.gridx = 0; mesgbc.gridy = 0;
    messagep.add (new Label("Message"), mesgbc);
    message = new TextArea ("", 6, 35);  // maybe larger?
    mesgbc.gridx = 0; mesgbc.gridy = 1;
    messagep.add(message, mesgbc);
    dpgbc.gridx = 2; dpgbc.gridy = 0;
    dpgbc.insets = new Insets (10,10,10,10);
    display.add (messagep, dpgbc);
    
    // Announecements and Speed Restrictions areas of Display panel
    Panel annspr = new Panel();
    annspr.setLayout (new GridBagLayout ());
    GridBagConstraints annsprgbc = new GridBagConstraints ();
    annsprgbc.gridx = 0; annsprgbc.gridy = 0;
    annspr.add (new Label("Announcements"), annsprgbc);
    annsprgbc.gridx = 1; annsprgbc.gridy = 0;
    annspr.add (new Label("Speed Restrictions"), annsprgbc);
    annsprgbc.gridx = 0; annsprgbc.gridy = 1;
    announcements = new TextArea ("", 6, 16);
    annspr.add (announcements, annsprgbc);
    // scrollbar
    annsprgbc.gridx = 1; annsprgbc.gridy = 1;
    speedrestrs = new TextArea ("", 6, 16);
    annspr.add (speedrestrs, annsprgbc);
    // scrollbar
    dpgbc.gridx = 3; dpgbc.gridy = 0;
    dpgbc.insets = new Insets (10,10,10,10);
    display.add (annspr, dpgbc);

    klvgbc.gridx = 0; klvgbc.gridy = 2;
    klv.add (display, klvgbc);

    
    // Initialising the VDM KLV , BeaconWindow, and TrainGui parts
    vdmklv = new VDMKLVcomm(args, this);
    beaconw = new BeaconWindow (this);
    setTrainLength();
    int msp = vdmklv.getMaxSpeed();
    setMaxSpeed (msp);

    // Initialise beaconState
    for (int i = 0; i < TrainGui.noOfFields; i++) {
      beaconState[i][0] = BeaconWindow.NoValue;
      beaconState[i][1] = BeaconWindow.NoValue;
    }
    
    klv.pack ();
    klv.setVisible (true);
  }

  // Attributes of KLVgui
  VDMKLVcomm vdmklv;
  Frame klv;
  MenuItem quitmenu;
  MenuItem resetmenu;
  TextField speedinput;
  Button checkspeed;
  Button releaseEB;
  Button trainlenb;
  Button step;
  TextField trainlength;
  TrainGui traingui;
  BeaconWindow beaconw;
  Button alarmlamp, emerbreaklamp, grfaultlamp;
  Label maxspeed;
  TextArea message;
  TextArea announcements, speedrestrs;
  private static String[] oldargs;

  static final int beaconSize = 2;
  int[][] beaconState = new int[TrainGui.noOfFields][beaconSize];
  int trainLocation = 1;


  // Event handling operations

  public void actionPerformed (ActionEvent e) {
    setMessage ("");
    if (e.getSource() == checkspeed)
      checkSpeed ();
    else if (e.getSource() == releaseEB)
      releaseEmergencyBreak ();
    else if (e.getSource() == trainlenb)
      setTrainLength ();
    else if (e.getSource() == step)
      stepTrain();
    else if (e.getSource() == resetmenu)
      resetKLV ();
    else if (e.getSource() == quitmenu) {
      klv.dispose();
      vdmklv.close();
      System.exit (0);
    }
    else
      ;
  }

  public void windowClosing (WindowEvent e) {
    klv.dispose(); // release the resources of the window
    System.exit (0);
  }

  // The remaining of the operations from the WindowListener interface
  public void windowActivated (WindowEvent e) {}
  public void windowClosed (WindowEvent e) {}
  public void windowDeactivated (WindowEvent e) {}
  public void windowDeiconified (WindowEvent e) {}
  public void windowIconified (WindowEvent e) {}
  public void windowOpened(WindowEvent e) {}


  // Operations implementing the events.

  private void checkSpeed () {
    int sp = new Integer (speedinput.getText()).intValue();
    Boolean[] res = vdmklv.checkSpeed (sp);
    
    setCabDisplay (res[0].booleanValue(), res[1].booleanValue(), res[2].booleanValue());
  }

  private void releaseEmergencyBreak () {
    int sp = new Integer (speedinput.getText()).intValue();
    if (sp != 0) 
      setMessage ("Emergency break cannot be released since speed is not 0");
    else {
      Boolean[] res = vdmklv.releaseEmergencyBreak (sp);
      setCabDisplay (res[0].booleanValue(), res[1].booleanValue(), res[2].booleanValue());
    }
  }

  private void setTrainLength () {
    int tl = new Integer (trainlength.getText()).intValue();
    traingui.setTrainLength (tl);
  }

  private void stepTrain () {
    trainLocation++;
    int tl = new Integer (trainlength.getText()).intValue();
    if (trainLocation-tl+1 > 0 && trainLocation-tl+1 < TrainGui.noOfFields &&
        beaconState[trainLocation-tl+1][0] != BeaconWindow.NoValue) {
      tailMeetBeacon (beaconState[trainLocation-tl+1]);
    }
    if (trainLocation < TrainGui.noOfFields &&
        beaconState[trainLocation][0] != BeaconWindow.NoValue) {
      headMeetBeacon (beaconState[trainLocation]);
    }
    checkSpeed();
    traingui.step (trainLocation);
  }

  private void headMeetBeacon (int[] beacon) {
    if (beacon[0] == BeaconWindow.TIVD)
      meetTIV_D (beacon[1]);
    else if (beacon[0] == BeaconWindow.TIVE)
      headMeetTIV_E ();
    else if (beacon[0] == BeaconWindow.TIVA)
      meetTIV_A ();
    else if (beacon[0] == BeaconWindow.NoBeacon)
      noBeaconMet();
    else
      ;
  }

  private void tailMeetBeacon (int[] beacon) {
    if (beacon[0] == BeaconWindow.TIVE)
      tailMeetTIV_E ();
    else if (beacon[0] == BeaconWindow.FLTV)
      tailMeetFLTV ();
    else
      ;
  }

  private void meetTIV_D (int ts) {
    String[] res = vdmklv.meetTIV_D (ts);
    int msp = vdmklv.getMaxSpeed();
    String newanns = res[0];
    String newsprstrs = res[1];
    setAnnouncements (newanns);
    setSpeedRestrictions (newsprstrs);
    setMaxSpeed (msp);
  }

  private void meetTIV_A () {
    String[] res = vdmklv.meetTIV_A();
    int msp = vdmklv.getMaxSpeed();
    String newanns = res[0];
    String newsprstrs = res[1];

    // Convert VDM values into JAVA values.
    setAnnouncements (newanns);
    setSpeedRestrictions (newsprstrs);
    setMaxSpeed (msp);
  }

  private void headMeetTIV_E () {
    String[] res = vdmklv.headMeetTIV_E();
    int msp = vdmklv.getMaxSpeed();
    String newanns = res[0];
    String newsprstrs = res[1];

    // Convert VDM values into JAVA values.
    setAnnouncements (newanns);
    setSpeedRestrictions (newsprstrs);
    setMaxSpeed (msp);
  }

  private void tailMeetTIV_E () {
    String[] res = vdmklv.tailMeetTIV_E();
    int msp = vdmklv.getMaxSpeed();
    String newanns = res[0];
    String newsprstrs = res[1];

    // Convert VDM values into JAVA values.
    setAnnouncements (newanns);
    setSpeedRestrictions (newsprstrs);
    setMaxSpeed (msp);
  }

  private void tailMeetFLTV () {
    String[] res = vdmklv.tailMeetFLTV();
    int msp = vdmklv.getMaxSpeed();
    String newanns = res[0];
    String newsprstrs = res[1];

    // Convert VDM values into JAVA values.
    setAnnouncements (newanns);
    setSpeedRestrictions (newsprstrs);
    setMaxSpeed (msp);
  }

  private void noBeaconMet () {
    String[] res = vdmklv.noBeaconMet();
    int msp = vdmklv.getMaxSpeed();
    Boolean[] cabdisp = vdmklv.getCabDisplay ();
    String newanns = res[0];
    String newsprstrs = res[1];

    // Convert VDM values into JAVA values.
    setAnnouncements (newanns);
    setSpeedRestrictions (newsprstrs);
    setMaxSpeed (msp);
    setCabDisplay (cabdisp[0].booleanValue(), cabdisp[1].booleanValue(), 
                   cabdisp[2].booleanValue());
  }

  private void setCabDisplay (boolean al, boolean eb, boolean gf) {
    // set Alarm lamp
    if (al) {
      alarmlamp.setBackground(Color.red);
    }
    else {
      alarmlamp.setBackground (Color.white);
    }

    // set Emergency Break lamp
    if (eb) {
      emerbreaklamp.setBackground (Color.red);
    }
    else {
      emerbreaklamp.setBackground (Color.white);
    }

    // set Ground Fault lamp
    if (gf) {
      grfaultlamp.setBackground (Color.red);
    }
    else {
      grfaultlamp.setBackground (Color.white);
    }
  }

  private void setAnnouncements (String anns) {
    announcements.setText (anns);
  }
    
  private void setSpeedRestrictions (String sprstrs) {
    speedrestrs.setText (sprstrs);
  }

  private void setMaxSpeed (int msp) {
    maxspeed.setText (""+msp);
  }

  public void setMessage (String str) {
    message.setText (str);
  }


  void createBeacon (int x) {
    // klv.disable();
    beaconw.readInBeacon (x);
  }

  void addBeacon (int x, int[] beacon) {
    if (beaconState[x][0] == BeaconWindow.NoValue) {
      beaconState[x] = beacon;
      traingui.repaint();
    }
    else
      setMessage ("Field already contains a beacon");
  }

  int[] getBeacon (int x) {
    if (x >= 0 && x < TrainGui.noOfFields)
      return beaconState[x];
    else {
      setMessage ("Wrong index in getBeacon");
      int[] res = { BeaconWindow.NoValue, BeaconWindow.NoValue };
      return res;
    }
  }

  private void resetKLV () {
    speedinput.setText ("0");
    trainlength.setText ("3");
    alarmlamp.setBackground (Color.white);
    emerbreaklamp.setBackground (Color.white);
    grfaultlamp.setBackground (Color.white);
    message.setText("");
    announcements.setText("");
    speedrestrs.setText("");
    vdmklv.initToolbox();
    trainLocation = 1;
    int msp = vdmklv.getMaxSpeed();
    setMaxSpeed (msp);
    
    // Initialise beaconState
    for (int i = 0; i < TrainGui.noOfFields; i++) {
      beaconState[i][0] = BeaconWindow.NoValue;
      beaconState[i][1] = BeaconWindow.NoValue;
    }

    setTrainLength();
    traingui.setTrainLocation (trainLocation);
  }


  public static void main (String[] args) {
    KLVgui klvgui = new KLVgui(args);
  }

} // end class KLVgui
