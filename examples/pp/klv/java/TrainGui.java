import java.awt.*;
import java.awt.event.*;

class TrainGui extends Canvas implements MouseListener {

  public TrainGui (KLVgui klvg, int trainloc) {
    super();
    addMouseListener (this);
    klvgui = klvg;
    trainLocation = trainloc;
    setSize(900,100);
  }

  public void paint (Graphics g) {
    Rectangle bound = getBounds();
    setBackground (Color.white);
    g.clearRect (bound.x, bound.y, bound.width, bound.height);
    g.setColor (Color.black);
    g.fillRect (0, bound.height/2, bound.width, trackHeight);
    for (int i = 1; i <= noOfFields; i++)
      g.drawLine (i*bound.width/noOfFields, bound.height/2+trackHeight,
                  i*bound.width/noOfFields, bound.height/2+2*trackHeight);
    
    for (int i = 0; i < noOfFields; i++) {
      drawBeacon (i, g);
    }

    drawTrainHead (trainLocation, g);
    for (int i = trainLocation-1; i >= 0 && i >= trainLocation-trainLength+1; i--)
      drawTrainWagon (i, g);
        

  }

  final static int noOfFields = 20;
  private final int trackHeight = 10;
  private final int beaconOffset = 5;
  private final int windowOffset = 10;
  private final int textOffset = 14;

  KLVgui klvgui;
  private int trainLocation;
  private int trainLength = 1;

  private void drawTrainHead (int tl, Graphics g) {
    Rectangle bound = getBounds();
    g.setColor (Color.blue);
    int l = (tl-3/2)*bound.width/noOfFields,
        t = 0,
        w = 2*bound.width/noOfFields,
        h = bound.height;
    g.fillArc (l, t, w, h, 0, 90);
  }

  private void drawTrainWagon (int tl, Graphics g) {
    Rectangle bound = getBounds();
    g.setColor (Color.blue);
    int x = tl*bound.width/noOfFields,
        y = 0,
        w = bound.width/noOfFields,
        h = bound.height/2;
    g.fillRect (x, y, w, h);
    g.setColor (Color.white);
    g.fillRect (x+windowOffset, y+windowOffset, w-2*windowOffset, h/2);
  }

  private void drawBeacon (int loc, Graphics g) {
    Rectangle bound = getBounds();
    g.setColor (Color.red);
    int x = loc*bound.width/noOfFields+beaconOffset,
      y = bound.height/2+3*trackHeight,
      w = bound.width/noOfFields-2*beaconOffset,
      h = bound.height-6*trackHeight;
    int[] beacon = klvgui.getBeacon(loc);
    switch (beacon[0]) {
    case BeaconWindow.TIVD:
      g.drawString ("TIV_D", x, y);
      g.drawString (""+beacon[1], x, y+textOffset);
      break;
    case BeaconWindow.TIVE:
      g.drawString ("TIV_E", x, y);
      break;
    case BeaconWindow.TIVA:
      g.drawString ("TIV_A", x, y);
      break;
    case BeaconWindow.FLTV:
      g.drawString ("FLTV", x, y);
      break;
    case BeaconWindow.NoBeacon:
      g.drawString ("No", x, y);
      g.drawString ("Beacon", x, y+textOffset);
      break;
    default:
      // Do nothing
    }
  }

  void step (int tl) {
    trainLocation = tl;
    repaint();
  }

  void setTrainLength (int tl) {
    trainLength = tl;
    repaint();
  }

  void setTrainLocation (int tl) {
    trainLocation = tl;
    repaint();
  }


  public static void main (String[] args) {
//    Frame f = new Frame();
//    f.setTitle("TrainGui");
//    //    f.setSize(300,100);
//    TrainGui traingui = new TrainGui(new KL,3);
//    f.add(traingui);
//    f.pack();
//    f.setVisible(true);
////    for (int i = 0; i < 25; i++) {
////      traingui.step(i);
////      try {
////        Thread.sleep(100);
////      } catch (Exception _) {
////      }
////    }
  }

  public void mousePressed (MouseEvent e) {
    Rectangle bound = getBounds();
    
    if (e.getY() > (bound.height/2)) {
      int fieldlen = bound.width/noOfFields;
      int x = e.getX()/fieldlen;
      klvgui.createBeacon (x);
    }   
  }

  public void mouseEntered (MouseEvent _) {}
  public void mouseExited (MouseEvent _) {}
  public void mouseClicked (MouseEvent _) {}
  public void mouseReleased (MouseEvent _) {}

}
