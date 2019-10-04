
import java.io.FilenameFilter;
import java.io.InputStream;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.util.LinkedList;
import java.util.Iterator;

import jp.vdmtools.api.ToolboxClient;

import jp.vdmtools.api.corba.VDM.VDMGeneric;
import jp.vdmtools.api.corba.VDM.VDMGenericHolder;
import jp.vdmtools.api.corba.VDM.VDMSequence;
import jp.vdmtools.api.corba.VDM.VDMSequenceHelper;
import jp.vdmtools.api.corba.VDM.VDMNumeric;
import jp.vdmtools.api.corba.VDM.VDMNumericHelper;
import jp.vdmtools.api.corba.VDM.VDMFactory;

import jp.vdmtools.api.corba.ToolboxAPI.VDMApplication;
import jp.vdmtools.api.corba.ToolboxAPI.VDMProject;
import jp.vdmtools.api.corba.ToolboxAPI.VDMParser;
import jp.vdmtools.api.corba.ToolboxAPI.VDMTypeChecker;
import jp.vdmtools.api.corba.ToolboxAPI.VDMInterpreter;
import jp.vdmtools.api.corba.ToolboxAPI.VDMErrors;
import jp.vdmtools.api.corba.ToolboxAPI.ErrorListHolder;
import jp.vdmtools.api.corba.ToolboxAPI.FileListHolder;
import jp.vdmtools.api.corba.ToolboxAPI.VDMModuleRepos;
import jp.vdmtools.api.corba.ToolboxAPI.ModuleStatus;
import jp.vdmtools.api.corba.ToolboxAPI.ModuleStatusHolder;
import jp.vdmtools.api.corba.ToolboxAPI.ModuleListHolder;
import jp.vdmtools.api.corba.ToolboxAPI.APIError;
import jp.vdmtools.api.corba.ToolboxAPI.ToolType;

import org.omg.CORBA.COMM_FAILURE;

public class client_example 
{
  public static void main(String args[])
  {
    try {
      String source_path = System.getenv("VDM_SOURCE_LOCATION");

      if (source_path == null)
      {
        File currentDirectory = new File(".");
        String pwd = currentDirectory.getAbsolutePath();
        int index = pwd.indexOf("api/corba");
        if (index > 0)
          source_path = pwd.substring(0, index);
        System.err.println("Environment variable VDM_SOURCE_LOCATION not set");
        System.err.println("Default location: " + source_path);
      }

      //
      // Create ORB
      //
      VDMApplication app;
      if (System.getProperty("VDMPP") == null)
        app = (new ToolboxClient ()).getVDMApplication(args, ToolType.SL_TOOLBOX);
      else
        app = (new ToolboxClient ()).getVDMApplication(args, ToolType.PP_TOOLBOX); // 
        //app = (new ToolboxClient ()).getVDMApplication(args, ToolType.PP_TOOLBOX, true); // use NameService
      
      // Register the client in the Toolbox:
      
      short client = app.Register();
      
      System.out.println ("registered: " + client);
            
      // First we acquire a handle to the VDMProject interface to
      // configure the current project:
            
      try {
        VDMProject prj = app.GetProject();

        prj.New(); // New project
                
        // Configure the project to contain the necessary files. 
        // The files must be located in the same directory as where
        // the VDM Toolbox was started. Otherwise the absolute path
        // to the files should be used

        if(app.Tool() == ToolType.SL_TOOLBOX) { 
          prj.AddFile(ToolboxClient.toISO(source_path + "sort.vdm"));
        }
        else {
          prj.AddFile(ToolboxClient.toISO(source_path + "implsort.vpp")); 
          prj.AddFile(ToolboxClient.toISO(source_path + "sorter.vpp"));
          prj.AddFile(ToolboxClient.toISO(source_path + "explsort.vpp"));
          prj.AddFile(ToolboxClient.toISO(source_path + "mergesort.vpp"));
          prj.AddFile(ToolboxClient.toISO(source_path + "sortmachine.vpp"));
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
        for(int i = 0; i < flist.length; i++) {
          System.out.println(ToolboxClient.fromISO(flist[i]));
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
        
        // If errors were encountered during the parse they can now be inspected:

        // The error handler

        VDMErrors errhandler = app.GetErrorHandler();  
        
        ErrorListHolder errs = new ErrorListHolder();
        // retrieve the sequence of errors
        int nerr = errhandler.GetErrors(errs); 
         jp.vdmtools.api.corba.ToolboxAPI.Error errlist[] = errs.value;
        if(nerr > 0) {
          // Print the errors:
          System.out.println("errors: ");
          for(int i = 0; i < errlist.length; i++) {
            System.out.println(ToolboxClient.fromISO(errlist[i].fname));
            System.out.println(errlist[i].line);
            System.out.println(ToolboxClient.fromISO(errlist[i].msg));
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
        System.out.println("Type checking all " + (app.Tool() == ToolType.SL_TOOLBOX ? "modules" : "classes") + "...");
        if(tchk.TypeCheckList(modules))
          System.out.println("done.");
        else
          System.out.println("errors.");
            
        // The warning handler
        VDMErrors warnhandler = app.GetErrorHandler();

        ErrorListHolder warns = new ErrorListHolder();
        // retrieve the sequence of errors
        int nwarn = errhandler.GetWarnings(warns);
         jp.vdmtools.api.corba.ToolboxAPI.Error warnlist[] = warns.value;
        if(nwarn > 0) {
          // Print the errors:
          System.out.println("warnings: ");
          for(int i = 0; i < warnlist.length; i++) {
            System.out.print(ToolboxClient.fromISO(warnlist[i].fname) + ", ");
            System.out.println(warnlist[i].line);
            System.out.println(ToolboxClient.fromISO(warnlist[i].msg));
          }
        }

        // List the new status of all modules:
        ListModules(app);
              
              
        // Finally we will show how to use the interpreter. 
        
        System.out.println("Interpreter tests:");
              
        VDMInterpreter interp = app.GetInterpreter();

              
        // Call a function that computes primes:
        EchoPrimes(20, interp, app, client);

        // Secondly we show how to use Apply:
        // Construct a sequence of integers to be sorted. In order 
        // to do so we need a handle to the VDMFactory to produce 
        // VDM values:
    
        VDMFactory fact = app.GetVDMFactory();
              
        app.PushTag(client); // Tag all objects created from now on
        
        VDMSequence list = fact.MkSequence(client);
        
        VDMNumeric elem;
        for(int j = 0; j < 20; j++) {
          elem = fact.MkNumeric(client, j);
          list.ImpPrepend(elem);
        }
              
        System.out.println("The sequence to be sorted: " + list.ToAscii());

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
        if(app.Tool() == ToolType.SL_TOOLBOX) {
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
          
        int sum = 0;
        for (boolean ii = s.First(eholder); ii; ii = s.Next(eholder)) {
          VDMNumeric num = VDMNumericHelper.narrow(eholder.value);
          sum = sum + GetNumeric( num );
        }

        System.out.println("The sum of all the elements: " +  sum);
        
        EchoPrimes2(50, interp, app, client);
        
        app.DestroyTag(client);

        app.Unregister(client);
        System.exit(0);
      }
      catch(APIError err) {
        System.err.println("API error"+err.getMessage ());
        System.exit(1);
      }
    }
    catch (ToolboxClient.CouldNotResolveObjectException ex) {
      System.err.println(ex.getMessage());
      System.exit(1);
    }
    catch(COMM_FAILURE ex) {
      System.err.println(ex.getMessage());
      ex.printStackTrace();
      System.exit(1);
    }
  };

  public static void ListModules(VDMApplication app)
  {
    try {
      // This function lists the modules and their status.
      // The project handle
      VDMProject prj = app.GetProject();
      
      // The Module Repository
      VDMModuleRepos repos = app.GetModuleRepos();
          
      ModuleListHolder ml = new ModuleListHolder();
      prj.GetModules(ml);
      String mlist[] = ml.value;
      System.out.println(app.Tool() == ToolType.SL_TOOLBOX ? "Modules:" : "Classes:");
          
      for(int i=0; i<mlist.length; i++) {
              
        // This struct is used to hold the status of a module:
        ModuleStatusHolder stateholder = new ModuleStatusHolder();
        // Get the status of the i'th module 
        repos.Status(stateholder, mlist[i]);  
        ModuleStatus stat = stateholder.value;
        
        // Print the status.
        System.out.println(ToolboxClient.fromISO(mlist[i]));
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

  public static void EchoPrimes(int n, VDMInterpreter interp, VDMApplication app, short client)
  {
    try {
      // Generates the sequence of primes below n and echoes the sequence to stdout.
      app.PushTag(client);
            
      // This Generic is used to hold the result from the interpreter.
      VDMGeneric g;
        
      // Use EvalExpression to compute the primes below 20
            
      String expr = "[e|e in set {1,...," + n + "} & exists1 x in set {2,...,e} & e mod x = 0]";
      g = interp.EvalExpression(client,expr);
            
      if(g.IsSequence()) {
        System.out.println("All primes below " + n + ": " + g.ToAscii());
      }
            
      VDMSequence s = VDMSequenceHelper.narrow(g);
      
      VDMGenericHolder eholder = new VDMGenericHolder();
            
      int sum = 0;
      for (boolean ii = s.First(eholder); ii; ii = s.Next(eholder)) {
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

  @SuppressWarnings("unchecked") 
  public static void EchoPrimes2(int n, VDMInterpreter interp, VDMApplication app, short client)
  {
    // Generates the sequence of primes below n and echoes the sequence to stdout.  
    // Additionally this function shows how GetCPPValue can be used to transfer
    // an entire VDM value from the toolbox to the client and convert it to a "real"
    // Java value as declared in jp.vdmtools.VDM
        
    try {
      app.PushTag(client);
      
      // This VDMGeneric is used to hold the result from the interpreter.
      VDMGeneric g;
        
      // Use EvalExpression to compute the primes below 20
            
      String expr = "[e|e in set {1,...," + n + "} & exists1 x in set {2,...,e} & e mod x = 0]";
      g = interp.EvalExpression(client,expr);
        
      if(g.IsSequence()) {
        System.out.println("All primes below " + n + ": " + g.ToAscii());
      }
            
      VDMSequence s = VDMSequenceHelper.narrow(g);
            
      // Conversion to real Java VDM value!
            
      LinkedList sj = new LinkedList ();
            
      VDMGenericHolder eholder = new VDMGenericHolder();
            
      // Convert the Generic g into a "real" Java Sequence value
      for (boolean ii = s.First(eholder); ii; ii = s.Next(eholder)) {
        VDMNumeric num = VDMNumericHelper.narrow(eholder.value);
        sj.add(Integer.valueOf( GetNumeric( num ) ));
      }
            
      int sum = 0;
      for (Iterator itr = sj.iterator(); itr.hasNext();){
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

