import java.awt.event.*;
// import javax.swing.*;
import java.rmi.RemoteException;

// import the toolbox VDM Library
//import jp.vdmtools.VDM.*;

// import the toolbox API interface
import jp.vdmtools.api.ToolboxClient;
import jp.vdmtools.api.corba.*;
import jp.vdmtools.api.corba.VDM.*;
import org.omg.CORBA.*;
import java.io.*;

/**
  Interface to the VDM++ toolbox for the KLV system.
  It reads the klv.prj project file.
  */

public class VDMKLVcomm {
  
    private VDMApplication app = null;
    private VDMInterpreter ip = null;
    private KLVgui klvgui;
    private short client = -1; 

  public VDMKLVcomm(String args[], KLVgui klvg) {
    try {
      //
      // Get "toolbox" object reference string
      //
      String ref = ToolboxClient.readRefFile();
      
      //
      // Create ORB
      //
      app = ToolboxClient.getVDMApplication(args,ref);
      
      client = app.Register();

      ip = app.GetInterpreter();
      if(ip == null)
	throw new RuntimeException();
      ip.Verbose(true);

      // open the project
      VDMProject prj = app.GetProject();
      FileListHolder holder = new FileListHolder();
      String workdir = System.getProperty ("user.dir");
      int count = prj.GetFiles(holder);
      if (count == 0)
	// No project opened
	prj.Open(workdir+"\\klv.prj");

      // Configure the interpreter
      ip.Init();
      ip.EvalCmd("create klv := new KLV()");

      klvgui = klvg;

    }
    catch(COMM_FAILURE ex) {
      System.err.println(ex.getMessage());
      ex.printStackTrace();
      System.exit(1);
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
      err.printStackTrace();
      System.exit(1);
    }
  }

  public void initToolbox () {
    try {
      ip.Init();
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
      err.printStackTrace();
      System.exit(1);
    }
  }

  // Unregister with toolbox
  public void close() {
    app.Unregister(client);
  }

  public Boolean[] checkSpeed (int sp) {
    try {
      app.PushTag(client);
      ip.EvalCmd("print klv.checkSpeed("+sp+")");
      Generic g = ip.EvalExpression(client, "klv.getCabDisplay().getDisplay()");
      Tuple cabdisp = TupleHelper.narrow(g);
      Boolean[] res = { convBoolean(cabdisp.GetField(1)), 
                        convBoolean(cabdisp.GetField(2)), 
                        convBoolean(cabdisp.GetField(3)), };
      app.DestroyTag(client);
      return res;
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
    }
    catch (VDMError err) {
      System.err.println("VDM error: " + err.getMessage());
    }
    Boolean[] res = { new Boolean(false), new Boolean(false), new Boolean(false) };
    return res;
  }

  public Boolean[] releaseEmergencyBreak (int sp) {
    try {
      app.PushTag(client);
      Generic pre = ip.EvalExpression (client, "klv.pre_releaseEmergencyBreak("+sp+")");
      if (BoolHelper.narrow(pre).GetValue()) {
        ip.EvalCmd("print klv.releaseEmergencyBreak("+sp+")");
        Generic g = ip.EvalExpression(client, "klv.getCabDisplay().getDisplay()");
        Tuple cabdisp = TupleHelper.narrow(g);
        Boolean[] res = { convBoolean(cabdisp.GetField(1)), 
                          convBoolean(cabdisp.GetField(2)), 
                          convBoolean(cabdisp.GetField(3)), };
        app.DestroyTag(client);
        return res;
      }
      else {
        klvgui.setMessage ("'Release Emergency Break' pre-condition violation");
        app.DestroyTag(client);
      }
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
    }
    catch (VDMError err) {
      System.err.println("VDM error: " + err.getMessage());
    }
    Boolean[] res = { new Boolean(false), new Boolean(false), new Boolean(false) };
    return res;
  }

  public Boolean[] getCabDisplay () {
    try {
      app.PushTag(client);
      Generic g = ip.EvalExpression(client,"klv.getCabDisplay().getDisplay()");
      Tuple cabdisp = TupleHelper.narrow(g);
      Boolean[] res = { convBoolean(cabdisp.GetField(1)), 
                        convBoolean(cabdisp.GetField(2)), 
                        convBoolean(cabdisp.GetField(3)), };
      app.DestroyTag(client);
      return res;
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
    }
    catch (VDMError err) {
      System.err.println("VDM error: " + err.getMessage());
    }
    Boolean[] res = { new Boolean(false), new Boolean(false), new Boolean(false) };
    return res;
  }

  public int getMaxSpeed () {
    try {
      app.PushTag(client);
      Generic g = ip.EvalExpression(client, "klv.getMaxSpeed()");
      Integer i = convInteger(g);
      app.DestroyTag(client);
      return i.intValue();
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
    }
    catch (VDMError err) {
      System.err.println("VDM error: " + err.getMessage());
    }
    Boolean[] res = { new Boolean(false), new Boolean(false), new Boolean(false) };
    return 0;
  }

  public String[] meetTIV_D (int ts) {
    app.PushTag(client);
    return meetBeacon("print klv.headMeetsBeacon(new TIV_D().create("+ts+"))");
  }
  
  public String[] meetTIV_A () {
    try {
      app.PushTag(client);
      Generic pre = ip.EvalExpression (client, "klv.pre_deleteAnnouncements()");
      if (BoolHelper.narrow(pre).GetValue()) {
        return meetBeacon("print klv.headMeetsBeacon(new TIV_A())");
      }
      else {
        klvgui.setMessage ("'Meet TIV_A (Head)' pre-condition violation:\n"+
                           "no announcements present");
        app.DestroyTag(client);
      }
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
    }
    String[] res = { new String(), new String() };
    return res;
  }

  public String[] headMeetTIV_E () {
    try {
      app.PushTag(client);
      Generic pre = ip.EvalExpression (client, "klv.pre_addSpeedRestriction(new TIV_E())");
      if (BoolHelper.narrow(pre).GetValue()) {
        return meetBeacon("print klv.headMeetsBeacon(new TIV_E())");
      }
      else {
        klvgui.setMessage ("'Meet TIV_E (Head)' pre-condition violation:\n"+
                           "no speed restrictions present");
        app.DestroyTag(client);
      }
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
    }
    String[] res = { new String(), new String() };
    return res;
  }

  public String[] tailMeetTIV_E () {
    try {
      app.PushTag(client);
      Generic pre = ip.EvalExpression (client, "klv.pre_removeSpeedRestriction()");
      if (BoolHelper.narrow(pre).GetValue()) {
        return meetBeacon("print klv.tailMeetsBeacon(new TIV_E())");
      }
      else {
        klvgui.setMessage ("'Meet TIV_E (Tail)' pre-condition violation:\n"+
                           "no speed restrictions present");
        app.DestroyTag(client);
      }
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
    }
    String[] res = { new String(), new String() };
    return res;
  }

  public String[] tailMeetFLTV () {
    try {
      app.PushTag(client);
      Generic pre = ip.EvalExpression (client, "klv.pre_removeSpeedRestriction()");
      if (BoolHelper.narrow(pre).GetValue()) {
        return meetBeacon("print klv.tailMeetsBeacon(new FLTV())");
      }
      else {
        klvgui.setMessage ("'Meet FLTV (Tail)' pre-condition violation:\n"+
                           "no speed restrictions present");
        app.DestroyTag(client);
      }
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
    }
    String[] res = { new String(), new String() };
    return res;
  }

  private String[] meetBeacon (String beacon) {
    try {
      ip.EvalCmd(beacon);
      Generic a = ip.EvalExpression(client, "let anns = klv.getAnnouncements() in [ anns(i).getTargetSpeed() | i in set inds anns ]");
      Generic s = ip.EvalExpression(client, "let restrs = klv.getSpeedRestrictions() in [ restrs(i).getSpeedRestriction() | i in set inds restrs ]");
      Sequence anns = SequenceHelper.narrow(a);
      Sequence restrs = SequenceHelper.narrow(s);
      GenericHolder holder = new GenericHolder();
      String annstr = new String();
      String restrstr = new String();
      for (int ii = anns.First(holder); ii != 0; ii = anns.Next(holder)) {
        Double dts = convReal (holder.value);
        annstr += "TIV_D("+dts+")\n";
      }
      for (int ii = restrs.First(holder); ii != 0; ii = restrs.Next(holder)) {
        Double spr = convReal (holder.value);
        restrstr += "TIV_E("+spr+")\n";
      }
      String[] res = { annstr, restrstr };
      app.DestroyTag(client);
      return res;
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
    }
    catch (VDMError err) {
      System.err.println("VDM error: " + err.getMessage());
    }
    String[] res = { new String(), new String() };
    return res;
  }
      
  public String[] noBeaconMet () {
    try {
      app.PushTag(client);
      Generic pre = ip.EvalExpression (client, "klv.pre_noBeaconMet()");
      if (BoolHelper.narrow(pre).GetValue()) {
        ip.EvalCmd ("print klv.noBeaconMet()");
        Generic a = ip.EvalExpression(client, "let anns = klv.getAnnouncements() in [ anns(i).getInformation() | i in set inds anns ]");
        Generic s = ip.EvalExpression(client, "let restrs = klv.getSpeedRestrictions() in [ restrs(i).getSpeedRestriction() | i in set inds restrs ]");
        Sequence anns = SequenceHelper.narrow(a);
        Sequence restrs = SequenceHelper.narrow(s);
        GenericHolder holder = new GenericHolder();
        String annstr = new String();
        String restrstr = new String();
        for (int ii = anns.First(holder); ii != 0; ii = anns.Next(holder)) {
          Tuple ann = TupleHelper.narrow(holder.value);
          Double dtd = convReal (ann.GetField(1));
          Double dts = convReal (ann.GetField(2));
          Double dd = convReal (ann.GetField(3));
          annstr += "TIV_D("+dtd+","+dts+","+dd+")\n";
        }
        for (int ii = restrs.First(holder); ii != 0; ii = restrs.Next(holder)) {
          Double spr = convReal (holder.value);
          restrstr += "TIV_E("+spr+")\n";
        }
        String[] res = { annstr, restrstr };
        app.DestroyTag(client);
        return res;
      }
      else {
        klvgui.setMessage ("'No Beacon Met' pre-condition violation:\n"+
                           "no announcements precent");
        app.DestroyTag(client);
      }
    }
    catch (APIError err) {
      System.err.println("API error: " + err.getMessage()); 
    }
    catch (VDMError err) {
      System.err.println("VDM error: " + err.getMessage());
    }
    String[] res = { new String(), new String() };
    return res;
  }


  private Integer convInteger(Generic g) 
  throws APIError, VDMError {
    Numeric num = NumericHelper.narrow(g);
    return new Integer((int) Math.round(num.GetValue()));
  }

  private Double convReal(Generic g) 
  throws APIError, VDMError {
    Numeric num = NumericHelper.narrow(g);
    return new Double(num.GetValue());
  }

  private Boolean convBoolean(Generic g) 
  throws APIError, VDMError {
    Bool bool = BoolHelper.narrow(g);
    return new Boolean(bool.GetValue());
  }

  private String convString(Generic g) 
  throws APIError, VDMError {

    if (g.IsNil())
      return null;

    Sequence seq = SequenceHelper.narrow(g);
    if (seq.IsEmpty())
      return new String();
    else {
      // remove the surrounding qoutes
      String str = new String(seq.ascii().toCharArray(),1,seq.Length());
      return str;
    }
  }



}
