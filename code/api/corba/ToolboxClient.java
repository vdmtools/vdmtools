package jp.vdmtools.api;

import java.util.Properties;
import java.io.File;
import java.io.FileInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.FilenameFilter;

import jp.vdmtools.api.corba.VDM.VDMGeneric;
import jp.vdmtools.api.corba.ToolboxAPI.VDMApplication;
import jp.vdmtools.api.corba.ToolboxAPI.VDMApplicationHelper;
import jp.vdmtools.api.corba.ToolboxAPI.VDMInterpreter;
import jp.vdmtools.api.corba.ToolboxAPI.APIError;
import jp.vdmtools.api.corba.ToolboxAPI.ToolType;

/** The ToolboxClient class implements methods used to connect client
  applications to the VDMTools through the VDMTools CORBA API.
  <p>
  The method readRefFile is used to read the [vpp|vdm]ior.rf file created by
  the Toolbox last started on the local PC.
  <p>
  The location of this file is read from the property VDM_OBJECT_LOCATION.
  This should be set on the command-line when Java is run, since Java
  does not support environment variables.
  <p>
  The method getVDMApplication is used to establish the connection to the
  Toolbox denoted by the object reference string and return the CORBA
  objest reference VDMApplication.

  @version $Id: ToolboxClient.java,v 1.3 2005/05/11 07:02:24 vdmtools Exp $
  @see jp.vdmtools.api.corba.ToolboxAPI.VDMApplication
*/

public class ToolboxClient
{
  public static String toISO (String str)
  {
    try {
      return new String(str.getBytes(), "ISO-8859-1");
    }
    catch (Exception ex)
    {
      System.err.println("Can't convert `" + ex.getMessage() + "'");
      throw new RuntimeException();
    }
  }

  public static String fromISO (String str)
  {
    try {
      return new String(str.getBytes("ISO-8859-1"));
    }
    catch (Exception ex)
    {
      System.err.println("Can't convert `" + ex.getMessage() + "'");
      throw new RuntimeException();
    }
  }

  public class CouldNotResolveObjectException extends Exception
  {
    public CouldNotResolveObjectException (String message)
    {
      super (message);
    }
  }

  public class IORFilter implements FilenameFilter
  {
    private String iorFilename = null;

    public IORFilter(String name)
    {
      this.iorFilename = name;
    }

    public boolean accept(File dir, String name)
    {
      //return name.startsWith(iorFilename);
      return (name.equals(iorFilename));
    }
  }

  /** The method readRefFile is used to read the object.string file created by
      the Toolbox last started on the local PC.
      @return the content of the object.string file
  */
  String readRefFile (String fileName)
  {
    String homeDir = System.getProperty("user.home", "") + "/";
    String location = System.getProperty("VDM_OBJECT_LOCATION", homeDir);

    String ref = null;
    try {
      IORFilter filter = new IORFilter(fileName);
      File dir = new File(location);
      File[] files = dir.listFiles(filter);
      if (files.length > 0) {
        System.out.println("refFile: " + files[0]);
        FileInputStream file = new FileInputStream(files[0]);
        BufferedReader in = new BufferedReader(new InputStreamReader(file));
        ref = in.readLine();
        file.close();
      }
      else {
        throw new RuntimeException();
      }
    }
    catch(IOException ex) {
      System.err.println("Can't read from `" + ex.getMessage() + "'");
      throw new RuntimeException();
    }
    //  System.out.println("ref: " + ref);
    return ref;
  }

  org.omg.CORBA.Object resolveVDMApplication (org.omg.CORBA.ORB orb, ToolType toolType)
        throws CouldNotResolveObjectException
  {
    try {
      org.omg.CORBA.Object obj = orb.resolve_initial_references ("NameService");
      org.omg.CosNaming.NamingContext ctx = org.omg.CosNaming.NamingContextHelper.narrow (obj);

      org.omg.CosNaming.NameComponent ct = new org.omg.CosNaming.NameComponent ("VDMTOOLBOX", "TOOL");

      org.omg.CosNaming.NameComponent nc = null;

      if (toolType == ToolType.SL_TOOLBOX) {
        nc = new org.omg.CosNaming.NameComponent ("SL_TOOLBOX", "VDMApplication");
      }
      else {
        nc = new org.omg.CosNaming.NameComponent ("PP_TOOLBOX", "VDMApplication");
      }
      org.omg.CosNaming.NameComponent[] name = {ct, nc};

      return ctx.resolve (name);
    }
    catch (Exception e) {
      throw new CouldNotResolveObjectException ("Object not found in NameService");
    }
  }

  /** The method getVDMApplication is used to establish the connection to the
      Toolbox denoted by the object reference string and return the CORBA
      objest reference VDMApplication.
      @param args command line parameters that should be send to the ORB. The args
      parameter is usually empty.
      @param toolType the ToolType.
      @return a VDMApplication (CORBA proxy) object reference to the Toolbox.
      The result is null if the method could not establish a connection
      the Toolbox.
      @throws CouldNotResolveObjectException If object not found in NameService.
  */
  public VDMApplication getVDMApplication(String[] args, ToolType toolType)
      throws CouldNotResolveObjectException
  {
    return getVDMApplication(args, toolType, false);
  }

  /** The method getVDMApplication is used to establish the connection to the
      Toolbox denoted by the object reference string and return the CORBA
      objest reference VDMApplication.
      @param args command line parameters that should be send to the ORB. The args
      parameter is usually empty.
      @param toolType the ToolType.
      @param useNs if true then check NameService.
      @return a VDMApplication (CORBA proxy) object reference to the Toolbox.
      The result is null if the method could not establish a connection
      the Toolbox.
      @throws CouldNotResolveObjectException If object not found in NameService.
  */
  public VDMApplication getVDMApplication(String[] args, ToolType toolType, boolean useNs)
      throws CouldNotResolveObjectException
  {
    //
    // Create ORB
    //
    org.omg.CORBA.ORB orb = null;
    try {
      Properties props = new Properties ();
      // props.put ("org.omg.CORBA.ORBInitialPort", 2809);
      //props.put("org.omg.CORBA.ORBInitRef", "NameService=corbaname::localhost::2809");
      orb = org.omg.CORBA.ORB.init(args, props);
    }
    catch (Exception e) {
      e.printStackTrace (System.err);
      throw new CouldNotResolveObjectException (e.getMessage ());
    }

    org.omg.CORBA.Object obj = null;

    if (useNs) {
      try {
        obj = resolveVDMApplication (orb, toolType);
      }
      catch (CouldNotResolveObjectException e) {
        System.err.println ("Application was not registered in NameService");
      }
    }

    if (obj == null) {
      String ref = null;

      if (toolType == ToolType.SL_TOOLBOX) {
        ref = readRefFile ("vdmref.ior");
      }
      else {
        ref = readRefFile ("vppref.ior");
      }
      obj = orb.string_to_object(ref);

      if(obj == null) {
        throw new CouldNotResolveObjectException ("Could not resolve VDM Application object");
      }
    }

    VDMApplication app = VDMApplicationHelper.narrow(obj);
    if(app == null) {
      throw new RuntimeException();
    }
    else {
      return app;
    }
  }

  public void runExample (String args[])
  {
    try {
      VDMApplication app = getVDMApplication(args,ToolType.SL_TOOLBOX);
      short client = app.Register();

      //
      // The Example:
      // print [e|e in set {1,...,20} & exists1 x in set {2,...,e} & e mod x = 0]
      //
      try {
        VDMInterpreter ip = app.GetInterpreter();
        if(ip == null) {
          throw new RuntimeException();
        }
        ip.Verbose(true);

        app.PushTag(client);
        VDMGeneric g = ip.EvalExpression(client, "[e|e in set {1,...,20} & "+
                                                         "exists1 x in set {2,...,e} & e mod x = 0]");
        System.out.println(g.ToAscii());
        app.DestroyTag(client);

        app.Unregister(client);
        System.exit(0);
      }
      catch(APIError err) {
        System.err.println("API error");
        System.exit(1);
      }
    }
    catch(org.omg.CORBA.COMM_FAILURE ex) {
      System.err.println(ex.getMessage());
      ex.printStackTrace();
      System.exit(-1);
    }
    catch (CouldNotResolveObjectException e) {
      e.printStackTrace (System.err);
      System.exit (-1);
    }
  }

  /** Standalone example program. This method will connect to a running Toolbox
   *  and evaulate the expression:
   *  <p>
   *  [e|e in set {1,...,20} &amp; exists1 x in set {2,...,e} &amp; e mod x = 0]
   *  <p>
   *  The result of this evaluation will be printed to standard output.
   *
   * @param args Arguments
   */
  public static void main(String args[])
  {
    ToolboxClient toolboxClient = new ToolboxClient ();
    toolboxClient.runExample (args);
  }
}

