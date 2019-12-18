/***
*  * WHAT
*  *    This file is an example of how to implement a client 
*  *    process that uses the CORBA API of the VDM Toolbox.
*  *                                                                       
*  *    The file can be compiled with MS VC++ 6.0 on 
*  *    windows NT/95 and with gcc 2.95.2 on Unix.
*  *                                                                       
*  *    Use the makefile Makefile.nm if you use nmake on 98/NT and         
*  *    Makefile if you compile on Linux
*  * ID
*  *    $Id: client_example.cc,v 1.27 2006/02/07 05:14:11 vdmtools Exp $
*  * AUTHOR
*  *    Ole Storm + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <iostream>
using namespace std;

#include <string>
// CORBA Initialisation and other stuff for omniORB4
#include "corba_client.h"

#ifdef _MSC_VER
#include <direct.h> // for getcwd
#else
#include <unistd.h> // for getcwd
#endif // _MSC_VER
#include <stdlib.h>

//char ABS_FILENAME[200];
VDM::ClientID client_id;

//#define ADD_PATH(p,s) strcat(strcpy(ABS_FILENAME, p), s)
#define SORT_NUM 20

void EchoPrimes(int, ToolboxAPI::VDMInterpreter_var, ToolboxAPI::VDMApplication_var);
void EchoPrimes2(int, ToolboxAPI::VDMInterpreter_var, ToolboxAPI::VDMApplication_var);
void ListModules(ToolboxAPI::VDMApplication_var app);

int main(int argc, char *argv[])
{
#ifdef VDMSL
  VDMCorbaClient client(ToolboxAPI::SL_TOOLBOX);
#endif //VDMSL
#ifdef VDMPP
  VDMCorbaClient client(ToolboxAPI::PP_TOOLBOX);
#endif //VDMPP

  const char * source_path_p = getenv("VDM_SOURCE_LOCATION");
  if( source_path_p == NULL ) {
    string sdir;
    char buf[1024];
    if( getcwd( buf, sizeof( buf ) ) != NULL ) {
#ifdef _MSC_VER
      // Convert backslash in path to forward slash as on Unix.
      for (char* s = buf; s = strchr(s, '\\'); s++) {
        *s = '/';
      }
#endif
      sdir = buf;
      for( int i = 0; i < 2; i++ ) {
        std::string::size_type index = sdir.find_last_of( '/' );
        if( index == string::npos ) break;
        sdir = sdir.substr( 0, index );
      }
      sdir += "/";
    }
    source_path_p = sdir.c_str();

    cerr << "Environment variable VDM_SOURCE_LOCATION not set" << endl;
    cerr << "Default location: " << source_path_p << endl;
  }
  string source_path (source_path_p);
  
  // The main handle to the VDM Toolbox:
  ToolboxAPI::VDMApplication_var app;

  // Initialise the ORB. Consult corba_client.{h,cc} and the
  // omniORB3 user maual for details on how this is done.
  client.init_corba(argc, argv);

  // Retrieve a handle to the VDMToolbox most recently started. The
  // handle is achieved through a string representation of a CORBA
  // object created by the VDM Toolbox. The string is written to a
  // file named object.string and located in the directory defined 
  // by VDM_OBJECT_LOCATION
  // If this is not set, get_app automatically searches for the file 
  // in the home (Unix) or profiles directory (Windows NT/95).

//#ifdef VDMSL
//  GetAppReturnCode rt = client.get_app(app, NULL, ToolboxAPI::SL_TOOLBOX);
//#endif //VDMSL
//#ifdef VDMPP
//  GetAppReturnCode rt = client.get_app(app, NULL, ToolboxAPI::PP_TOOLBOX);
//#endif //VDMPP
  GetAppReturnCode rt = client.get_app(app, NULL);
  switch(rt) {
    case VDM_OBJECT_LOCATION_NOT_SET: {
      cerr << "Environment variable VDM_OBJECT_LOCATION not set" << endl;
      exit(0);
    }
    case OBJECT_STRING_NON_EXISTING: {
        cerr << "The file " + client.GetIORFileName() + " could not be located. \
                 Make sure the Toolbox is running" << endl;
      exit(0);
    }
    case CORBA_ERROR: {
      cerr << "Unable to setup the CORBA environment" << endl;
      exit(0);
    }
    case CORBA_SUCCESS:
    default: {
      break;
    }
  }

  try {
    // Register the client in the Toolbox:
    client_id = app->Register();

    // First we acquire a handle to the VDMProject interface to
    // configure the current project:
    ToolboxAPI::VDMProject_var prj = app->GetProject();

    prj->New();  // New project

    // Configure the project to contain the necessary files. The
    // files must be located in the same directory as where the 
    // VDM Toolbox was started. Otherwise the absolute path to the 
    // files should be used
    if(app->Tool() == ToolboxAPI::SL_TOOLBOX) {
      //prj->AddFile(ADD_PATH(source_path, "sort.vdm"));
      prj->AddFile((source_path + "sort.vdm").c_str());
    }
    else {
      prj->AddFile((source_path + "implsort.vpp").c_str()); 
      prj->AddFile((source_path + "sorter.vpp").c_str());
      prj->AddFile((source_path + "explsort.vpp").c_str());
      prj->AddFile((source_path + "mergesort.vpp").c_str());
      prj->AddFile((source_path + "sortmachine.vpp").c_str());
    }
    // Parse the files:
    ToolboxAPI::VDMParser_var parser = app->GetParser();
    ToolboxAPI::FileList_var fl;
    prj->GetFiles(fl);

    // Parse the files in two different ways. First we traverse 
    // the list of files and parse each file individually. (OK, I
    // know that for the SL_TOOLBOX there is only one file 
    // configured, but it is fine for an illustration)
    cout << "Parsing files individually" << endl;
    for(unsigned int i = 0; i < fl->length(); i++) {
      cout << (char *)fl[i] << "...Parsing...";
      if(parser->Parse(fl[i])) {
        cout << "done." << endl;
      }
      else {
        cout << "error." << endl;
      }
    }
    cout << endl;
    
    // And then we parse all files in one go:
    cout << "Parsing entire list...";
    parser->ParseList(fl);
    cout << "done." << endl;
    
    // If errors were encountered during the parse they can now be
    // inspected:
    ToolboxAPI::VDMErrors_var errhandler = app->GetErrorHandler();  
    // The error handler
    ToolboxAPI::ErrorList_var errs;

    // retrieve the sequence of errors
    int nerr = errhandler->GetErrors(errs); 
    if(nerr) {
      // Print the error:
      cout << nerr << " errors:" << endl;
      for(int ierr = 0; ierr < nerr; ierr++) {
        cout << (char *) errs[ierr].fname << ", " 
             << errs[ierr].line << endl 
             << (char *) errs[ierr].msg << endl;
      }
    }
    // Warnings can be queried similarly.

    // List the names and status of all modules:
    ListModules(app);

    // Type check all modules:
    ToolboxAPI::VDMTypeChecker_var tchk = app->GetTypeChecker();
    ToolboxAPI::ModuleList_var modules;
    prj->GetModules(modules);
    cout << "Type checking all " << (app->Tool() == ToolboxAPI::SL_TOOLBOX ? "modules" : "classes") << "...";
    if(tchk->TypeCheckList(modules)) {
      cout << "done." << endl;
    }
    else {
      cout << "errors." << endl;
    }
    // If warnings were encountered during the type check they can now be inspected:
    ToolboxAPI::VDMErrors_var warnhandler = app->GetErrorHandler();
    // The error handler
    ToolboxAPI::ErrorList_var warns;

    // retrieve the sequence of warnings
    int nwarn = warnhandler->GetWarnings(warns);
    if(nwarn) {
      // Print the warning:
      if (nwarn == 1) {
        cout << nwarn << " warning:" << endl;
      }
      else {
        cout << nwarn << " warnings:" << endl;
      }
      for(int iwarn = 0; iwarn < nwarn; iwarn++) {
        cout << (char *) warns[iwarn].fname << ", "
             << warns[iwarn].line << endl
             << (char *) warns[iwarn].msg << endl;
      }
    }

    // List the new status of all modules:
    ListModules(app);

    // Finally we will show how to use the interpreter. 

    cout << endl << "Interpreter tests:" << endl << endl;
    ToolboxAPI::VDMInterpreter_var interp = app->GetInterpreter();

    // Call a function that computes primes:
    EchoPrimes(20, interp, app);

    // Secondly we show how to use Apply:
    // Construct a sequence of integers to be sorted. To do
    // so we need a handle to the VDMFactory to produce VDM values:
    VDM::VDMFactory_var fact = app->GetVDMFactory();

    app->PushTag(client_id); // Tag all objects created from now on

    VDM::VDMSequence_var list = fact->MkSequence(client_id);
    for(int j = 0; j < SORT_NUM; j++) {
      VDM::VDMNumeric_var elem = fact->MkNumeric(client_id, j);
      list->ImpPrepend(elem);
    }
    cout << "The sequence to be sorted: " << list->ToAscii() << endl;

    // Construct the argument list for the call. That is, construct 
    // a VDM::Sequence containing all arguments in the right order:
    VDM::VDMSequence_var arg_l = fact->MkSequence(client_id);
    arg_l->ImpAppend(list);

    // Set Verbose to true, to show the results of using the
    // interpreter in the user interface:
    interp->Verbose(true);
    interp->Debug(true);
    
    // First initialise the interpreter
    interp->Initialize();

    VDM::VDMGeneric_var g;
    if (app->Tool() == ToolboxAPI::SL_TOOLBOX) {
      g = interp->Apply(client_id, "MergeSort", arg_l);
    }
    else { // PP_TOOLBOX
      // First we create the main sort object:
      interp->EvalCmd("create o := new SortMachine()");
      
      // Next, the GoSorting method is called on this object:
      g = interp->Apply(client_id, "o.GoSorting", arg_l);
    }

    cout << "The sorted sequence: " << g->ToAscii() << endl;

    // Finally we iterate through the returned sequence to compute
    // the sum of all the elements of the sequence:

    VDM::VDMSequence_var s = VDM::VDMSequence::_narrow(g);
    int sum = 0;
    for (int k = 1; k <= s->Length(); k++) {
      VDM::VDMNumeric_var n = VDM::VDMNumeric::_narrow(s->Index(k));
      sum += (Int(client.GetCPPValue(n))).GetValue();
    }
    cout << "The sum of all the elements: " << sum << endl;

    EchoPrimes2(50, interp, app);

    app->DestroyTag(client_id);

    // Unregister the client:
    app->Unregister(client_id);
  }
  catch(ToolboxAPI::APIError &ex) {
    cerr << "Caught API error " << (char *)ex.msg << endl;
  }
  catch(CORBA::COMM_FAILURE &ex) {
    cerr << "Caught system exception COMM_FAILURE, unable to contact server" << endl;
  }
  catch(omniORB::fatalException& ex) {
    cerr << "Caught omniORB3 fatalException" << endl;
  }

  return 0;  
}

void EchoPrimes(int n, ToolboxAPI::VDMInterpreter_var interp, ToolboxAPI::VDMApplication_var app)
  // Generates the sequence of primes below n and echoes the sequence to stdout.
{
  app->PushTag(client_id);

  interp->Initialize ();

  // This VDM::Generic is used to hold the result from the interpreter.
  VDM::VDMGeneric_var g;
  
  // Use EvalExpression to compute the primes below 20
  char expr[200];
  sprintf(expr, "[e|e in set {1,...,%d} & exists1 x in set {2,...,e} & e mod x = 0 ]", n);
  g = interp->EvalExpression(client_id, expr);
  if (g->IsSequence()) {
    cout << "All primes below " << n << ":" << endl << g->ToAscii() << endl;
  }
  VDM::VDMSequence_var s = VDM::VDMSequence::_narrow(g);
  int sum = 0;
  for (int k = 1; k <= s->Length(); k++) {
    VDM::VDMNumeric_var n = VDM::VDMNumeric::_narrow(s->Index(k));
    sum += (Int(VDMCorbaClient::GetCPPValue(n))).GetValue();
  }
  cout << "The sum of all the primes: " << sum << endl;
  app->DestroyTag(client_id); // Clean up...
}

void EchoPrimes2(int n, ToolboxAPI::VDMInterpreter_var interp, ToolboxAPI::VDMApplication_var app)
  // Generates the sequence of primes below n and echoes the sequence to stdout.  
  // Additionally this function shows how GetCPPValue can be used to transfer
  // an entire VDM value from the toolbox to the client and convert it to a "real"
  // C++ value as declared in metaiv.h
{
  // This VDM::VDMGeneric is used to hold the result from the interpreter.
  VDM::VDMGeneric_var g;
  
  // Use EvalExpression to compute the primes below 20
  char expr[200];
  sprintf(expr, "[e|e in set {1,...,%d} & exists1 x in set {2,...,e} & e mod x = 0 ]", n);
  g = interp->EvalExpression(client_id, expr);

  // Convert the VDM::Generic g into a "real" metaiv-Sequence  value:
  Sequence s (VDMCorbaClient::GetCPPValue(g));
  
  // Now we can safely destroy g since the entire value has been transferred to the client:
  g->Destroy();

  cout << "All primes below " << n << ":" << endl << wstring2string(s.ascii()) << endl;
  int sum = 0;
  Generic gg;
  for(bool bb = s.First(gg); bb; bb = s.Next(gg)) {
    //sum += (int)Real(gg).GetValue();
    sum += Real(gg).GetIntValue();
  }
  cout << "The sum of all the primes: " << sum << endl;
}

void ListModules(ToolboxAPI::VDMApplication_var app)
  // This function lists the modules and their status.
{
 // The project handle
  ToolboxAPI::VDMProject_var prj = app->GetProject();

  // The Module Repository
  ToolboxAPI::VDMModuleRepos_var repos = app->GetModuleRepos();

  ToolboxAPI::ModuleList_var ml;
  prj->GetModules(ml);
  cout << (app->Tool() == ToolboxAPI::SL_TOOLBOX ? "Modules:" : "Classes:") << endl;
  for(unsigned int i = 0; i < ml->length(); i++) {
    // This struct is used to hold the status of a module:
    ToolboxAPI::ModuleStatus stat; 
    // Get the status of the i'th module
    repos->Status(stat, ml[i]);  
    // Print the status. 0 = none, 1 = OK
    cout << (int) stat.SyntaxChecked 
         << (int) stat.TypeChecked
         << (int) stat.CodeGenerated
         << (int) stat.PrettyPrinted
         << " " << (char *)ml[i] << endl;
  }
}

