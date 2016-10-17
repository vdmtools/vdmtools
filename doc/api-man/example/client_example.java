import org.omg.CORBA.*;
import java.io.*;

import jp.co.csk.vdm.toolbox.api.ToolboxClient;
import jp.co.csk.vdm.toolbox.api.corba.ToolboxAPI.*;
import jp.co.csk.vdm.toolbox.api.corba.VDM.*;

public class client_example 
{
  private static short client;
  private static VDMApplication app;

  private static final String VdmToolboxHomeWin=
      "C:\\Program Files\\The VDM-SL Toolbox v3.7.1\\examples";
  private static final String VppToolboxHomeWin=
      "C:\\Program Files\\The VDM++ Toolbox v6.7.17\\examples";
  private static final String VdmToolboxHome=
      "/home/vdm/toolbox/examples/sl";    
  private static final String VppToolboxHome=
      "/home/vdm/toolbox/examples/pp";
    
  public static void main(String args[])
  {
    try {
      //
      // Create ORB
      //

      String ToolboxHome = System.getProperty("TOOLBOXHOME");
      if (System.getProperty("VDMPP") == null) {
          app = (new ToolboxClient ()).getVDMApplication(args,
                                                         ToolType.SL_TOOLBOX);

	  if( null == ToolboxHome ) {
            if (System.getProperty("WIN") == null) 
                ToolboxHome = VdmToolboxHome;	 
            else 
              ToolboxHome = VdmToolboxHomeWin;
          }
      }
      else {
          app = (new ToolboxClient ()).getVDMApplication(args,
                                                         ToolType.PP_TOOLBOX);

	  if( null == ToolboxHome ) {
            if (System.getProperty("WIN") == null) 
              ToolboxHome = VppToolboxHome;
            else
              ToolboxHome = VppToolboxHomeWin;
          }
      }
      
      // Register the client in the Toolbox:
      
      client = app.Register();
      
      System.out.println ("registered: " + client);
            
      // First we acquire a handle to the VDMProject interface to
      // configure the current project:
            
      try{
        VDMProject prj = app.GetProject();
        prj.New();
                
        // Configure the project to contain the necessary files. 
        // The files must be located in the same directory as where
        // the VDM Toolbox was started. Otherwise the absolute path
        // to the files should be used

                
        if(app.Tool() == ToolType.SL_TOOLBOX){ 
          prj.AddFile(ToolboxHome + "/sort/sort.vdm");
        }
        else{
          prj.AddFile(ToolboxHome + "/sort/implsort.vpp"); 
          prj.AddFile(ToolboxHome + "/sort/sorter.vpp");
          prj.AddFile(ToolboxHome + "/sort/explsort.vpp");
          prj.AddFile(ToolboxHome + "/sort/mergesort.vpp");
          prj.AddFile(ToolboxHome + "/sort/sortmachine.vpp");
        }

        // Parse the files:
        VDMParser parser = app.GetParser();
        FileListHolder fl = new FileListHolder();
        int count = prj.GetFiles(fl);
        String flist[] = fl.value;
        
        // Parse the files in two different ways. First we traverse
        // the list of files and parses each file individually. 
        // (OK, I know that for the SL_TOOLBOX there is only one 
        // file configured, but it is fine for an illustration)
              
        System.out.println("Parsing files individually");
        for(int i=0; i<flist.length; i++){
          System.out.println(flist[i]);
          System.out.println("...Parsing...");
          if(parser.Parse(flist[i]))
            System.out.println("done.");
          else
            System.out.println("error.");
        }
              
        // And then we parse all files in one go:
        
        System.out.println("Parsing entire list...");
        parser.ParseList(flist);
        System.out.println("done.");
        
        // If errors were encountered during the parse they can now
        // be inspected:

        // The error handler

        VDMErrors errhandler = app.GetErrorHandler();  
        
        ErrorListHolder errs = new ErrorListHolder();
        // retrieve the sequence of errors
        int nerr = errhandler.GetErrors(errs); 
         jp.co.csk.vdm.toolbox.api.corba.ToolboxAPI.Error errlist[] = 
           errs.value;
        if(nerr>0){
          // Print the errors:
          System.out.println("errors: ");
          for(int i=0; i<errlist.length; i++){
            System.out.println(errlist[i].fname);
            System.out.println(errlist[i].line);
            System.out.println(errlist[i].msg);
          }
        }
              
        // Warnings can be queried similarly.
        
        // List the names and status of all modules:
        ListModules(app);
     
        // Type check all modules:

        VDMTypeChecker tchk = app.GetTypeChecker();
        ModuleListHolder moduleholder = new ModuleListHolder();
        prj.GetModules(moduleholder);
        String modules[] = moduleholder.value;
        System.out.println("Type checking all modules...");
        if(tchk.TypeCheckList(modules))
          System.out.println("done.");
        else
          System.out.println("errors.");
            
        // List the new status of all modules:
        ListModules(app);
              
              
        // Finally we will show how to use the interpreter. 
        
        System.out.println("Interpreter tests:");
              
        VDMInterpreter interp = app.GetInterpreter();

              
        // Call a function that computes primes:
        EchoPrimes(20, interp, app);

        // Secondly we show how to use Apply:
        // Construct a sequence of integers to be sorted. In order 
        // to do so we need a handle to the VDMFactory to produce 
        // VDM values:
    
        VDMFactory fact = app.GetVDMFactory();
              
        app.PushTag(client); // Tag all objects created from now on
        
        VDMSequence list = fact.MkSequence(client);
        
        VDMNumeric elem;
        for(int j=0; j<20; j++){
          elem = fact.MkNumeric(client, j);
          list.ImpPrepend(elem);
        }
              
        System.out.println("The sequence to be sorted: " + 
                         list.ToAscii());

        // Construct the argument list for the call. That is, 
        // construct a Sequence containing all arguments in the
        // right order:
        VDMSequence arg_l = fact.MkSequence(client);
              
        arg_l.ImpAppend(list);

        // Set Verbose to true, to show the results of using the
        // interpreter in the user interface:
              
        interp.Verbose(true);
        interp.Debug(true);
    
        // First initialise the interpreter
        System.out.println("About to initialize the interpreter");
        interp.Initialize();
              
        VDMGeneric g;
        if(app.Tool() == ToolType.SL_TOOLBOX){
          g = interp.Apply(client, "MergeSort", arg_l);
        }
        else{ // PP_TOOLBOX
          // First we create the main sort object:
          interp.EvalCmd("create o := new SortMachine()");
         
          // Next, the GoSorting method is called on this object:
          g = interp.Apply(client, "o.GoSorting", arg_l);
        }

        System.out.println("The sorted sequence: " + g.ToAscii());
              
        // Finally we iterate through the returned sequence to 
        // compute the sum of all the elements of the sequence:

        VDMSequence s = VDMSequenceHelper.narrow(g);
              
        VDMGenericHolder eholder = new VDMGenericHolder();
          
        int sum=0;
        for (int ii=s.First(eholder); ii != 0; ii=s.Next(eholder)) {
          VDMNumeric num = VDMNumericHelper.narrow(eholder.value);
          sum = sum + GetNumeric( num );
        }

        System.out.println("The sum of all the elements: " +  sum);
        
        EchoPrimes2(50, interp, app);
        
        app.DestroyTag(client);

        app.Unregister(client);
        System.exit(0);
      }
      catch(APIError err) {
        System.err.println("API error"+err.getMessage ());
        System.exit(1);
      }
    }
    catch
      (jp.co.csk.vdm.toolbox.api.ToolboxClient.CouldNotResolveObjectException ex) 
      {
        System.err.println(ex.getMessage());
        System.exit(1);
      }
    catch(COMM_FAILURE ex) {
      System.err.println(ex.getMessage());
      ex.printStackTrace();
      System.exit(1);
    }
  };

  public static void ListModules(VDMApplication app){
        
    try{
      // This function lists the modules and their status.
      // The project handle
        
      VDMProject prj = app.GetProject();
      
      // The Module Repository
      VDMModuleRepos repos = app.GetModuleRepos();
          
      ModuleListHolder ml = new ModuleListHolder();
      prj.GetModules(ml);
      String mlist[] = ml.value;
      System.out.println("Modules:");
          
      for(int i=0; i<mlist.length; i++){
              
        // This struct is used to hold the status of a module:
        ModuleStatusHolder stateholder = new ModuleStatusHolder();
        // Get the status of the i'th module 
        repos.Status(stateholder, mlist[i]);  
        ModuleStatus stat = stateholder.value;
        
        // Print the status.
        System.out.println(mlist[i]);
        System.out.println("SyntaxChecked: " + stat.SyntaxChecked);
        System.out.println("TypeChecked: " + stat.TypeChecked);
        System.out.println("Code generated: " + stat.CodeGenerated);
        System.out.println("PrettyPrinted: " + stat.PrettyPrinted);
      }
    }
    catch(APIError err) {
      System.err.println("API error");
      System.exit(1);
    }
  }
    

  public static void EchoPrimes(int n, VDMInterpreter interp, 
                                  VDMApplication app)
  {
    try{
      // Generates the sequence of primes below n and echoes the 
      // sequence to stdout.
        
      app.PushTag(client);
            
      // This Generic is used to hold the result from the 
      // interpreter.
      VDMGeneric g;
        
      // Use EvalExpression to compute the primes below 20
            
      String expr = "[e|e in set {1,...,"+n+"} &"+
                    " exists1 x in set {2,...,e} & e mod x = 0]";
      g = interp.EvalExpression(client,expr);
            
      if(g.IsSequence()){
        System.out.println("All primes below " + n + ": " + 
                           g.ToAscii());
      }
            
      VDMSequence s = VDMSequenceHelper.narrow(g);
      
      VDMGenericHolder eholder = new VDMGenericHolder();
            
      int sum=0;
      for (int ii=s.First(eholder); ii != 0; ii=s.Next(eholder)) {
        VDMNumeric num = VDMNumericHelper.narrow(eholder.value);
        sum = sum + GetNumeric( num );
      }
      System.out.println("The sum of all the primes: " + sum);
      app.DestroyTag(client); // Clean up...
    }
    catch(APIError err) {
      System.err.println("API error");
      System.exit(1);
    }
  }
    
  public static void EchoPrimes2(int n, VDMInterpreter interp, 
                                 VDMApplication app)
  {
    // Generates the sequence of primes below n and echoes the 
    // sequence to stdout.  
    // Additionally this function shows how GetCPPValue can be used
    // to transfer an entire VDM value from the toolbox to the 
    // client and convert it to a "real" Java value as declared in 
    // jp.co.csk.vdm.toolbox.VDM
        
    try{
      app.PushTag(client);
      
      // This VDMGeneric is used to hold the result from the 
      // interpreter.
      VDMGeneric g;
        
      // Use EvalExpression to compute the primes below 20
            
      String expr = "[e|e in set {1,...,"+n+"} &" +
                    " exists1 x in set {2,...,e} & e mod x = 0]";
      g = interp.EvalExpression(client,expr);
        
      if(g.IsSequence()){
        System.out.println("All primes below " + n + ": " + g.ToAscii());
      }
            
      VDMSequence s = VDMSequenceHelper.narrow(g);
            
      // Conversion to real Java VDM value!
            
      java.util.LinkedList sj = 
          new java.util.LinkedList ();
            
      VDMGenericHolder eholder = new VDMGenericHolder();
            
      // Convert the Generic g into a "real" Java Sequence value
            
      for (int ii=s.First(eholder); ii != 0; ii=s.Next(eholder)) {
        VDMNumeric num = VDMNumericHelper.narrow(eholder.value);
        sj.add(new Integer( GetNumeric( num ) ));
      }
            
      int sum=0;
      for (java.util.Iterator itr = sj.iterator(); 
           itr.hasNext();){
          Integer i = (Integer) itr.next ();
          sum = sum + i.intValue();
      }
      
      System.out.println("The sum of all the primes: " + sum);
      app.DestroyTag(client); // Clean up...
    }
    catch(APIError err) {
      System.err.println("API error");
      System.exit(1);
    }
  }

  public static int GetNumeric( VDMNumeric num )
  {
    byte[] b1 = num.GetCPPValue();
    try
    {
      InputStream is = new ByteArrayInputStream( b1 );
      int type = is.read(); 
      int c = -1;
      int last = -1;
      String str = "";
      while( true )
      {
        c = is.read();
        if ( ( c == -1 ) || ( c == ',' ) )
        {
          last = c;
          break;
        }
        str += Character.toString( (char)c );
      }
      return Integer.parseInt( str );
    }
    catch( Exception e )
    {
      return 0;
    }
  } 
}






