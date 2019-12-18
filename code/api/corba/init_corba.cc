/***
*  * WHAT
*  *    Functions used to initialize and terminate the CORBA API.
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/api/corba/init_corba.cc,v $
*  * VERSION
*  *    $Revision: 1.14 $
*  * DATE
*  *    $Date: 2005/09/22 00:30:50 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *
*  * PROJECT
*  *    INFORMA and Toolbox
*  * AUTHOR
*  *    Ole Storm + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <fstream>
#include <stdlib.h>
#include "internal_api.hh"
#include "corba_api.hh"
#include "corba_api.h"
#include "object_map.h"
#include "init_corba.h"
#include "tb_wstring.h"
#include "tbutils.h"
#ifdef _MSC_VER
#include <process.h>
#else
#include <unistd.h>
#endif // _MSC_VER

#define VDM_IOR_FILENAME "vdmref.ior"
#define VPP_IOR_FILENAME "vppref.ior"

#ifdef _MSC_VER
#define HOME_ENV_NAME "USERPROFILE"
#else
#define HOME_ENV_NAME "HOME"
#endif // _MSC_VER

VDMApplication_i * VDMCORBA::toolbox_app;
// A handle to the one and only instance of Application_i. Used
// interanlly by the code to add events to clients.

// The one and only hash table holding the CORBA objects currently
// alive in the server process.

CORBA::ORB_var VDMCORBA::orb;
PortableServer::POA_var VDMCORBA::poa;
ToolboxAPI::ToolType VDMCORBA::toolType;

std::string VDMCORBA::getIorFileLocation( const ToolboxAPI::ToolType& type )
{
  // First we test to see if the environment variable
  // VDM_OBJECT_LOCATION is set. If it is, the ior file is
  // stored here. Otherwise, on unix the file is written to the root
  // of the users home directory, whereas on win NT the file is written
  // to the directory pointed to by USERPROFILE If this variable is
  // not set the file is created in the directory where the toolbox
  // was started.

  char buf[11];
  memset(buf,'\0', 11);
#ifdef _MSC_VER
  sprintf(buf, "%d", _getpid());
#else
  sprintf(buf, "%d", getpid());
#endif // _MSC_VER
  string pid (buf);
  string prext;
  //string prext = "_" + pid;

  std::string iorFileName;
  if ( SL_TOOLBOX == type ) {
    iorFileName = VDM_IOR_FILENAME;
  }
  else {
    iorFileName = VPP_IOR_FILENAME;
  }
  iorFileName += prext;

  std::string iorFilePath;
  const char *location = getenv( "VDM_OBJECT_LOCATION" );
  if( NULL != location ) {
    iorFilePath = location;
  }
  else {
    const char* home = getenv( HOME_ENV_NAME );
    if( NULL != home ) {
      iorFilePath = home;
    }
    else {
      iorFilePath = ".";
    }
  }
  std::string iorfile_s = iorFilePath + "/" + iorFileName;
  return iorfile_s;
}

bool VDMCORBA::init_corba_api(int argc, char *argv[], std::wstring &err)
{
  // Corba initialisation:

  // Change the settings for argc and argv to the ones below, to
  // enable orb tracing.

  //  argc = 3;
  //  argv[1] = L"-ORBtraceLevel";
  //  argv[2] = L"20";
  /*
  argc = 8;
  argv[1] = "-ORBinConScanPeriod";
  argv[2] = "0";
  argv[3] = "-ORBoutConScanPeriod";
  argv[4] = "0";
  argv[5] = "-ORBtraceLevel";
  argv[6] = "20";
  argv[7] = "-ORBtraceInvocations";
  */
//  argc = 3;
//  argv[1] = "-ORBInitRef";
//  argv[2] = "NameService=corbaloc:iiop:1.0@localhost:2809/NameService";
  int nargc = 3;
  char * nargv [3];
  nargv[0] = CORBA::string_dup(argv[0]);
  nargv[1] = CORBA::string_dup("-ORBInitRef");
  nargv[2] = CORBA::string_dup("NameService=corbaloc:iiop:1.0@localhost:2809/NameService");

  try {
    // according to the CORBA-specification following methods should
    // not throw an exception, but omniORB2 does not follow the
    // standard here
    //orb = CORBA::ORB_init(argc, argv);
    VDMCORBA::orb = CORBA::ORB_init(nargc, nargv);
    CORBA::Object_var obj = VDMCORBA::orb->resolve_initial_references("RootPOA");
    VDMCORBA::poa = PortableServer::POA::_narrow(obj);
  }
  catch (CORBA::INITIALIZE &e) {
    vdm_err << L"Could not initialize CORBA ORB!" << endl;
    return false;
  }
  catch (CORBA::COMM_FAILURE& ex) {
    vdm_err << L"CORBA::COMM_FAILURE: CORBA ORB could not be initialized!" << endl;
    return false;
  }
  catch (omniORB::fatalException& ex) {
    cerr << L"Fatal excpetion when initializing ORB:" << ex.file() << L", " << ex.errmsg() << endl;
    return false;
  }
  catch (...) {
    vdm_err << L"Unknown exception when initializing ORB and BOA" << endl;
    return false;
  }

  //  if (CORBA::is_nil (orb) || CORBA::is_nil (vdmBoaRef))
  if (CORBA::is_nil (VDMCORBA::orb) || CORBA::is_nil (VDMCORBA::poa)) {
    vdm_err << L"Initialisation of ORB and BOA failed. ORB or BOA-object is nil." << endl;
    return false;
  }

  ToolboxAPI::VDMApplication_var app_ref;
  CORBA::String_var pp;

  try {
    // We create an instance of the Application interface (and NOT the
    // VDMApplication interface). The GUI client process uses this
    // (extended) interface, while all other client processes will cast
    // this object reference into a VDMApplication interface.
    VDMCORBA::toolbox_app = new VDMApplication_i();

    // Register the object
    //      VDMCORBA::toolbox_app->_obj_is_ready(vdmBoaRef);
    PortableServer::ObjectId_var appId = VDMCORBA::poa->activate_object(VDMCORBA::toolbox_app);
    //      vdmBoaRef->impl_is_ready(0,1); // Non-blocking

    // The object toolbox_app may be used outside this file. For
    // instance to add events to the event queues like
    // toolbox_app->AddEvent()

    // A reference to the application object to be distributed
    app_ref = VDMCORBA::toolbox_app->_this();
    pp = VDMCORBA::orb->object_to_string(app_ref);
  }
  // according to the CORBA specification, no exceptions should be raised
  catch (CORBA::COMM_FAILURE& ex) {
    vdm_err << L"CORBA::COMM_FAILURE: VDMApplication-object could not be created!" << endl;
    return false;
  }
  catch (omniORB::fatalException& ex) {
    cerr << L"Fatal excpetion when trying to create VDMApplication-object:"
         << ex.file() << L", " << ex.errmsg() << endl;
    return false;
  }
  catch (...) {
    vdm_err << L"Unknown exception occured when trying to create VDMApplication" << endl;
    return false;
  }

  // Insert object in hash table:
  // obj_map.Insert(app_ref, VDMCORBA::toolbox_app);

  // If there is a COS NamingService, make the application object known to it
  if (bindObjectToName (VDMCORBA::orb, app_ref)) {
    vdm_log << L"API-Info: Registered VDMApplication at COS-NamingService." << endl;
  }
  // Write a wstring representing the object to a file.

  PortableServer::POAManager_var pman = VDMCORBA::poa->the_POAManager();
  pman->activate();

  //
  if (CORBA::is_nil(app_ref)) {
    cerr << L"app_ref is nil - quitting" << endl;
    return false;
  }

  // Set ToolType
  VDMCORBA::toolType = app_ref->Tool ();

  // Get iorfile location
  std::string iorfile_s = getIorFileLocation( VDMCORBA::toolType );

  // Create file
  ofstream newfile;
  newfile.open(iorfile_s.c_str());
  if(newfile.good()) {
    //newfile << (char *)pp << endl << flush;
    char *p = (char *)pp;
    newfile.write(p, strlen(p));
    newfile.flush();
    newfile.close();
    return true;
  }
  else {
    err = L"Could not create file " + TBWSTR::string2wstring(iorfile_s);
    return false;
  }
}

/**
 * Used to bind the application object to a name
 * in the NameService. Returns false if binding
 * fails.
 */
bool VDMCORBA::bindObjectToName (CORBA::ORB_ptr orb, CORBA::Object_ptr appObj)
{
//  ToolboxAPI::VDMApplication_var app = ToolboxAPI::VDMApplication::_narrow (appObj);
//
//  if (CORBA::is_nil (app))
//    return 0;

  CosNaming::NamingContext_var rootContext;

  try {
    // Obtain a reference to the root context of the Name service:
    CORBA::Object_var obj = orb->resolve_initial_references ("NameService");
    if (CORBA::is_nil (obj))
    {
      return false;
    }

    // Narrow the reference returned.
    rootContext = CosNaming::NamingContext::_narrow(obj);
    if (CORBA::is_nil (rootContext)) {
//      cerr << "Failed to narrow the root naming context." << endl;
      return false;
    }
  }
  catch (CORBA::NO_RESOURCES &) {
//    cerr << "Caught NO_RESOURCES exception. You must configure omniORB "
//         << "with the location" << endl
//         << "of the naming service." << endl;
    return false;
  }
  catch (CORBA::ORB::InvalidName & e) {
    // This should not happen!
//    cerr << "Service required is invalid [does not exist]." << endl;
    return false;
  }
  catch (...) {
    return false;
  }

  try {
    // Bind a context called "SL_TOOLBOX" or "PP_TOOLBOX" to the root context:

    CosNaming::Name contextName;
    contextName.length (1);
    contextName[0].id = (const char*)"VDMTOOLBOX";
    contextName[0].kind = (const char*)"TOOL";

    CosNaming::NamingContext_var vdmContext;
    try {
      vdmContext = rootContext->bind_new_context (contextName);
    }
    catch (CosNaming::NamingContext::AlreadyBound& e)
    {
      CORBA::Object_var obj;
      obj = rootContext->resolve(contextName);
      vdmContext = CosNaming::NamingContext::_narrow(obj);
      if( CORBA::is_nil(vdmContext) ) {
        cerr << "Failed to narrow naming context." << endl;
        return false;
      }
    }
    CosNaming::Name objectName;
    objectName.length(1);
    if ((VDMCORBA::toolType = VDMCORBA::toolbox_app->Tool ()) == ToolboxAPI::SL_TOOLBOX) {
      objectName[0].id = (const char*)"SL_TOOLBOX";
    }
    else {
      objectName[0].id = (const char*)"PP_TOOLBOX";
    }
    objectName[0].kind = (const char*)"VDMApplication";
    // Note on kind: The kind field is used to indicate the type
    // of the object. This is to avoid conventions such as that used
    // by files (name.type -- e.g. test.ps = postscript etc.)

    try {
      vdmContext->bind(objectName, appObj);
    }
    catch (CosNaming::NamingContext::AlreadyBound& e) {
//      cerr << L"API-Error: Could not bind VDMApplication"
//           << L" to NamingService.\nAn instance of the "
//           << ( VDMCORBA::toolbox_app->Tool() == SL_TOOLBOX ? L"VDM-" : L"VDM++-" )
//           << L"Toolbox is probably already running." << endl;
//      cerr << L"(if not, restarting and cleaning up your NameService should help)" << endl;
//      return false;
      vdmContext->rebind(objectName, appObj);
    }
  }
  catch (CORBA::COMM_FAILURE& ex) {
    return false;
  }
  catch(CORBA::TRANSIENT& ex) {
    return false;
  }
  catch (omniORB::fatalException& ex) {
    cerr << L"Fatal excpetion:" << ex.file() << L", " << ex.errmsg() << endl;
    return false;
  }
  catch (...) {
    return false;
  }
  return true;
}

/**
 * Releases name in the NameService. If this fails, the user
 * will have to shutdown the naming service and restart it
 * before he can use it again.
 */
bool VDMCORBA::unbindObjectFromName (CORBA::ORB_ptr orb)
{
  CosNaming::NamingContext_var rootContext;
  try {
    CORBA::Object_ptr obj;
    obj = orb->resolve_initial_references ("NameService");
    if (CORBA::is_nil (obj)) {
      return false;
    }
    rootContext = CosNaming::NamingContext::_narrow (obj);
    if (CORBA::is_nil (rootContext)) {
      return false;
    }
  if (CORBA::is_nil (rootContext))
    return false;
  }
  catch (CORBA::ORB::InvalidName& e) {
    return false;
  }
  catch (...) {
    return false;
  }

  try {
    CosNaming::Name contextName;
    contextName.length (1);

    if (VDMCORBA::toolType == SL_TOOLBOX) {
      contextName[0].id = (const char*)"SL_TOOLBOX";
    }
    else {
      contextName[0].id = (const char*)"PP_TOOLBOX";
    }
    contextName[0].kind = (const char*)"VDMApplication";

    rootContext->unbind (contextName);
  }
  catch (...) {
    return false;
  }
  return true;
}

void VDMCORBA::terminate_corba_api()
  // This function removes the file object.[vdm|vpp]
{
  // Get iorfile location
  std::string home_s = getIorFileLocation( VDMCORBA::toolType );

  // Remove file
  remove(home_s.c_str());

  if (unbindObjectFromName (VDMCORBA::orb)) {
    vdm_log << L"API-Info: Unregistered VDMApplication from COS-NamingService." << endl;
  }
  try {
    //      VDMCORBA::toolbox_app->_dispose ();
    // FIXME: What should be here?
    //      vdmBoaRef->destroy ();
    // VDMCORBA::orb->NP_destroy ();
    VDMCORBA::orb->destroy ();
  }
  catch (CORBA::SystemException &e) {
    vdm_err << L"Caught a CORBA::SystemException when trying to shut down the BOA" << endl;
  }
  catch (omniORB::fatalException &ex) {
    vdm_err << L"Caught a omniORB::fatalException when trying to shut down the BOA: "
            << ex.file() << L", " << ex.errmsg() << endl;
  }
  catch (...) {
    vdm_err << L"Caught an unknown exception when trying to shut down the BOA." << endl;
  }
}
