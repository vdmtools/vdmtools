package jp.vdmtools.api;

import org.omg.CORBA.*;
import java.io.*;

import jp.vdmtools.api.corba.*;
import jp.vdmtools.api.corba.VDM.*;

/** The ToolboxClient class implements methods used to connect client
  applications to the VDMTools through the VDMTools CORBA API.
  <p>
  The method readRefFile is used to read the object.string file created by 
  the Toolbox last started on the local PC.
  <p>
  The method getVDMApplication is used to establish the connection to the 
  Toolbox denoted by the object reference string and return the CORBA
  objest reference VDMApplication.

  @version $Id: ToolboxClient.java,v 1.9 2005/05/11 07:02:24 vdmtools Exp $
  @see jp.co.vdmtools.api.corba.VDMApplication
*/

public class ToolboxClient
{

  /** The method readRefFile is used to read the object.string file created by 
    the Toolbox last started on the local PC.
    @return the content of the object.string file
    */
  public static String readRefFile() 
  {
    String winnt = "Windows NT";
    String win95 = "Windows 95";
    String linux = "Linux";
    String solaris = "Solaris";

    String obj = "object.string";
    String os = System.getProperty("os.name");
    String home = System.getProperty("user.home");
    String sep = System.getProperty("file.separator");

    String refFile;

    // ?? test on Win95/98
    if (os.equalsIgnoreCase(winnt) || os.equalsIgnoreCase(win95))
      refFile = home + sep + obj;
    //    else if (os.equalsIgnoreCase(win95)) 
    //      refFile = "C:" + sep + obj;
    else
      // other operating systems
      refFile = home + sep + obj;
    String ref = null;
      System.out.println("refFile: " + refFile);    
    try {
      FileInputStream file = new FileInputStream(refFile);
      BufferedReader in =
	new BufferedReader(new InputStreamReader(file));
      ref = in.readLine();
      file.close();
    }
    catch(IOException ex) {
      System.err.println("Can't read from `" +
			 ex.getMessage() + "'");
      throw new RuntimeException();
    }
    //    System.out.println("ref: " + ref);    
    return ref;
  }

  /** The method getVDMApplication is used to establish the connection to the 
    Toolbox denoted by the object reference string and return the CORBA
    objest reference VDMApplication.
    @param args command line parameters that should be send to the ORB. The args
           parameter is usually empty.
    @param ref the CORBA objest reference string read using readRefFile.
    @return a VDMApplication (CORBA proxy) object reference to the Toolbox. 
            The result is null if the method could not establish a connection
	    the Toolbox.
   */
  static public VDMApplication getVDMApplication(String[] args, String ref) 
  {
    //
    // Create ORB
    //
    ORB orb = ORB.init(args, new java.util.Properties());
    
    org.omg.CORBA.Object obj = orb.string_to_object(ref);

    if(obj == null)
      throw new RuntimeException();
    
    VDMApplication app = VDMApplicationHelper.narrow(obj);
    if(app == null)
      throw new RuntimeException();
    else
      return app;
  }

  /** Standalone example program. This method will connect to a running Toolbox
    and evaulate the expression:
    <p>
    [e|e in set {1,...,20} & exists1 x in set {2,...,e} & e mod x = 0]
    <p>
    The result of this evaluation will be printed to standard output.
    */
  public static void main(String args[])
  {
    try {
      //
      // Get "toolbox" object reference string
      //
      String ref = readRefFile();
      
      //
      // Create ORB
      //
      VDMApplication app = getVDMApplication(args,ref);
      short client = app.Register();
      
      //
      // The Example: 
      // print [e|e in set {1,...,20} & exists1 x in set {2,...,e} & 
      //                                e mod x = 0]
      //
      try {
	VDMInterpreter ip = app.GetInterpreter();
	if(ip == null)
	  throw new RuntimeException();
	ip.Verbose(true);
	
	app.PushTag(client);
	Generic g = ip.EvalExpression(client,"[e|e in set {1,...,20} & exists1 x in set {2,...,e} & e mod x = 0]");
	System.out.println(g.ascii());
	app.DestroyTag(client);
	
        app.Unregister(client);
	System.exit(0);
      }
      catch(APIError err) {
	System.err.println("API error");
	System.exit(1);
      }
    }
    catch(COMM_FAILURE ex) {
      System.err.println(ex.getMessage());
      ex.printStackTrace();
      System.exit(1);
    }
  }
}




